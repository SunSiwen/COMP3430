//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A02
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: Assignment 1, QUESTION: part2-thread
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
#include <pthread.h>
#include <time.h>

//global variables
long currentThread = 0;
long aimThread = 0;
long sign = 1;
long buff;
pthread_t pthread[1024];
int run[1024];

//threads' work
void *work(void *num) {
    long index;
    index = (long) num;
    printf("Thread %ld has started\n", index);
    fflush(stdout);
    //kill thread by this flag
    while (run[index]) {

    }
    printf("Thread %ld is going to a better place\n", index);
    fflush(stdout);
    return NULL;
}

//------------------------------------------------------
// killThreadByIndex()
//
// PURPOSE: kill thread by index
// INPUT PARAMETERS:
//     index: the index of thread inside arr
// OUTPUT PARAMETERS:
//      no return
//------------------------------------------------------
void killThreadByIndex(long index) {
    printf("Stopping %ld\n", index);
    fflush(stdout);
    //this is a flag
    run[index] = 0;
    pthread_join(pthread[index], NULL);
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
    fflush(stdout);
    fclose(fp);
    fflush(stdout);
}

//------------------------------------------------------
// exitHandler()
//
// PURPOSE: parent process will use it to kill threads first and self-end
// INPUT PARAMETERS:
//
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
void exitHandler() {
    aimThread = 0;
    for (int i = 0; i < currentThread; ++i) {
        killThreadByIndex(i);
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
            printf("Wrong configure!!!\n");
        } else if (buff == aimThread) {
            printf("Same Same\n");
        } else {
            aimThread = buff;
            sign = aimThread > currentThread ? 1 : -1;
            for (; currentThread != aimThread; currentThread += sign) {
                //create a new thread
                if (sign == 1) {
                    printf("Starting %ld\n", currentThread);
                    fflush(stdout);
                    run[currentThread] = 1;//turn flag to 1
                    pthread_create(&pthread[currentThread], NULL, work, (void *) currentThread);
//                    usleep(10000);
                } else {
                    //to kill thread by index
                    killThreadByIndex(currentThread + sign);
                }
            }
        }
        //wait for new command
        pause();
    }
}
