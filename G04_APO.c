#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

typedef struct User User;
typedef struct Appointment Appointment;
int getEndTime(int inTime, float inDuration);
void standardName(char* inName);

Appointment** totalAppointmentList;
User** totalUserList;

int userSize;
int startTime;
int endTime;
char** users = {0};
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
    int* user;
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
    self->appLists=(int*) malloc(sizeof(int)*300);
    return self;
}

void addAppointment(User* inUser, int inApp) {
    if (inUser->appCount<300) {
        inUser->appLists[inUser->appCount]=inApp;
        inUser->appCount++;
    }
    else {
        return;
    }
}

// --------Appointment Class Methods----------------
Appointment* newAppointment(int inAppName, int* user, int inUserCount, int inID, int inDate, int inTime, float inDuration) {
    Appointment* self=(Appointment *) malloc(sizeof(Appointment));
    self->appName=inAppName;
    self->user=user;
    self->userCount=inUserCount;
    self->appID=inID;
    self->date=inDate;
    self->time=inTime;
    self->duration=inDuration;
    self->status=0;
    return self;
}
// get the appointment name
int getAppName(Appointment* inAppName){
    return inAppName->appName;
}
// get the appointment end time through (startT + duration)
int getEndTime(int inTime, float inDuration){
    return inTime + ((int)inDuration * 100 + (inDuration - ((int)inDuration)) * 60);
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

// convert int to string
char* int2Str(int val, int base){ 
    if (val == 0) {return "0";}
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
    int duration = inDuration*60+100; // 101-400
    int* timeList = (int*) malloc(sizeof(int)*3);
    timeList[0] = date; // 3
    timeList[1] = startTime; // 4
    timeList[2] = duration; // 3
    return timeList;
}

// convert appointment to string
char* appToString(Appointment* inApp) {
    // ID(5)Date(3)Start(4)Duration(3)PR(1)Status(1) = 17
    char* str = (char*) malloc(sizeof(char)*17);
    int ss;
    for (ss=0;ss<17;ss++){
        str[ss]='\0';
    }
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
    // append users in app
    // ID(5)Date(3)Start(4)Duration(3)PR(1)Status(1) = 17
    char* str = (char*) malloc(sizeof(char)*21);
    int sss;
    for (sss=0;sss<21;sss++){
        str[sss]='\0';
    }
    strcat(str,int2Str((10000+inApp->appID),10)); // ID(5): 10000+id
    int* timestamp = getDateTimeTransfer(inApp->date, inApp->time, inApp->duration);
    strcat(str,int2Str(timestamp[0],10)); // Date(3)
    strcat(str,int2Str(timestamp[1],10)); // Start(4)
    strcat(str,int2Str(timestamp[2],10)); // Duration(3)
    strcat(str,int2Str(getAppName(inApp),10)); // PR(1)
    strcat(str,"0"); // Status(1) initial status is 0 (not rejected)
    int i = 0;
    int j = 0;
    int out = 0;
    char* str2 = (char*) malloc(sizeof(char)*10);
    for (i = 0; i<10; i++) str2[i] = '0';
    for (i = 0; i < inApp->userCount; i++) {
        for (j = 0; j < userSize; j++) {
            if (inApp->user[i] == j) {
                str2[j] = '1';
            }
        }
    }

    int weight = 1;
    for (i = 9; i>=0; i--) {
        if (str2[i] == '1') {
            out += weight;
        }
        weight *= 2;
    }
    if (out>999) {
        strcat(str,int2Str(out/1000,10));
        strcat(str,int2Str((out%1000)/100,10));
        strcat(str,int2Str((out%100)/10,10));
        strcat(str,int2Str(out%10,10));
    }
    else if (out>99) {
        strcat(str,"0");
        strcat(str,int2Str((out)/100,10));
        strcat(str,int2Str((out%100)/10,10));
        strcat(str,int2Str(out%10,10));
    }
    else if (out>9) {
        strcat(str,"0");
        strcat(str,"0");
        strcat(str,int2Str((out)/10,10));
        strcat(str,int2Str(out%10,10));
    }
    else if (out>=0) {
        strcat(str,"0");
        strcat(str,"0");
        strcat(str,"0");
        strcat(str,int2Str(out,10));
    }
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
    int i = 0;
    int j = 0;
    int inAppLength = 0;
    int n = strlen(inApp);
    char *inAppCopy = (char*)malloc(sizeof(char) * n);
    strcpy(inAppCopy, inApp);

    FILE* fp = fopen("All_Requests.dat", "a");
    fprintf(fp, "%s\n", inAppCopy);
    fclose(fp);

    char *inAppTemp = (char*)malloc(sizeof(char) * n);
    strcpy(inAppTemp, inApp);
    char* tokenTemp = strtok(inAppTemp, " ");
    while (tokenTemp != NULL) {
        inAppLength++;
        tokenTemp = strtok(NULL, " ");
    }
    char** splittedApp = (char**)malloc(sizeof(char*) * inAppLength);
    int appIndex = 0;
    char *token = strtok(inApp, " ");
    while (token != NULL) {
        splittedApp[appIndex] = token;
        appIndex++;
        token = strtok(NULL, " ");
    }

    // encode parameters for appointment from input string
    int appName = 0;
    int* user = (int*)malloc(sizeof(int) * (inAppLength - 4));
    int date = 0;
    int time = 0;
    float duration = 0.0;
    appName = getPriority(splittedApp[0]);
    date = atoi(splittedApp[2]);
    time = atoi(splittedApp[3]);
    duration = atof(splittedApp[4]);

    int t = getDateTimeTransfer(date,time,duration)[1];
    int d = getDateTimeTransfer(date,time,duration)[2]-100;
    
    // Error handling
    if (date < startTime || date > endTime ) {
        printf("-> [Event date out of range: %s]\n",inAppCopy);
        return;
    }
    if (date == 20230501 || date == 20230526 || date == 20230507 || date == 20230514 || date == 20230521 || date == 20230528) {
        printf("-> [No event is allowed in public holiday or sunday: %s]\n",inAppCopy);
        return;
    }
    if (time < 1800 || time >= 2300 ) {
        printf("-> [Event time out of range: %s]\n",inAppCopy);
        return;
    }
    else if (t+d>1300) {
        printf("-> [Event duration out of range: %s]\n",inAppCopy);
        return;
    }
    
    // give caller with a ID from input order
    int callerFound = 0;
    standardName(splittedApp[1]);
    for (i = 0; i < userSize; i++) {
        if (strcmp(totalUserList[i]->userName, splittedApp[1]) == 0) {
            user[0] = i;
            callerFound = 1;
            break;
        }
    }
    // Error handling
    if (callerFound == 0) {
        printf("-> [Caller not found: %s]\n",inAppCopy);
        return;
    }
    // give each callees with a ID from input order
    int calleeCount = 0;
    for (i = 0; i < userSize; i++) {
        for (j = 5; j < inAppLength; j++) {
            standardName(splittedApp[j]);
            if (strcmp(totalUserList[i]->userName, splittedApp[j]) == 0) {
                user[j-4] = i;
                calleeCount++;
            }
        }
    }
    // Error handling
    if (appName == 1 && (calleeCount>0)) {
        printf("-> [No callee can be in privateTime: %s]\n",inAppCopy);
        return;
    }
    else if (appName != 1 && (calleeCount == 0)) {
        printf("-> [At least one callee should be in this appointment: %s]\n",inAppCopy);
        return;
    }
    if (calleeCount != inAppLength-4-1) {
        printf("-> [Callee not found: %s]\n",inAppCopy);
        return;
    }
    Appointment* outAppointment = newAppointment(appName, user, inAppLength-4, appCount, date, time, duration);
    totalAppointmentList[appCount] = outAppointment;
    for (i = 0; i < inAppLength - 4; i++) {
        addAppointment(totalUserList[user[i]], appCount);
    }
    appCount++;
    printf("-> [Recorded: %s]\n", inAppCopy);
}

// command judge for both file and keyboard input
// single cmd: cmd
// file cmd: f filename
char** getCommandList(char* command){
    FILE* fp2;
    char* str = (char*) malloc(sizeof(char)*255);
    char** strList = (char**) malloc(sizeof(char*)*3000);
    int num = 0;
    int n = strlen(command);

    char* cmdCopy = (char*) malloc(sizeof(char)*n);
    strcpy(cmdCopy,command);
    char* split = strtok(cmdCopy," ");
    if(strcasecmp(split,"f") == 0){ // batch file input cmd
        char* split= strtok(NULL," ");
        if((fp2 = fopen(split,"r"))){
            while(!feof(fp2)){
                str=fgets(str,255,fp2);
                if (str != NULL){
                    strList[num] = (char*) malloc(sizeof(char)*255);
                    str[strlen(str)-1] = 0;
                    strcpy(strList[num],str);
                    num++;
                } 
            }
        }
        else{
            fileNum = 0;
            return strList;
        }
        fileNum=num;
        fclose(fp2);
    }
    else { // single command
        strList[num] = (char*) malloc(sizeof(char)*255);
        strcpy(strList[num],command);
        fileNum=1;
    }
    return strList;
}
// ---------- End of Input Module Functions ---------


// ---------- Scheduling Module Function----------

// from all appointments' ID who is collision with inAppA
int* isCollision(char* inAppA, char** inAllApp, int inSize) {
    // ID(5)Date(3)Start(4)Duration(3)PR(1)Status(1) = 17
    int *outCollision = (int *) malloc(sizeof(int) * inSize);
    int collisionCount = 0;
    int appLength = 17;
    int i = 0;
    int j = 0;
    char *idA = (char *) malloc(sizeof(char) * 5);
    char *idB = (char *) malloc(sizeof(char) * 5);
    char *dateA = (char *) malloc(sizeof(char) * 3);
    char *dateB = (char *) malloc(sizeof(char) * 3);
    char *startTimeA = (char *) malloc(sizeof(char) * 4);
    char *startTimeB = (char *) malloc(sizeof(char) * 4);
    char *durationA = (char *) malloc(sizeof(char) * 3);
    char *durationB = (char *) malloc(sizeof(char) * 3);
    char statusA;
    char statusB;

    for (i = 0; i < inSize; ++i) {  //idA
        statusA = inAppA[16];
        outCollision[i] = -1;
        strncpy(idA, inAppA, 5);
        strncpy(idB, inAllApp[i], 5);
        statusB = inAllApp[i][16];
        if (statusA == '1') {
            break;
        }
        if (statusB == '1' || strcmp(idA, idB) == 0) {    //avoid the same id compare and if status is 1, skip compare process
            continue;
        }
        else {   //compare dateA and dateB
            strncpy(dateA, inAppA + 5, 3);
            strncpy(dateB, inAllApp[i] + 5, 3);
            if (strcmp(dateA, dateB) == 0) {
                strncpy(startTimeA, inAppA + 8, 4);
                strncpy(startTimeB, inAllApp[i] + 8, 4);
                strncpy(durationA, inAppA + 12, 3);
                strncpy(durationB, inAllApp[i] + 12, 3);
                if (((atoi(startTimeA) > atoi(startTimeB)) && (atoi(startTimeA) < atoi(startTimeB) + (atoi(durationB)-100)))
                    || ((atoi(startTimeA) < atoi(startTimeB)) && (atoi(startTimeA) + (atoi(durationA)-100) > atoi(startTimeB)))
                    || ((atoi(startTimeA) == atoi(startTimeB)))){
                    outCollision[collisionCount] = atoi(idB);
                    collisionCount++;
                    continue;
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }
    }
    return outCollision;
}

// FCFS scheduling algorithm
int FCFS(char **inAppStrList, int inAppStrSize,int* inRejectedList, int inRejectedSize) {
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    int collision = 0;
    int collisionCount;
    char *idAStr = (char *) malloc(sizeof(char) * 5);
    int idA = 0;    //change with i
    int idB = 0;    //change with j as collision
    bool idAIsInRejectedList;

    for (i = 0; i< inAppStrSize; ++i) {
        idAIsInRejectedList = false;
        collisionCount=0;//all appointment
        strncpy(idAStr, inAppStrList[i], 5);
        idA = atoi(idAStr);
        for (l = 0; l < inRejectedSize; ++l) {
            if (inRejectedList[l] == idA) {
                idAIsInRejectedList = true;
                break;
            }
        }
        int *collisionList = isCollision(inAppStrList[i], inAppStrList, inAppStrSize);
        while(collisionList[collisionCount] != -1){
            collisionCount++;   //get the collision count
        }
        for (j = 0; j < collisionCount; ++j) { //collision appointment
            collision = collisionList[j];   //get the collision ids
            idB = collision;
            if(idA > idB && idAIsInRejectedList == false){
                inRejectedList[inRejectedSize] = idA;
                inRejectedSize++;
                for (k = 0; k<inAppStrSize ; ++k) {
                    char* temp = (char*) malloc(sizeof(char)*5);
                    strncpy(temp, inAppStrList[k], 5);
                    if(atoi(temp) == idA){
                        char *idBStr = (char *) malloc(sizeof(char) * 17);
                        strncpy(idBStr, inAppStrList[k], 16);
                        inAppStrList[k] = strcat(idBStr, "1");
                        break;
                    }
                }
            }
            break;
        }
    }
    for (j = 0; j < inAppStrSize; ++j) {
        inAppStrList[j][16] = '0';
    }
    return inRejectedSize;
}

// Priority scheduling algorithm
int PR(char **inAppStrList, int inAppStrSize,int* inRejectedList, int inRejectedSize){
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    bool idAIsInRejectedList;
    char *idAStr = (char *) malloc(sizeof(char) * 5);
    int idA = 0;    //change with i
    int idB = 0;    //change with j as collision
    char prAStr;
    char prBStr;
    int collisionCount;

    for (i = 0;i < inAppStrSize; ++i) {
        idAIsInRejectedList = false;
        collisionCount = 0;
        strncpy(idAStr, inAppStrList[i], 5);
        idA = atoi(idAStr);
        prAStr = inAppStrList[i][15];

        for (j = 0; j < inRejectedSize; ++j) {
            if (idA == inRejectedList[j]) {
                idAIsInRejectedList = true;
                break;
            }
        }
        int *collisionList = isCollision(inAppStrList[i], inAppStrList, inAppStrSize);
        while(collisionList[collisionCount] != -1){
            collisionCount++;   //get the collision count
        }
        for (k = 0; k < collisionCount; ++k) {
            idB = collisionList[k];
            char *temp = (char *) malloc(sizeof(char) * 5);
            for (l = 0; l < inAppStrSize; ++l) {    //find prB
                strncpy(temp, inAppStrList[l], 5);
                if(atoi(temp) == idB){
                    prBStr = inAppStrList[l][15];
                    break;
                }
            }
            if((prAStr>prBStr && idAIsInRejectedList == false)||(prAStr==prBStr && idA>idB && idAIsInRejectedList == false)){
                inRejectedList[inRejectedSize] = idA;
                inRejectedSize++;
                for (l = 0; l<inAppStrSize ; ++l) {
                    char* temp = (char*) malloc(sizeof(char)*5);
                    strncpy(temp, inAppStrList[l], 5);
                    if(atoi(temp) == idA){
                        char *idBStr = (char *) malloc(sizeof(char) * 17);
                        strncpy(idBStr, inAppStrList[l], 16);
                        inAppStrList[l] = strcat(idBStr, "1");
                        idAIsInRejectedList=true;
                        break;
                    }
                }
            }
        }
    }
    for (j = 0; j < inAppStrSize; ++j) {
        inAppStrList[j][16] = '0';
    }
    return inRejectedSize;
}

// schedule main function
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
    }
    if (inMode == 'F') { // FCFS
        int output = FCFS(appStrList, numApp, inRejectedList, inRejectSize);
        return output;
    }
    else if (inMode == 'P') { // Priority
        int out = PR(appStrList, numApp, inRejectedList, inRejectSize);
        return out;
    }
    return 0;
}

// ---------- End of Scheduling Module ----------


// ---------- Output Module Function ----------

void outputReject(char **inAppStrList, int inRejectSize, char **inTotalUserList, int totalUserNum, char *filename) {
    // int appLength = 27;
    int i = 0;
    int j = 0;
    char *id = (char *)malloc(sizeof(char) * 5);
    char *date = (char *)malloc(sizeof(char) * 3);
    char *startTime = (char *)malloc(sizeof(char) * 4);
    char *duration = (char *)malloc(sizeof(char) * 3);
    char *dateForOut = (char *)malloc(sizeof(char) * 10);
    char *startTimeForOut = (char *)malloc(sizeof(char) * 10);
    char priority;
    char *activeUsers1 = (char *)malloc(sizeof(char) * 4);
    char *activeUsers2 = (char *)malloc(sizeof(char) * 11);
    char *activeUsers = (char *)malloc(sizeof(char) * 10);
    char *activeUsersStr = (char *)malloc(sizeof(char) * 100);
    char *caller = (char *)malloc(sizeof(char) * 20);
    char *endTForOut = (char *)malloc(sizeof(char) * 10);
    char *AppointmentName = (char *)malloc(sizeof(char) * 20);
    char *appointment = (char *)malloc(sizeof(char) * 100);
    FILE *fp = NULL;
    fp = fopen(filename, "a");
    fprintf(fp, "\nAltogether there are %d appointments rejected\nIndex\tDate\t\tStart\tEnd\t\t\tType\t\t\t\tPeople\n", inRejectSize);
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "=========");
    }
    fprintf(fp, "====================================================================================================\n");    
    
    for (i = 0; i < inRejectSize; i++)
    {
        fprintf(fp, "%d.\t", i);    
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
            duration[j] = inAppStrList[i][j + 12];
        }

        // get priority (15)
        priority = inAppStrList[i][15];

        // get activeUsers (17:24)
        for (j = 0; j < 4; j++)
        {
            activeUsers1[j] = inAppStrList[i][j + 17];
        }
        int user1 = atoi(activeUsers1);
        user1 += 1024;
        activeUsers2 = int2Str(user1, 2);
        strcpy(activeUsers,activeUsers2+1);

        for (j = 0; j < 100; j++)
        {
            appointment[j] = ' ';
        }

        dateForOut = int2Str(atoi(date) - 100 + 20230500, 10);
        for (j = 0; j < 8; j++)
        {
            appointment[j] = dateForOut[j];
        }

        startTimeForOut = int2Str(((atoi(startTime) - 1000) / 60 * 100 + 1800) + ((atoi(startTime) - 1000) % 60), 10);
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
    fprintf(fp, "====================================================================================================\n");
    fclose(fp);
}

