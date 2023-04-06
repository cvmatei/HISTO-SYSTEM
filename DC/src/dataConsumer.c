#include "../inc/dataConsumer.h"

int main(int argc, char *argv[]) {
    // Read command line arguments
    int sharedMemoryID = atoi(argv[1]);
    dp1_pid = malloc(sizeof(int));
    dp2_pid = malloc(sizeof(int));
    *dp1_pid = atoi(argv[2]);
    *dp2_pid = atoi(argv[3]);

    // Attach to shared memory
    shmid = shmget(sharedMemoryID, sizeof(circular_buffer), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    shared_buffer = shmat(shmid, NULL, 0);
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
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Main processing loop
    int num_reads = 0;
    while (1) {
        // Wait for semaphore
        struct sembuf wait_op = {0, -1, 0};
        if (semop(semid, &wait_op, 1) == -1) {
            perror("semop wait");
            exit(1);
        }

        // Read from buffer
        while (shared_buffer->read_index != shared_buffer->write_index) {
            char letter = shared_buffer->buffer[shared_buffer->read_index];
            letter_counts[letter - 'A']++;
            shared_buffer->read_index = (shared_buffer->read_index + 1) % BUFFER_SIZE;
        }
        num_reads++;

        // Release semaphore
        struct sembuf signal_op = {0, 1, 0};
        if (semop(semid, &signal_op, 1) == -1) {
            perror("semop signal");
            exit(1);
        }

        // Check if we need to display histogram
        if (num_reads > 5) {
            display_histogram(letter_counts);
            num_reads = 0;
        }

        // Sleep for 2 seconds
        sleep(2);
    }

    return 0;
}

void handle_sigint(int sig) {
    // Send SIGINT to data producers
    kill(dp1_pid, SIGINT);
    kill(dp2_pid, SIGINT);
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
        int tens = (letter_counts[i] / 10) % 10;
        int hundreds = (letter_counts[i] / 100) % 10;

        // Scale count units based on maximum count
        ones = (int) ((float) ones / max_count * 50);
        tens = (int) ((float) tens / max_count * 50);
        hundreds = (int) ((float) hundreds / max_count * 50);

        // Display histogram bar
        printf("%c [%c%c%c] %c\n", 'A' + i, '*', '+', '-', 'A' + i);

        // Display histogram scale
        printf("  ");
        for (int j = 0; j < 50; j++) {
            if (j == ones) {
                printf("-");
            } else if (j == tens) {
                printf("+");
            } else if (j == hundreds) {
                printf("*");
            } else {
                printf(" ");
            }
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