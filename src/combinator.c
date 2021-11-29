/*****************************************************
 * The algorithm used here was inspired from the following page.
 *
 * geeksforgeeks.org/print-all-possible-combinations-of-r-elements-in-a-given-array-of-size-n/
 *
 * We had to adapt it to our problem.
 *
 *****************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "argumentsparser.h"
#include "point.h"
#include "func.h"
#include "filehandler.h"
#include "threadshandler.h"
#include "combinator.h"

/**
 * This function generates all possible combinations of centroids.
 * 
 * @param arr (point_t *) : An array of all centroids to generate combinations from.
 * @param tempData (point_t *) : An array to store a combinations thoughout the iterations.
 * @param start (uint32_t) : Start index.
 * @param end (uint32_t) : End index.
 * @param index (uint32_t) : The current index in arr.
 * @param r (uint32_t) : The length wanted that each combination will have.
 * @param finalResultHolder (array_of_arrays_of_centroids * ) : The structure that will hold all possible combinations.
 * 
 * @return (int) : Upon success 0 else :
 *                  -1 if the error is due to the buffer.
 *                  -2 if the error is due to the function it's self.
 */ 
int combinationHelper( 
                        point_t * arr, point_t * tempData,
                       size_t start, size_t end,
                       size_t index, size_t r,
                       circular_buf * finalResultHolder
                    )
{
    if (index == r)
    {
         int booleanToUseInPut;   
        array_of_centroids * toAddToFinal = (array_of_centroids * ) malloc( sizeof(array_of_centroids) );
        if(toAddToFinal == NULL)
        {
            fprintf(stderr, "[combinator.c] Failed malloc for the memory necesessary to hold an array of centroids structure\n");
            return -2;
        }
        // Initiate the array of centroid
        toAddToFinal->size = r;
        toAddToFinal->points = (point_t *) malloc( sizeof(point_t) * r );
        if (toAddToFinal->points == NULL) 
        { 
            toAddToFinal->size = 0;
            // We are going to free the components of return
            arrayOfPoints_destroy(toAddToFinal);
            fprintf(stderr, "[combinator.c] Failed malloc to hold an array of centroids\n");
            return -2; 
        }
        for (size_t i = 0; i < r; i++)
        {
            point_t * currentPoint = (toAddToFinal->points + i);
            currentPoint->dimension = tempData[i].dimension;
            currentPoint->values = (int64_t *) malloc( sizeof(int64_t) * currentPoint->dimension );
            if (currentPoint->values == NULL)
            {
                toAddToFinal->size = 0;
                arrayOfPoints_destroy(toAddToFinal);
                fprintf(stderr, "[combinator.c] Failed malloc to hold values of a certain point\n");
                return -2;
            }
            memcpy( currentPoint->values, tempData[i].values, sizeof(int64_t) * tempData[i].dimension );
        }
        // We add the combination to the circular buffer
        return circularbuffer_put(finalResultHolder, &booleanToUseInPut, (void *) toAddToFinal);;
    }
    int possibleError; // Will be used as a signal to know if an error occured 
    for(size_t i = start; i <= end && end-i+1 >= r - index; i++)
    {
        tempData[index] = arr[i];
        possibleError = combinationHelper(arr, tempData, i+1, end, index+1, r, finalResultHolder);
        if (possibleError != 0) // This means an error occured
        { 
            return possibleError;
        }
    }
    return 0;
}

/**
 * This function calculates the initial centroids combinations and stores them in a buffer.
 *
 * The number of elements in pointsToPickFrom >= K >= 1.
 *
 * @param pointsToPickFrom (point_t *) : An array of points to pick from.
 * @param inputArgs (args_t *) : The strcuture representing the user input files.
 * @param buff (circular_buf *) : The buffer in which to store the combinations.
 */
void * getAllCentroidCombinations( void * argT)
{
    int possibleError = 0;
    if (argT == NULL){ return (NULL); }
    combinations_args_t * args = (combinations_args_t *) argT;
    point_t temp[args->inputArgs->k];
    possibleError = combinationHelper(args->pointsToPickFrom, temp, 0,
                     args->inputArgs->n_first_initialization_points-1,
                     0, args->inputArgs->k, args->buff);
    if (possibleError == -2 )
    {
        circularbuffer_handleError(args->buff, "getAllCentroidCombinations");
    } else {
        circularbuffer_setDone(args->buff);
        wakeAllConsumers(args->buff);
    }
    return (NULL);
}