float outputSingleUser(char **inAppStrList, int inAppStrSize, char **inTotalUserList, int totalUserNum, int inUserIndex, char *filename, int schedulePeriod) {
    // inAppStrList[index]: 27 = 5(id: 0-4)+3(date: 5-7)+4(startT: 8-11)+3(duration: 12-14)+1(priority: 15)+1(status: 16)+10(17-26)
    // int appLength = 27;
    int i = 0;
    int j = 0;
    char *id = (char *)malloc(sizeof(char) * 5);
    char *date = (char *)malloc(sizeof(char) * 3);
    char *startTime = (char *)malloc(sizeof(char) * 4);
    char *duration = (char *)malloc(sizeof(char) * 3);
    char *dateForOut = (char *)malloc(sizeof(char) * 8);
    char *startTimeForOut = (char *)malloc(sizeof(char) * 4);
    char priority;
    char *activeUsers1 = (char *)malloc(sizeof(char) * 4);
    char *activeUsers2 = (char *)malloc(sizeof(char) * 11);
    char *activeUsers = (char *)malloc(sizeof(char) * 10);
    char *activeUsersStr = (char *)malloc(sizeof(char) * 20);
    char *caller = (char *)malloc(sizeof(char) * 20);
    char *endTForOut = (char *)malloc(sizeof(char) * 4);
    char *AppointmentName = (char *)malloc(sizeof(char) * 20);
    char *appointment = (char *)malloc(sizeof(char) * 100);
    float totalSingleUseTime = 0.0;
    int totalTime = 300*(schedulePeriod+1); // total mins for 31 days

    FILE *fp = NULL;
    fp = fopen(filename, "a");
    strcpy(caller, inTotalUserList[inUserIndex]);
    fprintf(fp, "\n%s, you have %d appointments\nDate\tStart\tEnd\t\tType\t\t\tPeople\n", caller, inAppStrSize);
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "=========");
    }
    fprintf(fp, "====================================================================================================\n");    
    
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

        // get startTime (8:11)
        for (j = 0; j < 4; j++)
        {
            startTime[j] = inAppStrList[i][j + 8];
        }

        // get duration (12:14)
        for (j = 0; j < 3; j++)
        {
            duration[j] = inAppStrList[i][j + 12];
        }

        // get priority (15)
        priority = inAppStrList[i][15];

        // get activeUsers (17:24)
        for (j = 0; j < 4; j++)
        {
            activeUsers1[j] = inAppStrList[i][j + 17];
        }
        int user1 = atoi(activeUsers1);
        user1 += 1024;
        activeUsers2 = int2Str(user1, 2);
        strcpy(activeUsers,activeUsers2+1);


        for (j = 0; j < 100; j++)
        {
            appointment[j] = ' ';
        }

        
        dateForOut = int2Str(atoi(date) - 100 + 20230500, 10);
        for (j = 0; j < 8; j++)
        {
            appointment[j] = dateForOut[j];
        }

        startTimeForOut = int2Str(((atoi(startTime) - 1000) / 60 * 100 + 1800) + ((atoi(startTime) - 1000) % 60), 10);
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
    fprintf(fp, "====================================================================================================\n");
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

    strcpy(startDate, int2Str((atoi(startPeriod) - 100 + 20230500), 10));
    strcpy(endDate,int2Str((atoi(endPeriod) - 100 + 20230500), 10));
    schedulePeriod = atoi(endPeriod) - atoi(startPeriod);
    if (inMode == 'F') {
        strcpy(algorithmName, "FCFS");
    }
    else if (inMode == 'P') {
        strcpy(algorithmName, "PRIORITY");
    }

    FILE *fp = NULL;
    fp = fopen(filename, "w");
    fprintf(fp, "Period: %s to %s\n", startDate, endDate);
    fprintf(fp, "Algorithm used: %s\n", algorithmName);
    fclose(fp);
    for (i = 0; i < totalUserNum; i++) {
        UtilizationList[i] = outputSingleUser(inAppStrList[i], inAppStrSize[i], inTotalUserList, totalUserNum, i, filename, schedulePeriod);
    }
    return UtilizationList;
}

