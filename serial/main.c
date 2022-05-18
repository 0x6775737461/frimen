#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 99999999

// this struct will be used in "data_sharing" and "slice_matrix"
// functions.
struct parse_data {
  int addr;
  char remainder;
  char n_times;
};

// at the end of each thread, must be added the total of
// local prime numbers in this global variable
int total_num_primes = 0;

// allocate matrix dynamically
int *alloc_matrix(int lines, int cols);

// fill the matrix with "random" data
void fill_matrix(int *matrix, int lines, int cols);

// distribute data addresses between threads
void data_sharing(int *matrix, int lines, int cols, int threads,
                  int *pair_addr);

// get data about address separation, for example
// the total elements to each thread.
struct parse_data slice_matrix(int lines, int cols, int threads);

// getting element address
int offset(int line, int col, int t_cols);

// finding the prime numbers
int find_pnum(int *matrix, int initial_addr, int last_addr);

int main(void) {
  clock_t initial_time = clock();

  srand(12345);

  int lines, cols, threads;

  lines = 50;
  cols = 50;
  threads = 1;

  // indexing the initial/final address of each "slice"
  // of data to give to the thread
  int *pair_addr = (int *)malloc(sizeof(int) * threads * 2);

  // we don't use the stack (matrix[L][C])
  // because its size limit is ~= 8KiB
  int *matrix = alloc_matrix(lines, cols);

  fill_matrix(matrix, lines, cols);

  // seeing the address interval each thread took
  // and assigning the pair address
  data_sharing(matrix, lines, cols, threads, pair_addr);

  int initial_addr = 0;
  int last_addr = 0;

  for (int i = 0; i < (threads * 2); i += 2) {

    initial_addr = pair_addr[i];
    last_addr = pair_addr[i + 1];

    total_num_primes += find_pnum(matrix, initial_addr, last_addr);
  }

  printf("Total of Prime Numbers: [%5d]\n", total_num_primes);

  initial_time = clock() - initial_time;

  printf("Time Elapsed: %.2f sec\n",
         ((double)initial_time) / CLOCKS_PER_SEC);

  free(matrix);
  free(pair_addr);

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
      matrix[offset(i, j, cols)] = rand() % MAX_SIZE;
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

void data_sharing(int *matrix, int lines, int cols, int threads,
                  int *pair_addr) {

  struct parse_data slice_data = slice_matrix(lines, cols, threads);

  int initial_addr = 1;
  int final_addr = 0;

  // just to set the correct address in *pair_addr
  int j = 0;

  for (char i = 1; i <= slice_data.n_times; i++) {

    // if it is the last "slice" of the matrix
    if (slice_data.remainder && i == slice_data.n_times) {
      final_addr += slice_data.addr;
      final_addr += slice_data.remainder;

    } else {
      final_addr += slice_data.addr;
    }

    // the attribution must be something like:
    // (0..1) (2..3) (4..5) ...
    pair_addr[j] = initial_addr;
    j++;
    pair_addr[j] = final_addr;

    j++;

    // the plus 1 is just to avoid the threads use
    // the same address
    initial_addr = final_addr + 1;
  }
}

struct parse_data slice_matrix(int lines, int cols, int threads) {
  // was said in the beginning of the code,
  // this struct should be used
  // to enter addresses to "balance data" on threads
  struct parse_data slice_data;

  slice_data.remainder = 0;

  int t_elements = lines * cols;

  if (t_elements % threads) {
    // this values will be inserted on the last thread
    slice_data.remainder = t_elements - (floor(t_elements / threads) * threads);
  }

  t_elements -= slice_data.remainder;

  // the number that is the right multiplier
  slice_data.addr = t_elements / threads;

  // getting the times that slice_data.addr must be multiplied
  slice_data.n_times = t_elements / slice_data.addr;

  return slice_data;
}

int find_pnum(int *matrix, int initial_addr, int last_addr) {
  // prime numbers found
  int pnum_found = 0;

  int actual_addr = initial_addr;

  for (int i = actual_addr; i <= last_addr; i++) {
    // number of divisors
    int n_div = 0;

    // TODO: change this to sound like a matrix
    for (int j = 1; j <= matrix[actual_addr]; j += 2) {

      if (matrix[actual_addr] % 2 == 0 || n_div > 2)
        break;

      if (matrix[actual_addr] % j == 0)
        n_div++;
    }

    if (n_div == 2) {
      printf("(%10d) [%8d]\n", actual_addr, matrix[actual_addr]);
      pnum_found++;
    }

    actual_addr++;
  }

  return pnum_found;
}
