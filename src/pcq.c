#include <pthread.h>
#include <stdlib.h>

#include "csesem.h"
#include "pcq.h"

/* This structure must contain everything you need for an instance of a
 * PCQueue.  The given definition is ABSOLUTELY NOT COMPLETE.  You will
 * have to add several items to this structure. */
struct PCQueue {
    int slots;
    void **queue;
	pthread_mutex_t lock;
	CSE_Semaphore has_space;
	CSE_Semaphore has_data;
	int head;
	int tail;
};

/* The given implementation performs no error checking and simply
 * allocates the queue itself.  You will have to create and initialize
 * (appropriately) semaphores, mutexes, condition variables, flags,
 * etc. in this function. */
PCQueue pcq_create(int slots) {
	if (slots <= 0) {
		return NULL;
	}

    PCQueue pcq;

    pcq = calloc(1, sizeof(*pcq));
	if (pcq == NULL) {
		return NULL;
	}

    pcq->queue = calloc(slots, sizeof(void *));
    pcq->slots = slots;

	if (pcq->queue == NULL) {
		return NULL;
	}

	int ret_code;
	ret_code = pthread_mutex_init(&pcq->lock, NULL);
	if (ret_code != 0) {
		return NULL;
	}

	pcq->has_space = csesem_create(slots);
	if (pcq->has_space == NULL) {
		return NULL;
	}

	pcq->has_data = csesem_create(0);
	if (pcq->has_data == NULL) {
		return NULL;
	}

	pcq->head = 0;
	pcq->tail = 0;

    return pcq;
}

/* This implementation does nothing, as there is not enough information
 * in the given struct PCQueue to even usefully insert a pointer into
 * the data structure. */
void pcq_insert(PCQueue pcq, void *data) {
	csesem_wait(pcq->has_space);
	pthread_mutex_lock(&pcq->lock);	

	pcq->queue[pcq->tail] = data;
	pcq->tail = (pcq->tail + 1) % pcq->slots;

	pthread_mutex_unlock(&pcq->lock);	
	csesem_post(pcq->has_data);
}

/* This implementation does nothing, for the same reason as
 * pcq_insert(). */
void *pcq_retrieve(PCQueue pcq) {
	csesem_wait(pcq->has_data);
	pthread_mutex_lock(&pcq->lock);

	void *ret_addr = pcq->queue[pcq->head];
	pcq->head = (pcq->head + 1) % pcq->slots;

	pthread_mutex_unlock(&pcq->lock);
	csesem_post(pcq->has_space);
    return ret_addr;
}

/* The given implementation blindly frees the queue.  A minimal
 * implementation of this will need to work harder, and ensure that any
 * synchronization primitives allocated here are destroyed; a complete
 * and correct implementation will have to synchronize with any threads
 * blocked in pcq_insert() or pcq_retrieve().
 *
 * You should implement the complete and correct clean teardown LAST.
 * Make sure your other operations work, first, as they will be tightly
 * intertwined with teardown and you don't want to be debugging it all
 * at once!
 */
void pcq_destroy(PCQueue pcq) {
	csesem_destroy(pcq->has_space);
	csesem_destroy(pcq->has_data);

	int ret_code = pthread_mutex_destroy(&pcq->lock);
	if (ret_code != 0) {
		return;
	}

    free(pcq->queue);
    free(pcq);
}
