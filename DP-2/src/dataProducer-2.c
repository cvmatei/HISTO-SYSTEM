/*
* FILE: dataProducer-2.c
* PROJECT: HISTO-SYSTEM
* FIRST VERSION: 04/05/2023
* PROGRAMMER(s): Cosmin Matei, Ahmed Ruda
* DESCRIPTION: This file contains the main function for the dataProducer 2 utility. It reads command line arguments, 
*              sets up shared memory and semaphore, handles signals, and writes to the circular buffer.
*/

#include "../inc/dataProducer-2.h"

int main(int argc, char* argv[]){
    int semID;
    int dp2PID = getpid();
    int dp1PID = getppid();
    int sharedMemoryID = atoi(argv[1]);

    pid_t dcPID = fork();
    if(dcPID == 0){
        //child process (DC application)
        char dp2PIDStr[PID_STRING];
        char dp1PIDStr[PID_STRING];
        char shmIDStr[PID_STRING];
        sprintf(dp1PIDStr, "%d", dp1PID);
        sprintf(dp2PIDStr, "%d", dp2PID);
        sprintf(shmIDStr, "%d", sharedMemoryID);
        if(execl("../../DC/bin/DC", "DC", shmIDStr, dp1PIDStr, dp2PIDStr, NULL) == -1){
            printf("Error in executing command line during DC execution in DP-2");
            return 1;
        }
    } else if(dcPID < 0){
        printf("fork failed at DP-2 forking DC\n");
        return 1;
    }

    // Getting shared memory key
    key_t shmKey = ftok("../../DP-1/bin", 16535);
    if(shmKey == -1){
        perror("Failed to get shared memory key\n");
        return 1;
    }

    // Getting shared memory ID
    shmid = shmget(shmKey, sizeof(circular_buffer), 0660);
    if(shmid == -1){
        perror("Failed to get shared memory ID\n");
        return 1;
    }

    // Attaching shared memory
    buffer = (circular_buffer*)shmat(shmid, NULL, 0);
    if(buffer == (void *) -1){
        perror("Failed to attach shared memory\n");
        return 1;
    }

    // Initializing semaphore
    if(init_semaphore(&semID) == 1)
    {
        printf("Error: Semaphore creation failed\n");
    }

    //Write random letters to circular buffer
    char letter;
    //Register signal handler for SIGINT
    signal(SIGINT, detachAndExit);
    while (1) {
        int randomInt = rand() % 20;
        letter = getChar(randomInt);

        // waiting for the semaphore
        struct sembuf semWait = {0, -1, 0};
        if(semop(semID, &semWait, 1) == -1){
            perror("Failed to wait for semaphore\n");
            return 1;
        }

        // Calculate number of available elements in the buffer
        int numAvailable = (buffer->read_index - buffer->write_index - 1 + BUFFER_SIZE) % BUFFER_SIZE;

        // Checking that there is space available to write to that doesn't go beyond the read index
        if (numAvailable > 0) {
            buffer->buffer[buffer->write_index] = letter;
            buffer->write_index = (buffer->write_index + 1) % BUFFER_SIZE;
        }

        // signal the semaphore
        struct sembuf semSignal = {0, 1, 0};
        if(semop(semID, &semSignal, 1) == -1){
            perror("Failed to wait for semaphore\n");
            return 1;
        }

        // Sleep for 1/20 of a second
        usleep(50000);
    }
    return 0;
}

// FUNCTION: 	void getChar(int randomInt)
// DESCRIPTION: This function gets a random character based off of the random number generated
// PARAMETERS:  int randomInt - a randomly generated integer
// RETURNS:     None.
char getChar(int randomInt){
    switch(randomInt){
        case 0:
            return 'A';
        case 1:
            return 'B';
        case 2:
            return 'C';
        case 3:
            return 'D';
        case 4:
            return 'E';
        case 5:
            return 'F';
        case 6:
            return 'G';
        case 7:
            return 'H';
        case 8:
            return 'I';
        case 9:
            return 'J';
        case 10:
            return 'K';
        case 11:
            return 'L';
        case 12:
            return 'M';
        case 13:
            return 'N';
        case 14:
            return 'O';
        case 15:
            return 'P';
        case 16:
            return 'Q';
        case 17:
            return 'R';
        case 18:
            return 'S';
        case 19:
            return 'T';
        default:
            return 'Z';
    }
}

// FUNCTION: 	void init_semaphore() 
// DESCRIPTION: This function initializes the semephore needed for the clients
//              while making sure that more semaphores are not created.
// PARAMETERS:  int *semID
// RETURNS:     None.
int init_semaphore(int *semID) 
{
    // get a unique key for the semaphore
    key_t semKey = ftok("../../DP-1/bin", 'S');
    if(semKey == -1) 
    {
        // can not create semaphore key, exiting
        return 1;
    }

    // create the semaphore
    *semID = semget(semKey, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (*semID == -1) 
    {
        if (errno == EEXIST) {
            // semaphore already exists
            *semID = semget(semKey, 1, 0666);
            if(*semID == -1){
                perror("DP2 semget fail");
            }
        } 
        else 
        {
            // cannot create semaphore
            perror("DP2 sem fail");
            return 1;
        }
    }

    // initialize the semaphore value to 1
    if (semctl(*semID, 0, SETVAL, 1) == -1) {
        perror("DP2 semctl");
        return 1;
    }
    return 0;
}

// FUNCTION: 	void detachAndExit(int sig)
// DESCRIPTION: This function handles the termination signal receieved by detaching from the memory and exiting the program
// PARAMETERS:  int sig - The signal received (SIGINT).
// RETURNS:     None.
void detachAndExit(int sig) {
    //Detach from shared memory segment
    if (shmdt(buffer) == -1) {
        perror("DP2 shmdt");
    }
    //Exit with no statement
    exit(0);
}