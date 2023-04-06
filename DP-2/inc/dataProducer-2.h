#include <stdio.h>
#include <stdlib.h>

#define PID_STRING 16;

#define BUFFER_SIZE 256
typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} circular_buffer;

char getChar(int randomInt);