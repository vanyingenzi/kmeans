#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#include "argumentsparser.h"
#include "distance.h"
#include "point.h"
#include "arrayofpoints.h"
#include "arrayofclusters.h"
#include "func.h"
#include "combinator.h"
#include "threadshandler.h"
#include "circularbuffer.h"

/**
 * Initiliaze the variables of the given buffer.
 * 
 * The initiliazing function presume that this buffer will be used in a consumer producer problem. Therefore, 
 * it will allocate 2 mutexes (full, empty) and mutex (mutex) on the heap.
 * 
 * ATTENTION : Think of freeing the structure with the specific function <circularbuffer_destroy> when done.
 * 
 * @param buff (circular_buf *) : The pointer to the buffer.
 * @param mutex (pthread_mutex_t *) : The mutex to synchronise for the circular buffer.
 * @param MaxSize (uint32_t) : The maximum size. (MaxSize > 0).
 * @param array (void **) : The array of pointers that will be holding the data.
 * 
 * MaxSize must be equal to the length used to initialize the buffer. 
 * 
 * @return signal(int) : 0 Upon succes else -1.
 */
int circulabuffer_init(circular_buf * buff, pthread_mutex_t * mutex, uint32_t MaxSize, void ** array){
    if ( buff == NULL ){ return -1; }
    buff->array = array;
    buff->maxSize = MaxSize;
    buff->takePointer = 0;
    buff->putPointer = 0;
    buff->in = 0;
    buff->done = 0;
    buff->empty = (sem_t *) malloc(sizeof(sem_t));
    if (buff->empty == NULL) { 
        fprintf(stderr, "[circularbuffer.c] Failed malloc when allocating the empty semphore \n");
        return -1; 
    }
    if (sem_init(buff->empty, 0, buff->maxSize) != 0)
    {
        free(buff->empty);
        fprintf(stderr, "[circularbuffer.c] Failed malloc when initiating the empty semphore\n");
        return -1;
    }

    buff->full = (sem_t *) malloc(sizeof(sem_t));
    if (buff->full == NULL) 
    { 
        free(buff->empty);
        fprintf(stderr, "[circularbuffer.c] Failed malloc when allocating the full semphore\n");
        return -1; 
    }

    if (sem_init(buff->full, 0, 0) != 0)
    {
        free(buff->empty);
        free(buff->full);
        fprintf(stderr, "[circularbuffer.c] Failed malloc when initiating the empty semphore\n");
        return -1;
    }

    buff->mutex = mutex;
    if ( pthread_mutex_init(buff->mutex, NULL) != 0 )
    {
        free(buff->empty);
        free(buff->full);
        fprintf(stderr, "[circularbuffer.c] Couldn't initiate the buffer mutex\n");
        return -1;
    }
    for (size_t i = 0; i < buff->maxSize; i++)
    {
        buff->array[i] = NULL; 
    }
    buff->waitingConsumers = 0;
    buff->waitingProducers = 0; 
    return 0; 
}

/** 
 * This function sets the done signal on the bufffer.
 * 
 * @param buff (circular_buf *) : The buffer.
 */ 
void circularbuffer_setDone(circular_buf * buff)
{
    pthread_mutex_lock(buff->mutex);
    buff->done = 1;
    pthread_mutex_unlock(buff->mutex);
}

/**
 * Wakes up all producers waiting on the empty semaphore of the buffer.
 * 
 * @param buff (circular_buf *) : A buffer that has been initliazed using <circulabuffer_init> function in [circularbuffer.c].
 */
void wakeAllProducers(circular_buf * buff)
{
    // When lock fails we still want to continue the execution cause the consumers might cause the program not to end
    pthread_mutex_lock(buff->mutex);
    // Critical Section
    for (size_t i = 0; i < buff->waitingProducers; i++)
    {
        sem_post(buff->empty);
    }
    // End of critical section 
    pthread_mutex_unlock(buff->mutex);
}

/**
 * Wakes up all producers waiting on the full semaphore of the buffer.
 * 
 * @param buff (circular_buf *) : A buffer that has been initliazed using <circulabuffer_init> function in [circularbuffer.c].
 */
void wakeAllConsumers(circular_buf * buff)
{
    // When lock fails we still want to continue the execution cause the producers might cause the program not to end
    pthread_mutex_lock(buff->mutex);
    // Critical Section
    for (size_t i = 0; i < buff->waitingConsumers; i++)
    {
        sem_post(buff->full);
    }
    // End of critical section 
    pthread_mutex_unlock(buff->mutex);
}

/**
 * This function is to be called in case of an error on the buffer.
 * It'S purpose is to ensure in case of an error every thread working on the buffer, joins the main_thread
 * 
 * @param buff (circular_buf *) : The circular buffer.
 * @param nameOfFunction (char *) : The name of the function that called this function.
 *  
 */
void circularbuffer_handleError(circular_buf * buff, char * nameOfFunction)
{
    fprintf(stderr, "[circularbuffer.c] Circular Buffer received a failure signal, in function %s\n", nameOfFunction);
    circularbuffer_setDone(buff);
    wakeAllProducers(buff);
    wakeAllConsumers(buff);
}


