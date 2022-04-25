#include <stdio.h>
#include <pthread.h>

#define MAX_LENGTH 1024

pthread_mutex_t only_lock;

pthread_cond_t wake_lower;
pthread_cond_t wake_upper;

char text[MAX_LENGTH];

/**
 * judge whether the letter is a-z
 * @param c : a letter
 * @return : 1 means true and 0 is false
 */
int isLowCase(char c) {
    if (c >= 97 && c <= 122) {
        return 1;
    }
    return 0;
}

/**
 * judge whether the letter is A-Z
 * @param c : a letter
 * @return 1 means true and 0 is false
 */
int isUpCase(char c) {
    if (c >= 65 && c <= 90) {
        return 1;
    }
    return 0;
}

/**
 * print the lowercase
 * @return
 */
void *printLowCase() {

    while (1) {
        pthread_cond_wait(&wake_lower, &only_lock);
        int index = 0;
        printf("This is lower case printer thread -> ");
        while (text[index] != '\0') {
            if (isLowCase(text[index])) {
                printf(" %c ", text[index]);
            }
            index++;
        }
        printf("\n");
        fflush(stdout);
        pthread_cond_signal(&wake_upper);
        pthread_mutex_unlock(&only_lock);
    }
}

/**
 * print the uppercase
 * @return
 */
void *printUpCase() {
    while (1) {
        pthread_cond_wait(&wake_upper, &only_lock);
        int index = 0;
        printf("This is upper case printer thread -> ");
        while (text[index] != '\0') {
            if (isUpCase(text[index])) {
                printf(" %c ", text[index]);
            }
            index++;
        }
        printf("\n");
        fflush(stdout);
        pthread_mutex_unlock(&only_lock);
    }
}

/**
 * Main: the entrance of my program
 * @return
 */
int main(void) {
    pthread_mutex_init(&only_lock, NULL);

    pthread_cond_init(&wake_upper, NULL);
    pthread_cond_init(&wake_lower, NULL);

    pthread_t tid1, tid2;
    pthread_create(&tid2, NULL, printUpCase, NULL);
    pthread_create(&tid1, NULL, printLowCase, NULL);

    while (1) {
        char input[MAX_LENGTH];
        fgets(input,MAX_LENGTH,stdin);
        pthread_mutex_lock(&only_lock);
        for (int i = 0; i < MAX_LENGTH; ++i) {
            text[i] = input[i];
            if (input[i] == '\0') {
                break;
            }
        }
        pthread_cond_signal(&wake_lower);
        pthread_mutex_unlock(&only_lock);
    }
}
