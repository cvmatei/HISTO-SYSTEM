#include <stdio.h>
#include <stdlib.h>

int main() {
    srand(time(NULL));
    int randomInt;

    // Check if shared memory segment already exists
    key_t sMemKey = ftok(".", 16535);
    int sMemID = shmget(sMemKey, sizeof(circular_buffer), IPC_CREAT | IPC_EXCL | 0660);
    if (sMemID == -1) {
        // Shared memory segment already exists, attach to it
        sMemID = shmget(sMemKey, sizeof(circular_buffer), 0660);
    }
    
    circular_buffer* buffer = (circular_buffer*) shmat(sMemID, NULL, 0);
    buffer->read_index = 0;
    buffer->write_index = 0;

    // Write random letters to circular buffer
    char letter;
    while (1) {
        randomInt = rand() % 20;
        letter = getChar(randomInt);
        buffer->buffer[buffer->write_index] = letter;
        buffer->write_index = (buffer->write_index + 1) % BUFFER_SIZE;
    }

    // Detach from shared memory segment
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