/**
 * Adds an element into a circular buffer without race conditions problems. 
 * 
 * ATTENTION : Note that the thread may have to wait for free space to be available in the buffer. Hence make sure that 
 * there are some other threads that are consuming the data and freeing spaces.
 * 
 * @param buff (circular_buf *) : A buffer that has been initliazed using <circulabuffer_init> function in [circularbuffer.c].
 * @param anErrorOccured (int *) : Variable used in order know if an error occured on the buffer. It doesn't need to be initiated.
 * @param toPut (void *) : The pointer to the object to add.
 *
 * @return int. Upon success 0 else -1.
 */
int circularbuffer_put(circular_buf * buff, int * anErrorOccured, void * toPut)
{
    pthread_mutex_lock(buff->mutex);
    *anErrorOccured = (buff->done == 1);
    buff->waitingProducers = (*anErrorOccured == 1 ) ? buff->waitingProducers : buff->waitingProducers + 1; 
    pthread_mutex_unlock(buff->mutex);

    if (*anErrorOccured == 1) // No need to add element 
    {
        return -1;
    }

    if (sem_wait(buff->empty) != 0) // Wait till there's a free zones
    {
        pthread_mutex_lock(buff->mutex);
        buff->waitingProducers--;
        pthread_mutex_unlock(buff->mutex);
        circularbuffer_handleError(buff, "circularbuffer_put");
        return -1;
    }

    pthread_mutex_lock(buff->mutex);
    // Start Critic Section

    // If it's not full, then we can set the next put point
    while ((buff->in < buff->maxSize) && buff->array[buff->putPointer] != NULL)
    {
        buff->putPointer = (buff->putPointer < buff->maxSize - 1) ? buff->putPointer + 1 : 0; 
    }
    
    buff->array[buff->putPointer] = toPut;
    buff->in = (buff->in == buff->maxSize) ? buff->maxSize : buff->in + 1;
    buff->waitingProducers--;
    
    // End of Critic Section
    pthread_mutex_unlock(buff->mutex);
    if (sem_post(buff->full) != 0)
    {
        circularbuffer_handleError(buff, "circularbuffer_put");
        return -1;
    } 
    return 0;
}

/**
 * Put an element from buffer in the holdResult pointer and removes the element
 * from the buffer.
 * 
 * ATTENTION : Note that the thread may have to wait for data to be put in the buffer. Hence make sure that 
 * there are some other threads that are producing the data.
 * 
 * @param buff (circular_buf *) : A buffer that has been initliazed using <circulabuffer_init> function.
 * @param noProducersLeftAndBufferEmpty (int *) : This will be used to store a proper variable to each 
 * thread as an inside signal to know if a thread has to wait or not.
 * @param holdResult (void **) : Pointer to the pointer that will hold the result.
 */
int circularbuffer_get(circular_buf * buff, int * noProducersLeftAndBufferEmpty, void ** holdResult)
{
    // Here we try to determine if it's worth entering the critical section or not
    pthread_mutex_lock(buff->mutex);
    *noProducersLeftAndBufferEmpty = ( buff->in == 0 && buff->done );
    if (!*noProducersLeftAndBufferEmpty){
        buff->waitingConsumers++;
    }
    pthread_mutex_unlock(buff->mutex);
    if ( *noProducersLeftAndBufferEmpty )
    {
        *holdResult = NULL;
        return -1;
    }

    if (sem_wait(buff->full) != 0) // Wait till there's a free zones
    {
        pthread_mutex_lock(buff->mutex);
        buff->waitingConsumers--;
        pthread_mutex_unlock(buff->mutex);
        circularbuffer_handleError(buff, "circularbuffer_get");
        return -1;
    }
    pthread_mutex_lock(buff->mutex);
    // Start Critic Section
    while ((buff->in > 0) && buff->array[buff->takePointer] == NULL)
    {
        buff->takePointer = (buff->takePointer < buff->maxSize - 1) ? buff->takePointer + 1 : 0;
    }

    buff->waitingConsumers--;
    *holdResult = buff->array[buff->takePointer];
    buff->array[buff->takePointer] = NULL;
    buff->in = ( buff->in == 0 ) ? 0 : buff->in - 1;

    // End of Critic Section
    pthread_mutex_unlock(buff->mutex);
    if ( sem_post(buff->empty) != 0)
    {
        circularbuffer_handleError(buff, "circularbuffer_get");
        return -1;
    } 
    return 0;
}

/**
 * Frees up a buffer that has been initialized.
 * 
 * @param buff (circular_buf *) : A buffer that has been initliazed using <circulabuffer_init> function in [circularbuffer.c]..
 */
void circularbuffer_destroy(circular_buf * buff){
    pthread_mutex_lock(buff->mutex);
    uint32_t waitingProducers = buff->waitingProducers;
    uint32_t waitingConsumers = buff->waitingConsumers;
    pthread_mutex_unlock(buff->mutex);

    while (waitingProducers > 0 && waitingConsumers > 0)
    {
        pthread_mutex_lock(buff->mutex);
        waitingProducers = buff->waitingProducers;
        waitingConsumers = buff->waitingConsumers;
        pthread_mutex_unlock(buff->mutex);
    }
    
    sem_destroy(buff->empty);
    sem_destroy(buff->full);
    free(buff->empty);
    free(buff->full);
}