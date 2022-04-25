//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A01
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: assignment 2, QUESTION: question 1
//
// REMARKS: What is the purpose of this program?
// The program realizes a simple shell.
//-----------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_COL 100
#define MAX_ROW 128


/**
 * reomve the white spaces and other special symbol before and after the string
 * @param input : a command
 * @return : a command without white spaces and other special symbol before and after itself
 */
char *trim(char *string) {
    char *input = malloc(MAX_COL * sizeof(char));
    strcpy(input, string);
    int validLength = strlen(input) - 1;
    int a = 0;
    int b = validLength;
    //search from head
    for (int i = 0; i < validLength; ++i)
        if (input[i] != ' ') {
            a = i;
            break;
        }
    //search from tail
    for (int i = validLength; i >= 0; --i) {
        if (input[i] != ' ' && input[i] != '\n' && input[i] != '\t' && input[i] != '\r') {
            b = i;
            break;
        }
    }
    input[b + 1] = '\0';
    //return the address of the first char
    return input + a;
}


/**
 * count the number of strings in the array
 * @param string : 2-d array
 * @return  : number of strings
 */
int countNum(char **string) {
    int cnt = 0;
    while (string[cnt] != NULL) {
        cnt++;
    }
    return cnt;
}

/**
 * split the string
 * @param line : the command
 * @return : return a 2_d array consisting of strings
 */
char **split(char *line) {
    char **string = malloc(MAX_ROW * sizeof(char *));
    char *temp;
    int cnt = 0;
    temp = strtok(line, " ");
    while (temp != NULL) {
        string[cnt] = malloc(MAX_COL * sizeof(char));
        strcpy(string[cnt++], temp);
        temp = strtok(NULL, " ");
    }
    string[cnt] = NULL;
    return string;
}

/**
 * deal with command to remove '<(' and ')'
 * @param pString a command like <(cat words)
 * @param start index of <(
 * @param end  index of )
 * @return
 */
char **removeBrackets(char **pString,int start,int end) {
    char **res = malloc(MAX_COL * sizeof(char*));
    int cnt = 0;
    for (int i = start; i < end; ++i) {
        res[cnt] = malloc(MAX_COL * sizeof(char));
        if(strstr(pString[i], "<(")){
            strcpy(res[cnt], &pString[i][2]);
        }else if(strstr(pString[i], ")")){
            char* s = trim(pString[i]);
            s[strlen(s) - 1] = '\0';
            strcpy(res[cnt], s);
        }else{
            strcpy(res[cnt], pString[i]);
        }
        cnt++;
    }
    res[cnt] = NULL;
    return res;
}

/**
 * execute the command
 * @param pString : a line of command
 * @param length : the position of pipe
 */
void execute(char **pString, int length) {
    if (length <= 0) {
        return;
    }
    //check the '<' or '>'
    char *inFile = NULL, *outFile = NULL;
    //check the <(
    char **substitution = NULL;
    int limit = length;
    //check whether it has "<" or ">"
    for (int i = 0; i < length; i++) {
        if (strstr(pString[i], "<(")) {
            substitution = removeBrackets(pString,i,length);
            limit = i;
            break;
        }
        if (strstr(pString[i], "<")) {
            inFile = pString[i + 1];
            limit = i;
            break;
        }
        if (strstr(pString[i], ">")) {
            outFile = pString[i + 1];
            limit = i;
            break;
        }
    }
    //not contain '<('
    if (substitution == NULL) {
        pid_t pid = fork();
        if (pid == 0) {
            //redirect the stdin or stdout
            if (inFile != NULL) {
                freopen(inFile, "r", stdin);
                inFile = NULL;
            }
            if (outFile != NULL) {
                fflush(stdout);
                freopen(outFile, "w", stdout);
                outFile = NULL;
            }
            //get the command
            char *cmd[MAX_ROW];
            for (int i = 0; i < limit; i++) {
                cmd[i] = pString[i];
            }
            //add a NULL to the end
            cmd[limit] = NULL;
            execvp(cmd[0], cmd);
            exit(0);
        } else {
            waitpid(pid, NULL, 0);
        }
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            int fd;
            //create FIFO
            unlink("FIFO");
            mkfifo("FIFO", 0666);
            int pid = fork();
            if (pid == 0) {
                fd = open("FIFO", O_WRONLY);
                dup2(fd, 1);
                close(fd);
                execvp(substitution[0], substitution);
                unlink("FIFO");
                exit(0);

            } else {
                fd = open("FIFO", O_RDONLY);
                dup2(fd, 0);
                close(fd);
                //get the command
                char *cmd[MAX_ROW];
                for (int i = 0; i < limit; i++) {
                    cmd[i] = pString[i];
                }
                cmd[limit] = NULL;
                execvp(cmd[0], cmd);
                unlink("FIFO");
                exit(0);
            }
            exit(0);
        } else {
            waitpid(pid, NULL, 0);
        }
    }

}


