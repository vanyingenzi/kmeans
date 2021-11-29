#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <sched.h>

#include "argumentsparser.h"
#include "distance.h"
#include "point.h"
#include "arrayofpoints.h"
#include "arrayofclusters.h"
#include "circularbuffer.h"
#include "func.h"
#include "combinator.h"
#include "threadshandler.h"
#include "filehandler.h"

int main(int argc, char *argv[]) 
{
    args_t program_arguments;  // Structure to store the input file 
    file_t inputFile; // Structure to store the binary file components
    int possibleError = 0; // The signal that we check throughout main to make sure that no error occured prior.
    bool combinationThreadLaunched = false;

    // Read the user arguments
    if ( parse_args(&program_arguments, argc, argv) != 0)
    {
        fprintf(stderr, "[main.c] An error occured when parsing the user inputs\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }                      
    
    // Read the input file 
    if ( fileRead(&inputFile, program_arguments.input_pathName) != 0 )
    { 
        fprintf(stderr, "[main.c] An error occured when reading the binary input file\n");
        return EXIT_FAILURE; 
    }

    // Check if -p is n't bigger than the available number of points
    if ( program_arguments.n_first_initialization_points > inputFile.nbOfPoints )
    {
        fprintf(stderr, "[main.c] -p argument must be less or equal than the number of points available in the input file\n");
        usage(argv[0]);
        freeFileStruct(&inputFile);
        return EXIT_FAILURE;
    }
    
    // Initiliaze the buffer that will contain the possible initial centroids combinations
    size_t bufferSize = program_arguments.n_threads * 10;
    array_of_centroids * arrayOfInitCentroids[bufferSize];
    circular_buf bufferForInitialCentroids;
    pthread_mutex_t buffer_mutex;

    if ( circulabuffer_init(&bufferForInitialCentroids, &buffer_mutex, bufferSize, (void **) arrayOfInitCentroids) != 0 )
    {
        fprintf(stderr, "[main.c] An error occured when initiating a the intial centroids buffer\n");
        freeFileStruct(&inputFile);
        return EXIT_FAILURE;
    }

    // We are going to create the thread that generates all combinations of initial centroids
    pthread_t threadForCombinations;

    // We are going to give this thread a high priority
    pthread_attr_t attr;
    possibleError = pthread_attr_init(&attr);
    // It's important that the combination thread has a higher priority.
    possibleError += (setHighestPriority(&attr) == 0) ? 0 : -1;

    if (possibleError == 0){ // Check if no error occured above
        // The argument to give to the combination thread
        combinations_args_t arg = { (&inputFile)->ptrToPoints, &program_arguments, &bufferForInitialCentroids };
        if(pthread_create(&threadForCombinations, &attr, &getAllCentroidCombinations, &arg) == 0)
        {
            combinationThreadLaunched = true;
        } else {
            fprintf(stderr, "[main.c] Creating the combinational centroid failed \n");
            possibleError += -1;
        }
    }
    
    // Now we are going to run the calculating threads and output-writer thread
    if (possibleError == 0) // Check if no error occured above
    {
        if (putThreadsToWork(&program_arguments, &inputFile, &bufferForInitialCentroids) != 0)
        {
            possibleError += -1;
            fprintf(stderr, "[main.c] An error occured when calling the function to run calculations threads and output writer thread\n");
        }
    }
    
    // Notice we don't check if an error occured here cause even if it occured we still want to free resources
    circularbuffer_destroy(&bufferForInitialCentroids);
    freeFileStruct(&inputFile);

    if (combinationThreadLaunched) 
    {
        pthread_join(threadForCombinations, NULL);
    }
    
    return possibleError;
}