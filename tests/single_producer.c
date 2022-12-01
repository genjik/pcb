#include <pthread.h>
#include <stdio.h>
#include <stdint.h>             /* for intptr_t */
#include <unistd.h>             /* for usleep */

#include "../src/pcq.h"

#define NMSGS 10         /* The producer will send this many messages */
#define MSG0 0           /* This is the first message that will be sent */
#define QSLOTS 5         /* The number of slots in the shared PCQueue */

/* This is the producer thread, which will be created by main().  It
 * simply places ten consecutive integers on the PCQueue that it is
 * passed as its argument.  Note that, as its argument is a void *, it
 * must first cast its argument to type PCQueue (which is struct PCQueue
 * *, as per pcq.h). */
void *producer(void *arg) {
    PCQueue pcq = arg;          /* A simple cast, see above */

    for (int i = MSG0; i < MSG0 + NMSGS; i++) {
        /* We can send an integer by casting it to void *, but we have
         * to cast to intptr_t first to avoid warnings about casting an
         * integer to a pointer of a different size. */
        pcq_insert(pcq, (void *)(intptr_t)i);
    }

    /* We don't care about this value */
    return NULL;
}

/* The main function will create a producer thread, then sleep long
 * enough to ensure that the producer is blocked trying to add more
 * items to the PCQueue.  It will then read the items off the queue and
 * ensure that they range from MSG0 to MSG0 + NMSGS - 1, in order. */
int main(int argc, char *argv[])
{
    PCQueue pcq = pcq_create(QSLOTS);
    pthread_t t;

    if (pcq == NULL) {
        puts("failed: pcq_create");
        return 1;
    }

    if (pthread_create(&t, NULL, producer, pcq) == -1) {
        puts("failed: pthread_create");
        return 1;
    }

    /* Sleep for 10 ms.  That's forever in computer years, so the
     * producer should already have inserted at least QSLOTS messages
     * and be blocked waiting for us to read. */
    usleep(10000);

    /* Read NMSGS messages off the queue, and ensure that they are the
     * correct values. */
    for (int i = MSG0; i < MSG0 + NMSGS; i++) {
        /* We cast to intptr_t and then int to avoid warnings; see
         * producer */
        int value = (int)(uintptr_t)pcq_retrieve(pcq);

        if (value != i) {
            puts("failed");
            return 1;
        }
    }

    /* At this point the producer thread should be done; join it to make sure. */
    pthread_join(t, NULL);

    /* Destroy the PCQueue */
    pcq_destroy(pcq);

    puts("passed");
    return 0;
}
