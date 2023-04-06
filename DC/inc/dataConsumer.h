#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define NUM_LETTERS 20

int shmid;
int semid;
circular_buffer *shared_buffer;
int *letter_counts;
int *dp1_pid;
int *dp2_pid;

typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} circular_buffer;

void handle_sigint(int sig);
void display_histogram(int *letter_counts);