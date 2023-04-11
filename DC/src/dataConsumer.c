#include "../inc/dataConsumer.h"

circular_buffer *shared_buffer;

int main(int argc, char *argv[]) {
    // Read command line arguments
    int sharedMemoryID = atoi(argv[1]);
    dp1_pid = malloc(sizeof(int));
    dp2_pid = malloc(sizeof(int));
    *dp1_pid = atoi(argv[2]);
    *dp2_pid = atoi(argv[3]);

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
    shared_buffer = (circular_buffer*)shmat(sharedMemoryID, NULL, 0);
    if (shared_buffer == (void *)-1) {
        perror("DC shmat");
        exit(1);
    }
    
    // Initializing semaphore
    if(init_semaphore(&semid) == 1) {
        printf("Error: Semaphore creation failed\n");
    }

    // Initialize letter count array
    letter_counts = calloc(NUM_LETTERS, sizeof(int));
    if (letter_counts == NULL) {
        perror("DC calloc");
        exit(1);
    }

    // Set up SIGINT handler
    signal(SIGINT, handle_sigint);

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
        if (num_reads > 5) {
            display_histogram(letter_counts);
            num_reads = 0;
        }
    }

    return 0;
}

void handle_sigint(int sig) {
    // Send SIGINT to data producers
    if (kill(*dp1_pid, SIGINT) == -1) {
        perror("DC kill dp1");
    }
    if (kill(*dp2_pid, SIGINT) == -1) {
        perror("DC kill dp2");
    }

    // Read all values from buffer
    while (shared_buffer->read_index != shared_buffer->write_index) {
        char letter = shared_buffer->buffer[shared_buffer->read_index];
        letter_counts[letter - 'A']++;
        // Update the read index
        shared_buffer->read_index = (shared_buffer->read_index + 1) % BUFFER_SIZE;
    }
    display_histogram(letter_counts);

    // Detach from shared memory segment
    if (shmdt(shared_buffer) == -1) {
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

void display_histogram(int *letter_counts) {
    // Clear screen
    system("clear");
    printf("Histogram:\n");

    // Display histogram for each letter
    for (int i = 0; i < NUM_LETTERS; i++) {
        // Calculate count units
        int ones = letter_counts[i] % 10;
        int tens = ((letter_counts[i] % 100) - ones)/10;
        int hundreds = (letter_counts[i] - (tens*10) - ones)/100;

        // Display histogram scale
        printf("%c-%.3d ", 'A'+i, letter_counts[i]);
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

// FUNCTION: 	void init_semaphore() 
// DESCRIPTION: This function initializes the semephore needed for the clients
//              while making sure that more semaphores are not created.
// PARAMETERS:  int *semID
// RETURNS:     None.

int init_semaphore(int *semID) 
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

void readBuffer(int sig)
{
    // waiting for the semaphore
    struct sembuf semWait = {0, -1, 0};
    if (semop(semid, &semWait, 1) == -1) {
        perror("DC semop");
        exit(1);
    }

    // Read all values from buffer
    while (shared_buffer->read_index != shared_buffer->write_index) {
        char letter = shared_buffer->buffer[shared_buffer->read_index];
        letter_counts[letter - 'A']++;
        // Update the read index
        shared_buffer->read_index = (shared_buffer->read_index + 1) % BUFFER_SIZE;
    }

    struct sembuf semSig = {0, 1, 0};
    if (semop(semid, &semSig, 1) == -1) {
        perror("DC semop");
        exit(1);
    }
}