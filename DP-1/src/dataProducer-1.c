#include <stdio.h>
#include <stdlib.h>
#include <../HISTO-SYSTEM/DP-1/inc/dataProducer-1.h>

int main() {
    srand(time(NULL));
    int randomInt;
    int semID;

    // Initialize the semaphore
    init_semaphore(&semID);

    //Check if shared memory segment already exists
    key_t sMemKey = ftok(".", 16535);
    int sMemID = shmget(sMemKey, sizeof(circular_buffer), IPC_CREAT | IPC_EXCL | 0660);
    if (sMemID == -1) {
        //Shared memory segment already exists, get the shared memory ID
        sMemID = shmget(sMemKey, sizeof(circular_buffer), 0660);
    }
    
    //Attach circular buffer to shared memory
    circular_buffer* buffer = (circular_buffer*) shmat(sMemID, NULL, 0);
    buffer->read_index = 0;
    buffer->write_index = 0;

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
        if(execl("./DP-2", "DP-2", sMemIDString, NULL) == -1){
            //If execl returns, there was an error
            printf("Error: execl() failed\n");
            return 1;
        } //Launch DP-2 with shmID as argument
    }

    //Write random letters to circular buffer
    char letter;
    while (1) {
        randomInt = rand() % 20;
        letter = getChar(randomInt);
        buffer->buffer[buffer->write_index] = letter;
        buffer->write_index = (buffer->write_index + 1) % BUFFER_SIZE;
    }

    //Detach from shared memory segment
    shmdt(buffer);

    return 0;
}

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