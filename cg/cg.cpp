#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>
#include<cstdio>
#include<cfloat>
#include <chrono>
using namespace std::chrono;

using namespace std;
int N;
inline double dot(const double *a, const double *b) {
    double r = 0.0;

    for (int i = 0; i < N; i++) r += a[i] * b[i];
    return r;
}
int main (int argc, char *argv[]) {
    double *x, *b, **A, *Ap, *r, *p;
    double rk2, rk2n, alpha, beta, pAp;
    double epsilon = FLT_EPSILON;
    if(argc == 1) exit(1);
    if(argc > 1) N = atoi(argv[1]);
    x = new double[N];
    b = new double[N];
    A = new double*[N];
    Ap = new double[N];
    r = new double[N];
    p = new double[N];
    for (int i = 0; i < N; i++) A[i] = new double[N];
    // read input files
    ifstream Afile("A.txt");
    ifstream bfile("b.txt");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) Afile >> A[i][j];
        bfile >> b[i];
    }
    Afile.close();
    bfile.close();

    auto start = high_resolution_clock::now();

    // iteration initialization: x0, r0 and p0
    for (int i = 0; i < N; i++) x[i] = 0.0;
    for (int i = 0; i < N; i++) p[i] = r[i] = b[i];
    // iteration
    //int count = 0;
    while (true) {
        rk2 = dot(r, r);
        for (int i = 0; i < N; i++) Ap[i] = dot(A[i], p);
        pAp = dot(p, Ap);
        alpha = rk2 / pAp;
        for (int i = 0; i < N; i++) x[i] = x[i] + alpha * p[i];
        for (int i = 0; i < N; i++) r[i] = r[i] - alpha * Ap[i];
        rk2n = dot(r, r);
        // printf("# Iter %d: residue = %.5f\n", ++count, sqrt(rk2n));
        if (sqrt(rk2n) < epsilon) break;
        if (sqrt(rk2n) > 1000000) {printf("Maybe overflow, exit!!!!!\n"); break;}
        beta = rk2n / rk2;
        for (int i = 0; i < N; i++) {
            p[i] = r[i] + beta * p[i];
        }
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    printf("Time: %.3f ms\n", duration.count()/1000.0);
    // compare with truth
    ifstream xfile("X.txt");
    double meandiff=0.0;
    for (int i=0; i<N; i++) {
        double tmp=0.0;
        xfile >> tmp;
        meandiff += abs(tmp-x[i]);
    }
    printf("Mean difference with true X: %.5f\n", meandiff/N);
    // free memory
    for (int i=0; i<N; i++) {
        delete[] A[i];
    }
    delete [] x;
    delete [] b;
    delete [] A;
    delete [] Ap;
    delete [] r;
    delete [] p;
    return 0;
}
