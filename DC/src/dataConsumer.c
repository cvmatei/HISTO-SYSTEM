/*
* FILE: dataConsumer.c
* PROJECT: HISTO-SYSTEM
* FIRST VERSION: 04/05/2023
* PROGRAMMER(s): Cosmin Matei, Ahmed Ruda
* DESCRIPTION: This file contains the main function for the dataConsumer utility. It reads command line arguments, 
*              sets up shared memory and semaphore, handles signals, and displays the histogram of letter counts.
*/

#include "../inc/dataConsumer.h"

circular_buffer *sharedBuffer;

int main(int argc, char *argv[]) {
    // Read command line arguments
    int sharedMemoryID = atoi(argv[1]);
    dp1PID = malloc(sizeof(int));
    dp2PID = malloc(sizeof(int));
    *dp1PID = atoi(argv[2]);
    *dp2PID = atoi(argv[3]);

    // Getting shared memory key
    key_t shmKey = ftok("../../DP-1/bin", 16535);
    if (shmKey == -1) {
        perror("DC ftok");
        exit(1);
    }

    // Getting shared memory id
    shmid = shmget(shmKey, sizeof(circular_buffer), 0660);
    if (shmid == -1) {
        if (errno == EEXIST) {
            // shared memory exists
        } 
        else {
            perror("DC shmget");
            exit(1);
        }
    }

    // Attaching shared memory
    sharedBuffer = (circular_buffer*)shmat(sharedMemoryID, NULL, 0);
    if (sharedBuffer == (void *)-1) {
        perror("DC shmat");
        exit(1);
    }
    
    // Initializing semaphore
    if(initSemaphore(&semid) == 1) {
        printf("Error: Semaphore creation failed\n");
    }

    // Initialize letter count array
    letterCounts = calloc(NUM_LETTERS, sizeof(int));
    if (letterCounts == NULL) {
        perror("DC calloc");
        exit(1);
    }

    // Set up SIGINT handler
    signal(SIGINT, handleSigInt);

    // Set up SIGALRM handler
    signal(SIGALRM, readBuffer);

    // Main processing loop
    int num_reads = 0;
    while (1) {
        // Set alarm for 2 seconds
        alarm(2);

        // Wait for alarm signal
        pause();
        num_reads++;
        // Check if we need to display histogram
        if (num_reads > 4) {
            displayHistogram(letterCounts);
            num_reads = 0;
        }
    }

    return 0;
}

// FUNCTION: 	void handleSigInt(int sig)
// DESCRIPTION: This function is a signal handler for SIGINT signal. It sends SIGINT signal to data producers, reads all values from the circular buffer,
//              displays the histogram, detaches from the shared memory segment, removes the semaphore, removes the shared memory segment, and exits the program.
// PARAMETERS:  int sig - The signal received (SIGINT).
// RETURNS:     None.

void handleSigInt(int sig) {
    // Send SIGINT to data producers
    if (kill(*dp1PID, SIGINT) == -1) {
        perror("DC kill dp1");
    }
    if (kill(*dp2PID, SIGINT) == -1) {
        perror("DC kill dp2");
    }

    // Read all values from buffer
    while (sharedBuffer->read_index != sharedBuffer->write_index) {
        char letter = sharedBuffer->buffer[sharedBuffer->read_index];
        letterCounts[letter - 'A']++;
        // Update the read index
        sharedBuffer->read_index = (sharedBuffer->read_index + 1) % BUFFER_SIZE;
    }
    displayHistogram(letterCounts);

    // Detach from shared memory segment
    if (shmdt(sharedBuffer) == -1) {
        perror("DC shmdt");
    }
    
    // Remove the semaphore
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("DC semctl");
    }
    
    // Remove shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("DC shmctl");
    }

    printf("\nShazam !!\n");
    // Exit with no statement
    exit(0);
}

// FUNCTION: 	void displayHistogram(int *letterCounts)
// DESCRIPTION: This function displays the histogram of all the letters.
// PARAMETERS:  int *letterCounts - Pointer to an array containing the counts of each letter.
// RETURNS:     None.

void displayHistogram(int *letterCounts) {
    // Clear screen
    system("clear");
    printf("Histogram:\n");

    // Display histogram for each letter
    for (int i = 0; i < NUM_LETTERS; i++) {
        // Calculate count units
        int ones = letterCounts[i] % 10;
        int tens = ((letterCounts[i] % 100) - ones)/10;
        int hundreds = (letterCounts[i] - (tens*10) - ones)/100;

        // Display histogram scale
        printf("%c-%.3d ", 'A'+i, letterCounts[i]);
        while(hundreds != 0)
        {
            printf("*");
            hundreds--;
        }
        while(tens != 0)
        {
            printf("+");
            tens--;
        }
        while(ones != 0)
        {
            printf("-");
            ones--;
        }
        printf("\n");
    }
}

// FUNCTION:    int initSemaphore(int *semID)
// DESCRIPTION: This function initializes a semaphore needed for access to the buffer. It creates a new semaphore with a key generated using ftok function,
//              or retrieves an existing semaphore with the same key if it already exists. The semaphore value is then set to 1.
// PARAMETERS:  int *semID - Pointer to an integer that will hold the semaphore ID.
// RETURNS:     Returns 0 on success, and returns 1 on failure with an appropriate error message printed.

int initSemaphore(int *semID) 
{
    // Getting semaphore key
    key_t semKey = ftok("../../DP-1/bin", 'S');
    if(semKey == -1) {
        perror("DC sem ftok");
        return 1;
    }

    // Create or get the semaphore ID
    *semID = semget(semKey, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (*semID == -1) {
        if (errno == EEXIST) {
            // semaphore already exists
            *semID = semget(semKey, 1, 0666);
        } 
        else {
            perror("DC semget");
            return 1;
        }
    }

    // initialize the semaphore value to 1
    if (semctl(*semID, 0, SETVAL, 1) == -1) {
        perror("DC semctl");
        return 1;
    }

    return 0;
}

// FUNCTION: 	void readBuffer(int sig)
// DESCRIPTION: This function is a signal handler for reading values from the circular buffer.
//              It waits for the semaphore, reads all values from the circular buffer, and releases the semaphore.
// PARAMETERS:  int sig - The signal received.
// RETURNS:     None.

void readBuffer(int sig)
{
    // waiting for the semaphore
    struct sembuf semWait = {0, -1, 0};
    if (semop(semid, &semWait, 1) == -1) {
        perror("DC semop");
        exit(1);
    }

    // Read all values from buffer
    while (sharedBuffer->read_index != sharedBuffer->write_index) {
        char letter = sharedBuffer->buffer[sharedBuffer->read_index];
        letterCounts[letter - 'A']++;
        // Update the read index
        sharedBuffer->read_index = (sharedBuffer->read_index + 1) % BUFFER_SIZE;
    }

    struct sembuf semSig = {0, 1, 0};
    if (semop(semid, &semSig, 1) == -1) {
        perror("DC semop");
        exit(1);
    }
}