*** Preparation ***
    - Environment: COMP Linux Server Apollo/Apollo2
    - File checklist:
        - G04_APO.c
        - G04_tests.dat
        - G04_Readme.txt


*** To compile the program ***:
gcc G04_APO.c -o G04_APO
    - There are no additional libraries in the program, use the standard compilation statements.


*** To run the program ***:
./G04_APO [startDate] [endDate] [user1] [user2] ... [user10]
- For example (used in G04_tests.dat): ./G04_APO 20230501 20230531 paul mary lucy david jerry mike wayne
- The user needs to specify the start date, the end date and the names of all users involved in scheduling at the start of the program. 
- The start and end dates need to be between "20230501" and "20230531", and the end date needs to be greater than the start date. 
- The number of users needs to be greater than three and less than ten. 
- These parameters which are established in the program start statement, cannot be changed during the program.
- Error message in run the program:	
    - User Size out of Range [3,10], try again!
    - Time out of Range [20230501, 20230531], try again!


*** To record single appointment ***:
[type] [caller] [date] [startTime] [duration] [callee1] ... [callee9]
- For example: gathering lucy 20230504 1900 4.0 john paul mary
- There are four types of programs, in descending order of priority: privateTime, projectMeeting, groupStudy, gathering. 
- All three commands except the privateTime accept several callee users from 1 to 9, while privateTime does not allow callee users. 
- The date needs to be set between the startDate and endDate set in the start command.  
- The caller and callee should be the subset of all users from start command. 
- The startTime is specified as one slot of total five slots between 18:00 and 23:00, 
- the duration cannot be "0" and the startTime plus the duration cannot be more than 23:00.


*** To record appointments from batch file ***:
f [fileName]
- For example: f G04_tests.dat
- When the command starts with f, the command line function is to read the fileName. 
- The fileName needs to be in the correct format and exist under that folder.


*** To print the timetable and the rejected list ***:
printSchd [Algorithm]
- For example: printSchd PRIORITY
- Use the schedule command to invoke the corresponding Algorithm for scheduling, 
- which includes FCFS, PRIORITY and ALL. 
- The command also calls the Output Module to write the results of the Algorithm to a single file.
- The saved single file name will be shown in the command line.
- The single file contains 
    - Appointment scheduling result for each user, 
    - Rejected appointment list, and 
    - Performance results.


*** Error message in appointment record command ***:
- Event date out of range: entered appointment data is out of set data range
- Event time out of range: entered appointment time is out of set time range
- Event duration out of range: entered appointment duration is out of set time range 
- Caller not found: entered appointment’s caller is not found among all users
- No callee can be in privateTime: callees are found in entered privateTime appointment
- At least one callee should be in this appointment: no callee in non-privateTime appointment
- Callee not found: entered appointment’s callees are not found among all users


*** To end the program ***:
endProgram
- This ends the program completely, upon collecting all the child processes and closing all the files.
