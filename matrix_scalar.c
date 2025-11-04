#include <stdio.h>
#include <omp.h>

#define N 3
#define SCALAR 5

int main() {
    int A[N][N], B[N][N];

    // Initialize matrix A
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            A[i][j] = i + j;

    // Parallel scalar multiplication
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B[i][j] = A[i][j] * SCALAR;
        }
    }

    // Print result
    printf("Resultant Matrix (B = A * %d):\n", SCALAR);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }

    return 0;
}
