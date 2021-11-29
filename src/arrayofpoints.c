#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>

#include "arrayofpoints.h"
#include "point.h"


/**
 * Initialize the array_of_points structure by allocating the specified size of point_t
 * the total number of bytes allocated will be [ sizeof(point_t) * size ].
 * 
 * @param structure : The pointer to the structure.
 * @param size : The initial capacity of point_t it can hold.
 * 
 * @return int : 0 upon succes else -1. 
 * 
 */
int arrayOfPoints_init(array_of_points * structure, uint32_t size){
    int possibleError = 0;
    if (size == 0)
    {
        structure->size = 0;
        structure->points = NULL;
        structure->allocatedSize = 0;
        return possibleError;
    }
    
    structure->points = (point_t *) malloc( sizeof(point_t) * size );
    possibleError = (structure->points != NULL) ? 0 : -1;
    if (possibleError == 0)  //Check to see if no error occured
    {
        structure->allocatedSize = size;
        structure->size = 0;
    }
    return possibleError;
}

/**
 * Adds the point_t to the array of points. If the array is full it will reallocate
 * memory and increase the allocatedSize by a factor of 2.
 * 
 * @param structure (array_of_points *) : The structure.
 * @param element (point_t *) : The pointer to the point to add.
 * @param hold (void **) : Temporary variable used to hold a pointer within the function.
 * 
 * @return 0 upon success else < 0.
 */
int arrayOfPoints_append(array_of_points * structure, point_t * element, void ** hold){
    int possibleError = 0;
    // Condition to know if reallocation is needed
    bool reallocationNeeded = (structure->allocatedSize == 0) || structure->allocatedSize == structure->size;

    // Initiliaze allocatedSize on empty list
    structure->allocatedSize = (structure->allocatedSize == 0) ? 2 : structure->allocatedSize;
    
    if (reallocationNeeded) // There's no space
    {
        // Reallocate memory
        *hold = realloc(structure->points, sizeof(point_t) * (structure->allocatedSize * 2) );
        possibleError = (*hold != NULL) ? 0 : -1;
        if (possibleError == 0) // Check to see if no error occured
        {
            // Copy the new adress and increase allocatedSize
            structure->points = (point_t *) *hold;
            structure->allocatedSize*=2;
        }
    } 
    
    if (possibleError == 0) // Check if error occured
    {
        *(structure->points + structure->size) = *element; 
        structure->size+=1;
    }

    return possibleError;
}

/**
 * Write the content of the array of points to the file given in parameter
 * 
 * @param file (FILE *) : The FILE structure of an opened file. 
 * @param arrayOfPoints (array_of_points *) : The pointer to the array of points.
 * @param quotations (bool) : Boolean indicating if the <"? should be added at the start and end.
 * 
 * @return 0 upon success else, -1.
 */
int arrayOfPoints_writeToFILE(FILE * file, array_of_points * arrayOfPoints, bool quotations){
    int error = 0; 
    if (quotations)
    {
        error += fprintf(file, "\"") < 0 ? 0 : -1 ; 
    }
    
    error += fprintf(file, "[") < 0 ? 0 : -1 ; 
    for (size_t i = 0; i < arrayOfPoints->size; i++)
    {
        error = pointToFILE(file, arrayOfPoints->points + i);
        if (i < arrayOfPoints->size-1)
        {
            error += fprintf(file, ", ") < 0 ? 0 : -1 ; 
        }
    }
    error += fprintf(file, "]") < 0 ? 0 : -1 ; 
    if (quotations)
    {
        error += fprintf(file, "\"") < 0 ? 0 : -1 ; 
    }
    return (error < 0) ? -1 : 0;
}

/**
 * Frees the content values of each point in the array of point and the 
 * pointer to the points.
 *  
 * @param ptr (array_of_points *) : pointer to the array_of_points.
 * 
 * Note that it does not free the ptr it's self.
 */ 
void arrayOfPoints_destroy(array_of_points * ptr){
    // Frees the content of the each point
    for (size_t i = 0; i < ptr->size; i++)
    {
        free( (ptr->points + i)->values );
    }
    // Free the pointer to the array of point_t structures
    free(ptr->points);
}