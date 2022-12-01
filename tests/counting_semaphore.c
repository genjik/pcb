#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/csesem.h"

#define THREADS 5

CSE_Semaphore sem;

int count;
int quit;

pthread_mutex_t lock;
pthread_cond_t entered;
pthread_cond_t done;

void *test_thread(void *data) {
    csesem_wait(sem);

    /* Lock the lock, increment count, then notify the main thread that
     * this thread successfully entered the semaphore. */
    pthread_mutex_lock(&lock);
    count++;
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&entered);

    /* Wait for the main thread to indicate that this thread can exit
     * the semaphore. */
    pthread_mutex_lock(&lock);
    while (!quit) {
        pthread_cond_wait(&done, &lock);
    }
    pthread_mutex_unlock(&lock);

    /* Post the semaphore to let other threads in */
    csesem_post(sem);

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t[THREADS];
    int result;

    /* Configure the first THREADS - 1 threads to enter immediately. */
    sem = csesem_create(THREADS - 1);

    /* Create the lock and condition variables */
    if ((result = pthread_mutex_init(&lock, NULL)) != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(result));
        return 1;
    }

    if ((result = pthread_cond_init(&entered, NULL)) != 0) {
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(result));
        return 1;
    }

    if ((result = pthread_cond_init(&done, NULL)) != 0) {
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(result));
        return 1;
    }

    /* Create THREADS threads, all but one of which will enter
     * immediately, and the last one will block. */
    for (int i = 0; i < THREADS; i++) {
        if ((result = pthread_create(&t[i], NULL, test_thread, NULL)) != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(result));
            return 1;
        }
    }

    /* This is not really safe synchronization, but since we're trying
     * to test a synchronization mechanism for correctness, we'll just
     * delay for a bit here.  We will sleep for 100 ms (which is, of
     * course, forever in computer time), then lock the mutex and check
     * that count == THREADS - 1.  If it doesn't, that means that either
     * not all of the threads that should have been able to enter the
     * semaphore succeeded, or the last, extra thread got in
     * erroneously. */
    usleep(100000);
    pthread_mutex_lock(&lock);
    if (count != THREADS - 1) {
        if (count < THREADS - 1) {
            printf("failed: not all threads were able to wait\n");
        } else if (count == THREADS) {
            printf("failed: an extra thread was able to wait\n");
        } else {
            printf("failed: something unexpected happened\n");
        }
        return 1;
    }
    /* Let the waiting threads know they can exit. */
    quit = 1;
    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&done);

    /* Once again, wait 100 ms and make sure that the final thread was
     * able to get in. */
    usleep(100000);
    pthread_mutex_lock(&lock);
    if (count != THREADS) {
        printf("failed: final thread was unable to wait\n");
        return 1;
    }
    pthread_mutex_unlock(&lock);

    printf("passed\n");

    return 0;
}
