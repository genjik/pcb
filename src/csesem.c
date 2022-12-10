#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "csesem.h"

/* This definition of struct CSE_Semaphore is only available _inside_
 * your semaphore implementation.  This prevents calling code from
 * inadvertently invalidating the internal representation of your
 * semaphore.  See csesem.h for more information.
 *
 * You may place any data you require in this structure. */
struct CSE_Semaphore {
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int count;
};

/* This function must both allocate space for the semaphore and perform
 * any initialization that is required for safe operation on the
 * semaphore.  The user should be able to immediately call csesem_post()
 * or csesem_wait() after this routine returns. */
CSE_Semaphore csesem_create(int count) {
	if (count < 0) {
		return NULL;
	}

    CSE_Semaphore sem = calloc(1, sizeof(struct CSE_Semaphore));
	if (sem == NULL) {
		return NULL;
	}

	int ret_code;
	ret_code = pthread_mutex_init(&sem->lock, NULL);
	if (ret_code != 0) {
		return NULL;
	}

	ret_code = pthread_cond_init(&sem->cond, NULL);
	if (ret_code != 0) {
		return NULL;
	}

	sem->count = count;

    return sem;
}

void csesem_post(CSE_Semaphore sem) {
	pthread_mutex_lock(&sem->lock);
	sem->count++;
	pthread_cond_signal(&sem->cond);
	pthread_mutex_unlock(&sem->lock);
}

void csesem_wait(CSE_Semaphore sem) {
	pthread_mutex_lock(&sem->lock);
	while (sem->count < 1) {
		pthread_cond_wait(&sem->cond, &sem->lock);
	}
	sem->count--;
	pthread_mutex_unlock(&sem->lock);
}

/* This function should destroy any resources allocated for this
 * semaphore; this includes mutexes or condition variables. */
void csesem_destroy(CSE_Semaphore sem) {
	sem->count = 10000;
	pthread_cond_broadcast(&sem->cond);

	int ret_code = 0;

	ret_code = pthread_cond_destroy(&sem->cond);
	if (ret_code != 0) { 
		fprintf(stderr, "error: csesem_destroy() while destroying cond\n");
		return;
	}

	ret_code = pthread_mutex_destroy(&sem->lock);
	if (ret_code != 0) { 
		fprintf(stderr, "error: csesem_destroy() while destroying mutex\n");
		return;
	}
	
    free(sem);
}
