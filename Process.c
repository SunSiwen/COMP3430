//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A01
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: assignment 1, QUESTION: question 2  -> process
//
// REMARKS: What is the purpose of this program?
// The program can test how long my pc will take to run total processes with up to limit processes running at a time
//-----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NANOS_PER_SECOND 1000000000

/**
 *
 * @param work : time
 * @return : work
 */
static void *hard_work(void *work) {
    long *amount = (long *) work;
    long seconds = *amount / NANOS_PER_SECOND;
    long nanos = *amount % NANOS_PER_SECOND;
    struct timespec t = {.tv_sec = seconds, .tv_nsec = nanos};
    int ret;
    do {
        ret = nanosleep(&t, &t); // **Really** hard work.
        // need to loop, `nanosleep` might return before sleeping
        // for the complete time (see `man nanosleep` for details)
    } while (ret == -1 && (t.tv_sec || t.tv_nsec));

    return work;
}

/**
 * Come from Gun Rutenbery
 * @param start : start time
 * @param end : end time
 * @return : time diff
 */
struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

/**
 * to compare two numbers, and return the min one
 * @param num1
 * @param num2
 * @return : the min number
 */
int min(int num1, int num2) {
    if (num1 < num2) {
        return num1;
    } else {
        return num2;
    }
}

/**
 *
 * @param argc : the size of argv
 * @param argv : get from command line
 * @return
 */
int main(int argc, const char *argv[]) {
    if (argc == 3) {
        int totalProcess = atoi(argv[1]);        //the total number of process system could be created
        int limitProcess = atoi(argv[2]);        //the max number of process could be running at a time
        long work = 1024;
        struct timespec startTime, endTime;
        clock_gettime(CLOCK_REALTIME, &startTime);      //the start time.
        while (totalProcess > 0) {
            for (int i = 0; i < min(limitProcess,
                                    totalProcess); i++, totalProcess--) { //Made sure that there are only limitProcess processes running at a time
                int res = fork();
                if (res == 0) {
                    hard_work(&work);
                    exit(0);
                }
            }
            int pid;
            do {               //wait for all of child process done.
                pid = wait(NULL);
            } while (pid != -1);
        }

        clock_gettime(CLOCK_REALTIME, &endTime); //the end time.
        printf("In process experiment, system creates %s processes in total, and run %s processes at a time, The total time is %ld:%ld\n",
               argv[1], argv[2],
               diff(startTime, endTime).tv_sec, diff(startTime, endTime).tv_nsec);
        return 0;
    } else {
        printf("Wrong arguments input");
        return -1;
    }
}
