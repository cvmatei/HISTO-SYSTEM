#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} circular_buffer;

char getChar(int randomInt);