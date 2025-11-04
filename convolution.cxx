#include <mpi.h>
#include <bits/stdc++.h>

using namespace std;
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = 1024;
    const int kernel_size = 3;
    const int pad = kernel_size / 2;

    float* kernel = new float[kernel_size * kernel_size];
    if (rank == 0) {
        for (int i = 0; i < kernel_size * kernel_size; ++i)
            kernel[i] = 1.0f / (kernel_size * kernel_size);
    }
    MPI_Bcast(kernel, kernel_size * kernel_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float* image = nullptr;
    if (rank == 0) {
        image = new float[n * n];
        srand(42);
        for (int i = 0; i < n * n; ++i)
            image[i] = static_cast<float>(rand()) / RAND_MAX;
    }

    vector<int> sendcounts(size, 0), displs(size, 0);
    int base_rows = n / size;
    int remainder = n % size;
    for (int r = 0; r < size; ++r) {
        int rows = base_rows + (r < remainder ? 1 : 0);
        sendcounts[r] = rows * n;
    }
    displs[0] = 0;
    for (int r = 1; r < size; ++r)
        displs[r] = displs[r - 1] + sendcounts[r - 1];

    int local_elements = sendcounts[rank];
    int local_n = local_elements / n;

    float* local_image = new float[local_elements];
    MPI_Scatterv(image, sendcounts.data(), displs.data(), MPI_FLOAT,
                 local_image, local_elements, MPI_FLOAT, 0, MPI_COMM_WORLD);

    int buffer_rows = local_n + 2 * pad;
    float* local_buffer = new float[buffer_rows * n];
    memset(local_buffer, 0, buffer_rows * n * sizeof(float));

    for (int r = 0; r < local_n; ++r)
        memcpy(&local_buffer[(r + pad) * n], &local_image[r * n], n * sizeof(float));

    MPI_Request reqs[4];
    int req_count = 0;
    int top_rank = (rank > 0) ? rank - 1 : MPI_PROC_NULL;
    int bottom_rank = (rank < size - 1) ? rank + 1 : MPI_PROC_NULL;

    if (top_rank != MPI_PROC_NULL)
        MPI_Irecv(&local_buffer[0 * n], n, MPI_FLOAT, top_rank, 100, MPI_COMM_WORLD, &reqs[req_count++]);
    if (bottom_rank != MPI_PROC_NULL)
        MPI_Irecv(&local_buffer[(local_n + pad) * n], n, MPI_FLOAT, bottom_rank, 101, MPI_COMM_WORLD, &reqs[req_count++]);
    if (top_rank != MPI_PROC_NULL)
        MPI_Isend(&local_buffer[pad * n], n, MPI_FLOAT, top_rank, 101, MPI_COMM_WORLD, &reqs[req_count++]);
    if (bottom_rank != MPI_PROC_NULL)
        MPI_Isend(&local_buffer[(pad + local_n - 1) * n], n, MPI_FLOAT, bottom_rank, 100, MPI_COMM_WORLD, &reqs[req_count++]);
    if (req_count > 0)
        MPI_Waitall(req_count, reqs, MPI_STATUSES_IGNORE);

    double start = MPI_Wtime();

    float* result = new float[local_elements];
    for (int i_local = 0; i_local < local_n; ++i_local) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;
            for (int dx = 0; dx < kernel_size; ++dx) {
                for (int dy = 0; dy < kernel_size; ++dy) {
                    int buf_row = i_local + dx;
                    int col = j + dy - pad;
                    if (buf_row >= 0 && buf_row < buffer_rows && col >= 0 && col < n)
                        sum += kernel[dx * kernel_size + dy] * local_buffer[buf_row * n + col];
                }
            }
            result[i_local * n + j] = sum;
        }
    }

    double end = MPI_Wtime();

    float* global_result = nullptr;
    if (rank == 0)
        global_result = new float[n * n];
    MPI_Gatherv(result, local_elements, MPI_FLOAT,
                global_result, sendcounts.data(), displs.data(), MPI_FLOAT,
                0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("P=%d, n=%d, time=%.6f seconds\n", size, n, end - start);

    delete[] result;
    delete[] local_buffer;
    delete[] local_image;
    delete[] kernel;
    if (rank == 0) {
        delete[] image;
        delete[] global_result;
    }

    MPI_Finalize();
    return 0;
}
