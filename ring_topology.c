#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size, recv_data, send_data;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    send_data = rank;
    int left  = (rank - 1 + size) % size;
    int right = (rank + 1) % size;

    MPI_Sendrecv(&send_data, 1, MPI_INT, right, 0,
                 &recv_data, 1, MPI_INT, left,  0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("Process %d received data %d from process %d\n",
            rank, recv_data, left);

    MPI_Finalize();
    return 0;
}
