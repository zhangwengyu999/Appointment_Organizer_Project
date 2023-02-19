while (!timeFlag) {
int sTime;
int eTime;
printf("Enter Start Time: ");
scanf("%d",&sTime);
printf("Enter End Time: ");
scanf("%d",&eTime);
if (sTime > eTime || sTime < 20230501 || eTime > 20230531) {
printf("Time Error!\n");
timeFlag = 0;
}
else {
timeFlag = 1;
startTime = sTime;
endTime = eTime;
}
}

while (!userFlag) {
char *newUsersStr;
char *newUsers[10];
int newUsersSize=0;
printf("Enter Users: ");
scanf("%[^\n]",newUsersStr);
char *token = strtok(newUsersStr, " ");
while(token != NULL){
newUsers[newUsersSize] = token;
newUsersSize++;
token = strtok(NULL," ");
}

if (newUsersSize > 10 || newUsersSize < 3) {
printf("User Size out of Range!\n");
userFlag = 0;
}
else {
userFlag = 1;
userSize=newUsersSize;
//            users=newUsers;
int j;
for (j=0;j<newUsersSize;j++) {
//                users[j]=NULL;
//                users[j]=newUsers[j];
strcpy(users[j],NULL);
strcpy(users[j],newUsers[j]);
}
}
}
