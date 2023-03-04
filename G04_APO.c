//// COMP2432 Operating Systems - Group Project
//// Appointment Organizer
//// Group 04 - CHEN Derun, LIU Minghao, YE Haowen, and ZHANG Wengyu
//// Second Stage
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

typedef struct User User;
typedef struct Appointment Appointment;
int getEndTime(int inTime, float inDuration);

// ------------Define User Class-------------
typedef struct User {
    int appCount;
    char* userName;
    Appointment** appLists;
} User;

// --------Define Appointment Class----------
typedef struct Appointment {
    char* appName;
    User* caller;
    User** callees;
    int date;
    int time;
    int status;
    float duration;
} Appointment;

// --------User Class Methods----------------
User* newUser(char* inName) {
    User* self=(User*) malloc(sizeof(User));
    self->appCount;
    self->userName=inName;
    self->appLists=(Appointment**) malloc(sizeof(Appointment*)*150);
    return self;
}

char* getUserName(User* inUser){
    return inUser->userName;
}

Appointment** getAppList(User* inUser){
    return inUser->appLists;
}

void addAppointment(User* inUser, Appointment* inApp) {
    if (inUser->appCount<150) {
        inUser->appLists[inUser->appCount]=inApp;
        inUser->appCount++;
    }
    else {
        return;
    }
}

int isAvailableIn(User* inUser, Appointment* inApp) {
    int inDate = inApp->date;
    int inTime = inApp->time;
    float inDuration = inApp->duration;
    int inEndTime = getEndTime(inTime, inDuration);
    int i;
    for (i=0;i<inUser->appCount;i++) {
        Appointment* app = inUser->appLists[i];
        if (app->date==inDate) {
            int start = app->time;
            int end = getEndTime(start,app->duration);
            if (inTime < start && inEndTime > start) {return 0;}
            if (inEndTime > end && inTime < end){return 0;}
            if (inEndTime <= end && inTime >= start){return 0;}
        }
        else {
            continue;
        }
    }
    return 1;
}
// --------Appointment Class Methods----------------
Appointment* newAppointment(char* inAppName, User* inCaller, User** inCallees, int inDate, int inTime, float inDuration) {
    Appointment* self=(Appointment *) malloc(sizeof(Appointment));
    self->appName=inAppName;
    self->caller=inCaller;
    self->callees=inCallees;
    self->date=inDate;
    self->time=inTime;
    self->duration=inDuration;
    self->status=0;
    return self;
}

char* getAppName(Appointment* inAppName){
    return inAppName->appName;
}

User* getCaller(Appointment* inCaller){
    return inCaller->caller;
}

User** getCallee(Appointment* inCallees){
    return inCallees->callees;
}

int getDate(Appointment* inDate){
    return inDate->date;
}

int getAppTime(Appointment* inTime){
    return inTime->time;
}

float getDuration(Appointment* inDuration){
    return inDuration->duration;
}

int getStatus(Appointment* app){
    return app->status;
}

void setStatus(Appointment* app, int inStatus){
    app->status = inStatus;
}

int getEndTime(int inTime, float inDuration){
    return inTime + ((int)inDuration * 100 + (inDuration - ((int)inDuration)) * 60);
}

int getTimeOut(int inDate, int inTime, int endTime){
    if(inTime < 1800 || endTime > 2300 || inDate < 20230501 || inDate > 20230531){
        return 0; // false
    }
    else{
        return 1; // true
    }
}

int judgeCommand(char* command){
    FILE* fp;
    char* str;
    char* strList[1500];
    int num;
    printf("Input command: ");
    scanf("%s", &command);
    if(strcasecmp(command,"f")){
        char* split= strtok(NULL," ");
        fp = fopen(split,"r");
        while(!feof(fp)){
            str=fgets(str,100,fp);
            strList[num]=str;
            num++;
        }
        fclose(fp);
    }
    else{
        fgets(str,100,stdin);
        strList[num]=str;
        num++;
    }


    return 0;
}


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

    printf("Welcome to APpointment Organizer (APO)\n");
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
    // printf("%d",getEndTime(1800,2.3));
    exit(0);
}

