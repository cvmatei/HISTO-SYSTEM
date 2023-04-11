/*
 * FILE: dataConsumer.h
 * PROJECT: HISTO-SYSTEM
 * FIRST VERSION: 04/05/2023
 * PROGRAMMER(s): Cosmin Matei, Ahmed Ruda
 * DESCRIPTION: Header file for the dataConsumer program. It contains function declarations for handling 
 *              shared memory, semaphores, signals, and displaying histograms of the letter counts.
 */

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
int *letterCounts;
int *dp1PID;
int *dp2PID;

typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} circular_buffer;

void handleSigInt(int sig);
void displayHistogram(int *letter_counts);
int initSemaphore(int *semID);
void readBuffer(int sig);