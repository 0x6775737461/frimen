#include <stdio.h>
#include <stdlib.h>

#define size_int sizeof(int)

// treating the offsets because the matrix
// will be defined in a one-dimensional array
#define OFFSET(lines,cols) \
    ((size_int)*(lines)+(cols))

int *alloc_matrix(int lines, int cols);

int main(void) {
   
    int lines, cols;

    lines = 10000;
    cols = 10000;

    // we don't use the stack (matrix[L][C])
    // because its size limit is ~= 8KiB
    int *matrix = alloc_matrix(lines, cols);
    
    matrix[OFFSET(5789, 4567)] = 99999997;
    matrix[OFFSET(8976, 675)] = 257;

    printf("%d\n", matrix[OFFSET(5789, 4567)]);
    printf("%d\n", matrix[OFFSET(8976, 675)]);

    return 0;
}

int *alloc_matrix(int lines, int cols) {
     // in this arch (amd64), 1 int holds 4 bytes of data
     // 4 bytes == 32 bits, but we need just 27 bits
     // 2**(27) == 134.217.728 > 99.999.999
    
    return malloc(lines * cols * sizeof(int));
}
