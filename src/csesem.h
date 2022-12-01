#ifndef CSESEM_H_
#define CSESEM_H_

/* The CSE_Semaphore structure is an example of how encapsulation is
 * often implemented in C.  This header defines a CSE_Semaphore as a
 * pointer to type struct CSE_Semaphore, but does not define the struct
 * in this file.  It is valid in C to declare a pointer to a structure
 * for which there is no definition _as long as you never dereference
 * the pointer_.  The pointer then becomes what is called an opaque
 * type; the code using the pointer knows that there is some data stored
 * there, but cannot access it.
 *
 * The implementation file contains a definition for the struct, and can
 * therefore access its contents.
 *
 * This is sometimes accomplished by using the type void *, but the
 * technique used here is more type safe.
 */
typedef struct CSE_Semaphore *CSE_Semaphore;

/* Create a CSE_Semaphore with the initial value count.  This function
 * should return NULL if the semaphore cannot be created for some
 * reason. (E.g., count is negative, an initialization function returns
 * an error, etc.) */
CSE_Semaphore csesem_create(int count);

/* Post on a CSE_Semaphore.  This will increase the count on the
 * semaphore, and notify a waiting task if any such task exists. */
void csesem_post(CSE_Semaphore sem);

/* Wait on a CSE_Semaphore.  If the count on this semaphore is greater
 * than zero, it will decrement the count and return immediately.  If
 * the count is zero, csesem_wait() will wait until the count is
 * greater than zero, then decrement the count and return. */
void csesem_wait(CSE_Semaphore sem);

/* Destroy a CSE_Semaphore, releasing any resources it may be using.
 * After calling this function, any access to the semaphore is an
 * invalid operation.  Your code does not need to check for access to an
 * invalid semaphore.  (Because there's no safe way to do so once it's
 * freed!) */
void csesem_destroy(CSE_Semaphore sem);

#endif /* CSESEM_H_ */
