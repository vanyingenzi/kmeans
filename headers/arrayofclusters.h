#ifndef ARRAY_OF_CLUSTERS
#define ARRAY_OF_CLUSTERS

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "point.h"
#include "arrayofpoints.h"

/**
 * This structure represents a 2D array of point_t. In otherwords, an array of array_of_points structures. 
 * 
 * @param size (uint32_t) : The number of array_of_points that it holds.
 * @param array (array_of_points *) :  The pointer to the first array_of_points element
 * 
 * Python Equivalent Representation : List [ List [ Tuple ] ] 
 */
typedef struct
{
    uint32_t size;
    array_of_points * array;
} array_of_arrays_of_points ;

typedef array_of_arrays_of_points array_of_arrays_of_centroids;
typedef array_of_arrays_of_points array_of_clusters;

void arrayOfClusters_destroy(array_of_arrays_of_points * , bool);

int arrayOfClusters_toFILE(FILE *, array_of_arrays_of_points *);

#endif //ARRAY_OF_CLUSTERS
