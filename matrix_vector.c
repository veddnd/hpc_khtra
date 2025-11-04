#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size, n = 4;        
    int A[4][4], x[4], y[4];
    int local_A[4], local_y = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Matrix A:\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = i + j + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("Vector x:\n");
        for (int i = 0; i < n; i++) {
            x[i] = i + 1;
            printf("%d ", x[i]);
        }
        printf("\n");
    }

    MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(A, n, MPI_INT, local_A, n, MPI_INT, 0, MPI_COMM_WORLD);

    for (int j = 0; j < n; j++) {
        local_y += local_A[j] * x[j];
    }

    MPI_Gather(&local_y, 1, MPI_INT, y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Result vector y = A * x :\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", y[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
