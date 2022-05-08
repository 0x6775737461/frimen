#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 99999999
#define size_int sizeof(int)

#define valid_num \
    ((rand())%(MAX_SIZE))

// treating the offsets because the matrix
// will be defined in a one-dimensional array
#define OFFSET(lines,cols) \
    ((size_int)*(lines)+(cols))

int *alloc_matrix(int lines, int cols);

void fill_matrix(int *matrix, int lines, int cols);

int main(void) {
    srand(12345);

    int lines, cols;

    lines = 10000;
    cols = 10000;

    // we don't use the stack (matrix[L][C])
    // because its size limit is ~= 8KiB
    int *matrix = alloc_matrix(lines, cols);

    fill_matrix(matrix, lines, cols);

    for(int i = 0; i < lines; i++) {
        for(int j = 0; j < cols; j++) {
            printf("[%8d] ", matrix[OFFSET(i, j)]);
            if(j + 1 == cols)
                puts("");
        }
    }

    return 0;
}

int *alloc_matrix(int lines, int cols) {
    // in this arch (amd64), 1 int holds 4 bytes of data
    // 4 bytes == 32 bits, but we need just 27 bits
    // 2**(27) == 134.217.728 > 99.999.999
    int *matrix = malloc(lines * cols * sizeof(int));

    if(!matrix) {
        fprintf(stderr, "Something is wrong!");
        exit(1);
    }

    return matrix;
}

void fill_matrix(int *matrix, int lines, int cols) {
    for(int i = 0; i < lines; i++) {
        for(int j = 0; j < cols; j++) {
            matrix[OFFSET(i, j)] = valid_num;
        }
    }
}
