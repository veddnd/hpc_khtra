#include <stdio.h>
#include <omp.h>

int main() {
    long long num_steps;
    printf("Enter number of steps (higher = better accuracy): ");
    scanf("%lld", &num_steps);

    double step = 1.0 / (double) num_steps;
    double pi = 0.0;

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        double x, sum = 0.0;
        #pragma omp for
        for (long long i = 0; i < num_steps; i++) {
            x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x*x);
        }

        #pragma omp atomic
        pi += sum;
    }

    pi *= step;
    double end_time = omp_get_wtime();

    printf("Calculated Pi = %.15f\n", pi);
    printf("Execution time: %lf seconds\n", end_time - start_time);

    return 0;
}
