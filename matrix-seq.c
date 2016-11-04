#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAT_ELT(mat, cols, i, j) *(mat + (i * cols) + j)

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

void mat_mult(int *c, int *a, int *b, int m, int n, int p) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < p; j++) {
      for (int k = 0; k < n; k++) {
        MAT_ELT(c, p, i, j) += MAT_ELT(a, n, i, k) * MAT_ELT(b, p, k, j);
      }
    }
  }
}

int main(int argc, char **argv) {

  //init variables for file I/O
  FILE *fp;
  char buffer[5];
  int row = 16;
  int col = 16;
  int A[16][16];
  int B[16][16];

  //Read inputs for Matrix A
  fp = fopen("a.txt", "r");
  for(int i = 0; i < row; i++) {
    for(int j = 0; j < col; j++){
      fscanf(fp, "%s", buffer);
      A[i][j] = atoi(&buffer[0]);
    }
  }
  fclose(fp);

  //Read inputs for Matrix B
  fp = fopen("b.txt", "r");
  for(int i = 0; i < row; i++) {
    for(int j = 0; j < col; j++){
      fscanf(fp, "%s", buffer);
      B[i][j] = atoi(&buffer[0]);
    }
  }
  fclose(fp);

  //init Matrix C
  int C[16][16];
  memset(&C, 0, sizeof(C));

  //Perform calculations
  mat_mult((int *)C, (int *)A, (int *)B, 16, 16, 16);

  //Print output
  mat_print("A", (int *)A, 16, 16);
  mat_print("B", (int *)B, 16, 16);
  mat_print("C", (int *)C, 16, 16);
}