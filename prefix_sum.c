#include <stdio.h>
#include <omp.h>

#define N 8

int main() {
    int A[N], prefix[N];

    for (int i = 0; i < N; i++) {
        A[i] = i + 1;
    }

    prefix[0] = A[0];


    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 1; i < N; i++) {
            prefix[i] = prefix[i-1] + A[i];
        }
    }

    printf("Prefix Sum Array:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", prefix[i]);
    }
    printf("\n");

    return 0;
}
