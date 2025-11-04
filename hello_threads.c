#include <stdio.h>
#include <omp.h>

int main() {
    int threads;
    printf("Enter number of threads: ");
    scanf("%d", &threads);

    printf("\nSequential Hello World:\n");
    for(int i = 0; i < threads; i++) {
        printf("Hello, World from iteration %d\n", i);
    }

    printf("\nParallel Hello World using OpenMP:\n");
    omp_set_num_threads(threads);

    #pragma omp parallel
    {
        printf("Hello, World from thread %d\n", omp_get_thread_num());
    }

    return 0;
}
