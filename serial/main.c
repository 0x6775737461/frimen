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

// this struct will be used in "colors" and "slice_matrix"
// functions.
struct parse_data {
  int addr;
  char remainder;
  char n_times;
};

// alloc the matrix dynamically
int *alloc_matrix(int lines, int cols);

// fill the matrix with "random" data
void fill_matrix(int *matrix, int lines, int cols);

// showing the thread separation with colors
// in a ficcional matrix
void colors(int *matrix, int lines, int cols, int threads);

// get data about address separation, for example
// the total elements to each thread.
struct parse_data slice_matrix(int lines, int cols, int threads);

// getting element address
int offset(int line, int col, int t_cols);

int main(void) {
  srand(12345);

  int lines, cols, threads;

  lines = 15;
  cols = 20;
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

  struct parse_data slice_data = slice_matrix(lines, cols, threads);

  // color id
  int cid = 0;

  // number of color change
  char n_color_chg = 1;

  for (int i = 0; i < lines; i++) {
    for (int j = 0; j < cols; j++) {

      if (n_color_chg <= slice_data.n_times &&
          (offset(i, j, cols) == (slice_data.addr * n_color_chg) + 1)) {

        cid++;
        n_color_chg++;
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

struct parse_data slice_matrix(int lines, int cols, int threads) {
  // was said in the beginning of the code, this struct should be used
  // to enter addresses to "balance data" on threads
  struct parse_data slice_data;

  slice_data.remainder = 0;

  int t_elements = lines * cols;
  printf("t_elements: [%5d]\n", t_elements);

  if (t_elements % threads) {
    // this values will be inserted on the last thread
    slice_data.remainder = t_elements - (floor(t_elements / threads) * threads);
    printf("slice_data.remainder: [%5d]", slice_data.remainder);
  }

  t_elements -= slice_data.remainder;
  printf("t_elements: [%5d]\n", t_elements);

  // the number that is the right multiplier
  slice_data.addr = t_elements / threads;
  printf("slice_data.addr: [%5d]\n", slice_data.addr);

  // getting the times that slice_data.addr must be multiplied
  slice_data.n_times = t_elements / slice_data.addr;
  printf("slice_data.n_times: [%5d]\n", slice_data.n_times);

  return slice_data;
}
