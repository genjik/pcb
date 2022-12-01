#include <pthread.h>
#include <stdlib.h>

#include "csesem.h"

/* This definition of struct CSE_Semaphore is only available _inside_
 * your semaphore implementation.  This prevents calling code from
 * inadvertently invalidating the internal representation of your
 * semaphore.  See csesem.h for more information.
 *
 * You may place any data you require in this structure. */
struct CSE_Semaphore {
};

/* This function must both allocate space for the semaphore and perform
 * any initialization that is required for safe operation on the
 * semaphore.  The user should be able to immediately call csesem_post()
 * or csesem_wait() after this routine returns. */
CSE_Semaphore csesem_create(int count) {
    CSE_Semaphore sem = calloc(1, sizeof(struct CSE_Semaphore));

    return sem;
}

void csesem_post(CSE_Semaphore sem) {
}

void csesem_wait(CSE_Semaphore sem) {
}

/* This function should destroy any resources allocated for this
 * semaphore; this includes mutexes or condition variables. */
void csesem_destroy(CSE_Semaphore sem) {
    free(sem);
}
