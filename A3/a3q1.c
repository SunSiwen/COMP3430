//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A02
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: assignment 3
//
// REMARKS: What is the purpose of this program?
// The program can implement multi-level feedback queue
//-----------------------------------------


#include <stdio.h>
#include <sys/queue.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define NANOS_PER_USEC 1000
#define USEC_PER_MSEC 1000
#define USEC_PER_SEC   1000000

//quantum and allotment
#define QUANTUM   50
#define TIME_SLICE   200

//lock of task queue 0~2->level lock while 3 is used for working lock
pthread_mutex_t mutex[4];
//no task available
pthread_cond_t cond_task;
//complete all tasks
pthread_cond_t cond_done;

//number of cpus
long cnt;
//how many threads are working
volatile int working = 0;
//load all task?
int loadDone = 0;
//how many tasks of all types
int taskNum[4];
//how much time cost by turnaround group by type
long turnaroundTime[4];
//how much time cost by waiting group by type
long waitTime[4];

/**
 * let thread sleep for a certain time
 * @param usecs : time in usecs
 */
static void microsleep(unsigned int usecs) {
    long seconds = usecs / USEC_PER_SEC;
    long nanos = (usecs % USEC_PER_SEC) * NANOS_PER_USEC;
    struct timespec t = {.tv_sec = seconds, .tv_nsec = nanos};
    int ret;
    do {
        ret = nanosleep(&t, &t);
        // need to loop, `nanosleep` might return before sleeping
        // for the complete time (see `man nanosleep` for details)
    } while (ret == -1 && (t.tv_sec || t.tv_nsec));
}

/**
 * my task
 */
struct task {
    int type; //type 0->short 1->mid 2->long 3->io
    int time; //how much time it will cost
    int oddsOfIO; //io possibility
    int allotment; // time for lowing priority
    int new; // whether it is a new task
    //set the begin time of simulation
    struct timespec begin;
    STAILQ_ENTRY(task)
    next;
};


STAILQ_HEAD(, task) tempLists[3];

/**
 * wake all worker up to work
 */
void wakeUp() {
    for (int i = 0; i < cnt; ++i) {
        pthread_cond_signal(&cond_task);
    }
}

/**
 * load tasks from tasks.txt
 */
void *produce(void *para) {
    char *file = (char *) para;
    FILE *fp = fopen(file, "r");
    char line[1024];
    char *delimiter = " ";
    while (!feof(fp)) {
        for (int i = 0; i < 1024; ++i) {
            line[i] = '\0';
        }
        // read every line
        fgets(line, 1024, fp);
        if (line[0] == 'D' && line[1] == 'E' && line[2] == 'L') {
            printf("sleep for delay\n");
            wakeUp();
            usleep(atoi(line + 6) * USEC_PER_MSEC);
            continue;
        }

        if (line[0] != '\0') {
            //read tasks
            char *name = strtok(line, delimiter);
            printf("loading.....%s\n", name);
            struct task *t = (struct task *) malloc(sizeof(struct task));
            t->new = 1;//at first, it is new
            t->type = atoi(strtok(NULL, delimiter));
            taskNum[t->type]++;//statistics
            t->time = atoi(strtok(NULL, delimiter));
            t->oddsOfIO = atoi(strtok(NULL, delimiter));
            t->allotment = TIME_SLICE;
            clock_gettime(CLOCK_REALTIME, &t->begin);
            //add to mlfq
            pthread_mutex_lock(&mutex[0]);
            STAILQ_INSERT_TAIL(&tempLists[0], t, next);
            wakeUp();
            pthread_mutex_unlock(&mutex[0]);
        }
    }
    //close file
    fclose(fp);
    //mark as load done
    loadDone = 1;
    printf("load over\n");
    return NULL;
}


/**
 * used by master thread to manage threads and tasks
 * @param argv
 * @return
 */
