#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 99999999
#define size_int sizeof(int)

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

// distribute data addresses between threads
void data_sharing(int *matrix, int lines, int cols, int threads);

// get data about address separation, for example
// the total elements to each thread.
struct parse_data slice_matrix(int lines, int cols, int threads);

// getting element address
int offset(int line, int col, int t_cols);

int main(void) {
  srand(12345);

  int lines, cols, threads;

  lines = 71;
  cols = 13;
  threads = 9;

  // we don't use the stack (matrix[L][C])
  // because its size limit is ~= 8KiB
  int *matrix = alloc_matrix(lines, cols);

  fill_matrix(matrix, lines, cols);

  // seeing the address interval each thread took
  data_sharing(matrix, lines, cols, threads);

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

    for (int j = 0; j < cols; j++)
      matrix[offset(i, j, cols)] = valid_num;
  }
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

void data_sharing(int *matrix, int lines, int cols, int threads) {

  struct parse_data slice_data = slice_matrix(lines, cols, threads);

  int begin_addr = 1;
  int last_addr = 0;

  for(char i = 1; i <= slice_data.n_times; i++) {

    // if it is the last "slice" of the matrix
    if(slice_data.remainder &&
        i == slice_data.n_times) {
          last_addr += slice_data.addr;
          last_addr += slice_data.remainder;

    } else {
      last_addr += slice_data.addr;
    }

    printf("thread[%d] - [%5d - %5d]\n",
      i, begin_addr, last_addr);

    // the plus 1 is just to avoid the threads use
    // the same address
    begin_addr = last_addr + 1;
  }
}

struct parse_data slice_matrix(int lines, int cols, int threads) {
  // was said in the beginning of the code, this struct should be used
  // to enter addresses to "balance data" on threads
  struct parse_data slice_data;

  slice_data.remainder = 0;

  int t_elements = lines * cols;

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
