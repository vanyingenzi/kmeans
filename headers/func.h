#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <stdint.h>

#include "arrayofpoints.h"
#include "arrayofclusters.h"
#include "filehandler.h"

typedef struct {
    array_of_centroids *finalCentroids;
    array_of_clusters *finalClusters;
}list_of_centroids_and_clusters_only;
 
int k_means(list_of_centroids_and_clusters_only * ptr,
            array_of_centroids *,
            uint32_t , file_t *);

#endif //FUNC_H