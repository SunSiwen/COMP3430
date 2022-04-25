//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A01
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: lab 1, QUESTION 2: process
//
// REMARKS: What is the purpose of this program?
//  practise using fork and signals
//-----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


//global variables
long total = 999;
volatile long count = 0;
long argues = 2;
int flag = 1;

//------------------------------------------------------
// childHandler()
//
// PURPOSE: child process exits
// INPUT PARAMETERS:
//          signal: signal#
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
void childHandler( ) {
    printf("I am child process, and my pid is %d \n", getpid());
    exit(EXIT_SUCCESS);
}

//------------------------------------------------------
// parentHandler()
//
// PURPOSE: parent process exits, and it will wait for its children process exit
// INPUT PARAMETERS:
//          signal: signal#
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
void parentHandler( ) {
    printf("I am parent process, and my pid is %d \n", getpid());
    int pid;
    do {
        pid = wait(NULL);
    } while (pid != -1);
    printf("\nParent process completed normally.\n");
    exit(EXIT_SUCCESS);
}

//------------------------------------------------------
// countHandler()
//
// PURPOSE: count the number of children processes
// INPUT PARAMETERS:
//          signal: signal#
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
void countHandler( ) {
    count++;
    flag = 0;
}

//------------------------------------------------------
// main()
//
// PURPOSE: the entrance of my code
// INPUT PARAMETERS:
//     argc: the numbers of argv
//     argv: the input
// OUTPUT PARAMETERS:
//      status code
//------------------------------------------------------
int main(int argc, const char *argv[]) {

    if (argc != argues) {
        printf("Wrong input parameters\n");
        return 0;
    }

    //regster handler
    signal(SIGUSR2, parentHandler);
    signal(SIGUSR1, countHandler);

    total = strtol(argv[1], NULL, 10);

    //to create children processes by fork
    for (int i = 0; i < total; i++) {
        int pid = fork();
        if (pid == 0) {
            //register handler for every children process
            signal(SIGUSR2, childHandler);
            kill(getppid(), SIGUSR1);
            while (1) {
                sleep(1);
            }
        } else {
            while (flag) {
            }
            flag = 1;
        }
    }

    //to check the number of children processes, if count == total, send signal to the process group
    while (1) {
        if (count == total) {
            kill(-1 * getpgrp(), SIGUSR2);
            count++;
        }
    }
}

