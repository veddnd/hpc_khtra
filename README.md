!nvcc -arch=sm_75 -o device_props device_props.cu
!./device_props

gcc -fopenmp min_scalar_pro.c -o min_scalar_pro
./min_scalar_pro.exe

mpicc hello_world.c -o hello_world
mpirun -np 10 ./hello_world

mpicxx q1.cxx -o q1
mpirun -np 4 ./q1

nano hello.c
gcc hello.c -o hello
./hello
