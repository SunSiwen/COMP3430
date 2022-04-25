//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A02
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: lab 2, QUESTION: Question 1
//
// REMARKS: What is the purpose of this program?
// Work with pipes, and see how processes really interact with files.
//-----------------------------------------

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#include <stdlib.h>

int total;

int main(int arg, char *args[]) {
    if (arg <= 1) {
        //no child process, exit
        return 0;
    }

    //define pipes for child process
    total = arg - 1;
    int fd[total][2];

    //define the max byte length
    char buff[1024];
    for (int i = 0; i < total; ++i) {
        //open the pipes
        pipe(fd[i]);
    }
    //reset the buff to 0
    memset(buff, 0, sizeof(buff));
    for (int i = 0; i < total; ++i) {
        //create child processes
        int pid = fork();
        if (pid == 0) {
            //child process starts
            printf("%d is about to start [%s]\n", getpid(), args[i + 1]);
            fflush(stdout);

            //deal with string
            char *p = strtok(args[i + 1], " ");
            char *word[128];
            int pos = 0;
            word[pos++] = p;
            p = strtok(NULL, " ");
            while (p) {
                word[pos++] = p;
                p = strtok(NULL, " ");
            }
            //put NULL into the end of data
            word[pos] = NULL;
            for (int j = 0; j < total; ++j) {
                if (i == j) {
                    //close the output end of the pipe
                    close(fd[j][1]);
                    //make the input end of the pipe as the stdin
                    dup2(fd[j][0], 0);
                    //close the input end of the pipe
                    close(fd[j][0]);
                } else {
                    //close the input end of the pipe
                    close(fd[j][0]);
                    //close the output end of the pipe
                    close(fd[j][1]);
                }
            }

            //system call
            do {
                execvp(word[0], word);
            } while (read(0, buff, sizeof(buff)) > 0);
            close(0);
            printf("%d ends", getpid());
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < total; ++i) {
        //close all input end of the pipes
        close(fd[i][0]);
    }
    while (fgets(buff, sizeof(buff), stdin) != NULL) {
        //count the length of this message
        int cnt = 0;
        for (int i = 0; i < 1024; ++i) {
            if (buff[i] != '\0') {
                cnt++;
            } else {
                break;
            }
        }
        //send message to its child processes
        for (int i = 0; i < total; ++i) {
            write(fd[i][1], buff, cnt);
        }
        //reset to 0
        memset(buff, 0, sizeof(buff));
    }

    //close all output end of the pipes
    for (int i = 0; i < total; ++i) {
        close(fd[i][1]);
    }

    //wait all child processes end
    int pid;
    do {
        pid = wait(NULL);
    } while (pid != -1);
    printf("\nParent process completed normally.\n");

    return 0;
}

