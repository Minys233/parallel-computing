//
// Created by 闵垚森 on 2019/12/17.
//

#include <cstdio>
#include <cstring>
#include <mpi.h>

const size_t bufsize = 1024*1024*100; // 100M

int main(int argc, char **argv) {
    int rank, nproc;
    char *a = nullptr; // stores all data
    char *arr = nullptr; // stores a share in every process
    //MPI_Offset n = bufsize;
    MPI_File fh;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    arr = new char[bufsize];
    // send a[] to all processes
    if(rank==0){
        a = new char[bufsize*nproc];
        printf("1. Sending an array among processes evenly.\n");
        // 0 to 0: direct copy
        printf("\tCopy from proc 0 to 0\n");
//        memcpy(arr, a, bufsize* sizeof(char));
        // 0 to 1...n: mpi send
        for(int i=1; i<nproc; i++) {
            printf("\tSending from proc 0 to %d\n", i);
            MPI_Send(a+bufsize*i, bufsize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else { // others receive
        printf("\tProc %d: Recieving from proc 0\n", rank);
        MPI_Recv(arr, bufsize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    }

    // assign values
    if(rank==0) {printf("2. Parallel assigning values for every sub-arrays.\n");}
    for (size_t i = 0; i < bufsize-1; i++) {
        arr[i] = (char) ('0' + rank);  // e.g. on processor 3 creates a=’3333333333....\n’
    }
    arr[bufsize-1] = '\n';

    // parallel io
    if(rank==0) {printf("3. Parallel IO, writing to data.out file.\n");}
    MPI_File_open(MPI_COMM_WORLD, "data.out", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_Offset displace = rank * bufsize * sizeof(char); // start of the view for each processor

    MPI_File_set_view(fh, displace, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
    // etype and filetype are the same
    MPI_File_write(fh, arr, bufsize, MPI_CHAR, &status);
    MPI_File_close(&fh);
    printf("\tWrote %.3f MB data from rank %d process\n", bufsize/1024.0/1024.0, rank);

    // all send back to process 0
    if(rank==0) {printf("4. Sending sub-arraies to proc 0 and check.\n");}
    if(rank != 0) { // slave processes
        printf("\tSending from process %d to 0\n", rank);
        MPI_Send(arr, bufsize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        delete []arr;
    } else { // main process 0
        // direct copy 0 to 0
        memcpy(a, arr, bufsize*sizeof(char));
        delete [] arr;
        for(int r=1; r<nproc; r++) {
            MPI_Recv(a+r*bufsize, bufsize, MPI_CHAR, r, 0, MPI_COMM_WORLD, &status);
            printf("\tReceiving %zu entries from process %d, with tag %d, error %d\n",
                    status._ucount, status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR);
        }
        printf("\tChecking on process %d...\n", rank);
        for(size_t idx=0; idx<bufsize*nproc; idx++) {
            if(a[idx] != '0' + idx/bufsize && (idx == bufsize-1 && a[idx]!='\n')) {
                printf("Array is not consistent with file!\n");
                return 1;
            }
        }
        printf("Conclusion: Array has same content with file!\n");
        delete []a;
    }

    MPI_Finalize();
    return 0;
}