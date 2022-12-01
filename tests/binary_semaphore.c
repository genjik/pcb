#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/csesem.h"

#define COUNT 100000
#define THREADS 2
#define OFFSET 0x1134A

CSE_Semaphore lock;

int value;

/* This thread will simply add COUNT to the global value, one at a
 * time. */
void *counter(void *arg) {
    for (int i = 0; i < COUNT; i++) {
        csesem_wait(lock);
        value++;
        csesem_post(lock);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t[THREADS];
    int result;

    /* Create this semaphore with a value of 0, so that with a working
     * semaphore the counter threads will immediately block.  This lets
     * us prevent the test from spuriously passing because the created
     * threads happen to run to completion before the next thread is
     * created, by adding an offset after creating all of the threads
     * and before releasing them. */
    lock = csesem_create(0);

    for (int i = 0; i < THREADS; i++) {
        if ((result = pthread_create(&t[i], NULL, counter, NULL)) != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(result));
            return 1;
        }
    }

    /* Wait briefly for the threads to block. */
    usleep(10000);

    /* Set the global value to OFFSET, as a place for the counting to
     * start.  If the threads above have already been released, this
     * will force a failure. */
    value = OFFSET;

    /* Release the threads to do their thing. */
    csesem_post(lock);

    for (int i = 0; i < THREADS; i++) {
        pthread_join(t[i], NULL);
    }

    if (value == COUNT * THREADS + OFFSET) {
        printf("passed\n");
    } else {
        printf("failed\n");
        return 1;
    }

    return 0;
}
