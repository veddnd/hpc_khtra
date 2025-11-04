#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int count = 0; // number of items in buffer

int main() {
    int produced = 0, consumed = 0;

    #pragma omp parallel sections shared(produced, consumed, buffer, count)
    {
        // Producer
        #pragma omp section
        {
            for (int i = 1; i <= NUM_ITEMS; i++) {
                #pragma omp critical
                {
                    if (count < BUFFER_SIZE) {
                        buffer[count] = i;
                        count++;
                        produced++;
                        printf("Producer produced: %d | Buffer Count: %d\n", i, count);
                    }
                }
                #pragma omp flush
            }
        }

        // Consumer
        #pragma omp section
        {
            for (int i = 1; i <= NUM_ITEMS; i++) {
                int item;
                #pragma omp critical
                {
                    if (count > 0) {
                        item = buffer[count-1];
                        count--;
                        consumed++;
                        printf("Consumer consumed: %d | Buffer Count: %d\n", item, count);
                    }
                }
                #pragma omp flush
            }
        }
    }

    printf("\nTotal Produced: %d\n", produced);
    printf("Total Consumed: %d\n", consumed);

    return 0;
}