void outputPerformance(int inTotalSize, int* inAcceptSize, int inRejectSize, char **inTotalUserList, int totalUserNum, float *Utilization, char *filename) {
    int i = 0;
    float totalSize = 0.0;
    totalSize = inTotalSize * 1.0;
    int totalAccept = inTotalSize-inRejectSize;
    FILE *fp = NULL;
    fp = fopen(filename, "a");

    fprintf(fp, "\nPerformance:\n");
    fprintf(fp, "\nTotal:\n\t\tNumber of Requests Received: %.0f\n\t\tNumber of Requests Accepted: %d (%.1f%%)\n\t\tNumber of Requests Rejected: %d (%.1f%%)\n", totalSize, totalAccept, totalAccept/totalSize*100, inRejectSize, inRejectSize/totalSize*100);
    fprintf(fp, "\n\nNumber of Requests Accepted by Individual:\n");
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "\t\t%s\t\t\t\t%d\n", inTotalUserList[i], inAcceptSize[i]);
    }
    fprintf(fp, "\n\nUtilization of Time Slot:\n");
    for (i = 0; i < totalUserNum; i++) {
        fprintf(fp, "\t\t%s\t\t\t\t%.1f%%\n", inTotalUserList[i], Utilization[i]*100);
    }
    fclose(fp);
}

