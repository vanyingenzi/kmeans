#ifndef COMBINATOR_H
#define COMBINATOR_H

#include "point.h"
#include "func.h"
#include "filehandler.h"
#include "arrayofpoints.h"
#include "arrayofclusters.h"
#include "threadshandler.h"
#include "argumentsparser.h"

/** This structure is going to be passed to the thread that generates combinations
 * 
 * @param pointsToPickFrom (point_t *) : The array of point_t to use within combinations.
 * @param inputArgs (args_t *) : The input arguments
 * @param buff (circular_buf *) : The circular buffer in which the thread will write the combinations into.
 */ 
typedef struct
{
    point_t * pointsToPickFrom;
    args_t * inputArgs;
    circular_buf * buff;
} combinations_args_t;

void * getAllCentroidCombinations(void *);

#endif //COMBINATOR_H