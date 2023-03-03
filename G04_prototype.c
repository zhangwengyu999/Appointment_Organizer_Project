//// COMP2432 Operating Systems - Group Project
//// Appointment Organizer
//// Group 04 - CHEN Derun, LIU Minghao, YE Haowen, and ZHANG Wengyu
//// First Stage
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int userSize;
int startTime;
int endTime;
char *users[10];

// child task
int run(int inCNum) {
    printf("Child %d with pid %d created for user %s \n", inCNum+1, getpid(),users[inCNum]);
    sleep(inCNum+1);
    exit(inCNum+1);
}

// Convert names to the standard format
// first letter capitalized regardless of the input string
void standardName(char* inName) {
    int j=0;
    while (inName[j]) {
        char ch = inName[j];
        inName[j]=tolower(ch);
        j++;
    }
    inName[0]=toupper(inName[0]);
}

// main function
int main(int argc, char *argv[]) {
    userSize = argc-3;
    startTime = atoi(argv[1]);
    endTime = atoi(argv[2]);
    int i;
    for (i=0;i<userSize;i++) {
        users[i]=argv[i+3];
    }

    int userFlag = 1;
    int timeFlag = 1;
    if (userSize > 10 || userSize < 3) {
        printf("User Size out of Range [3,10], try again!\n");
        userFlag = 0;
    }
    if (startTime >= endTime || startTime < 20230501 || endTime > 20230531) {
        printf("Time out of Range [20230501, 20230531], try again!\n");
        timeFlag = 0;
    }

    // Wrong input
    if (userFlag == 0 || timeFlag==0) {
        exit(0);
    }

    printf("Time: [%d, %d]\n",startTime, endTime);
    printf("Users: [");
    int k;
    for (k=0;k<userSize-1;k++) {
        standardName(users[k]);
        printf("%s, ",users[k]);
    }

    // Convert username into standard format
    standardName(users[userSize-1]);
    printf("%s]\n",users[userSize-1]);

    printf("Parent, pid %d: \n", getpid());
    int j;
    for (j=0;j<userSize;j++) {
        int cid = fork(); // creat child process
        if (cid < 0) { // error occurred
            printf("Fork Failed\n");
            exit(1);
        } else if (cid == 0) { // child process created
            run(j); // do child task
        }
    }

    // wait for all child processes to complete repeatedly
    int cret;
    int w;
    for(w=0;w<userSize;w++) {
        int cid = wait(&cret); // wait for child to complete, get child process number from return value
        printf("Parent, pid %d: ", getpid());
        printf("child %d with id %d for user %s completed execution\n", cret/256,cid,users[cret/256-1]);
    }
    printf("Parent, pid %d completed execution\n", getpid());
    exit(0);
}
