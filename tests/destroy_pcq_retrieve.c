#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "../src/pcq.h"

/* This thread waits about 10 ms and then destroys the PCQueue that is
 * passed in as its argument. */
void *destroyer(void *arg) {
    usleep(10000);
    pcq_destroy(arg);           /* No need to cast void * */
    return NULL;
}

/* This test creates a PCQueue, passes it to a destroyer thread that
 * will destroy the queue after 10 ms, and then tries to retrieve a
 * value from the queue.  If it receives NULL after approximately 10 ms,
 * then it means that from main's point of view the destruction of the
 * PCQueue went as expected. */
int main(int argc, char *argv[])
{
    pthread_t t;
    struct timeval before, after, diff;
    void *result;
    PCQueue pcq = pcq_create(1); /* slots doesn't matter here */

    if (pcq == NULL) {
        puts("failed: pcq_create");
        return 1;
    }

    /* Get the time before we create the destroyer thread */
    gettimeofday(&before, NULL);
    pthread_create(&t, NULL, destroyer, pcq);

    /* Now start waiting on pcq, and expect to return in ~10 ms */
    result = pcq_retrieve(pcq);

    /* Take another timestamp */
    gettimeofday(&after, NULL);

    if (result != NULL) {
        puts("failed");
        return 1;
    }

    /* Subtract before from after to get the elapsed time */
    timersub(&after, &before, &diff);

    /* Allow for sleeping anywhere between 9.5 and 20 ms */
    if (diff.tv_sec > 0 || diff.tv_usec < 9500 || diff.tv_usec > 30000) {
        puts("failed");
        return 1;
    }

    puts("passed");
    return 0;
}
