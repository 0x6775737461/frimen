#include <stdio.h>
#include <stdlib.h>

//#define MAX_SIZE 99999999
#define MAX_SIZE 100
#define size_int sizeof(int)

#define valid_num \
    ((rand())%(MAX_SIZE))

int *alloc_matrix(int lines, int cols);
void fill_matrix(int *matrix, int lines, int cols);
void red_color(int *matrix, int lines, int cols);
int offset(int line, int col, int t_cols);

int main(void) {
    srand(12345);

    int lines, cols;

    lines = 5;
    cols = 5;

    // we don't use the stack (matrix[L][C])
    // because its size limit is ~= 8KiB
    int *matrix = alloc_matrix(lines, cols);

    fill_matrix(matrix, lines, cols);

    for(int i = 0; i < lines; i++) {
        for(int j = 0; j < cols; j++) {
            printf("(%3x) ", matrix + (i + j) + 1);
            printf("(%2d)", offset(i, j, cols));

            printf("[%8d] ", matrix[offset(i, j, cols)]);
            //if(j + 1 == cols)
                puts("");
        }
    }

    free(matrix);

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
            matrix[offset(i, j, cols)] = valid_num;
            printf("[%d]", matrix[offset(i, j, cols)]);
        }
    }
    puts("");
}

// treating the offsets because the matrix
// will be defined in a one-dimensional array
int offset(int line, int col, int t_cols) {
    int addr;

    if(line != 0) {
        addr = ((line * t_cols) - (t_cols - col));
        return addr + t_cols + 1;
    }

    return col + 1;
}
