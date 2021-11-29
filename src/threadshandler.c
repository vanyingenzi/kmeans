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

#include "threadshandler.h"

/** 
 * It's structure of arguments given to the function to be executed by a thread calculating thread.
 * 
 * @param inputFile (file_t *) : The structure representing the input binary file.
 * @param programArgs (args_t *) : A structure containing the user input arguments.
 * @param read_buffer (circular_buf *) : A circular buffer in which to fetch initial centroids.
 * @param write_buffer (circular_buf *) : A circular buffer in which the string representations of the final clusers and centroids will be stored.
 *
 */ 
typedef struct {
    file_t * inputFile;
    args_t * programArgs;
    circular_buf * read_buffer;
    circular_buf * writer_buffer;
} calculation_thread_arguments_t ;


/**
 * This function must be given to each calculating thread. 
 * 
 * @param argT (void *) : This must be a casted (calculation_thread_arguments_t *) pointer. This structure contains the 
 * arguments are necessary to the function well being.
 * 
 * /!\ Note that this function may loop forever if the buffer in the structure is not well 
 * maintained, meaning that once producers are done they must set a signal and wake all consumers 
 * that might be waiting on the circular buffer.
 * 
 * @return (void *) NULL.
 */ 
void * calculationsFunction(void * argT)
{
    int possibleError = 0;
    // We cast the argument
    calculation_thread_arguments_t * args = (calculation_thread_arguments_t *) argT;
    list_of_centroids_and_clusters_only answerFromKeams;
    int booleanToUseInGet; 
    int booleanToUseInPut; 
    
    // Variables that are going to hold results in each iteration
    calculation_result_holder * tempHolder;

    // Local variable to hold centroids obtained from the buffer
    array_of_centroids * centroids;
    // Get the a initial centroid combination
    circularbuffer_get(args->read_buffer, &booleanToUseInGet, (void **)&centroids);
   
    while( centroids != NULL && possibleError == 0)
    {
        possibleError = k_means(&answerFromKeams, centroids, args->programArgs->k, args->inputFile);
        if (possibleError != 0)
        {
            fprintf(stderr, "[threadshandler.c] An error occured in kmeans function.\n");
            circularbuffer_handleError(args->read_buffer, "calculationsFunction");
            circularbuffer_handleError(args->writer_buffer, "calculationsFunction");
            return(NULL);
        }
        tempHolder = (calculation_result_holder * ) malloc( sizeof(calculation_result_holder));
        if (tempHolder == NULL)
        {
            printf("[threadshandler.c] A failed malloc for tempHolder \n");
            circularbuffer_handleError(args->read_buffer, "calculationsFunction");
            circularbuffer_handleError(args->writer_buffer, "calculationsFunction");
            return(NULL);
        }
        
        tempHolder->initialCentroids = centroids;
        tempHolder->finalCentroids = answerFromKeams.finalCentroids;
        tempHolder->distortion_distance = distortion_distance(answerFromKeams.finalCentroids, answerFromKeams.finalClusters);
        tempHolder->finalClusters = answerFromKeams.finalClusters;

        // Write the output to the buffer
        possibleError = circularbuffer_put(args->writer_buffer, &booleanToUseInPut,(void *) tempHolder);
        if(possibleError == 0)
        {
            // Get the next centroids from the buffer
            possibleError = circularbuffer_get(args->read_buffer, &booleanToUseInGet, (void **) &centroids);
        }
    }
    return(NULL);
}

/**
 * Sets the highest priority on the given attribute.
 * 
 * @param attribute (pthread_attr_t *) : The pointer to the attribute
 * 
 * @return 0 upon succes else, -1. In case of an error the default values of the attribute are not changed, 
 */
int setHighestPriority(pthread_attr_t * attribute){
    int possibleError = 0;
    struct sched_param param;
    int maxPriority;
    int schedulerPolicy;

    // We are going to give this attribute a high priority
    if (possibleError == 0){ // Check if an error occured above
        possibleError = pthread_attr_getschedpolicy(attribute, &schedulerPolicy); 
    }
    if (possibleError == 0){ // Check if an error occured above
        maxPriority = sched_get_priority_max(schedulerPolicy);
        possibleError = ( maxPriority == -1 ) ? -1 : 0;
    }
    if (possibleError == 0){ // Check if an error occured above
        param.sched_priority = maxPriority;
        possibleError = pthread_attr_setschedparam(attribute, &param);
    }

    return possibleError;
}

