#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu_rdma_comm.h"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Test: Multi-Device - Rank %d\n", rank);
    MPI_Finalize();
    return 0;
}
