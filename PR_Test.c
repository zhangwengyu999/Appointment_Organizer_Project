
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

char* int2Str(int val, int base){ // convert int to string
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }
    return &buf[i+1];
}

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

int FCFS(char **inAppStrList, int inAppStrSize,char** inRejectedList, int inRejectedSize) {
    int *acceptAppList = (int *)malloc(sizeof(int) * inAppStrSize);
//    int rejectLength = inRejectedSize;
    int acceptLength = 0;
    int i;

    for (i=0;i<inAppStrSize;i++) {
        int* collisions = isCollision(inAppStrList[i],inAppStrList, inAppStrSize);
        int k,l;
        int collision;
        int collisionFlag = 0;
        char* idA = (char*)malloc(sizeof(char) * 5);
        char* idB = (char*)malloc(sizeof(char) * 5);
//        char prA = inAppStrList[i][15];
//        int prAStr = prA-'0';
//        char prB[1];
        char* temp = (char*)malloc(sizeof(char) * 17);
        for (l = 0; l < 5; l++) {
            idA[l] = inAppStrList[i][l];
        }
        for (k=0;k<inAppStrSize;k++) {
            collision = collisions[k];
            if (collision == -1 && k==0) {
                // no collision, accept app
                // acceptAppList[acceptLength] = inAppStrList[i];
                // acceptLength++;
                // break;
                break;
            }
            else {
                // collision found, reject app with lower priority only
                int m;
                for (m=0;m<inAppStrSize;m++) {
                    char* app = inAppStrList[m];
                    // check app ID with collision
                    // get app's ID
                    int n;
                    for (n = 0; n < 5; n++) {
                        idB[n] = app[n];
                    }
                }
//                // prB
//                if (prAStr > atoi(prB)) {
//                    // rejected
//                    char* temp1 = (char*)malloc(sizeof(char) * 17);
//                    char* temp2 = (char*)malloc(sizeof(char) * 17);
//                    strncpy(temp1,inAppStrList[i]+5,1);
//                    strncpy(temp2,inAppStrList[i],4);
//                    temp2 = int2Str(10000+inRejectedSize,10);
////                    printf("%s\n",temp2);
////                    printf("%s\n",temp1);
////                    printf(".%s\n",temp1);
//                    strcat(temp2,temp1);
//                    inRejectedList[inRejectedSize] = temp;
////                    inRejectedList[inRejectedSize] = inAppStrList[i];
//                    inRejectedSize++;
//                    strncpy(temp,inAppStrList[i],16);
//                    strcat(temp,"1");
//                    inAppStrList[i] = temp;
////                    printf("%s\n",inAppStrList[i]);
//                    printf("size = %d and %s\n",inRejectedSize,inRejectedList[inRejectedSize-1]);
////                    int temp= atoi(inAppStrList[i])+1;
////                    inAppStrList[i] = int2Str(temp,10);
//                    break;
//
//                }
//                else if (prAStr == atoi(prB)) {
                if (atoi(idA) > collision) {
                    // rejected
                    char* temp1 = (char*)malloc(sizeof(char) * 17);
                    char* temp2 = (char*)malloc(sizeof(char) * 17);
                    strncpy(temp1,inAppStrList[i]+5,1);
                    strncpy(temp2,inAppStrList[i],4);
                    temp2 = int2Str(10000+inRejectedSize,10);
//                    printf("%s\n",temp2);
//                    printf("%s\n",temp1);
//                    printf(".%s\n",temp1);
                    strcat(temp2,temp1);
                    inRejectedList[inRejectedSize] = temp;
//                    inRejectedList[inRejectedSize] = inAppStrList[i];
                    inRejectedSize++;
                    strncpy(temp,inAppStrList[i],16);
                    strcat(temp,"1");
                    inAppStrList[i] = temp;
//                    printf("%s\n",inAppStrList[i]);
                    printf("size = %d and %s\n",inRejectedSize,inRejectedList[inRejectedSize-1]);
//                    int temp= atoi(inAppStrList[i])+1;
//                    inAppStrList[i] = int2Str(temp,10);
                    break;

                }
            }
        }
    }
    return inRejectedSize;
}

