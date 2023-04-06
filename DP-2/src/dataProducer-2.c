#include <stdio.h>
#include <stdlib.h>
#include <../HISTO-SYSTEM/DP-2/inc/dataProducer-2.h>

#define PID_STRING 16;

int main(int argc, char* argv[]){
    int dp2PID = getpid();
    int dip1PID = getppid();
    int shmID = atoi(argv[1]);

    pid_t dcPID = fork();
    if(dcPID == 0){
        //child process (DC application)
        char dp2PIDStr[PID_STRING], dp1PIDStr[PID_STRING], shmIDStr[PID_STRING];
        sprintf(dp2PIDStr, "%d", dp2PID);
        sprintf(dp2PIDStr, "%d", dip1PID);
        sprintf(shmIDStr, "%d", shmID);
        if(execl("../../DC", "DC", dp2PIDStr, dp1PIDStr, shmIDStr, NULL) == -1){
            printf("Error in executing command line during DC execution in DP-2");
            return 1;
        }
    } else if(dcPID < 0){
        printf("fork failed at DP-2 forking DC\n");
        return 1;
    }

    circular_buffer* buffer = (circular_buffer*)shmat(shmID, NULL, 0);

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