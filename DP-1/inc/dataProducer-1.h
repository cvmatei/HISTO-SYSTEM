#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>S
#include <errno.h>

#define BUFFER_SIZE 256
int sMemID;
circular_buffer* buffer;

typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} circular_buffer;

char getChar(int randomInt);