int PR(char **inAppStrList, int inAppStrSize,char** inRejectedList, int inRejectedSize) {
    int *acceptAppList = (int *)malloc(sizeof(int) * inAppStrSize);
//    int rejectLength = inRejectedSize;
    int acceptLength = 0;
    int i;

    for (i=0;i<inAppStrSize;i++) {
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
                // no collision, accept app
                // acceptAppList[acceptLength] = inAppStrList[i];
                // acceptLength++;
                // break;
                break;
            }
            else {
                // collision found, reject app with lower priority only
                int m;
                for (m=0;m<inAppStrSize;m++) {
                    char* app = inAppStrList[m];
                    // check app ID with collision
                    // get app's ID
                    int n;
                    for (n = 0; n < 5; n++) {
                        idB[n] = app[n];
                    }
                    if (atoi(idB) == collision) {
                        // get app with ID:[collision]
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
                    char* temp2 = (char*)malloc(sizeof(char) * 17);
                    strncpy(temp1,inAppStrList[i]+5,1);
                    strncpy(temp2,inAppStrList[i],4);
                    temp2 = int2Str(10000+inRejectedSize,10);
//                    printf("%s\n",temp2);
//                    printf("%s\n",temp1);
//                    printf(".%s\n",temp1);
                    strcat(temp2,temp1);
                    inRejectedList[inRejectedSize] = temp;
//                    inRejectedList[inRejectedSize] = inAppStrList[i];
                    inRejectedSize++;
                    strncpy(temp,inAppStrList[i],16);
                    strcat(temp,"1");
                    inAppStrList[i] = temp;
//                    printf("%s\n",inAppStrList[i]);
                    printf("size = %d and %s\n",inRejectedSize,inRejectedList[inRejectedSize-1]);
//                    int temp= atoi(inAppStrList[i])+1;
//                    inAppStrList[i] = int2Str(temp,10);
                    break;

                }
                else if (prAStr == atoi(prB)) {
                    if (atoi(idA) > collision) {
                        // rejected
                        char* temp1 = (char*)malloc(sizeof(char) * 17);
                        char* temp2 = (char*)malloc(sizeof(char) * 17);
                        strncpy(temp1,inAppStrList[i]+5,1);
                        strncpy(temp2,inAppStrList[i],4);
                        temp2 = int2Str(10000+inRejectedSize,10);
//                    printf("%s\n",temp2);
//                    printf("%s\n",temp1);
//                    printf(".%s\n",temp1);
                        strcat(temp2,temp1);
                        inRejectedList[inRejectedSize] = temp;
//                    inRejectedList[inRejectedSize] = inAppStrList[i];
                        inRejectedSize++;
                        strncpy(temp,inAppStrList[i],16);
                        strcat(temp,"1");
                        inAppStrList[i] = temp;
//                    printf("%s\n",inAppStrList[i]);
                        printf("size = %d and %s\n",inRejectedSize,inRejectedList[inRejectedSize-1]);
//                    int temp= atoi(inAppStrList[i])+1;
//                    inAppStrList[i] = int2Str(temp,10);
                        break;

                    }
                }
            }
        }
    }
    return inRejectedSize;
}

int main()
{
    /* code */
    char* inAppStrList[] = {"10001101100006030"/*ID:10001 date:101 time:1000 duration:060 priority:3 status:0*/

,"10002101100012020"/*ID:10002 date:101 time:1000 duration:120 priority:2 status:0*/
,"10003101106006010",
"10004101112006040",
"10009101112012030"};/*ID:10003 date:101 time:1060 duration:060 priority:2 status:0*/
    int inAppStrSize = sizeof(inAppStrList)/sizeof(inAppStrList[0]);
    char** inRejectedList = (char**)malloc(sizeof(int) * inAppStrSize);
    int i;
    int inRejectedSize = 0;
    for (i = 0;i<inAppStrSize;++i){
        if (inAppStrList[i] != NULL){
            printf("%s\n", inAppStrList[i]);
        }
        else{
            break;
        }

    }
//    printf("%d\n",PR(inAppStrList, inAppStrSize, inRejectedList, inRejectedSize));
    printf("%d\n", FCFS(inAppStrList, inAppStrSize, inRejectedList, inRejectedSize));
    for (i = 0;i<inAppStrSize;++i){
        printf("%s\n", inAppStrList[i]);
    }
    for (i = 0;i<inRejectedSize;++i){
        printf("%d", inRejectedList[i]);
    }


}