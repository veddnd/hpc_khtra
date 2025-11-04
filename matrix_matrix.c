#include <stdio.h>
#include <mpi.h>

#define N 4

int main(int argc, char* argv[]) {
    int rank, size;
    int A[N][N], B[N][N], C[N][N];
    int local_A[N], local_C[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Matrix A:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("Matrix B:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = (i == j) ? 1 : 0; 
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(A, N, MPI_INT, local_A, N, MPI_INT, 0, MPI_COMM_WORLD);

    for (int j = 0; j < N; j++) {
        local_C[j] = 0;
        for (int k = 0; k < N; k++) {
            local_C[j] += local_A[k] * B[k][j];
        }
    }

    MPI_Gather(local_C, N, MPI_INT, C, N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Result Matrix C = A * B:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
