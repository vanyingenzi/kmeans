#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h> 

#include "arrayofpoints.h"

/**
 * A fixed size circular buffer.
 * 
 * @param maxSize (uint32_t) : The maximum size of the buffer.
 * @param array (void **) : The array in which the pointers will be stored. The size of the array must be equal the maxSize.
 * @param takePointer (uint32_t) : The pointer to where to pick data from the array.
 * @param putPointer (uint32_t) :  The pointer to where to put data from the array.
 * @param in (uint32_t) : The number of elements in the array.
 * @param done (int) : A signal to know if producers are done. 1 if producers are done, 0 else.
 * @param empty (sem_t *) : A semaphore.
 * @param full (sem_t *) : A semaphore.
 * @param mutex (pthread_mutex_t *) : A mutex for synchronisation.
 * @param waitingConsumers (uint64_t) : The number of consumers waiting on the buffer.
 * @param waitingProducers (uint64_t) : The number of producers waiting on the buffer.
 * 
 * ATTENTION : Note that there are specific functions for :
 *              - initializing a circular buffer : <circulabuffer_init>
 *              - adding elements : <circularbuffer_put> 
 *              - get elements : <circularbuffer_get>
 *              - wake producers threads : <wakeAllProducers>
 *              - wake consumers threads : <circularbuffer_get>
 * 
 * It's better using these APIs for race free conditions.
 */
typedef struct 
{
    uint32_t maxSize;
    void ** array;
    uint32_t takePointer;
    uint32_t putPointer;
    uint32_t in;
    int done;
    sem_t * empty;
    sem_t * full;
    pthread_mutex_t * mutex;
    uint64_t waitingConsumers;
    uint64_t waitingProducers;
} circular_buf ;

typedef void (*putFunction) (circular_buf *, void *);
typedef void * (*getFunction) (circular_buf *);
int circulabuffer_init(circular_buf * buff, pthread_mutex_t * mutex, uint32_t MaxSize, void ** array);
void circularbuffer_setDone(circular_buf * );
void wakeAllProducers(circular_buf * );
void wakeAllConsumers(circular_buf * );
void circularbuffer_handleError(circular_buf *, char *);
int circularbuffer_put(circular_buf *, int *, void *);
int circularbuffer_get(circular_buf *, int *, void **);
void circularbuffer_destroy(circular_buf *);

#endif // CIRCULAR_BUFFER