/**
 * This functions initialize the calculating threads and the output-writer thread. 
 * 
 * @param program_arguments (args_t) : The structure presenting program arguments.
 * @param inputFile (file_t *) : The pointer to the input file structure.
 * @param combinationsOfCentroids (circular_buf *) : The circular buffer in which calculating threads should get the centroids from.
 * 
 * @return int. O upon succesfull, else -1.
 */
int putThreadsToWork(   
                        args_t * program_arguments, file_t * inputFile,
                        circular_buf * initialCentroidsBuffer
                    )
{
    // An array of threads 
    pthread_t threadList[program_arguments->n_threads];
    uint32_t initiatedThreads = 0;
    int possibleError = 0;

    // We open the output file in order to pass it to the writing  thread
    FILE * outPutFile = fopen(program_arguments->output_pathName, "w+");
    if (outPutFile == NULL)
    {
        fprintf(stderr,"[threadsHandler.c]Error when opening the saving file < %s >:\n\t%s\n", program_arguments->output_pathName, strerror(errno) );
        circularbuffer_handleError(initialCentroidsBuffer, "putThreadsToWork");
        return -1;
    }

    // Write the first row
    if ( program_arguments->quiet )
    {
        fprintf(outPutFile, "initialization centroids,distortion,centroids\n");
    } else {
        fprintf(outPutFile, "initialization centroids,distortion,centroids,clusters\n");
    }

    // Initiliaze the stack for intermediare string before they are written into csv
    circular_buf bufferForCalculationsHolder;
    pthread_mutex_t buffer_mutex;
    calculation_result_holder * arrayToHoldResults[program_arguments->n_threads];
    if (circulabuffer_init(&bufferForCalculationsHolder, &buffer_mutex, program_arguments->n_threads, (void **) arrayToHoldResults) != 0)
    {
        fprintf(stderr, "[threadshandler.c] Could not initialise the circular buffer for calculations\n");
        fclose(outPutFile);
        return -1;
    }

    // We are going to create the thread that generates all combinations of initial centroids
    pthread_t outputWriterThread;
    bool initatedOutputWriterThread = false;

    // We are going to give this thread a high priority
    pthread_attr_t attr;
    possibleError = pthread_attr_init(&attr);
    if (possibleError == 0) 
    {
        setHighestPriority(&attr); // If the highest priority isn't set it's not a problem we just lose the time efficiency
    }
    
    writerThreadArgs_t argForWriter = { &bufferForCalculationsHolder, outPutFile, program_arguments->quiet };

    if (possibleError == 0)
    {
        if (pthread_create(&outputWriterThread, &attr, &writeToCSVFromBuffer, &argForWriter) == 0)
        {
            initatedOutputWriterThread = true;
        } else {
            circularbuffer_handleError(&bufferForCalculationsHolder, "putThreadsToWork");
            return -1;
        }
    }

    calculation_thread_arguments_t argumentToAllCalculatingThreads = { inputFile, program_arguments, initialCentroidsBuffer, &bufferForCalculationsHolder };
    
    if (possibleError == 0) // No error occured
    {
        // Start the calculating threads 
        for (size_t i = 0; i < program_arguments->n_threads; i++)
        {
            if (pthread_create( &(threadList[i]), NULL, &calculationsFunction, &argumentToAllCalculatingThreads ) == 0){
                initiatedThreads++;
            } else {
                // If no thread was initiated that means that no calculations will be made and that's a problem error otherwise it may due to max capacity
                if (initiatedThreads <= 0){ 
                    possibleError -= 1;
                }
                fprintf(stderr, "[threadshandler.c] Warning -- an error occured initiating calculation threads, had already initiated %d calculator threads\n", initiatedThreads);
                break;
            }
        }
    }
    
    // Wait for each calculating thread to finish
    if (initatedOutputWriterThread)
    {
        for (size_t i = 0; i < initiatedThreads; i++)
        {
            pthread_join( threadList[i], NULL );
        }
    }
    
    // Since we have more than one calculating thread 
    // we give the role to set the done signal, to the main thread 
    circularbuffer_setDone(&bufferForCalculationsHolder);

    // Wake all consumers on the output-string buffer holder
    wakeAllConsumers(&bufferForCalculationsHolder);
    
    // We wait for the output-writer thread to finish
    if(initatedOutputWriterThread)
    {
        pthread_join(outputWriterThread, NULL);
    }
    
    circularbuffer_destroy(&bufferForCalculationsHolder);
    
    if (EOF == fclose(outPutFile))
    { 
        fprintf(stderr,"[threadshandler.c] Error when closing < %s >:\n\t%s\n", program_arguments->output_pathName, strerror(errno) );
        return -1; 
    }
    return possibleError;
}