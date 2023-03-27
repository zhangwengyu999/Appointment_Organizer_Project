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
    int* appLists; // 1,2,4,5 
} User;

// --------Define Appointment Class----------
typedef struct Appointment {
    int appName;
    int appID;
    // int caller; // 1
    int* user; // 2,3
    int userCount;
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
Appointment* newAppointment(int inAppName, int* user, int inUserCount, int inID, int inDate, int inTime, float inDuration) {
    Appointment* self=(Appointment *) malloc(sizeof(Appointment));
    self->appName=inAppName;
    self->user=user;
    self->userCount=inUserCount;
    // self->callees=inCallees;
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

// int getCaller(Appointment* inCaller){
//     return inCaller->caller;
// }

int* getUser(Appointment* inUser){
    return inUser->user;
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

char* int2Str(int val, int base){ // convert int to string
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }
	return &buf[i+1];
}

// get Data and Time transfer
int* getDateTimeTransfer(int inDate, int inTime,float inDuration){
    int date = 100+(inDate-20230500); // 101-131
    int startTime = 1000+((inTime/100)*60 + inTime%100-1080); // 1000-1300
    // int endTime = (inTime+ inDuration*60)-1080;
    int duration = inDuration*60+100; // 101-400
    int* timeList = (int*) malloc(sizeof(int)*3);
    timeList[0] = date; // 3
    timeList[1] = startTime; // 4
    timeList[2] = duration; // 3
    return timeList;
}

char* appToString(Appointment* inApp) {
    // ID(5)Date(3)Start(4)Duration(3)PR(1)Status(1) = 17
    char* str = (char*) malloc(sizeof(char)*17);
    strcat(str,int2Str((10000+inApp->appID),10)); // ID(5): 10000+id
    int* timestamp = getDateTimeTransfer(inApp->date, inApp->time, inApp->duration);
    strcat(str,int2Str(timestamp[0],10)); // Date(3)
    strcat(str,int2Str(timestamp[1],10)); // Start(4)
    strcat(str,int2Str(timestamp[2],10)); // Duration(3)
    strcat(str,int2Str(getAppName(inApp),10)); // PR(1)
    strcat(str,"0"); // Status(1) initial status is 0 (not rejected)
    return str;
}

char* appToString2(Appointment* inApp) {
    // appToString()
    // append users in app
    // ID(5)Date(3)Start(4)Duration(3)PR(1)Status(1) = 17
    char* str = (char*) malloc(sizeof(char)*27);
    strcat(str,int2Str((10000+inApp->appID),10)); // ID(5): 10000+id
    int* timestamp = getDateTimeTransfer(inApp->date, inApp->time, inApp->duration);
    strcat(str,int2Str(timestamp[0],10)); // Date(3)
    strcat(str,int2Str(timestamp[1],10)); // Start(4)
    strcat(str,int2Str(timestamp[2],10)); // Duration(3)
    strcat(str,int2Str(getAppName(inApp),10)); // PR(1)
    strcat(str,"0"); // Status(1) initial status is 0 (not rejected)
    int i = 0;
    int j = 0;
    char* str2 = (char*) malloc(sizeof(char)*10);
    for (i = 0; i<10; i++) str2[i] = '0';
    for (i = 0; i < inApp->userCount; i++) {
        for (j = 0; j < userSize; j++) {
            if (inApp->user[i] == j) {
                str2[j] = '1';
            }
        }
    }
    strcat(str,str2);
    return str;
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
    // int caller = 0;
    int* user = (int*)malloc(sizeof(int) * (inAppLength - 4));
    int date = 0;
    int time = 0;
    float duration = 0.0;
    appName = getPriority(splittedApp[0]);
    date = atoi(splittedApp[2]);
    time = atoi(splittedApp[3]);
    duration = atof(splittedApp[4]);
    
    // give caller with a ID from input order
    // standardName(splittedApp[1]);
    for (i = 0; i < userSize; i++) {
        if (strcmp(totalUserList[i]->userName, splittedApp[1]) == 0) {
            user[0] = i;
        }
    }
    // give each callees with a ID from input order
    for (i = 0; i < userSize; i++) {
        // printf("%s11 %d\n", totalUserList[i]->userName,userSize);
        for (j = 5; j < inAppLength; j++) {
            standardName(splittedApp[j]);
            if (strcmp(totalUserList[i]->userName, splittedApp[j]) == 0) {
                // printf("%s\n", splittedApp[j]);
                user[j-4] = i;
            }
        }
    }
    Appointment* outAppointment = newAppointment(appName, user, inAppLength-4, appCount, date, time, duration);
    totalAppointmentList[appCount] = outAppointment;
    // addAppointment(totalUserList[caller], appCount);
    for (i = 0; i < inAppLength - 4; i++) {
        addAppointment(totalUserList[user[i]], appCount);
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

// void FCFS(char** inAppStrList, int inSize, int* inRejectedList, int inRejectedSize);
// int PR(char **inAppStrList, int inAppStrSize,char** inRejectedList, int inRejectedSize);




int* isCollision(char* inAppA, char** inAllApp, int inSize) {
    // ID(5)Date(3)Start(4)Duration(3)PR(1)Status(1) = 17
    int* outCollision = (int*)malloc(sizeof(int) * inSize);
    int collisionCount = 0;
    int appLength = 17;
    int i = 0;
    int j = 0;
    char* idA = (char*)malloc(sizeof(char) * 5);
    char* idB = (char*)malloc(sizeof(char) * 5);
    char* dateA = (char*)malloc(sizeof(char) * 3);
    char* dateB = (char*)malloc(sizeof(char) * 3);
    char* startTimeA = (char*)malloc(sizeof(char) * 4);
    char* startTimeB = (char*)malloc(sizeof(char) * 4);
    char* durationA = (char*)malloc(sizeof(char) * 3);
    char* durationB = (char*)malloc(sizeof(char) * 3);
    char status;

    for (j=0;j<inSize; j++) {
        outCollision[j] = -1;
        char *inAppB = inAllApp[j];
        for (i = 0; i < 5; i++) {
            idA[i] = inAppA[i];
            idB[i] = inAppB[i];
        }
        for (i = 0; i < 3; i++) {
            dateA[i] = inAppA[i + 5];
            dateB[i] = inAppB[i + 5];
        }
        for (i = 0; i < 4; i++) {
            startTimeA[i] = inAppA[i + 8];
            startTimeB[i] = inAppB[i + 8];
        }
        for (i = 0; i < 3; i++) {
            durationA[i] = inAppA[i + 12];
            durationB[i] = inAppB[i + 12];
        }
        status = inAppB[16];

        if (strcmp(idA, idB) < 0) {
            if (status == '1') {
                return (int *) -1;
            }
            // same app, no collision for sure, continue
            // different app date, no collision for sure, continue
            if (strcmp(dateA, dateB) != 0) {
                continue;
            }
            // in same data, possible collision, need to check start time and duration
            if (strcmp(startTimeA, startTimeB) > 0) {
                if (atoi(startTimeB) + atoi(durationB) <= atoi(startTimeA)) {
                    continue;
                } else {
                    if (status == 0) {
                        outCollision[collisionCount] = atoi(idB);
                        collisionCount++;
                    }
                }
            } else if (strcmp(startTimeA, startTimeB) < 0) {
                if (atoi(startTimeA) + atoi(durationA) <= atoi(startTimeB)) {
                    continue;
                } else {
                    if (status-'0' == 0) {
                        outCollision[collisionCount] = atoi(idB);
                        collisionCount++;

                    }
                }
            } else if (strcmp(idA, idB) != 0 && strcmp(startTimeA, startTimeB) == 0) {
                outCollision[collisionCount] = atoi(idB);
                collisionCount++;
            }
        }
        else{
            continue;
        }
    }
    return outCollision;
}

// input: string list of apps 17 bit string
// output: int list [10001, 12342, 10293, 11328, 14384]
int FCFS(char **inAppStrList, int inAppStrSize,int* inRejectedList, int inRejectedSize) {
    printf("FCFS1\n");
    int *acceptAppList = (int *)malloc(sizeof(int) * inAppStrSize);
    int acceptLength = 0;
    int i,k,l,j,m;

    for (i=0;i<inAppStrSize;i++) {
        printf("FCFS2\n");
        int* collisions = isCollision(inAppStrList[i],inAppStrList, inAppStrSize);
        printf("collisionFinish\n");
        int collision;
        int collisionFlag = 0;
        int existCheck = 0;
        char* idA = (char*)malloc(sizeof(char) * 5);
        char* idB = (char*)malloc(sizeof(char) * 5);
        char* temp1 = (char*)malloc(sizeof(char) * 17);
        int temp = 0;
        for (l = 0; l < 5; l++) {
            idA[l] = inAppStrList[i][l];
        }
        for (k=0;k<inAppStrSize;k++) {
            collision = collisions[k];
            if (collision == -1) {
                break;
            }
            else {
                for (l = 0; l < inRejectedSize; l++) {
                    temp = inRejectedList[l];
                    if (atoi(idA) == temp) {
                        collisionFlag = 1;
                        break;
                    }
                }
                if (collisionFlag == 1) {
                    collisionFlag = 0;
                    break;
                }
                else {
                    for (m = 0; m < inRejectedSize; ++m) {
                        if (collision== inRejectedList[m]) {
                            existCheck++;
                            break;
                        }
                    }
                    if (atoi(idA) < collision && existCheck==0) {
                        inRejectedList[inRejectedSize] = collision;
                        inRejectedSize++;
                        for(j=0;j<inAppStrSize;j++){
                            strncpy(temp1, inAppStrList[j], 5);
                            if (atoi(temp1) == collision) {
                                strncpy(temp1, inAppStrList[j], 16);
                                inAppStrList[j] = strcat(temp1, "1");
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    printf("FCFS3\n");
    return inRejectedSize;
}
// void FCFS(char** inAppStrList, int inSize, int* inRejectedList, int inRejectedSize) {
// //    int *acceptAppList = (int *)malloc(sizeof(int) * inAppStrSize);
//     int rejectLength = inRejectedSize;
//     int acceptLength = 0;
//     int i;
//     for (i=0;i<inSize;i++) {
//         int* collisions = isCollision(inAppStrList[i],inAppStrList, inSize);
//         int k,l;
//         int collision;
//         int collisionFlag = 0;
//         char* idA = (char*)malloc(sizeof(char) * 5);
//         char* idB = (char*)malloc(sizeof(char) * 5);

//         for (l = 0; l < 5; l++) {
//             idA[l] = inAppStrList[i][l];
//         }
//         for (k=0;k<inSize;k++) {
//             collision = collisions[k];
//             if (collision == -1 && k==0) {
//                 // no collision, accept app
// //                acceptAppList[acceptLength] = inAppStrList[i];
// //                acceptLength++;
//                 break;
//             }
//             else {
//                 // collision found, reject app with higher id only
//                 if (atoi(idA) > collision) {
//                     // rejected
//                     inRejectedList[rejectLength] = atoi(inAppStrList[i]);
//                     rejectLength++;
//                 }
//             }
//         }
//     }
// }

// ID(5)Date(3)Start(4)Duration(3)PR(1) = 16
int PR(char **inAppStrList, int inAppStrSize, int* inRejectedList, int inRejectedSize) {
    printf("mode4: P\n");
    int *acceptAppList = (int *)malloc(sizeof(int) * inAppStrSize);
    int acceptLength = 0;
    int i;

    for (i=0;i<inAppStrSize;i++) {
        int existCheck = 0;
        printf("mode5: P\n");
        int* collisions = isCollision(inAppStrList[i],inAppStrList, inAppStrSize);
        int k,l;
        int collision;
        int collisionFlag = 0;
        char* idA = (char*)malloc(sizeof(char) * 5);
        char* idB = (char*)malloc(sizeof(char) * 5);
        char prA = inAppStrList[i][15];
        int prAStr = prA-'0';
        char prB[1];
        char* temp = (char*)malloc(sizeof(char) * 17);
        for (l = 0; l < 5; l++) {
            idA[l] = inAppStrList[i][l];
        }
        for (k=0;k<inAppStrSize;k++) {
            collision = collisions[k];
            if (collision == -1 && k==0) {
                break;
            }
            else {
                int m;
                for (m=0;m<inAppStrSize;m++) {
                    char* app = inAppStrList[m];
                    int n;
                    for (n = 0; n < 5; n++) {
                        idB[n] = app[n];
                    }
                    if (atoi(idB) == collision) {
                        *prB = app[15]; // priorityB
                        break;
                    }
                    else {
                        continue;
                    }
                }
                // prB
                if (prAStr > atoi(prB)) {
                    // rejected
                    char* temp1 = (char*)malloc(sizeof(char) * 17);
                    strncpy(temp1,inAppStrList[i],5);
                    for (m = 0; m < inRejectedSize; ++m) {
                        if (atoi(temp1)== inRejectedList[m]) {
                            existCheck++;
                            break;
                        }
                    }
                    if (existCheck == 0) {
                        inRejectedList[inRejectedSize] = atoi(temp1);
                        inRejectedSize++;
                        strncpy(temp,inAppStrList[i],16);
                        strcat(temp,"1");
                        inAppStrList[i] = temp;
                        printf(">%d\n",inRejectedList[inRejectedSize-1]);
                    }
                    break;
                }
                else if (prAStr == atoi(prB)) {
                    if (atoi(idA) > collision) {
                        // rejected
                        char *temp1 = (char *) malloc(sizeof(char) * 17);
                        strncpy(temp1, inAppStrList[i], 5);
                        for (m = 0; m < inRejectedSize; ++m) {
                            if (atoi(temp1) == inRejectedList[m]) {
                                existCheck++;
                                break;
                            }
                        }
                        if (existCheck == 0) {
                            inRejectedList[inRejectedSize] = atoi(temp1);
                            inRejectedSize++;
                            strncpy(temp, inAppStrList[i], 16);
                            strcat(temp, "1");
                            inAppStrList[i] = temp;
                            printf(">%d\n", inRejectedList[inRejectedSize - 1]);
                        }
                        break;
                    }
                }
            }
        }
    }
    return inRejectedSize;
}

int scheduleModule(char* inStr, char inMode, int* inRejectedList, int inRejectSize) {
    int size = strlen(inStr);
    if (size == 0) {
        return inRejectSize;
    }
    int numApp = size/17;
    char** appStrList = (char**)malloc(sizeof(char*) * numApp);
    int i;
    for (i=0;i<numApp;i++) {
        appStrList[i] = (char*)malloc(sizeof(char) * 17);
        strncpy(appStrList[i], inStr+(i*17), 17);
        printf("%s\n", appStrList[i]);
    }
    printf("mode: %c\n", inMode);
    if (inMode == 'F') {
        return FCFS(appStrList, numApp, inRejectedList, inRejectSize);
    }
    else if (inMode == 'P') {
        printf("mode2: %c\n", inMode);
        int out = PR(appStrList, numApp, inRejectedList, inRejectSize);
        printf("mode3: %c\n", inMode);
        return out;
    }
    return 0;
}

// ---------- End of Scheduling Module ----------


// ---------- Output Module Function ----------
void outputModule(char* inStr) {
    int fileIndex = 0; 
    char* filename = (char *)malloc(sizeof(char) * 20);
    
//     int size = strlen(inStr);
//     if (size == 0) {
//         return inStr;
//     }
//     int numApp = size/17;
//     char** appStrList = (char**)malloc(sizeof(char*) * numApp);
//     int i;
//     for (i=0;i<numApp;i++) {
//         appStrList[i] = (char*)malloc(sizeof(char) * 17);
//         strncpy(appStrList[i], inStr+(i*17), 17);
//         printf("%s\n", appStrList[i]);
//     }
//     return inStr;
}

void outputReject(char **inAppStrList, int inRejectSize, char **inTotalUserList, int totalUserNum, char *filename) {
    int appLength = 27;
    int i = 0;
    int j = 0;
    char *id = (char *)malloc(sizeof(char) * 5);
    char *date = (char *)malloc(sizeof(char) * 3);
    char *startTime = (char *)malloc(sizeof(char) * 4);
    char *duration = (char *)malloc(sizeof(char) * 3);
    char *dateForOut = (char *)malloc(sizeof(char) * 10);
    char *startTimeForOut = (char *)malloc(sizeof(char) * 10);
    char priority;
    char *activeUsers = (char *)malloc(sizeof(char) * 10);
    char *activeUsersStr = (char *)malloc(sizeof(char) * 100);
    char *caller = (char *)malloc(sizeof(char) * 20);
    char *endTForOut = (char *)malloc(sizeof(char) * 10);
    char *AppointmentName = (char *)malloc(sizeof(char) * 20);
    char *appointment = (char *)malloc(sizeof(char) * 100);
    FILE *fp = NULL;
    fp = fopen(filename, "a");
    fprintf(fp, "\nAltogether there are %d appointments rejected\n  Date\t\tStart\tEnd\t\t\tType\t\t\t\tPeople\n", inRejectSize);
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "=========");
    }
    fprintf(fp, "==================================================================================================\n");    
    
    for (i = 0; i < inRejectSize; i++)
    {
        fprintf(fp, "%d.", i);    
        // get id (0:4)
        for (j = 0; j < 5; j++)
        {
            id[j] = inAppStrList[i][j];
        }

        // get date (5:7)
        for (j = 0; j < 3; j++)
        {
            date[j] = inAppStrList[i][j + 5];
        }

        // get startTime (8:10)
        for (j = 0; j < 4; j++)
        {
            startTime[j] = inAppStrList[i][j + 8];
        }

        // get duration (11:13)
        for (j = 0; j < 3; j++)
        {
            duration[j] = inAppStrList[i][j + 11];
        }

        // get priority (15)
        priority = inAppStrList[i][15];

        // get activeUsers (17:24)
        for (j = 0; j < 10; j++)
        {
            activeUsers[j] = inAppStrList[i][j + 17];
        }

        for (j = 0; j < 100; j++)
        {
            appointment[j] = ' ';
        }

        dateForOut = int2Str(atoi(date) - 100 + 20230500, 10);
        for (j = 0; j < 8; j++)
        {
            appointment[j] = dateForOut[j];
        }

        startTimeForOut = int2Str(((atoi(startTime) - 1000) / 60 * 100 + 1800) + (atoi(startTime - 1000) % 60), 10);
        for (j = 12; j < 16; j++)
        {
            appointment[j] = startTimeForOut[j - 12];
        }

        endTForOut = int2Str(atoi(startTimeForOut) + ((atoi(duration)-100) / 60 * 100) + ((atoi(duration)-100) % 60), 10);
        for (j = 20; j < 24; j++)
        {
            appointment[j] = endTForOut[j - 20];
        }


        // get AppointmentName
        if (priority == '1')
        {
            strcpy(AppointmentName, "privateTime    ");
        }
        else if (priority == '2')
        {
            strcpy(AppointmentName, "projectMeeting ");
        }
        else if (priority == '3')
        {
            strcpy(AppointmentName, "groupStudy     ");
        }
        else if (priority == '4')
        {
            strcpy(AppointmentName, "gathering      ");
        }
        for (j = 28; j < 48; j++)
        {
            appointment[j] = AppointmentName[j - 28];
        }
        fprintf(fp, "%s", appointment);
        fprintf(fp, "\t\t");
        for (j = 0; j < 10; j++)
        {
            if (activeUsers[j] == '1')
            {
                strcpy(activeUsersStr, inTotalUserList[j]);
                
                fprintf(fp, "%s", activeUsersStr);
                fprintf(fp, "  ");
            }
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n\t\t\t\t\t- End of Rejected List -\n");
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "=========");
    }
    fprintf(fp, "===================================================================================================\n");
    fclose(fp);
}
float outputSingleUser(char **inAppStrList, int inAppStrSize, char **inTotalUserList, int totalUserNum, int inUserIndex, char *filename, int schedulePeriod) {
    // inAppStrList[index]: 27 = 5(id: 0-4)+3(date: 5-7)+4(startT: 8-11)+3(duration: 12-14)+1(priority: 15)+1(status: 16)+10(17-26)
    int appLength = 27;
    int i = 0;
    int j = 0;
    char *id = (char *)malloc(sizeof(char) * 5);
    char *date = (char *)malloc(sizeof(char) * 3);
    char *startTime = (char *)malloc(sizeof(char) * 4);
    char *duration = (char *)malloc(sizeof(char) * 3);
    char *dateForOut = (char *)malloc(sizeof(char) * 10);
    char *startTimeForOut = (char *)malloc(sizeof(char) * 10);
    char priority;
    char *activeUsers = (char *)malloc(sizeof(char) * 10);
    char *activeUsersStr = (char *)malloc(sizeof(char) * 100);
    char *caller = (char *)malloc(sizeof(char) * 20);
    char *endTForOut = (char *)malloc(sizeof(char) * 10);
    char *AppointmentName = (char *)malloc(sizeof(char) * 20);
    char *appointment = (char *)malloc(sizeof(char) * 100);
    float totalSingleUseTime = 0.0;
    int totalTime = 300*schedulePeriod; // total mins for 31 days
    FILE *fp = NULL;
    fp = fopen(filename, "a");
    strcpy(caller, inTotalUserList[inUserIndex]);
    fprintf(fp, "\n%s, you have %d appointments\n  Date\t\tStart\tEnd\t\t\tType\t\t\t\tPeople\n", caller, inAppStrSize);
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "=========");
    }
    fprintf(fp, "==================================================================================================\n");    
    for (i = 0; i < inAppStrSize; i++)
    {
        // get id (0:4)
        for (j = 0; j < 5; j++)
        {
            id[j] = inAppStrList[i][j];
        }

        // get date (5:7)
        for (j = 0; j < 3; j++)
        {
            date[j] = inAppStrList[i][j + 5];
        }

        // get startTime (8:10)
        for (j = 0; j < 4; j++)
        {
            startTime[j] = inAppStrList[i][j + 8];
        }

        // get duration (11:13)
        for (j = 0; j < 3; j++)
        {
            duration[j] = inAppStrList[i][j + 11];
        }

        // get priority (15)
        priority = inAppStrList[i][15];

        // get activeUsers (17:24)
        for (j = 0; j < 10; j++)
        {
            activeUsers[j] = inAppStrList[i][j + 17];
        }

        for (j = 0; j < 100; j++)
        {
            appointment[j] = ' ';
        }

        dateForOut = int2Str(atoi(date) - 100 + 20230500, 10);
        for (j = 0; j < 8; j++)
        {
            appointment[j] = dateForOut[j];
        }

        startTimeForOut = int2Str(((atoi(startTime) - 1000) / 60 * 100 + 1800) + (atoi(startTime - 1000) % 60), 10);
        for (j = 12; j < 16; j++)
        {
            appointment[j] = startTimeForOut[j - 12];
        }
        totalSingleUseTime += (atoi(duration)-100);
        endTForOut = int2Str(atoi(startTimeForOut) + ((atoi(duration)-100) / 60 * 100) + ((atoi(duration)-100) % 60), 10);
        for (j = 20; j < 24; j++)
        {
            appointment[j] = endTForOut[j - 20];
        }


        // get AppointmentName
        if (priority == '1')
        {
            strcpy(AppointmentName, "privateTime    ");
        }
        else if (priority == '2')
        {
            strcpy(AppointmentName, "projectMeeting ");
        }
        else if (priority == '3')
        {
            strcpy(AppointmentName, "groupStudy     ");
        }
        else if (priority == '4')
        {
            strcpy(AppointmentName, "gathering      ");
        }
        for (j = 28; j < 48; j++)
        {
            appointment[j] = AppointmentName[j - 28];
        }
        fprintf(fp, "%s", appointment);
        fprintf(fp, "\t\t");
        for (j = 0; j < 10; j++)
        {
            if (activeUsers[j] == '1')
            {
                strcpy(activeUsersStr, inTotalUserList[j]);
                
                fprintf(fp, "%s", activeUsersStr);
                fprintf(fp, "  ");
            }
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n\t\t\t\t\t- End of %s List -\n", caller);
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "=========");
    }
    fprintf(fp, "===================================================================================================\n");
    fclose(fp);
    return totalSingleUseTime/totalTime;
}

float* outputSchd(char ***inAppStrList, int *inAppStrSize, char **inTotalUserList, int totalUserNum, char *filename, char* startPeriod, char* endPeriod, char inMode) {
    int i = 0;
    char *startDate = (char *)malloc(sizeof(char) * 10);
    char *endDate = (char *)malloc(sizeof(char) * 10);
    char *algorithmName = (char *)malloc(sizeof(char) * 10);
    float *UtilizationList = (float *)malloc(sizeof(float) * 10);
    int schedulePeriod = 0;

    startDate = int2Str(atoi(startPeriod) - 100 + 20230500, 10);
    endDate = int2Str(atoi(endPeriod) - 100 + 20230500, 10);
    schedulePeriod = atoi(endPeriod) - atoi(startPeriod);
    if (inMode == 'F') {
        strcpy(algorithmName, "FCFS");
    }
    else if (inMode == 'P') {
        strcpy(algorithmName, "PRIORITY");
    }

    FILE *fp = NULL;
    fp = fopen(filename, "a");
    fprintf(fp, "Period: %s to %s\n", startDate, endDate);
    fprintf(fp, "Algorithm used: %s\n", algorithmName);
    for (i = 0; i < totalUserNum; i++) {
        UtilizationList[i] = outputSingleUser(inAppStrList[i], inAppStrSize[i], inTotalUserList, totalUserNum, i, filename, schedulePeriod);
    }
    fclose(fp);
    return UtilizationList;
}



void outputPerformance(int inTotalSize, int* inAcceptSize, int inRejectSize, char **inTotalUserList, int totalUserNum, float *Utilization, char *filename) {
    int i = 0;
    float totalSize = 0.0;
    totalSize = inTotalSize * 1.0;
    int totalAccept = 0;
    for (i = 0; i < totalUserNum; i++) {
        totalAccept += inAcceptSize[i];
    }
    FILE *fp = NULL;
    fp = fopen(filename, "a");
    
    fprintf(fp, "Performance:\n\n");
    fprintf(fp, "Total:\n\t\tNumber of Requests Received: %f\n\t\tNumber of Requests Accepted: %d (%.1f%%)\n\t\tNumber of Requests Rejected: %d (%.1f%%)\n", totalSize, totalAccept, totalAccept/totalSize, inRejectSize, inRejectSize/totalSize);
    fprintf(fp, "Number of Requests Accepted by Individual:\n");
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "\t\t%s\t\t\t\t%d\n", inTotalUserList[i], inAcceptSize[i]);
    }
    fprintf(fp, "\n\nUtilization of Time Slot:\n");
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "\t\t%s\t\t\t\t%.1f%%\n", inTotalUserList[i], Utilization[i]);
    }
    fclose(fp);
}
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
                char buf1[2554];
                char buf2[2554];
                buf1[0] = '\0';
                buf2[0] = '\0';
                char scheduleMode = '0';
                int userNum = 0;

                // 1. Waiting for cmd "H" to receive hello from parent
                int m = read(fd[2*(i-1)][0],buf1,2551);
                // printf("recv %s\n", buf1);
                if (m>0 && buf1[0]=='H') {
                    // printf("Scheduling Module %d <- Parent: %c\n", i, buf1[0]);
                    write(fd[2*(i-1)+1][1], "A", strlen("A")); // write "A" back to ACK parent
                    // printf("Scheduling Module %d -> Parent: 'A'\n", i);
                }

                int c;
                for(c = 0; c<2551 ; c++) buf1[c] = '\0';
                for(c = 0; c<2551 ; c++) buf2[c] = '\0';

                int* outRejectedList = (int*) malloc(sizeof(int)*1500);
                int outRejectedListSize = 0;
                
                while(1) {
                    // printf("waiting read2\n");
                    // 1. Waiting for cmd "H" to receive hello from parent
                    int n = read(fd[2*(i-1)][0],buf1,2551);
                    // printf("recv %s\n", buf1);
                    if (n>0 && buf1[0]=='S') {
                        // printf("recv %c\n", buf1[0]);
                        // start scheduling
                        scheduleMode = buf1[1];
                        int num1 = buf1[2]-'0';
                        int num2 = buf1[3]-'0';
                        userNum = num1*10+num2;
                        // printf("Scheduling Module %d <- Parent: %c\n", i, buf1[0]);
                        // printf("%c, %d\n", scheduleMode, userNum);
                        write(fd[2*(i-1)+1][1], "S", strlen("S")); // write "A" back to ACK parent
                        // printf("Scheduling Module %d -> Parent: 'S'\n", i);
                    }
                    if (n>0 && userNum > 0 && buf1[0]=='A') {
                        // start scheduling
                        char* recvAppStr;
                        if (strlen(buf1) > 1) {
                            recvAppStr = buf1 + 1;
                            // printf("Scheduling Module Receive Apps %d <- Parent: %s\n", i, recvAppStr);
                            outRejectedListSize = scheduleModule(recvAppStr, scheduleMode, outRejectedList, outRejectedListSize);
                        }
                        char* outRejectedStr = (char*) malloc(sizeof(char)*5*outRejectedListSize+1+4);
                        char* rejectedNumStr = (char*) malloc(sizeof(char)*4);
                        rejectedNumStr[0] = outRejectedListSize/1000 + '0';
                        rejectedNumStr[1] = (outRejectedListSize%1000)/100 + '0';
                        rejectedNumStr[2] = (outRejectedListSize%100)/10 + '0';
                        rejectedNumStr[3] = outRejectedListSize%10 + '0';
                        strcat(outRejectedStr, "A");
                        strcat(outRejectedStr, rejectedNumStr);
                        int j;
                        // char outRejectedListStr = (char*) malloc(sizeof(char)*5*outRejectedListSize+1);
                        for (j=0;j<outRejectedListSize;j++) {
                            // outRejectedListStr[j] = int2Str(outRejectedList[j],10);
                            strcat(outRejectedStr, int2Str(outRejectedList[j],10));
                        }
                        write(fd[2*(i-1)+1][1], outRejectedStr, strlen(outRejectedStr)); // write "A" back to ACK parent
                        // Split into 17*n
                        // char* out = outputModule(recvAppStr);
                        // printf("Scheduling Module %d -> Parent: 'A'\n", i);
                    }

                    // 2. Waiting for cmd "E" from parent to exit
                    // int p = read(fd[0][0],buf1,10);
                    if (n>0 && buf1[0]=='E') {
                        // printf("Scheduling Module %d <- Parent: %c\n", i, buf1[0]);
                        break;
                    }
                    for(c = 0; c<2554 ; c++) buf1[c] = '\0';
                    for(c = 0; c<2554 ; c++) buf2[c] = '\0';
                }
                // close used ends
                close(fd[2*(i-1)+1][1]); // close child out for child -> parent
                close(fd[2*(i-1)][0]); // close child in for parent -> child
                // printf("Scheduling Module %d exit\n", i);
                exit(0); 
            }
            else if (i == 2) { // Output Module
                // close unused ends
                close(fd[2*(i-1)+1][0]); // close child in for child -> parent pipe
                close(fd[2*(i-1)][1]); // close child out for parent -> child pipe
                char buf3[10];
                char buf4[4050];
                char* startTimeStr = (char*) malloc(sizeof(char)*3);
                char* endTimeStr = (char*) malloc(sizeof(char)*3);
                char* schdMode = (char*) malloc(sizeof(char)*1);
                char* userNumStr = (char*) malloc(sizeof(char)*2);
                char* appsCountStr = (char*) malloc(sizeof(char)*4);
                char* rejectedCountStr = (char*) malloc(sizeof(char)*4);
                char* userNamesStr = (char*) malloc(sizeof(char)*200);
                int userNum = 0;
                int appsCount = 0;
                int rejectedCount = 0;
                int d;
                for(d = 0; d<10 ; d++) buf3[d] = '\0';
                for(d = 0; d<4050 ; d++) buf4[d] = '\0';
                // 1. Waiting for cmd "H" to receive hello from parent
                int m = read(fd[2*(i-1)][0],buf3,10);
                if (m>0 && buf3[0]=='H') {
                    // printf("Output Module %d <- Parent: %c\n", i, buf1[0]);
                    write(fd[2*(i-1)+1][1], "A", strlen("A")); // write "A" back to ACK parent
                    // printf("Output Module %d -> Parent: 'A'\n", i);
                }
                while(1) {
                    int n = read(fd[2*(i-1)][0],buf4,218);
                    if (n>0 && buf4[0]=='O') {
                        strncpy(startTimeStr, buf4+1, 3);
                        strncpy(endTimeStr, buf4+4, 3);
                        strncpy(schdMode, buf4+7, 1);
                        strncpy(appsCountStr, buf4+8, 4);
                        strncpy(rejectedCountStr, buf4+12, 4);
                        strncpy(userNumStr, buf4+16, 2);
                        appsCount = atoi(appsCountStr)-1000;
                        rejectedCount = atoi(rejectedCountStr)-1000;
                        userNum = atoi(userNumStr)-10;
                        strncpy(userNamesStr, buf4+18, userNum*20);
                        printf("> Output Module %d <- Parent: %s\n", i, buf4);
                        printf("%s %s %s %d %d %d %s\n", startTimeStr, endTimeStr, schdMode, appsCount,rejectedCount, userNum, userNamesStr);
                        write(fd[2*(i-1)+1][1], "O", strlen("O"));
                    }
                    if (n>0 && buf4[0]=='A') {
                        printf(">2 Output Module %d <- Parent: %s\n", i, buf4);
                        write(fd[2*(i-1)+1][1], "A", strlen("A"));
                    }

                    // 2. Waiting for cmd "E" from parent to exit
                    if (n>0 && buf4[0]=='E') {
                        // printf("Output Module %d <- Parent: %c\n", i, buf2[0]);
                        break;
                    }
                    for(d = 0; d<10 ; d++) buf3[d] = '\0';
                    for(d = 0; d<4050 ; d++) buf4[d] = '\0';
                }
                // close used ends
                close(fd[2*(i-1)+1][1]); // close child out for child -> parent
                close(fd[2*(i-1)][0]); // close child in for parent -> child
                // printf("Output Module %d exit\n", i);
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
        // printf("Parent -> CP %d: 'H'\n", j);
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
                // printf("Parent <- CP %d: %s\n",j,buf3);
                idCount++;
            }
        }
        if (idCount==2) {
            // printf("Parent: All children ACKed\n");
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
                
                char* cmdToken = strtok(cmdCopy, " ");
                // appointment types
                if (strcmp(cmdToken, "privateTime") == 0 || strcmp(cmdToken, "projectMeeting") == 0 || 
                    strcmp(cmdToken, "groupStudy") == 0 || strcmp(cmdToken, "gathering") == 0) {
                    recordApp(cmdList[i]);
                    printf("-> [Recorded]\n");
                }
                // printSchd command
                else if (strcmp(cmdToken, "printSchd") == 0) {
                    // printf("1\n");
                    char* cmdCopy2 = (char*) malloc(sizeof(cmdList[i]));
                    strcpy(cmdCopy2,cmdList[i]);
                    // printf("%s\n",cmdList[i]);
                    char* cmdToken2 = cmdList[i] + 10;
                    char scheduleType = 'A';
                    // printf("%s\n",cmdToken2);
                    if (strcmp(cmdToken2, "FCFS") == 0) {scheduleType = 'F';}
                    else if (strcmp(cmdToken2, "PRIORITY") == 0) {scheduleType = 'P';}
                    else if (strcmp(cmdToken2, "ALL") == 0) {scheduleType = 'A';}

                    char* scheduleCmd = (char*) malloc(sizeof(char)*4+1);
                    scheduleCmd[0] = 'S';
                    scheduleCmd[1] = scheduleType;
                    if (userSize<=9) {
                        scheduleCmd[2] = '0';
                        scheduleCmd[3] = '0'+userSize;
                    }
                    else {
                        scheduleCmd[2] = '0'+userSize/10;
                        scheduleCmd[3] = '0'+userSize%10;
                    }
                    // printf("%s\n", scheduleCmd);
                    write(fd[0][1], scheduleCmd, strlen(scheduleCmd)); // write to schedule module
                    // printf("Parent sent apps to Schedule Module: %s\n", scheduleCmd);
                    // wait for schedule module to send ack
                    while(1) {
                        // printf("waiting\n");
                        char buf1[10];
                        int m = read(fd[1][0],buf1,1);
                        if (m>0 && buf1[0]=='S') { // get ack from schedule module
                            // printf("Parent <- Schedule Module: %s\n", buf1);
                            break;
                        }
                    }

                    int j;
                    char buf2[755]; // 5*150+1+4
                    for (j=0; j<userSize; j++) {
                        User* currUser = totalUserList[j];
                        int userAppSize = currUser->appCount;
                        char* userAppStr = (char*) malloc(sizeof(char)*(userAppSize*17+1));
                        int d;
                        for(d = 0; d<(userAppSize*17+1); d++) userAppStr[d] = '\0';

                        // strcat(userAppStr, "A"); // A for App
                        userAppStr[0]='A';
                        
                        int k;
                        // convert user's all app into appStr, and concat into userAppStr
                        for (k=0;k<userAppSize;k++) {
                            int currAppId = currUser->appLists[k];
                            Appointment* currApp = totalAppointmentList[currAppId];
                            strcat(userAppStr, appToString(currApp));
                            // printf("%s\n",appToString(currApp));
                        }
                        // printf("Parent -> Schedule Module: %s\n", userAppStr);
                        write(fd[0][1], userAppStr, strlen(userAppStr)); // write to schedule module

                        // wait for schedule module to send ack
                        while(1) {
                            int m = read(fd[1][0],buf2,755);
                            if (m>0 && buf2[0]=='A') { // get ack from schedule module
                            printf("Parent <- Schedule Module: %s\n", buf2+1+4);
                                break;
                            }
                        }
                    }
                    
                    // output module command
                    // get reject count
                    
                    char* rejectCountStr = (char*) malloc(sizeof(char)*4+1);
                    strncpy(rejectCountStr,buf2+1,4);
                    int rejectCount = atoi(rejectCountStr);
                    int* rejectList = (int*) malloc(sizeof(int)*rejectCount);
                    int r;
                    for (r=0;r<rejectCount;r++) {
                        char* rejectIdStr = (char*) malloc(sizeof(char)*5);
                        strncpy(rejectIdStr,buf2+1+4+5*r,5);
                        rejectList[r] = atoi(rejectIdStr)-10000;
                    }
                    

                    char* outputInitCmd = (char*) malloc(sizeof(char)*(10+userSize*20+8));

                    strcat(outputInitCmd, "O");
                    strcat(outputInitCmd, int2Str(startTime-20230400,10)); // start time
                    strcat(outputInitCmd, int2Str(endTime-20230400,10)); // end time
                    // outputInitCmd[8]=scheduleType; // schedule mode
                    char temp[2]; 
                    temp[0] = scheduleType; 
                    temp[1] = '\0';
                    strcat(outputInitCmd, temp); // schedule mode
                    strcat(outputInitCmd, int2Str(appCount+1000,10)); // apps count
                    strcat(outputInitCmd, int2Str(rejectCount+1000,10)); // rejected count
                    strcat(outputInitCmd, int2Str(userSize+10,10)); // user size
                    // strcat(outputInitCmd, int2Str(userSize%10, 10)); // user size
                    int p;
                    for (p=0;p<userSize;p++) { // user names
                        char* userNameStr = (char*) malloc(sizeof(char)*20);
                        char* userName = totalUserList[p]->userName;

                        strcpy(userNameStr,userName);
                        int c;
                        for(c = strlen(userName); c<20; c++) userNameStr[c] = '1';
                        strcat(outputInitCmd, userNameStr);
                    }
                    write(fd[2][1], outputInitCmd, strlen(outputInitCmd)); // write to output module
                    char buf3[10];
                    while(1) {
                        int m = read(fd[3][0],buf3,10);
                        if (m>0 && buf3[0]=='O') { // get ack from schedule module
                        printf("!!!Parent <- output Module: %s\n", buf3);
                            break;
                        }
                    }

                    // send all user's apps to output module
                    int a;
                    for (a=0;a<userSize;a++) {
                        User* currUser = totalUserList[a];
                        int userAppSize = currUser->appCount;
                        char* userAppStr = (char*) malloc(sizeof(char)*(userAppSize*27+1));
                        strcat(userAppStr, "A"); // A for App
                        int k;
                        for (k=0;k<userAppSize;k++) {
                            int currAppId = currUser->appLists[k];
                            int isReject = 0;
                            int l;
                            for (l=0;l<rejectCount;l++) {
                                if (currAppId == rejectList[l]) {
                                    isReject = 1;
                                    break;
                                }
                            }
                            if (isReject == 1) continue;
                            Appointment* currApp = totalAppointmentList[currAppId];
                            strcat(userAppStr, appToString2(currApp)); // get 27 char app string
                        }
                        write(fd[2][1], userAppStr, strlen(userAppStr)); // write to output module
                        
                        char buf4[10];
                        while(1) {
                            int m = read(fd[3][0],buf4,10);
                            if (m>0 && buf4[0]=='A') { // get ack from schedule module
                            printf("Parent <- output Module: %s\n", buf4);
                                break;
                            }
                        }

                    }

                    printf("-> [Exported file: G04_01_FCFS.txt]\n");
                }
                else if (strcmp(cmdList[i], "endProgram") == 0) {
                    printf("-> Bye!\n");
                    // 3. send cmd "E" to child for exit
                    for (j=1;j<=2;j++) {
                        // printf("Parent -> CP %d: 'E'\n", j);
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
                    // printf("Parent exit\n");
                    exit(0);
                }
                else {
                    printf("-> [Invalid Command]\n");
                }
            }
        } 
    }
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
// 比如日期转化为101-131，时间转化为1000-1300，持续时间100-159，最后一位为priority

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


