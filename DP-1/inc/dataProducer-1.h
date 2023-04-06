#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define BUFFER_SIZE 256

typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} circular_buffer;

int sMemID;
circular_buffer* buffer;
char getChar(int randomInt);
int init_semaphore(int *semID);
void detachAndExit(int sig);