void outputModule(char*** inTotalAppStrList, int* inUserAppCountList, char* inTotalUserName, 
                int inTotalUserNum, int fileNum, char* inStartTime, char* inEndTime, 
                char* inMode, char** inAllUserRejectAppList, int inRejectNum, int inAllAppNum) {
    
    char** totalUserNames = (char**)malloc(sizeof(char*) * inTotalUserNum);
    int i;
    for (i=0;i<inTotalUserNum;i++) {
        totalUserNames[i] = (char*)malloc(sizeof(char) * 20);
        strncpy(totalUserNames[i], inTotalUserName+(i*20), 20);
        int j;
        for (j=0;j<20;j++) {
            if (totalUserNames[i][j] == '1') {
                totalUserNames[i][j] = '\0';
            }
        }
    }
    char schdMode = inMode[0];
    char* filename = (char *)malloc(sizeof(char) * 20);
    int f;
    for (f=0;f<20;f++) {
        filename[f] = '\0';
    }
    strcat(filename, "G04_");
    if (fileNum<10) {
        strcat(filename, "0");
        strcat(filename, int2Str(fileNum,10));
    }
    else {
        strcat(filename, int2Str(fileNum,10));
    }
    strcat(filename, "_");
    if (schdMode == 'F') {
        strcat(filename, "FCFS.txt");
    }
    else if (schdMode == 'P') {
        strcat(filename, "PRIORITY.txt");
    }
    // start output
    float *Utilization = (float*)malloc(sizeof(float) * inTotalUserNum);
    Utilization = outputSchd(inTotalAppStrList, inUserAppCountList, totalUserNames, inTotalUserNum, filename, inStartTime, inEndTime, schdMode);
    outputReject(inAllUserRejectAppList, inRejectNum, totalUserNames, inTotalUserNum, filename);
    outputPerformance(inAllAppNum, inUserAppCountList, inRejectNum, totalUserNames, inTotalUserNum, Utilization, filename);
    printf("-> [Exported file: %s]\n",filename);
}

