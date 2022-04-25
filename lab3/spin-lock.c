#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>


int g_number = 0;

#define MAX_COUNT 100000000
#define THREAD_NUM 2

typedef bool spinLock;

spinLock lock;

/**
 * my thread_lock
 * @param ptr
 */
void mutex_lock(spinLock *ptr) {
    while (*ptr) {}
    *ptr = true;
}

/**
 * my thread_unlock
 * @param ptr
 */
void mutex_unlock(spinLock *ptr) {
    *ptr = false;
}

/**
 * add 1 to a number
 * @return
 */
void *counter() {
    for (int i = 0; i < MAX_COUNT; ++i) {
        mutex_lock(&lock);
        g_number++;
        mutex_unlock(&lock);
    }
    return NULL;
}


int main() {
    lock = false;
    pthread_t threads[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&threads[i], NULL, counter, NULL);
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("The result should be %d but in fact it is %d\n", MAX_COUNT * THREAD_NUM, g_number);
    printf("\nThis program completed normally.\n");

    return 0;
}

