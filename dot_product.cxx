#include <mpi.h>
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 10000000;
    vector<double> A, B;

    vector<int> sendcounts;
    vector<int> displs;
    int local_N;

    if (rank == 0) {
        A.resize(N);
        B.resize(N);
        srand(42);
        for (int i = 0; i < N; i++) {
            A[i] = static_cast<double>(rand()) / RAND_MAX;
            B[i] = static_cast<double>(rand()) / RAND_MAX;
        }

        int base_count = N / size;
        int remainder = N % size;

        sendcounts.resize(size);
        displs.resize(size);
        int current_disp = 0;
        
        for (int r = 0; r < size; ++r) {
            sendcounts[r] = base_count + (r < remainder ? 1 : 0);
            displs[r] = current_disp;
            current_disp += sendcounts[r];
        }
    }

    MPI_Scatter(rank == 0 ? sendcounts.data() : nullptr, 1, MPI_INT, 
                &local_N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<double> local_A(local_N), local_B(local_N);

    MPI_Scatterv(A.data(), rank == 0 ? sendcounts.data() : nullptr, rank == 0 ? displs.data() : nullptr, MPI_DOUBLE,
                 local_A.data(), local_N, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);
                 
    MPI_Scatterv(B.data(), rank == 0 ? sendcounts.data() : nullptr, rank == 0 ? displs.data() : nullptr, MPI_DOUBLE,
                 local_B.data(), local_N, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);

    double local_dot = 0.0;

    double start = MPI_Wtime();
    for (int i = 0; i < local_N; i++)
        local_dot += local_A[i] * local_B[i];
    double end = MPI_Wtime();

    double global_dot = 0.0;
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << fixed << setprecision(6);
        cout << "Processes: " << size << endl;
        cout << "Dot Product = " << global_dot << endl;
        cout << "Time = " << end - start << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}