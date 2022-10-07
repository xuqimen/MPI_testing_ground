#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include <unistd.h>

// this function does some meaningless work that consumes some time
double do_some_work() {
    sleep(23);
    return 0.0;
    // int rank;
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // double rand_val = 0.0, temp = 0.0;
    // for (int i = 0; i < 20e6; i++) {
    // // for (int i = 0; i < 2e1; i++) {
    //     srand(i+rank+1);
    //     temp = ((double) rand() / RAND_MAX);
    //     rand_val = (rand_val > temp) ? rand_val : temp;
    // }
    // return rand_val;
}


int main(int argc, char **argv) {
    MPI_Init(&argc,&argv);
    int rank, size;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // printf("Hello world from %d/%d\n",rank,size);
    double t1, t2, t3, t4, t_start, t_end;

    t_start = MPI_Wtime();
    
    // test allreduce
    size_t n = 50000000;
    // n = 100;
    double *x = malloc(n * sizeof(*x));
    assert(x != NULL);

    for (int i = 0; i < n; i++) x[i] = (double) rank;

    MPI_Request req;

    // warm up
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < 5; i++) {
        MPI_Iallreduce(MPI_IN_PLACE, x, n, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD, &req);
        MPI_Wait(&req, MPI_STATUS_IGNORE);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);



    // start the non-blocking operation
    MPI_Iallreduce(MPI_IN_PLACE, x, n, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD, &req);
    
    // do some work here while the communication is going on
    // sleep(2);
    t1 = MPI_Wtime();
    double rand_val = do_some_work();
    t2 = MPI_Wtime();
    if (rank == 0) printf("rand_val = %.3f, time taken: %.3f ms\n", rand_val, (t2-t1)*1e3);
    
    t1 = MPI_Wtime();
    MPI_Wait(&req, MPI_STATUS_IGNORE);
    t2 = MPI_Wtime();
    
    // do it again without doing any work in between 
    MPI_Barrier(MPI_COMM_WORLD);
    // sleep(10);
    t3 = MPI_Wtime();
    // MPI_Iallreduce(MPI_IN_PLACE, x, n, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD, &req);
    // MPI_Wait(&req, MPI_STATUS_IGNORE);
    MPI_Allreduce(MPI_IN_PLACE, x, n, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    t4 = MPI_Wtime();

  
    t_end = MPI_Wtime();

    printf("min = %.1f in rank %3d, wait time (overlap): %7.3f ms, actual reduce time (no overlap): %6.3f ms, "
            "total time: %.3f ms\n",
            x[n/2], rank, (t2-t1)*1e3, (t4-t3)*1e3, (t_end-t_start)*1e3);
    
    free(x);
    MPI_Finalize();
    return 0;
}
