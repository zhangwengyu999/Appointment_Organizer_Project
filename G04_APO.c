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
void standardName(char* inName);

Appointment** totalAppointmentList;
User** totalUserList;

int userSize;
int startTime;
int endTime;
char *users[10];
int fileNum=0;
int appCount=0;

// ------------Define User Class-------------
typedef struct User {
    int appCount;
    char* userName;
    int* appLists; // 1,2,4,5 哪个app是他的
} User;

// --------Define Appointment Class----------
typedef struct Appointment {
    int appName;
    int appID;
    int caller; // 1
    int* callees; // 2,3
    int date;
    int time;
    int status;
    float duration;
} Appointment;

// --------User Class Methods----------------
User* newUser(char* inName) {
    User* self=(User*) malloc(sizeof(User));
    self->appCount=0;
    self->userName=inName;
    self->appLists=(int*) malloc(sizeof(int)*150);
    return self;
}

char* getUserName(User* inUser){
    return inUser->userName;
}

int* getAppList(User* inUser){
    return inUser->appLists;
}

void addAppointment(User* inUser, int inApp) {
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
        Appointment* app = totalAppointmentList[inUser->appLists[i]];
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
Appointment* newAppointment(int inAppName, int inCaller, int* inCallees, int inID, int inDate, int inTime, float inDuration) {
    Appointment* self=(Appointment *) malloc(sizeof(Appointment));
    self->appName=inAppName;
    self->caller=inCaller;
    self->callees=inCallees;
    self->appID=inID;
    self->date=inDate;
    self->time=inTime;
    self->duration=inDuration;
    self->status=0;
    return self;
}

int getAppName(Appointment* inAppName){
    return inAppName->appName;
}

int getCaller(Appointment* inCaller){
    return inCaller->caller;
}

int* getCallee(Appointment* inCallees){
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

int getStatus(Appointment* inApp){
    return inApp->status;
}

void setStatus(Appointment* inApp, int inStatus){
    inApp->status = inStatus;
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

// get priority of appointment from 1 to 4 with descending priority
int getPriority(char* inAppName){
    if (strcmp(inAppName, "privateTime")==0){
        return 1;
    } 
    else if (strcmp(inAppName, "projectMeeting")==0){
        return 2;
    } 
    else if (strcmp(inAppName, "groupStudy")==0){
        return 3;
    } 
    else if (strcmp(inAppName, "gathering")==0){
        return 4;
    }
    else{
        return 0;
    } 
}

char* itoa(int val, int base){ // convert int to string
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }
	return &buf[i+1];
}

void appToString(Appointment* inApp) {
    // ID(4)PR(1)Date(2)Start(3)Duration(3) = 13
    char* str = (char*) malloc(sizeof(char)*13);
    strcat(str,itoa(inApp->appID,10));
    // ...
}

// get Data and Time transfer
int* getTimeTransfer(int inTime,float inDuration){
    int startTime = (inTime/100)*60 + inTime%100-1080;
    // int endTime = (inTime+ inDuration*60)-1080;
    int duration = inDuration*60;
    int* timeList = (int*) malloc(sizeof(int)*2);
    timeList[0] = startTime;
    timeList[1] = duration;
    return timeList;
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
// ---------- End of Appointment Class Methods ---------




// ---------- Input Module Functions ---------
// record appointment
void recordApp(char* inApp) {
    // 1. store input Appointment log file
    FILE* fp = NULL;
    fp = fopen("log.txt", "a");
    fprintf(fp, "%s\n", inApp);
    fclose(fp);

    // 2. new
    int i = 0;
    int j = 0;
    int inAppLength = 0;
    char *inAppTemp = (char*)malloc(sizeof(char) * strlen(inApp));
    strcpy(inAppTemp, inApp);
    char* tokenTemp = strtok(inAppTemp, " ");
    while (tokenTemp != NULL) {
        inAppLength++;
        tokenTemp = strtok(NULL, " ");
    }
    // printf("app length: %d\n", inAppLength);
    char** splittedApp = (char**)malloc(sizeof(char*) * inAppLength);
    int appIndex = 0;
    char *token = strtok(inApp, " ");
    while (token != NULL) {
        splittedApp[appIndex] = (char*)malloc(sizeof(char) * strlen(token));
        splittedApp[appIndex] = token;
        appIndex++;
        token = strtok(NULL, " ");
    }

    // encode parameters for appointment from input string
    int appName = 0;
    int caller = 0;
    int* callees = (int*)malloc(sizeof(int) * (inAppLength - 5));
    int date = 0;
    int time = 0;
    float duration = 0.0;
    appName = getPriority(splittedApp[0]);
    date = atoi(splittedApp[2]);
    time = atoi(splittedApp[3]);
    duration = atof(splittedApp[4]);
    
    // give caller with a ID from input order
    standardName(splittedApp[1]);
    for (i = 0; i < userSize; i++) {
        if (strcmp(totalUserList[i]->userName, splittedApp[1]) == 0) {
            caller = i;
        }
    }
    // give each callees with a ID from input order
    for (i = 0; i < userSize; i++) {
        // printf("%s11 %d\n", totalUserList[i]->userName,userSize);
        for (j = 5; j < inAppLength; j++) {
            standardName(splittedApp[j]);
            if (strcmp(totalUserList[i]->userName, splittedApp[j]) == 0) {
                // printf("%s\n", splittedApp[j]);
                callees[j-5] = i;
            }
        }
    }
    Appointment* outAppointment = newAppointment(appName, caller, callees, appCount, date, time, duration);
    totalAppointmentList[appCount] = outAppointment;
    addAppointment(totalUserList[caller], appCount);
    for (i = 0; i < inAppLength - 5; i++) {
        addAppointment(totalUserList[callees[i]], appCount);
    }
    appCount++;
}

// command judge for both file and keyboard input
// single cmd: cmd
// file cmd: f filename
char** getCommandList(char* command){
    FILE* fp;
    char* str = (char*) malloc(sizeof(char)*255);
    char** strList = (char**) malloc(sizeof(char*)*1500);
    int num = 0;
    // printf("%s\n",command);

    char* cmdCopy = (char*) malloc(sizeof(command));
    strcpy(cmdCopy,command);
    char* split = strtok(cmdCopy," ");
    // printf("%s\n",command);
    if(strcasecmp(split,"f") == 0){ // file cmd
        char* split= strtok(NULL," ");
        // printf("%s\n",split);
        fp = fopen(split,"r");
        while(!feof(fp)){
            str=fgets(str,255,fp);
            // printf("%s\n",str);
            if (str != NULL){
                strList[num] = (char*) malloc(sizeof(char)*255);
                str[strlen(str)-1] = 0;
                strcpy(strList[num],str);
                // printf("%s\n",strList[num]);
                // printf("%lu\n",strlen(strList[num]));
                num++;
            } 
        }
        fileNum=num;
        fclose(fp);
    }
    else { // single command
        // printf("1\n");
        strList[num] = (char*) malloc(sizeof(char)*255);
        strcpy(strList[num],command);
        fileNum=1;
        // printf("%lu\n",strlen(command));
    }
    return strList;
}
// ---------- End of Input Module Functions ---------




// ---------- Scheduling Module Function----------
void scheduleModule() {}

void FCFS(char** inAppStrList) {}

void PR(char** inAppStrList) {}

void crossCheck(int* rejectedList, int* inAppList) {}
// ---------- End of Scheduling Module ----------




// ---------- Output Module Function ----------
void outputModule() {}
// ---------- End of Output Module ----------


// main function
int main(int argc, char *argv[]) {
    
    // ---------- Initialization ----------
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

    // initialize appointment list
    totalAppointmentList=(Appointment**) malloc(sizeof(Appointment*)*1500);
    // initialize user list
    totalUserList=(User**) malloc(sizeof(User*)*userSize);
    int k;
    for (k=0;k<userSize;k++) {
        standardName(users[k]);
        // totalUserList[k] = (User*) malloc(sizeof(User));
        User* user = newUser(users[k]);
        totalUserList[k]=user;
    }
    printf("Welcome to APpointment Organizer (APO)\n");
    // ---------- End of Initialization ----------

    // fork and pipe
    int	fd[4][2]; // 4 pipe, a pair for each 2 child process
    for (i=1;i<=2;i++) { // loop for 4 process
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
        // -----------------Child 1-2 -----------------
        else if (pid == 0) { // child process 
            if (i == 1) { // Scheduling Module
                // close unused ends
                close(fd[2*(i-1)+1][0]); // close child in for child -> parent pipe
                close(fd[2*(i-1)][1]); // close child out for parent -> child pipe
                char buf1[10];
                char buf2[10];
                while(1) {
                    // 1. Waiting for cmd "H" to receive hello from parent
                    int m = read(fd[2*(i-1)][0],buf1,10);
                    if (m>0 && buf1[0]=='H') {
                        printf("Scheduling Module %d <- Parent: %c\n", i, buf1[0]);
                        write(fd[2*(i-1)+1][1], "A", strlen("A")); // write "A" back to ACK parent
                        printf("Scheduling Module %d -> Parent: 'A'\n", i);
                    }
                    // 2. Waiting for cmd "E" from parent to exit
                    int n = read(fd[2*(i-1)][0],buf2,10);
                    if (n>0 && buf2[0]=='E') {
                        printf("Scheduling Module %d <- Parent: %c\n", i, buf2[0]);
                        break;
                    }
                }
                // close used ends
                close(fd[2*(i-1)+1][1]); // close child out for child -> parent
                close(fd[2*(i-1)][0]); // close child in for parent -> child
                printf("Scheduling Module %d exit\n", i);
                exit(0); 
            }
            else if (i == 2) { // Output Module
                // close unused ends
                close(fd[2*(i-1)+1][0]); // close child in for child -> parent pipe
                close(fd[2*(i-1)][1]); // close child out for parent -> child pipe
                char buf1[10];
                char buf2[10];
                while(1) {
                    // 1. Waiting for cmd "H" to receive hello from parent
                    int m = read(fd[2*(i-1)][0],buf1,10);
                    if (m>0 && buf1[0]=='H') {
                        printf("Output Module %d <- Parent: %c\n", i, buf1[0]);
                        write(fd[2*(i-1)+1][1], "A", strlen("A")); // write "A" back to ACK parent
                        printf("Output Module %d -> Parent: 'A'\n", i);
                    }
                    // 2. Waiting for cmd "E" from parent to exit
                    int n = read(fd[2*(i-1)][0],buf2,10);
                    if (n>0 && buf2[0]=='E') {
                        printf("Output Module %d <- Parent: %c\n", i, buf2[0]);
                        break;
                    }
                }
                // close used ends
                close(fd[2*(i-1)+1][1]); // close child out for child -> parent
                close(fd[2*(i-1)][0]); // close child in for parent -> child
                printf("Output Module %d exit\n", i);
                exit(0); 
            }

        }
    }

    // ---------- Enter Input Module (Parent Process) ----------
    // 1. handle input from keyboard or files
    // 2. check cmd format > log file
    // 3. check cmd type
    // 4. invoke corresponding functions
    // > appointment types: recordApp()
    // > print schedule: func()
    // > exit program: break
    
    // close unused pipe ends
    int j;
    for (j=0;j<2;j++) {
        close(fd[2*(j)+1][1]); // close parent out for child -> parent
        close(fd[2*(j)][0]); // close parent in for parent -> child
    }
    // 1. say hello to all children
    for (j=1;j<=2;j++) { 
        printf("Parent -> CP %d: 'H'\n", j);
        write(fd[2*(j-1)][1], "H", strlen("H")); // write child -> parent
    }
    // 2. wait all child to send back ACK
    int idCount=0;
    while(1) {
        for (j=1;j<=2;j++) {
            char buf3[10]={0};
            int n;
            n = read(fd[2*(j-1)+1][0],buf3,1); // read from child
            if (n>0 && buf3[0]=='A') {
                printf("Parent <- CP %d: %s\n",j,buf3);
                idCount++;
            }
        }
        if (idCount==2) {
            printf("Parent: All children ACKed\n");
            break;
        }
    }

    while (1) {
        char cmd[255];
        printf("Please enter appointment:\n");
        fgets(cmd, 255, stdin);
        if (strlen(cmd)==1) {
            printf("-> [Invalid Command]\n");
            continue;
        }
        cmd[strlen(cmd)-1] = 0;
        // printf("cmd: %s\n", cmd);
        char** cmdList = getCommandList(cmd);
        // printf("%s\n",cmdList[0]);
        // printf("%lu, %lu\n", sizeof(cmdList), sizeof(cmdList[0]));
        int cmdListLength = fileNum;
        if (cmdListLength >= 1) { // single cmd or cmd from file
            int i;
            for (i=0;i<cmdListLength;i++) {
                char* cmdCopy = (char*) malloc(sizeof(cmdList[i]));
                strcpy(cmdCopy,cmdList[i]);
                // printf("%s\n",cmdList[i]);
                char* cmdToken = strtok(cmdCopy, " ");

                if (strcmp(cmdToken, "privateTime") == 0 || strcmp(cmdToken, "projectMeeting") == 0 || 
                    strcmp(cmdToken, "groupStudy") == 0 || strcmp(cmdToken, "gathering") == 0) {
                    recordApp(cmdList[i]);
                    printf("-> [Recorded]\n");
                }
                else if (strcmp(cmdToken, "printSchd") == 0) {
                    printf("-> [Exported file: Ggg_01_FCFS.txt]\n");
                }
                else if (strcmp(cmdList[i], "endProgram") == 0) {
                    printf("-> Bye!\n");
                    // 3. send cmd "E" to child for exit
                    for (j=1;j<=2;j++) {
                        printf("Parent -> CP %d: 'E'\n", j);
                        write(fd[2*(j-1)][1], "E", strlen("E")); // write p->c
                    }
                    // wait for all child to exit
                    for (j=1;j<=2;j++) {
                        wait(NULL);
                    }
                    // close used pipes
                    for (j=1;j<=2;j++) {
                        close(fd[2*(j-1)+1][0]); // close parent in for child -> parent
                        close(fd[2*(j-1)][1]); // close parent out for parent -> child
                    }
                    printf("Parent exit\n");
                    exit(0);
                }
                else {
                    printf("-> [Invalid Command]\n");
                }
                
            }
        } 
    }


    // Output Module
    // scheduled appointments file
    // rejected appointments file

    // Schedule Module
    // FCFS
    // 



    // PR
    // ...



    // int j;
    // for (j=0;j<userSize;j++) {
    //     int cid = fork(); // creat child process
    //     if (cid < 0) { // error occurred
    //         printf("Fork Failed\n");
    //         exit(1);
    //     } else if (cid == 0) { // child process created
    //         run(j); // do child task
    //     }
    // }

    // // wait for all child processes to complete repeatedly
    // int cret;
    // int w;
    // for(w=0;w<userSize;w++) {
    //     int cid = wait(&cret); // wait for child to complete, get child process number from return value
    //     printf("Parent, pid %d: ", getpid());
    //     printf("child %d with id %d for user %s completed execution\n", cret/256,cid,users[cret/256-1]);
    // }
    // printf("Parent, pid %d completed execution\n", getpid());
    // printf("%d",getEndTime(1800,2.3));
    exit(0);
}

// split totalList into different user's list
// sort each user's list

// index作为最后的判断选项，来判断在相同priority，相同时间的情况下，按照index大小顺序比较

// [OK] 排序按照输入时间排，不同算法考虑不同的排序方式 (totalAppointmentList)
// (时间的比较，先比较日期，再比较时间)

// 从第一个开始遍历totalAppointmentList
// reject的情况，判断有冲突的：
// FCFS:index大的被拒绝
// PR: 优先级低的被拒绝（默认优先级越高数字越大）

// 将所有转化成数字传给pipe去分割，保证数字位数不变的情况下组合到一起
// 比如日期转化为101-131，时间转化为1001-1300，持续时间100-159，最后一位为priority

// for app in totalAppointmentList:
//      if app appears in ALL app's user's list (or do not appears in rejected list): accept
//      else if do NOT appears in any app's user's list (or appears in rejected list): reject

// [OK] add appID in app Class; 
// appointment caller and callees all change to user;

// [OK] split total list into user lists;
// (order function (in user list); func(int*))
// check overlap, and reject (FCFS, PR) function (in user list);
// check totalAppList, to check all accept or reject with user lists, reject list (in total list);

// pipe
