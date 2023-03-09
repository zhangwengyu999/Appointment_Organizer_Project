// To compile: gcc unnamed_pipe_ipc.c -o unnamed_pipe_ipc
// To run: ./unnamed_pipe_ipc
// Mike_Zhang 2023-03-09
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
int	fd[8][2]; // 8 pipe, a pair for each 4 process
int main() {
    int i;
    for (i=1;i<=4;i++) { // loop for 4 process
        // parent -> child pipe 0,2,4,6
        if (pipe(&fd[2*(i-1)][0]) < 0) {
            printf("Pipe creation error\n");
            exit(1);
        }
        // child -> parent pipe 1,3,5,7
        if (pipe(&fd[2*(i-1)+1][0]) < 0) {
            printf("Pipe creation error\n");
            exit(1);
        }
        int pid = fork();
        if (pid < 0) { // error occurred
            printf("Fork Failed\n");
            exit(1);
        } 
        // -----------------Child 1-4 -----------------
        else if (pid == 0) { // child process 
            // close unused ends
            close(fd[2*(i-1)+1][0]); // close child in for child -> parent pipe
            close(fd[2*(i-1)][1]); // close child out for parent -> child pipe
            char buf1[10];
            char buf2[10];
            while(1) {
                // 1. Waiting for cmd "H" to receive hello from parent
                int m = read(fd[2*(i-1)][0],buf1,10);
                if (m>0 && buf1[0]=='H') {
                    printf("Child %d <- Parent: %c\n", i, buf1[0]);
                    write(fd[2*(i-1)+1][1], "A", strlen("A")); // write "A" back to ACK parent
                    printf("Child %d -> Parent: 'A'\n", i);
                }
                // 2. Waiting for cmd "E" from parent to exit
                int n = read(fd[2*(i-1)][0],buf2,10);
                if (n>0 && buf2[0]=='E') {
                    printf("Child %d <- Parent: %c\n", i, buf2[0]);
                    break;
                }
            }
            // close used ends
            close(fd[2*(i-1)+1][1]); // close child out for child -> parent
            close(fd[2*(i-1)][0]); // close child in for parent -> child
            printf("Child %d exit\n", i);
            exit(0); 
        }
    }
    // -----------------End of Child---------------

    // -----------------Parent---------------------
    // close unused ends
    int j;
    for (j=1;j<=4;j++) {
        close(fd[2*(j-1)+1][1]); // close parent out for child -> parent
        close(fd[2*(j-1)][0]); // close parent in for parent -> child
    }

    // 1. say hello to all children
    for (j=1;j<=4;j++) { 
        printf("Parent -> CP %d: 'H'\n", j);
        write(fd[2*(j-1)][1], "H", strlen("H")); // write child -> parent
    }

    // 2. wait all child to send back ACK
    int idCount=0;
    while(1) {
        for (j=1;j<=4;j++) {
            char buf3[10]={0};
            int n;
            n = read(fd[2*(j-1)+1][0],buf3,1); // read from child
            if (n>0 && buf3[0]=='A') {
                printf("Parent <- CP %d: %s\n",j,buf3);
                idCount++;
            }
        }
        if (idCount==4) {
            printf("Parent: All children ACKed\n");
            break;
        }
    }

    // 3. send cmd "E" to child for exit
    for (j=1;j<=4;j++) {
        printf("Parent -> CP %d: 'E'\n", j);
        write(fd[2*(j-1)][1], "E", strlen("E")); // write p->c
    }
    // wait for all child to exit
    for (j=1;j<=4;j++) {
        wait(NULL);
    }
    // close used pipes
    for (j=1;j<=4;j++) {
        close(fd[2*(j-1)+1][0]); // close parent in for child -> parent
        close(fd[2*(j-1)][1]); // close parent out for parent -> child
    }
    printf("Parent exit\n");
    exit(0);
}