void *manage(void *num) {
    long setTime;
    setTime = (long) num;
    while (1) {
        //sleep for a certain period
        microsleep(setTime * USEC_PER_MSEC);

        pthread_mutex_lock(&mutex[0]);
        //try to level up
        pthread_mutex_lock(&mutex[1]);
        while (!(STAILQ_EMPTY(&tempLists[1]))) {
            struct task *t = STAILQ_FIRST(&tempLists[1]);
            STAILQ_REMOVE_HEAD(&tempLists[1], next);
            STAILQ_INSERT_TAIL(&tempLists[0], t, next);
        }
        pthread_mutex_unlock(&mutex[1]);

        //try to level up
        pthread_mutex_lock(&mutex[2]);
        while (!(STAILQ_EMPTY(&tempLists[2]))) {
            struct task *t = STAILQ_FIRST(&tempLists[2]);
            STAILQ_REMOVE_HEAD(&tempLists[2], next);
            STAILQ_INSERT_TAIL(&tempLists[0], t, next);
        }
        pthread_mutex_unlock(&mutex[2]);
        pthread_mutex_unlock(&mutex[0]);

        //if there is any tasks, tell the worker to work
        if (!((STAILQ_EMPTY(&tempLists[0])) && (STAILQ_EMPTY(&tempLists[1])) && (STAILQ_EMPTY(&tempLists[2])))) {
            wakeUp();
        }

        //if there is no task and no working worker
        pthread_mutex_lock(&mutex[3]);
        if (working == 0 && loadDone &&
            ((STAILQ_EMPTY(&tempLists[0])) && (STAILQ_EMPTY(&tempLists[1])) && (STAILQ_EMPTY(&tempLists[2])))) {
            pthread_cond_signal(&cond_done);
        }
//        printf("worker is %d\n", working);
        pthread_mutex_unlock(&mutex[3]);
    }
}

/**
 * worker function to do tasks
 * @param argv
 * @return
 */
void *consume() {
    //local lock is used to avoid wasting cpu
    pthread_mutex_t local;
    pthread_mutex_init(&local, NULL);

    while (1) {
        struct task *t = NULL;
        int level = 0;
        while (1) {
            //try to gain task from level 0
            pthread_mutex_lock(&mutex[0]);
            if (!(STAILQ_EMPTY(&tempLists[0]))) {
                t = STAILQ_FIRST(&tempLists[0]);
                STAILQ_REMOVE_HEAD(&tempLists[0], next);
                pthread_mutex_unlock(&mutex[0]);
                break;
            }
            pthread_mutex_unlock(&mutex[0]);

            //try to gain task from level 1
            pthread_mutex_lock(&mutex[1]);
            if (!(STAILQ_EMPTY(&tempLists[1]))) {
                t = STAILQ_FIRST(&tempLists[1]);
                STAILQ_REMOVE_HEAD(&tempLists[1], next);
                level = 1;
                pthread_mutex_unlock(&mutex[1]);
                break;
            }
            pthread_mutex_unlock(&mutex[1]);

            //try to gain task from level 2
            pthread_mutex_lock(&mutex[2]);
            if (!(STAILQ_EMPTY(&tempLists[2]))) {
                t = STAILQ_FIRST(&tempLists[2]);
                STAILQ_REMOVE_HEAD(&tempLists[2], next);
                level = 2;
                pthread_mutex_unlock(&mutex[2]);
                break;
            }
            pthread_mutex_unlock(&mutex[2]);

            //wait here, util get the condition
            pthread_cond_wait(&cond_task, &local);
            pthread_mutex_unlock(&local);
        }
        //change the number of working
        pthread_mutex_lock(&mutex[3]);
        working++;
        pthread_mutex_unlock(&mutex[3]);

        if (t->new) {
            //current time of simulation
            struct timespec curr;
            clock_gettime(CLOCK_REALTIME, &curr);
            int usecs =
                    (curr.tv_sec - t->begin.tv_sec) * USEC_PER_SEC + (curr.tv_nsec - t->begin.tv_nsec) / NANOS_PER_USEC;
            t->new = 0;
            waitTime[t->type] += (usecs);
        }

        int io = rand() % 100 + 1;
        int sleep = QUANTUM;
        int ioTime;
        if (io < t->oddsOfIO) {
            //to generate io time
            ioTime = rand() % QUANTUM;
        } else {
            ioTime = 0;
        }
        //get the left time by define method
        int left = MIN(QUANTUM - ioTime, MIN(t->time, t->allotment));
        sleep = left + ioTime;
        t->time -= left;
        t->allotment -= left;
        //simulate to work or io
        microsleep(sleep);

        //where to put the task
        if (t->time == 0) {
            //current time of simulation
            struct timespec curr;
            clock_gettime(CLOCK_REALTIME, &curr);
            int usecs =
                    (curr.tv_sec - t->begin.tv_sec) * USEC_PER_SEC + (curr.tv_nsec - t->begin.tv_nsec) / NANOS_PER_USEC;

            turnaroundTime[t->type] += (usecs);
            //finish, free it
            free(t);
        } else if (t->allotment == 0) {
            //run out allotment, level down
            level = MIN(level + 1, 2);
            t->allotment = TIME_SLICE;
            pthread_mutex_lock(&mutex[level]);
            STAILQ_INSERT_TAIL(&tempLists[level], t, next);
            wakeUp();
            pthread_mutex_unlock(&mutex[level]);

        } else {
            //put back
            pthread_mutex_lock(&mutex[level]);
            STAILQ_INSERT_TAIL(&tempLists[level], t, next);
            wakeUp();
            pthread_mutex_unlock(&mutex[level]);
        }

        //update working
        pthread_mutex_lock(&mutex[3]);
        working--;
        pthread_mutex_unlock(&mutex[3]);

    }
}


