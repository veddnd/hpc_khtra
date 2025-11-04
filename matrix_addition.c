#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void add(int size) {
    int **A = malloc(size * sizeof(int*));
    int **B = malloc(size * sizeof(int*));
    int **C = malloc(size * sizeof(int*));

    for (int i = 0; i < size; i++) {
        A[i] = malloc(size * sizeof(int));
        B[i] = malloc(size * sizeof(int));
        C[i] = malloc(size * sizeof(int));
        for (int j = 0; j < size; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }
    }

    double start = omp_get_wtime();

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            C[i][j] = A[i][j] + B[i][j];

    double end = omp_get_wtime();
    printf("Size %d Time: %f\n", size, end - start);

    for (int i = 0; i < size; i++) {
        free(A[i]); free(B[i]); free(C[i]);
    }
    free(A); free(B); free(C);
}

int main() {
    omp_set_num_threads(4);
    int sizes[] = {250, 500, 750, 1000, 2000};
    for (int i = 0; i < 5; i++) add(sizes[i]);
    return 0;
}
