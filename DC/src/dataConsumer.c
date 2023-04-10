#include "../inc/dataConsumer.h"

circular_buffer *shared_buffer;

int main(int argc, char *argv[]) {
    // Read command line arguments
    int sharedMemoryID = atoi(argv[1]);
    dp1_pid = malloc(sizeof(int));
    dp2_pid = malloc(sizeof(int));
    *dp1_pid = atoi(argv[2]);
    *dp2_pid = atoi(argv[3]);

    printf("%d %d %d\n", sharedMemoryID, *dp1_pid, *dp2_pid);

    // Attach to shared memory
    key_t shmKey = ftok("../../DP-1/bin", 16535);
    shmid = shmget(shmKey, sizeof(circular_buffer), 0660);
    if (shmid == -1) {
        if (errno == EEXIST) {
            // shared memory exists
        } 
        else 
        {
            perror("Shared Memory Does NOT Exist");
            exit(1);
        }
    }
    printf("shmid is %d and passed is %d", shmid, sharedMemoryID);
    shared_buffer = (circular_buffer*)shmat(shmid, NULL, 0);
    if (shared_buffer == (void *) -1) {
        perror("shmat");
        exit(1);
    }
    

    if(init_semaphore(&semid) == 1)
    {
        printf("Error: Semaphore creation failed\n");
    }

    // Initialize letter count array
    letter_counts = calloc(NUM_LETTERS, sizeof(int));

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
    // Detach from shared memory segment
    shmdt(shared_buffer);
    
    // Remove the semaphore
    semctl(semid, 0, IPC_RMID);

    // Send SIGINT to data producers
    kill(*dp1_pid, SIGINT);
    kill(*dp2_pid, SIGINT);

    // Remove shared memory segment
    shmctl(shmid, IPC_RMID, NULL);

    printf("Shazam !!");
    //Exit with no statement
    exit(0);
}

void display_histogram(int *letter_counts) {
    printf("\033[2J\033[H"); // clear screen
    printf("Histogram:\n");

    // Calculate maximum count to scale histogram
    int max_count = 0;
    for (int i = 0; i < NUM_LETTERS; i++) {
        if (letter_counts[i] > max_count) {
            max_count = letter_counts[i];
        }
    }

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

void readBuffer(int sig)
{
    // Read from buffer
        while (shared_buffer->read_index != shared_buffer->write_index) {
            char letter = shared_buffer->buffer[shared_buffer->read_index];
            letter_counts[letter - 'A']++;
            shared_buffer->read_index = (shared_buffer->read_index + 1) % BUFFER_SIZE;
        }
}