// ---------- End of Output Module ----------

// main function
int main(int argc, char *argv[]) {
    
    // ---------- Initialization ----------
    userSize = argc-3;
    startTime = atoi(argv[1]);
    endTime = atoi(argv[2]);
    users = (char**) malloc(sizeof(char*)*10);
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
    totalAppointmentList=(Appointment**) malloc(sizeof(Appointment*)*3000);
    // initialize user list
    totalUserList=(User**) malloc(sizeof(User*)*userSize);
    
    int k;
    for (k=0;k<userSize;k++) {
        standardName(users[k]); // convert user name to standard format
        User* user = newUser(users[k]); // new User object
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
            if (i == 1) {
    // ---------- Scheduling Module ----------
                // close unused ends
                close(fd[2*(i-1)+1][0]); // close child in for child -> parent pipe
                close(fd[2*(i-1)][1]); // close child out for parent -> child pipe
                char buf1[5104];
                char buf2[10];
                buf1[0] = '\0';
                buf2[0] = '\0';
                char scheduleMode = '0';
                int userNum = 0;

                // Waiting for cmd "H" to receive hello from parent
                int m = read(fd[2*(i-1)][0],buf2,10);
                if (m>0 && buf2[0]=='H') {
                    write(fd[2*(i-1)+1][1], "A", strlen("A")); // write "A" back to ACK parent
                }

                int c;
                for(c = 0; c<5104 ; c++) buf1[c] = '\0';
                
                int* outRejectedList = (int*) malloc(sizeof(int)*3000);
                int outRejectedListSize = 0;
                
                while(1) {
                    int n = read(fd[2*(i-1)][0],buf1,5104);
                    if (n>0 && buf1[0]=='S') {
                        // scheduling initialization
                        scheduleMode = buf1[1];
                        int num1 = buf1[2]-'0';
                        int num2 = buf1[3]-'0';
                        userNum = num1*10+num2;
                        outRejectedListSize = 0;
                        int d;
                        for (d=0;d<3000;d++) outRejectedList[d] = -1;
                        write(fd[2*(i-1)+1][1], "S", strlen("S")); // write "S" back to ACK parent
                    }
                    if (n>0 && userNum > 0 && buf1[0]=='A') {
                        // start scheduling
                        char* recvAppStr;
                        if (strlen(buf1) > 1) {
                            recvAppStr = buf1 + 1;
                            // pass all appointment string to scheduling module
                            outRejectedListSize = scheduleModule(recvAppStr, scheduleMode, outRejectedList, outRejectedListSize);
                        }
                        char* outRejectedStr = (char*) malloc(sizeof(char)*5*outRejectedListSize+1+4);
                        int g;
                        for (g=0;g<(5*outRejectedListSize+1+4);g++) {outRejectedStr[g] = '\0';}
                        char* rejectedNumStr = (char*) malloc(sizeof(char)*4);
                        rejectedNumStr[0] = outRejectedListSize/1000 + '0';
                        rejectedNumStr[1] = (outRejectedListSize%1000)/100 + '0';
                        rejectedNumStr[2] = (outRejectedListSize%100)/10 + '0';
                        rejectedNumStr[3] = outRejectedListSize%10 + '0';
                        strcat(outRejectedStr, "A");
                        strcat(outRejectedStr, rejectedNumStr);
                        int j;
                        for (j=0;j<outRejectedListSize;j++) {
                            strcat(outRejectedStr, int2Str(outRejectedList[j],10));
                        }
                        // write rejected data back to parent
                        write(fd[2*(i-1)+1][1], outRejectedStr, strlen(outRejectedStr));
                        for (g=0;g<(5*outRejectedListSize+1+4);g++) {outRejectedStr[g] = '\0';}
                    }

                    // Waiting for cmd "E" from parent to exit
                    if (n>0 && buf1[0]=='E') {
                        break;
                    }
                    for(c = 0; c<5104 ; c++) buf1[c] = '\0';

                }
                
                // close used ends
                close(fd[2*(i-1)+1][1]); // close child out for child -> parent
                close(fd[2*(i-1)][0]); // close child in for parent -> child
                exit(0); 
            }
            else if (i == 2) { 
    // ---------- Output Module ----------
                // close unused ends
                close(fd[2*(i-1)+1][0]); // close child in for child -> parent pipe
                close(fd[2*(i-1)][1]); // close child out for parent -> child pipe
                char buf3[10];
                char buf4[6301];
                char* startTimeStr = (char*) malloc(sizeof(char)*3);
                char* endTimeStr = (char*) malloc(sizeof(char)*3);
                char* schdMode = (char*) malloc(sizeof(char)*1);
                char* userNumStr = (char*) malloc(sizeof(char)*2);
                char* outputCallCountStr = (char*) malloc(sizeof(char)*4);
                char* appsCountStr = (char*) malloc(sizeof(char)*4);
                char* rejectedCountStr = (char*) malloc(sizeof(char)*4);
                char* userNamesStr = (char*) malloc(sizeof(char)*200);
                int userNum = 0;
                int appsCount = 0;
                int outputCallCount = 0;
                int rejectedCount = 0;
                int recvUserCount = 0;
                char*** allUserAppStrList = {0};
                char** allUserRejectedAppStrList = {0};
                int* allUserAppCountList = {0};
                int d;
                for(d = 0; d<10 ; d++) buf3[d] = '\0';
                for(d = 0; d<6301 ; d++) buf4[d] = '\0';
                // Waiting for cmd "H" to receive hello from parent
                int m = read(fd[2*(i-1)][0],buf3,10);
                if (m>0 && buf3[0]=='H') {
                    write(fd[2*(i-1)+1][1], "A", strlen("A")); // write "A" back to ACK parent
                }
                
                while(1) {
                    int n = read(fd[2*(i-1)][0],buf4,6301);
                    if (n>0 && buf4[0]=='O') {
                        // get initial Output info msg
                        strncpy(startTimeStr, buf4+1, 3);
                        strncpy(endTimeStr, buf4+4, 3);
                        strncpy(schdMode, buf4+7, 1);
                        strncpy(outputCallCountStr, buf4+8, 4);
                        strncpy(appsCountStr, buf4+12, 4);
                        strncpy(rejectedCountStr, buf4+16, 4);
                        strncpy(userNumStr, buf4+20, 2);
                        outputCallCount = atoi(outputCallCountStr)-1000;
                        appsCount = atoi(appsCountStr)-1000;
                        rejectedCount = atoi(rejectedCountStr)-1000;
                        userNum = atoi(userNumStr)-10;
                        allUserAppStrList = (char***) malloc(sizeof(char**)*userNum);
                        allUserAppCountList = (int*) malloc(sizeof(int)*userNum);
                        allUserRejectedAppStrList = (char**) malloc(sizeof(char*)*rejectedCount);
                        strncpy(userNamesStr, buf4+22, userNum*20);                    
                        write(fd[2*(i-1)+1][1], "O", strlen("O"));
                    }
                    if (n>0 && buf4[0]=='A') {
                        // user App str msg
                        int appNum = (strlen(buf4)-1)/21;
                        char** singleUserAppList = (char**) malloc(sizeof(char*)*appNum);
                        int q;
                        for (q=0;q<appNum;q++) {
                            singleUserAppList[q] = (char*) malloc(sizeof(char)*21);
                            strncpy(singleUserAppList[q], (buf4+1)+21*q, 21);
                        }
                        allUserAppStrList[recvUserCount] = (char**) malloc(sizeof(char*)*appNum); 

                        // get accepted App str msg
                        allUserAppStrList[recvUserCount] = singleUserAppList;

                        allUserAppCountList[recvUserCount] = appNum; 

                        recvUserCount++;
                        if (recvUserCount == userNum) {

                            recvUserCount = 0;
                        }

                        write(fd[2*(i-1)+1][1], "A", strlen("A"));

                    }
                    if (n>0 && buf4[0]=='R') {
                        // user rejected App str msg
                        int r;
                        for (r=0;r<rejectedCount;r++) {
                            allUserRejectedAppStrList[r] = (char*) malloc(sizeof(char)*21);
                            strncpy(allUserRejectedAppStrList[r], (buf4+1)+21*r, 21);
                        }
                        // pass all data to output module
                        outputModule(allUserAppStrList, allUserAppCountList, userNamesStr, 
                                    userNum, outputCallCount, startTimeStr, endTimeStr,
                                    schdMode, allUserRejectedAppStrList, rejectedCount, appsCount);
                        
                        write(fd[2*(i-1)+1][1], "R", strlen("R"));
                    }

                    // Waiting for cmd "E" from parent to exit
                    if (n>0 && buf4[0]=='E') {
                        break;
                    }
                    int e;
                    for(e = 0; e<6301 ; e++) buf4[e] = '\0';
                }
                // close used ends
                close(fd[2*(i-1)+1][1]); // close child out for child -> parent
                close(fd[2*(i-1)][0]); // close child in for parent -> child
                exit(0); 
            }
        }
    }
    // ---------- End of 2 child processes ----------


    // ---------- Enter Input Module (Parent Process) ----------
    
    // close unused pipe ends
    int j;
    for (j=0;j<2;j++) {
        close(fd[2*(j)+1][1]); // close parent out for child -> parent
        close(fd[2*(j)][0]); // close parent in for parent -> child
    }
    // 1. say hello to all children
    for (j=1;j<=2;j++) { 
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
                idCount++;
            }
        }
        if (idCount==2) {
            break;
        }
    }
    int outputCallCount = 0; //[1000,9999]
    while (1) {
        char* cmd = (char*) malloc(sizeof(char)*255);
        printf("Please enter appointment:\n");
        fgets(cmd, 255, stdin);
        if (strlen(cmd)==1) {
            printf("-> [Invalid Command]\n");
            continue;
        }
        cmd[strlen(cmd)-1] = 0;
        char** cmdList = getCommandList(cmd);
        int cmdListLength = fileNum;
        
        if (cmdListLength >= 1) { // single cmd or cmd from file
            int i;
            for (i=0;i<cmdListLength;i++) {
                int n = strlen(cmdList[i]);
                char* cmdCopy = (char*) malloc(sizeof(char)*n);
                strcpy(cmdCopy,cmdList[i]);
                
                char* cmdToken = strtok(cmdCopy, " ");
                // appointment types
                if (strcmp(cmdToken, "privateTime") == 0 || strcmp(cmdToken, "projectMeeting") == 0 || 
                    strcmp(cmdToken, "groupStudy") == 0 || strcmp(cmdToken, "gathering") == 0) {
                    recordApp(cmdList[i]);
                }
                // printSchd command
                else if (strcmp(cmdToken, "printSchd") == 0) {
                    int n = strlen(cmdList[i]);
                    char* cmdCopy2 = (char*) malloc(sizeof(char)*n);
                    strcpy(cmdCopy2,cmdList[i]);
                    char* cmdToken2 = cmdList[i] + 10;
                    char scheduleType = 'A'; // default is all
                    int outputCount = 2; // default is all
                    if (strcmp(cmdToken2, "FCFS") == 0) {scheduleType = 'F'; outputCount = 1;}
                    else if (strcmp(cmdToken2, "PRIORITY") == 0) {scheduleType = 'P'; outputCount = 1;}
                    else if (strcmp(cmdToken2, "ALL") == 0) {scheduleType = 'A'; outputCount = 2;}
                
                    while(outputCount > 0) {
                        if (scheduleType == 'A') {scheduleType = 'F';}
                    
                        outputCallCount++;
                        char* scheduleCmd = (char*) malloc(sizeof(char)*(4+1));
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
                        // send schedule initialization cmd to schedule module
                        write(fd[0][1], scheduleCmd, strlen(scheduleCmd));
                        // wait for schedule module to send ack
                        while(1) {
                            char buf5[10];
                            int m = read(fd[1][0],buf5,1);
                            if (m>0 && buf5[0]=='S') { // get ack from schedule module
                                break;
                            }
                        }

                        int j;
                        char buf6[1505]; // 5*150+1+4
                        for (j=0; j<userSize; j++) {
                            User* currUser = totalUserList[j];
                            int userAppSize = currUser->appCount;
                            char* userAppStr = (char*) malloc(sizeof(char)*(userAppSize*17+1));
                            int d;
                            for(d = 0; d<(userAppSize*17+1); d++) userAppStr[d] = '\0';
                            userAppStr[0]='A';
                            
                            int k;
                            // convert user's all app into appStr, and concat into userAppStr
                            for (k=0;k<userAppSize;k++) {
                                int currAppId = currUser->appLists[k];
                                Appointment* currApp = totalAppointmentList[currAppId];
                                strcat(userAppStr, appToString(currApp));
                            }

                            write(fd[0][1], userAppStr, strlen(userAppStr)); // write to schedule module
                            for(d = 0; d<(userAppSize*17+1); d++) userAppStr[d] = '\0';
                            // wait for schedule module to send ack
                            while(1) {
                                int m = read(fd[1][0],buf6,1505);
                                if (m>0 && buf6[0]=='A') { // get ack from schedule module
                                    break;
                                }
                            }
                        }

                        // output module command
                        // get reject count
                        char* rejectCountStr = (char*) malloc(sizeof(char)*4+1);
                        strncpy(rejectCountStr,buf6+1,4);
                        int rejectCount = atoi(rejectCountStr);
                        int* rejectList = (int*) malloc(sizeof(int)*rejectCount);
                        int r;
                        for (r=0;r<rejectCount;r++) {
                            char* rejectIdStr = (char*) malloc(sizeof(char)*5);
                            strncpy(rejectIdStr,buf6+1+4+5*r,5);
                            rejectList[r] = atoi(rejectIdStr)-10000;
                        }
                        
                        char* outputInitCmd = (char*) malloc(sizeof(char)*(10+userSize*20+8+4));
                        int h;
                        for (h=0;h<(10+userSize*20+8+4);h++) {outputInitCmd[h] = '\0';}

                        strcat(outputInitCmd, "O");
                        strcat(outputInitCmd, int2Str(startTime-20230400,10)); // start time
                        strcat(outputInitCmd, int2Str(endTime-20230400,10)); // end time
                        char temp[2];
                        temp[0] = scheduleType; 
                        temp[1] = '\0';
                        strcat(outputInitCmd, temp); // schedule mode
                        strcat(outputInitCmd, int2Str(outputCallCount+1000,10)); // output call count
                        strcat(outputInitCmd, int2Str(appCount+1000,10)); // apps count
                        strcat(outputInitCmd, int2Str(rejectCount+1000,10)); // rejected count
                        strcat(outputInitCmd, int2Str(userSize+10,10)); // user size
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
                        for (h=0;h<(10+userSize*20+8+4);h++) {outputInitCmd[h] = '\0';}
                        char buf7[10];
                        while(1) {
                            int m = read(fd[3][0],buf7,10);
                            if (m>0 && buf7[0]=='O') { // get ack from schedule module
                                break;
                            }
                        }
                        
                        // send all user's apps to output module
                        int a;
                        for (a=0;a<userSize;a++) {
                            User* currUser = totalUserList[a];
                            int userAppSize = currUser->appCount;
                            char* userAppListStr = (char*) malloc(sizeof(char)*(userAppSize*21+1));
                            int k;
                            for (k = 0;k<(userAppSize*21+1);k++) {userAppListStr[k]='\0';}
                            strcat(userAppListStr, "A"); // A for App

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
                                if (isReject == 1) {
                                    continue;
                                }
                                Appointment* currApp = totalAppointmentList[currAppId];
                                strcat(userAppListStr, appToString2(currApp)); // get 27 char app string

                            }
                            write(fd[2][1], userAppListStr, strlen(userAppListStr)); // write to output module
                            for (k = 0;k<(userAppSize*21+1);k++) {userAppListStr[k]='\0';}
                            char buf8[10];
                            while(1) {
                                int m = read(fd[3][0],buf8,10);
                                if (m>0 && buf8[0]=='A') { // get ack from output module
                                    break;
                                }
                            }
                        }

                        // send rejected apps to output module
                        char* rejectedAppStr = (char*) malloc(sizeof(char)*(rejectCount*21+1));
                        int s;
                        for (s = 0;s<(rejectCount*21+1);s++) {rejectedAppStr[s]='\0';}
                        strcat(rejectedAppStr, "R"); // A for App
                        for (r=0;r<rejectCount;r++) {
                            int rejectedAppIdx = rejectList[r];
                            strcat(rejectedAppStr, appToString2(totalAppointmentList[rejectedAppIdx])); // get 27 char app string
                        }
                        write(fd[2][1], rejectedAppStr, strlen(rejectedAppStr)); // write to output module
                        for (s = 0;s<(rejectCount*21+1);s++) {rejectedAppStr[s]='\0';}
                        char buf9[10];
                        while(1) {
                            int s = read(fd[3][0],buf9,10);
                            if (s>0 && buf9[0]=='R') { // get ack from output module
                                break;
                            }
                        }
                        if (outputCount == 2) {scheduleType = 'P';}
                        outputCount--;
                    }
                }
                else if (strcmp(cmdList[i], "endProgram") == 0) {
                    printf("-> Bye!\n");
                    // send cmd "E" to child for exit
                    for (j=1;j<=2;j++) {
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
                    exit(0);
                }
                else {
                    printf("-> [Invalid Command]\n");
                }
            }
        } 
        else{
            printf("-> [File not found]\n");
        }
    }
    exit(0);
}