/**
 * analyze a command
 * @param pString: a line of command
 */
void analyze(char **pString) {
    //count the number of strings of the command
    int length = countNum(pString);
    if (length == 0) {
        return;
    }
    //the position of the first |
    int whereIsPipe = -1;
    for (int i = 0; i < length; ++i) {
        if (!strcmp(pString[i], "|")) {
            whereIsPipe = i;
            break;
        }
    }
    //if the command contains pipe
    if (whereIsPipe == -1) {
        //execute the simple command
        execute(pString, length);
        exit(0);
    } else {
        //create the pipe
        int myPipe[2];
        pipe(myPipe);
        pid_t pid = fork();
        if (pid == 0) {
            //execute the command before the first |
            close(myPipe[0]);
            dup2(myPipe[1], 1);
            close(myPipe[1]);
            execute(pString, whereIsPipe);
            exit(0);
        } else {
            //recursion function for the command after the first |
            if (whereIsPipe + 1 < length) {
                close(myPipe[1]);
                dup2(myPipe[0], 0);
                close(myPipe[0]);
                analyze(&pString[whereIsPipe + 1]);
            }
        }
    }
}

/**
 * prepare to execute system call
 * @param argv : command
 */
void systemCall(char **argv) {
    // system call
    pid_t pid = fork();
    if (pid == 0) {
        //close stdin stdout
        int inFds = dup(0);
        int outFds = dup(1);
        analyze(argv);
        //recovery
        dup2(inFds, 0);
        dup2(outFds, 1);
        exit(0);
    } else {
        waitpid(pid, NULL, 0);
    }
}

/**
 * print the command line
 * @param line:  a line of command
 */
void shell(char *line) {
    //print the banner
    printf("sunsiwen@shell: %s", line);
    fflush(stdout);
    char *t = trim(line);
    char **argv = split(t);

    if (NULL == argv[0] || !strcmp(argv[0], "quit")) {
        //quit the program
        exit(0);
    } else if (!strcmp(argv[0], "cd")) {
        //change the directory
        chdir(argv[1]);
        return;
    } else {
        //system call
        systemCall(argv);
        return;
    }
}

/**
 * read commands from the file
 * @param path : the file name
 */
void readFile(char *path) {
    //open the file
    FILE *script = fopen(path, "r");
    char lines[MAX_ROW][MAX_COL];
    int cnt = 0;
    //read every line of the script until EOF
    while (!feof(script)) {
        fgets(lines[cnt], MAX_COL, script);
        cnt++;
    }
    *lines[cnt] = '\0';
    fclose(script);
    cnt = 0;
    //start looping until NULL or space line
    while (lines[cnt] != NULL && countNum((char **) lines[cnt]) > 0 && strcmp(lines[cnt], "")) {
        shell(lines[cnt]);
        cnt++;
    }
}

/**
 * the entrance of the code
 * @param argc number of parameters
 * @param argv the file name
 * @return
 */
int main(int argc, char *argv[]) {
    if (argc == 2) {
        readFile(argv[1]);
        printf("\n");
        unlink("FIFO");
        return 0;
    }
}

