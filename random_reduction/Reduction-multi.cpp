#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sys/time.h>
#include <mpi.h>

#define epsilon 1.e-8

using namespace std;

int main(int argc, char *argv[]) {
    int M, N;
    MPI_Init(&argc, &argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Status status;
    int tag = 1;

    string T, P, Db;
    M = atoi(argv[1]);
    N = atoi(argv[2]);

    // split matrix by columns (ceiling)
    int split_size = (N + world_size - 1) / world_size;

    double elapsedTime;
    timeval start, end;

    if (argc > 3) {
        T = argv[3];
        if (argc > 4) {
            P = argv[4];
            if (argc > 5) {
                Db = argv[5];
            }
        }
    }
    // cout<<T<<P<<endl;
    double alpha, beta, gamma;
    double **U_t;
    double **Alphas, **Betas, **Gammas;
    U_t = new double *[M];
    Alphas = new double *[M];
    Betas = new double *[M];
    Gammas = new double *[M];

    for (int i = 0; i < M; i++) {
        if (world_rank == 0) U_t[i] = new double[N]; // U_t for 0 process
        else                 U_t[i] = new double[split_size]; // U_t for other process
        Alphas[i] = new double[M];
        Betas[i] = new double[M];
        Gammas[i] = new double[M];
    }
    //Read from file matrix, if not available, app quit
    //Already transposed

    if (world_rank == 0) { // proc 0 read from file
        ifstream matrixfile("matrix");
        if (!(matrixfile.is_open())) {
            cout << "Error: file not found" << endl;
            return 0;
        }
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {

                matrixfile >> U_t[i][j];
            }
        }
        matrixfile.close();
        /* Reductions */

        gettimeofday(&start, nullptr);

        /* send each proc a split of U_t */
        auto *buffer = new double[split_size * M]();
        for (int i = world_size - 1; i > 0; --i) {
            int next = 0;
            for (int c = i * split_size; c < (i + 1) * split_size && c < N; ++c) {
                for (int r = 0; r < M; ++r) {
                    buffer[next++] = U_t[r][c];
                }
            }
            MPI_Send(buffer, split_size * M, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
        }
        delete [] buffer;
    } else {
        /* other proc receive split of U_t from proc 0 */
        auto *buffer = new double[split_size * M];
        buffer[2] = -1;
        buffer[3] = -2;
        int next = 0;
        MPI_Recv(buffer, split_size * M, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
        /*
        cout << "receive" << endl;
        for (int i = 0; i < split_size * M; ++i) {
            cout << buffer[i] << " ";
        }
        */
        cout << endl;
        for (int j = 0; j < split_size; ++j) {
            for (int i = 0; i < M; ++i) {
                U_t[i][j] = buffer[next++];
            }
        }
        delete[] buffer;
    }

    for (int i = 0; i < M; i++) {        //convergence
        for (int j = 0; j < M; j++) {
            alpha = 0.0;
            beta = 0.0;
            gamma = 0.0;
            for (int k = 0; k < split_size; k++) {
                alpha = alpha + (U_t[i][k] * U_t[i][k]);
                beta = beta + (U_t[j][k] * U_t[j][k]);
                gamma = gamma + (U_t[i][k] * U_t[j][k]);
            }
            Alphas[i][j] = alpha;
            Betas[i][j] = beta;
            Gammas[i][j] = gamma;
        }
    }

    if (world_rank == 0) {
        auto *buffer = new double[M * M];
        for (int proc = 1; proc < world_size; ++proc) {
            MPI_Recv(buffer, M * M, MPI_DOUBLE, proc, tag, MPI_COMM_WORLD, &status);
            for (int i = 0; i < M; ++i) {
                for (int j = 0; j < M; ++j) {
                    Alphas[i][j] += buffer[i * M + j];
                }
            }
            MPI_Recv(buffer, M * M, MPI_DOUBLE, proc, tag, MPI_COMM_WORLD, &status);
            for (int i = 0; i < M; ++i) {
                for (int j = 0; j < M; ++j) {
                    Betas[i][j] += buffer[i * M + j];
                }
            }
            MPI_Recv(buffer, M * M, MPI_DOUBLE, proc, tag, MPI_COMM_WORLD, &status);
            for (int i = 0; i < M; ++i) {
                for (int j = 0; j < M; ++j) {
                    Gammas[i][j] += buffer[i * M + j];
                }
            }
        }
        delete[] buffer;

        gettimeofday(&end, nullptr);

        // fix final result
        //Output time and iterations
        if (T == "-t" || P == "-t") {
            elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;
            elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;
            cout << "Time: " << elapsedTime << " ms." << endl << endl;
        }
        // Output the matrixes for debug
        if (T == "-p" || P == "-p") {
            cout << "Alphas" << endl << endl;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++) {
                    cout << Alphas[i][j] << "  ";
                }
                cout << endl;
            }
            cout << endl << "Betas" << endl << endl;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++) {
                    cout << Betas[i][j] << "  ";
                }
                cout << endl;
            }
            cout << endl << "Gammas" << endl << endl;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++) {
                    cout << Gammas[i][j] << "  ";
                }
                cout << endl;
            }
        }

        //Generate files for debug purpouse
        if (Db == "-d" || T == "-d" || P == "-d") {
            ofstream Af;
            //file for Matrix A
            Af.open("AlphasMPI.mat");
            /*    Af<<"# Created from debug\n# name: A\n# type: matrix\n# rows: "<<M<<"\n# columns: "<<N<<"\n";*/
            //Af<<M<<"  "<<N;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++) {
                    Af << " " << Alphas[i][j];
                }
                Af << "\n";
            }
            Af.close();
            ofstream Uf;
            //File for Matrix U
            Uf.open("BetasMPI.mat");
            /*    Uf<<"# Created from debug\n# name: Ugpu\n# type: matrix\n# rows: "<<M<<"\n# columns: "<<N<<"\n";*/
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++) {
                    Uf << " " << Betas[i][j];
                }
                Uf << "\n";
            }
            Uf.close();

            ofstream Vf;
            //File for Matrix V
            Vf.open("GammasMPI.mat");
            /*    Vf<<"# Created from debug\n# name: Vgpu\n# type: matrix\n# rows: "<<M<<"\n# columns: "<<N<<"\n";*/
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++) {
                    Vf << " " << Gammas[i][j];
                }
                Vf << "\n";
            }
            Vf.close();
            ofstream Sf;
        }
    } else {
        auto *buffer = new double[M * M];
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < M; ++j) {
                buffer[i * M + j] = Alphas[i][j];
            }
        }
        MPI_Send(buffer, M * M, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);

        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < M; ++j) {
                buffer[i * M + j] = Betas[i][j];
            }
        }
        MPI_Send(buffer, M * M, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);

        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < M; ++j) {
                buffer[i * M + j] = Gammas[i][j];
            }
        }
        MPI_Send(buffer, M * M, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        delete[] buffer;
    }

    for(int i = 0; i<M;i++){
        delete [] Alphas[i];
        delete [] U_t[i];
        delete [] Betas[i];
        delete [] Gammas[i];

    }
    delete [] Alphas;
    delete [] Betas;
    delete [] Gammas;
    delete [] U_t;
    MPI_Finalize();
    return 0;
}