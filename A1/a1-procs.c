//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A01
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: Assignment 1, QUESTION: part2-process
//
// REMARKS: What is the purpose of this program?
// Read a configuration file to decide how many worker processes or threads should be used.
// Also, we want to be able to change the number of worker processes or thread at run time.
// On receiving a SIGHUP, re-read the configuration file,
// and change the number of current workers appropriately - creating or removing processes or threads.
//-----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

//global variables
long currentThread = 0;
long aimThread = 0;
long sign = 1;
long buff;
int arr[1024];

//------------------------------------------------------
// killProcessByIndex()
//
// PURPOSE: kill child process by index
// INPUT PARAMETERS:
//     index: the index of process inside arr
// OUTPUT PARAMETERS:
//      no return
//------------------------------------------------------
void killProcessByIndex(long index) {
    //to get child process pid by index
    int cid = arr[index];
    printf("%d is being killed\n", cid);
    //send signal to a child process by cid
    kill(cid, SIGINT);
    //wait util child process exits
    wait(NULL);
}

//------------------------------------------------------
// hupHandler()
//
// PURPOSE: read from the config.txt again
// INPUT PARAMETERS:
//
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
void hupHandler() {
    FILE *fp = NULL;
    fp = fopen("config.txt", "r");
    printf("Reading config.txt file\n");
    fflush(stdout);
    char str[10];
    fscanf(fp, "%s", str);
    buff = strtol(str, NULL, 10);
    fclose(fp);
}

//------------------------------------------------------
// intHandler()
//
// PURPOSE: child process will use it to self-end
// INPUT PARAMETERS:
//
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
void intHandler() {
    printf("%d is exiting\n", getpid());
    fflush(stdout);
    exit(0);
}

//------------------------------------------------------
// exitHandler()
//
// PURPOSE: parent process will use it to kill child processes first and self-end
// INPUT PARAMETERS:
//
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
void exitHandler() {
    aimThread = 0;
    for (int i = 0; i < currentThread; ++i) {
        //kill child process by index
        killProcessByIndex(i);
    }
    exit(0);
}

//------------------------------------------------------
// main()
//
// PURPOSE: the entrance of my code
// INPUT PARAMETERS:
//
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
int main() {
    //read file for initial
    hupHandler();

    //register monitor signal and handler
    signal(SIGHUP, hupHandler);
    signal(SIGINT, exitHandler);

    printf("I am %d\n", getpid());
    fflush(stdout);

    //endless loop
    while (1) {
        printf("Change setting to %ld\n", buff);
        if (buff < 0) {
            //buff should be a positive integer
            printf("Wrong configure!!!\n");
        } else if (buff == aimThread) {
            printf("Same Same\n");
        } else {
            aimThread = buff;
            sign = aimThread > currentThread ? 1 : -1;
            for (; currentThread != aimThread; currentThread += sign) {
                // to add a new process
                if (sign == 1) {
                    int pid = fork();
                    if (pid == 0) {
                        printf("%d is starting\n", getpid());
                        //register monitor signal and handler
                        signal(SIGINT, intHandler);
                        while (1) {
                            //endless job
                        }
                    } else {
                        //put child process pid into arr
                        arr[currentThread] = pid;
                    }
                } else {
                    //to kill child process
                    killProcessByIndex(currentThread + sign);
                }
            }
        }
        //wait for new command
        pause();
    }
}
