#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../src/pcq.h"

#define NMSGS 10                /* The number of messages each producer
                                 * should send */
#define NSLOTS 3                /* The number of slots in the PCQueue */
#define NTHREADS 5              /* The number of producers */

/* This structure is what each producer thread places on the queue.  It
 * contains the thread's ID and the item that it has just placed on the
 * queue.  It is also passed to the thread at creation time to
 * communicate the thread's ID and the first item it should send. */
struct Info {
    int thread;
    int value;
};

/* All threads will use this global PCQueue, which is initialized by
 * main(). */
PCQueue pcq;

/* Each producer thread runs this function.  The function accepts a
 * struct Info * argument to identify itself and its starting message,
 * and then sends increasing integers along with its thread ID back to
 * main. */
void *producer(void *voidarg) {
    struct Info *arg = voidarg; /* Cast to struct Info * */

    for (int i = 0; i < NMSGS; i++) {
        struct Info *info = malloc(sizeof(*info));

        info->thread = arg->thread;
        info->value = arg->value + i;

        pcq_insert(pcq, info);
    }

    /* Ignored */
    return NULL;
}

int main(int argc, char *argv[])
{
    /* This array contains the initial value to be read from each
     * thread. */
    int initial[NTHREADS];
    /* This array contains the next value expected to be read from each
     * thread. */
    int expected[NTHREADS];
    /* Thread IDs for each thread */
    pthread_t tid[NTHREADS];

    for (int i = 0; i < NTHREADS; i++) {
        /* Some arbitrary starting positions */
        initial[i] = expected[i] = i * 37;
    }

    if ((pcq = pcq_create(NSLOTS)) == NULL) {
        puts("failed: pcq_create");
        return 1;
    }

    for (int i = 0; i < NTHREADS; i++) {
        struct Info *info = malloc(sizeof(*info));
        info->thread = i;
        info->value = expected[i];

        if (pthread_create(&tid[i], NULL, producer, info) < 0) {
            puts("failed: pthread_create");
            return 1;
        }
    }

    /* At this point we have created all of the producer threads; let's
     * go ahead and pause for just a moment to let the queue fill up,
     * then start reading. */
    usleep(10000);              /* 10 ms */

    /* Now we loop until every thread has sent NMSGS messages, and make
     * sure that each thread sends the expected message. */
    for (int i = 0; i < NMSGS * NTHREADS; i++) {
        struct Info *received = pcq_retrieve(pcq);

        if (received == NULL) {
            puts("failed: received NULL");
            return 1;
        }

        if (received->thread < 0 || received->thread > NTHREADS) {
            puts("failed: corrupt message");
            return 1;
        }

        if (received->value > initial[received->thread] + NMSGS) {
            puts("failed: bad value");
            return 1;
        }

        if (received->value != expected[received->thread]) {
            puts("failed: bad value");
            return 1;
        }

        expected[received->thread]++;
    }

    /* Join every thread to make sure they're not waiting on pcq */
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    /* Tear down the queue */
    pcq_destroy(pcq);

    puts("passed");
    return 0;
}
