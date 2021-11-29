#ifndef POINT_H
#define POINT_H

#include <string.h>
#include <stdint.h>

/**
 * This structures is used to represent a point of a certain dimension. 
 * - dimension (uint32_t *) : is the pointer to the dimension of the point.
 * - values (int64_t *) : is the pointer to the first element of an array composed of int64_t elements.
 *
 * Python Equivalent Presentation : Tuple.
 *
 * Example : 
 * For a point in 3D, (-12, 3, 5).
 * 
 * The C code initialisation is :
 * 
 * ```
 * int dim = 3
 * int64_t points[] = {-12, 3, 5}.
 * point_t point = {&dim, points}
 * 
 * ```
 */
typedef struct 
{
    uint32_t dimension;
    int64_t * values;

} point_t;

typedef point_t centroid_t ; 

int pointToFILE(FILE *, point_t *);

#endif //POINT_H
