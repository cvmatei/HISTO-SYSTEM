/*
 * FILE: dataProducer-2.h
 * PROJECT: HISTO-SYSTEM
 * FIRST VERSION: 04/05/2023
 * PROGRAMMER(s): Cosmin Matei, Ahmed Ruda
 * DESCRIPTION: Header file for the dataProducer-2 program. It contains function declarations for handling 
 *              shared memory, semaphores, signals, and the struct for the circular buffer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>

#define PID_STRING 16
#define BUFFER_SIZE 256

typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} circular_buffer;

int shmid;
circular_buffer* buffer;
char getChar(int randomInt);
int init_semaphore(int *semID);
void detachAndExit(int sig);