#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = 8;
    int A[8] = {1,2,3,4,5,6,7,8};
    int half = n / 2;
    int partial_sum = 0, total_sum = 0;

    if (rank == 0)
        for (int i = 0; i < half; i++) partial_sum += A[i];
    else
        for (int i = half; i < n; i++) partial_sum += A[i];

    MPI_Reduce(&partial_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("Total Sum = %d\n", total_sum);

    MPI_Finalize();
    return 0;
}
