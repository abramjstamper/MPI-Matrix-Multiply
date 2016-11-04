#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "mpi.h"

//value stored at the location
#define MAT_ELT(matrix, numCols, row, col) *(matrix + (row * numCols) + col)

//address of the location
#define MAT_LOC(matrix, numCols, row, col) (matrix + (row * numCols) + col)

#define convert2D(numCols, row, col) (numCols * row + col)

#define rankForIO 0

double now(void) {
  struct timeval time_val;
  gettimeofday(&time_val, 0);
  double now = (double)time_val.tv_sec
               + (double)time_val.tv_usec / 1000000.0;
  return now;
}

void mat_print(char *msge, int *a, int m, int n) {
  printf("\n== %s ==\n%7s", msge, "");
  for (int j = 0; j < n; j++) {
    printf("%6d|", j);
  }
  printf("\n");

  for (int i = 0; i < m; i++) {
    printf("%5d|", i);
    for (int j = 0; j < n; j++) {
      printf("%7d", MAT_ELT(a, n, i, j));
    }
    printf("\n");
  }
}

void mat_print_rank(int rank, int *a, int m, int n) {
  printf("\n== %i ==\n%7s", rank, "");
  for (int j = 0; j < n; j++) {
    printf("%6d|", j);
  }
  printf("\n");

  for (int i = 0; i < m; i++) {
    printf("%5d|", i);
    for (int j = 0; j < n; j++) {
      printf("%7d", MAT_ELT(a, n, i, j));
    }
    printf("\n");
  }
}

void mat_mult(int *c, int *a, int *b, int aRows, int rowsBColsA, int colsB, int startCol) {
  for (int i = 0; i < aRows; i++) {
    for (int j = 0; j < colsB; j++) {
      for (int k = 0; k < rowsBColsA; k++) {
        MAT_ELT(c, colsB, i, j + startCol) += MAT_ELT(a, rowsBColsA, i, k) * MAT_ELT(b, colsB, k, j);
      }
    }
  }
}

int main(int argc, char **argv) {

  double tStart = now();
  //init MPI
  int num_procs;
  int rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("%d: hello (p=%d)\n", rank, num_procs);

  int subRowA = 1;
  int subMiddleNums = 4;
  int subColB = 1;
  int row = 16;
  int col = 16;
  int ARowsPerProc = row / num_procs;
  int BColsPerProc = col / num_procs;
  int subA[col * subRowA];
  int subB[row * subColB];
  int C[row * col];
  memset(C, 0, sizeof(C));

  //Send chunks out to each processor
  if (rank == rankForIO) {
    //init variables for file I/O
    FILE *fp;
    char buffer[5];
    int A[row * col];
    int B[num_procs][row * subColB];

    //Read inputs for Matrix A
    fp = fopen("a.txt", "r");
    printf("STATUS: Reading A in from IO\n");
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
        fscanf(fp, "%s", buffer);
        A[convert2D(col, i, j)] = atoi(&buffer[0]);
      }
    }
    fclose(fp);
    printf("STATUS: Matrix A read in from file\n");

    //Read inputs for Matrix B
    fp = fopen("b.txt", "r");
    printf("STATUS: Reading B in from IO\n");
    int int_buffer;
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < num_procs; j++) {
        for (int k = 0; k < subColB; k++) {
          fscanf(fp, "%d", &int_buffer);
          B[j][(i * subColB) + k] = int_buffer;
        }
      }
    }
    fclose(fp);
    printf("STATUS: Matrix B read in from file\n");

    printf("STATUS: Sending initial values to processors\n");
    for (int i = 1; i < num_procs; i++) {
      MPI_Send((void *) &A[convert2D(col, i * subColB, 0)], col * subRowA, MPI_INT, i, 0, MPI_COMM_WORLD);
      printf("MPI: %i sent Matrix A %i\n", i, A[convert2D(col, i, 0)]);

      MPI_Send((void *) &B[i][0], subColB * row, MPI_INT, i, 0, MPI_COMM_WORLD);
      printf("MPI: %i sent Matrix B %i\n", i, A[convert2D(col, i, 0)]);
    }
    memcpy(&subA, &A, sizeof(subRowA) * col * subRowA);
    printf("MPI: %i sent %i\n", rank, A[convert2D(col, 0, 0)]);
    memcpy(&subB, &B, sizeof(subRowA) * row * subColB);
    printf("MPI: %i sent %i\n", rank, A[convert2D(col, 0, 0)]);
    printf("STATUS: Finished sending initial values to processors\n");
  } else {
    MPI_Status status;
    MPI_Recv(&subA, col * subRowA, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    printf("MPI: %i received Matrix A %i\n", rank, subA[0]);

    MPI_Recv(&subB, row * subColB, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    printf("MPI: %i received Matrix B %i\n", rank, subA[0]);
    printf("STATUS: Finished receiving initial values to processor %i\n", rank);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  //Matrix Multiplication Step
  //init Matrix C
  int subC[col * subRowA];
  memset(subC, 0, sizeof(subC));

  //Multiply
  //mat_mult((int *)C, (int *)subA, (int *)subB, 4, subColB, 4);

//  mat_print("A", (int *) subA, subRowA, col);
  MPI_Barrier(MPI_COMM_WORLD);
//  mat_print("B", (int *) subB, row, subColB);
  int rank_next = rank + 1;
  int rank_prev = rank - 1;

  if (rank_prev < 0) {
    rank_prev = num_procs - 1;
  }
  if (rank_next >= num_procs) {
    rank_next = 0;
  }
  int c_offset = rank * BColsPerProc;
  //perform multiply
  for (int p = 0; p < num_procs; p++) {
    printf("STATUS: multiplying slice B - %i\n", p);


    mat_mult((int *) subC, (int *) subA, (int *) subB, subRowA, subMiddleNums, subColB, c_offset);
    //rotate B
    MPI_Status status;
    //printf("%i sending subB to %i\n", rank, rank_next);
    MPI_Sendrecv(&subB, row * subColB, MPI_INT, rank_prev, 0, &subB, row * subColB, MPI_INT, rank_next, 0,
                 MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);
    c_offset = (1 + c_offset) * BColsPerProc;
    if(c_offset % col == 0){
      c_offset = 0;
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);

  //gather subC from all procs
  MPI_Request request;
  if(rank == rankForIO){
    //gather rankforIO's elements from subC
    for(int a = 0; a < col * subRowA; a++){
        C[a] = subC[a];
    }
    //Gather rest of of subC from other procs
    for(int i = 1; i < num_procs; i++){
      MPI_Irecv((void *) &C[convert2D(col, i * subColB, 0)], subRowA * col, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }
  } else {
    MPI_Isend((void *) &subC, subRowA * col, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == rankForIO) {
    mat_print("C", (int *) C, row, col);

    FILE *fp;
    char buffer[5];
    fp = fopen("c.txt", "w");
    printf("STATUS: Writing C to IO\n");

    for(int i = 0; i < row; i++){
      for(int j = 0; j < col; j++){
        fprintf(fp, "%i ", C[convert2D(col, i, j)]);
      }
      fprintf(fp, "\n");
    }

    fclose (fp);
    printf("STATUS: Matrix C written to file\n");

    double tElapsed = now() - tStart;
    printf("STATUS: Elapsed Time - %f\n", tElapsed);
  }
  MPI_Finalize();
}