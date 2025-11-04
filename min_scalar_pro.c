#include <stdio.h>
#include <omp.h>
#define SIZE 1000

int main() {
    int a[SIZE], b[SIZE], dot_product = 0;
    for (int i = 0; i < SIZE; i++) {
        a[i] = i;
        b[i] = SIZE - i;
    }

    #pragma omp parallel for reduction(+:dot_product)
    for (int i = 0; i < SIZE; i++)
        dot_product += a[i] * b[i];

    printf("Dot Product: %d\n", dot_product);
    return 0;
}
