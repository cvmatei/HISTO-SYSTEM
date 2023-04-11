/*
* FILE: dataProducer-1.c
* PROJECT: HISTO-SYSTEM
* FIRST VERSION: 04/05/2023
* PROGRAMMER(s): Cosmin Matei, Ahmed Ruda
* DESCRIPTION: This file contains the main function for the dataProducer 1 utility. It sets up shared memory and semaphore, 
*              handles signals, and writes to the circular buffer.
*/
#include "../inc/dataProducer-1.h"

int main() {
    srand(time(NULL));
    int randomInt;
    int semID;

    //Check if shared memory segment already exists
    key_t sMemKey = ftok(".", 16535);
    if (sMemKey == -1) {
        printf("Error: ftok() failed to generate shared memory key\n");
        return 1;
    }
    
    int sMemID = shmget(sMemKey, sizeof(circular_buffer), IPC_CREAT | IPC_EXCL | 0660);
    if (sMemID == -1) {
        if (errno == EEXIST) {
            sMemID = shmget(sMemKey, sizeof(circular_buffer), 0660);
            if (sMemID == -1) {
                printf("Error: failed to get shared memory segment ID\n");
                return 1;
            }
        } else {
            printf("Error: failed to create shared memory segment\n");
            return 1;
        }
    }
    
    buffer = (circular_buffer*) shmat(sMemID, NULL, 0);
    if (buffer == (void *) -1) {
        printf("Error: failed to attach shared memory segment\n");
        return 1;
    }
    buffer->read_index = 0;
    buffer->write_index = 0;

    if(init_semaphore(&semID) == 1)
    {
        printf("Error: Semaphore creation failed\n");
        return 1;
    }

    //Launch DP-2 through the use of fork()
    pid_t pid = fork();
    if (pid == -1) {
        //Fork failed
        printf("Error: fork() failed\n");
        return 1;
    } else if (pid == 0) {
        //Child process (DP-2)
        char sMemIDString[32];
        sprintf(sMemIDString, "%d", sMemID); //Convert shmID to string
        if(execl("../../DP-2/bin/DP-2", "DP-2", sMemIDString, NULL) == -1){
            //If execl returns, there was an error
            printf("Error: execl() failed\n");
            return 1;
        } //Launch DP-2 with shmID as argument
    }

    //Register signal handler for SIGINT
    signal(SIGINT, detachAndExit);

    // Generate 20 random letters and write all 20 letters into the sharedMemory buffer, then sleep for 2 seconds
    while (1) {
        // waiting for the semaphore
        struct sembuf semWait = {0, -1, 0};
        semop(semID, &semWait, 1);

        // Calculate number of available elements in the buffer
        int numAvailable = (buffer->read_index - buffer->write_index - 1 + BUFFER_SIZE) % BUFFER_SIZE;

        if (numAvailable >= 20) {
            for (int i = 0; i < 20; i++) {
                int randomInt = rand() % 20;
                char letter = getChar(randomInt);
                buffer->buffer[buffer->write_index] = letter;
                buffer->write_index = (buffer->write_index + 1) % BUFFER_SIZE;
            }
        } else if (numAvailable > 0) {
            for (int i = 0; i < numAvailable; i++) {
                int randomInt = rand() % 20;
                char letter = getChar(randomInt);
                buffer->buffer[buffer->write_index] = letter;
                buffer->write_index = (buffer->write_index + 1) % BUFFER_SIZE;
            }
        }

        // signal the semaphore
        struct sembuf semSignal = {0, 1, 0};
        semop(semID, &semSignal, 1);

        sleep(2);
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
    key_t semKey = ftok(".", 'S');
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
        } 
        else 
        {
            // cannot create semaphore
            return 1;
        }
    }

    // initialize the semaphore value to 1
    semctl(*semID, 0, SETVAL, 1);
    return 0;
}

// FUNCTION: 	void detachAndExit(int sig)
// DESCRIPTION: This function handles the termination signal receieved by detaching from the memory and exiting the program
// PARAMETERS:  int sig - The signal received (SIGINT).
// RETURNS:     None.
void detachAndExit(int sig) {
    //Detach from shared memory segment
    shmdt(buffer);
    //Exit with no statement
    exit(0);
}