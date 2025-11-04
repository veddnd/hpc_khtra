#include <stdio.h>
#include <omp.h>

#define N 20   // number of Fibonacci terms

int main() {
    int fib[N];
    fib[0] = 0;
    fib[1] = 1;

    // Parallel computation with synchronization
    #pragma omp parallel
    {
        for (int i = 2; i < N; i++) {
            #pragma omp critical   // ensures only one thread updates at a time
            {
                fib[i] = fib[i-1] + fib[i-2];
            }
        }
    }

    printf("Fibonacci Series up to %d terms:\n", N);
    for (int i = 0; i < N; i++) {
        printf("%d ", fib[i]);
    }
    printf("\n");

    return 0;
}
