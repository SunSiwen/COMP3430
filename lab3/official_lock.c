#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

int g_number = 0;


#define MAX_COUNT 100000000
#define THREAD_NUM 2

pthread_mutex_t official_lock;

/**
 * add 1 to a number
 * @return
 */
void *counter() {
    for (int i = 0; i < MAX_COUNT; ++i) {
        pthread_mutex_lock(&official_lock);
        g_number++;
        pthread_mutex_unlock(&official_lock);
    }
    return NULL;
}


int main() {

    pthread_t threads[THREAD_NUM];
    pthread_mutex_init(&official_lock, NULL);

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&threads[i], NULL, counter, NULL);
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("The result should be %d and in fact it is %d\n", MAX_COUNT * THREAD_NUM,
           g_number);

    printf("\nThis program completed normally.\n");

    return 0;
}
