#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define MAX_SIZE 99999999
#define MAX_SIZE 100
#define size_int sizeof(int)

#define RED "\e[0;31m"
#define GRE "\e[0;32m"
#define YLW "\e[0;33m"
#define BLU "\e[0;34m"
#define PRP "\e[0;35m"
#define CLR "\e[0m"

#define valid_num ((rand()) % (MAX_SIZE))

int *alloc_matrix(int lines, int cols);
void fill_matrix(int *matrix, int lines, int cols);
void colors(int *matrix, int lines, int cols, int threads);
int slice_matrix(int lines, int cols, int threads);
int offset(int line, int col, int t_cols);

int main(void) {
  srand(12345);

  int lines, cols, threads;

  lines = 10;
  cols = 10;
  threads = 5;

  // we don't use the stack (matrix[L][C])
  // because its size limit is ~= 8KiB
  int *matrix = alloc_matrix(lines, cols);

  fill_matrix(matrix, lines, cols);

  // just to view the slice in the matrix
  colors(matrix, lines, cols, threads);

  free(matrix);

  return 0;
}

int *alloc_matrix(int lines, int cols) {
  // in this arch (amd64), 1 int holds 4 bytes of data
  // 4 bytes == 32 bits, but we need just 27 bits
  // 2**(27) == 134.217.728 > 99.999.999

  int *matrix = malloc(lines * cols * sizeof(int));

  if (!matrix) {
    fprintf(stderr, "Something is wrong!");
    exit(1);
  }

  return matrix;
}

void fill_matrix(int *matrix, int lines, int cols) {
  for (int i = 0; i < lines; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[offset(i, j, cols)] = valid_num;
      // printf("[%d]", matrix[offset(i, j, cols)]);
    }
  }
  puts("");
}

// treating the offsets because the matrix
// will be defined in a one-dimensional array
int offset(int line, int col, int t_cols) {
  int addr;

  if (line != 0) {
    addr = ((line * t_cols) - (t_cols - col));
    return addr + t_cols + 1;
  }

  return col + 1;
}

void colors(int *matrix, int lines, int cols, int threads) {
  int addr = slice_matrix(lines, cols, threads);
  int initial_addr = addr;

  // color id
  int cid = 0;

  for (int i = 0; i < lines; i++) {
    for (int j = 0; j < cols; j++) {
      if (offset(i, j, cols) == addr + 1) {
        addr += initial_addr;
        cid++;
      }

      switch (cid) {
      case 0:
        printf(RED "[%3d]" CLR, offset(i, j, cols));
        break;

      case 1:
        printf(GRE "[%3d]" CLR, offset(i, j, cols));
        break;

      case 2:
        printf(YLW "[%3d]" CLR, offset(i, j, cols));
        break;

      case 3:
        printf(BLU "[%3d]" CLR, offset(i, j, cols));
        break;

      case 4:
        printf(PRP "[%3d]" CLR, offset(i, j, cols));
        break;

      default:
        printf("[%3d]" CLR, offset(i, j, cols));
      }

      if (j + 1 == cols)
        puts("");
    }
  }
}

int slice_matrix(int lines, int cols, int threads) {
  int t_elements = lines * cols;
  int remainder = 0;

  if (t_elements % threads) {
    // this values will be inserted on the last thread
    remainder = t_elements - round(t_elements / threads);

    return remainder;
  }

  // returning the number that is the right multiplier
  return t_elements / threads;
}