/**
 * used by master thread to manage threads and tasks
 * @param argv
 * @return
 */
void *clean(void *num) {
    long core;
    core = (long) num;
    pthread_cond_wait(&cond_done, &mutex[3]);
    printf("\nUsing mlfq with %ld CPUs.\n\n", core);

    printf("Average turnaround time per type:\n\n");
    printf("\tType 0: %ld usec\n", turnaroundTime[0] / taskNum[0]);
    printf("\tType 1: %ld usec\n", turnaroundTime[1] / taskNum[1]);
    printf("\tType 2: %ld usec\n", turnaroundTime[2] / taskNum[2]);
    printf("\tType 3: %ld usec\n\n\n", turnaroundTime[3] / taskNum[3]);

    printf("Average response time per type:\n\n");
    printf("\tType 0: %ld usec\n", waitTime[0] / taskNum[0]);
    printf("\tType 1: %ld usec\n", waitTime[1] / taskNum[1]);
    printf("\tType 2: %ld usec\n", waitTime[2] / taskNum[2]);
    printf("\tType 3: %ld usec\n\n\n", waitTime[3] / taskNum[3]);
    //The output of a program should contain a message to the effect that the program completed its task normally
    printf("\nProgram completed normally.\n");
    fflush(stdout);
    exit(0);
}


/**
 * the entrance of this program
 * @param argc
 * @param argv : read from command line
 * @return
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        return 0;
    }
    //number of cpu cores
    long core = atoi(argv[1]);
    cnt = core;
    //set time
    long setTime = atoi(argv[2]);
    //the fourth parameter is the file path
    char *file = argv[3];
    for (int i = 0; i < 4; ++i) {
        pthread_mutex_init(&mutex[i], NULL);
    }
    pthread_cond_init(&cond_done, NULL);
    pthread_cond_init(&cond_task, NULL);


    STAILQ_INIT(&tempLists[0]);
    STAILQ_INIT(&tempLists[1]);
    STAILQ_INIT(&tempLists[2]);

    pthread_t consumer[core];
    pthread_t manager;
    pthread_t producer;
    pthread_t cleaner;


    for (int j = 0; j < core; ++j) {
        pthread_create(&consumer[j], NULL, consume, NULL);
    }
    pthread_create(&cleaner, NULL, clean, (void *) core);
    pthread_create(&manager, NULL, manage, (void *) setTime);
    microsleep(500 * USEC_PER_MSEC);
    pthread_create(&producer, NULL, produce, file);
    for (int k = 0; k < core; ++k) {
        pthread_join(consumer[k], NULL);
    }
    pthread_join(cleaner, NULL);
    return 0;

}
