#ifndef ARRAY_OF_POINTS_H
#define ARRAY_OF_POINTS_H

#include <stdint.h>
#include <stdbool.h>

#include "point.h"

/**
 * This structure represents an array of point_t structures.
 *
 * It's a dynamic array. Meaning by calling the specific API <arrayOfPoints_init> function the array can be increased
 * 
 * @param size (uint32_t) : The number of elements inside the array.
 * @param points (point_t *) : The content, elements which are point_t *.
 * @param allocatedSize (uint32_t) : The allocated number of sizeof(point_t) bytes that need to be free.\
 * 
 * Note that there are specific function for this data structure.
 * 
 *      - Initialize            = <arrayOfPoints_init>.
 *      - Add a point_t *       = <arrayOfPoints_append>.
 *      - Destroy with content  = <arrayOfPoints_destroy>. 
 * 
 */
typedef struct {
    uint32_t size;
    point_t * points;
    uint32_t allocatedSize;
} array_of_points ;

typedef array_of_points array_of_centroids;

typedef array_of_points cluster_t;

int arrayOfPoints_init(array_of_points *, uint32_t);

int arrayOfPoints_append(array_of_points *, point_t *, void **);

int arrayOfPoints_writeToFILE(FILE *, array_of_points *, bool);

void arrayOfPoints_destroy(array_of_points *);



#endif //ARRAY_OF_POINTS_H