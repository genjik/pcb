#ifndef PCQ_H_
#define PCQ_H_

/* This is another encapsulated data type as described in the handout
 * and csesem.h.  The implementation file contains a definition for this
 * struct.
 */
typedef struct PCQueue *PCQueue;

/* Create a PCQueue with the specified number of slots.  This queue must
 * accept slots insertions with no intervening retrievals before an
 * insertion would block, and must block any insertions after slots
 * insertions until at least one item is retrieved.
 *
 * This function should return NULL if initialization cannot be
 * accomplished (for example, if slots is <= 0 or if memory allocation
 * or other initializations fail).*/
PCQueue pcq_create(int slots);

/* Insert a single item into pcq.  This method should succeed
 * immediately if there is at least one available slot in the queue, and
 * block until a slot becomes available, otherwise.  You may assume
 * that, at the time that pcq_insert() is called, pcq is a queue
 * returned by a successful call to pcq_create() and that pcq_destroy()
 * has not yet been called on it.
 *
 * If the queue pcq is destroyed before this insert can complete, this
 * function should return with no effect.
 */
void pcq_insert(PCQueue pcq, void *data);

/* Remove a single item from pcq.  This method should succeed
 * immediately and return the retrieved item if there is at least one
 * available item in the queue, and block until an item is available
 * otherwise.  You may assume that, at the time that pcq_retrieve() is
 * called, pcq is a queue returned by a successful call to pcq_create()
 * and that pcq_destroy() has not yet been called on it.
 *
 * If the queue pcq is destroyed before this retrieval can complete,
 * this function should return NULL.
 */
void *pcq_retrieve(PCQueue pcq);

/* Destroy the PCQueue pcq, releasing any resources it may be using.  If
 * any threads are blocked on pcq_insert() or pcq_retrieve(), they
 * should be released according to the documentation for their
 * respective functions before the synchronization primitives and memory
 * for pcq are destroyed/released.  Once this function returns, any
 * access to pcq is invalid.  Your code does not need to check for
 * access to an invalid queue.
 */
void pcq_destroy(PCQueue pcq);

#endif /* PCQ_H_ */
