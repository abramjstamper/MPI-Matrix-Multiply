CC = gcc-6
MPI = mpicc
numProcessors = 1
CFLAGS = -I /usr/local/include -L /usr/local/lib -Wall
matrixSeq: matrix-seq.c
	$(CC) $(CFLAGS) -o $@ $^
matrix: matrix.c
	$(MPI) $(CFLAGS) -o $@ $^
mpi-blocking: mpi-blocking.c
	$(MPI) $(CFLAGS) -o $@ $^
mpi-sendrec: mpi-sendrec.c
	$(MPI) $(CFLAGS) -o $@ $^
run: matrix
	mpirun -np $(numProcessors) ./$^
clean:
	rm -f matrixSeq matrixMPI mpi-blocking mpi-sendRec
