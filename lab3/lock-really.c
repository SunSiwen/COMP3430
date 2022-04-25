#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

int g_number = 0;


#define MAX_COUNT 100000000
#define THREAD_NUM 2
typedef atomic_flag lockReally;
lockReally lock;

/**
 * my thread_lock
 * @param ptr
 */
void mutex_lock(lockReally *ptr) {
    while (atomic_flag_test_and_set(ptr)) {}
}

/**
 * my thread_unlock
 * @param ptr
 */
void mutex_unlock(lockReally *ptr) {
    atomic_flag_clear(ptr);
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
    atomic_flag_clear(&lock);
    pthread_t threads[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&threads[i], NULL, counter, NULL);
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("The result should be %d and in fact it is %d\n", MAX_COUNT * THREAD_NUM, g_number);
    printf("\nThis program completed normally.\n");
    return 0;
}
