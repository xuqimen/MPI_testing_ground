### Usage

Compile the code by `make clean; make`. A binary named `hello` will be created. To run the test, simply do
```bash
$ mpirun -np <nproc> ./hello
```
Estimated Memory Usage: 166 MB.

### Description of the test
This is a simple MPI test to test the non-blocking MPI operations. We perform a non-blocking collective operation. While the operation is going on in the background, we do some work (in this case, some random work that takes a lot of time, more than it would take for the collective opertion to be completed). Then we do `MPI_Wait` to make sure the non-blocking operation is completed. Ideally, the time spent at `MPI_Wait` should be close to 0 if overlapping the communication and computation is working as expected. We do the non-blocking operation again without doing any work in between as a reference.
