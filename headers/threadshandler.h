/****************
 * This header contains the declaretion to the function that will handle all the multi-threading
 ****************/

#ifndef THREADHANDLER_H
#define THREADHANDLER_H

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h> 

#include "argumentsparser.h"
#include "filehandler.h"
#include "circularbuffer.h"
#include "arrayofclusters.h"

typedef struct {
    array_of_centroids *initialCentroids;
    int64_t distortion_distance;
    array_of_centroids *finalCentroids;
    array_of_clusters *finalClusters;
}calculation_result_holder;
 
int putThreadsToWork(args_t *, file_t *, circular_buf * );
int setHighestPriority(pthread_attr_t * );

#endif //THREADHANDLER_H