#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>

#include "arrayofclusters.h"

/**
 * Frees up the content of array_of_arrays_of_points without freeing the pointer.
 *  
 * @param ptr (array_of_arrays_of_points *) : ptr to whom, it's array will be freed and
 * all the points and point's values.
 * @param freePointsValuesToo (bool) : A boolean to notify that the points values should be freed too.
 * 
 * /!\ It does not free the ptr it's self.
 * 
 * Since we free according to the sizes of each point and array of points, the user must 
 * make sure that the size of each structure reflect the size of it's child components alloc'd.
 */ 
void arrayOfClusters_destroy(array_of_arrays_of_points * ptr, bool freePointsValuesToo){
    for (size_t i = 0; i < ptr->size; i++)
    {
        if (freePointsValuesToo)
        {
            arrayOfPoints_destroy((ptr->array + i));
        } else {
            free( (ptr->array + i)->points );
        }
    }
    free(ptr->array);
}

/**
 * Write the content of the array of clusters to the file given in parameter
 * 
 * @param file (FILE *) : The FILE structure of an opened file. 
 * @param clusters (array_of_arrays_of_points *) : The pointer to the array of clusters.
 * 
 * @return 0 upon success else, -1.
 */
int arrayOfClusters_toFILE(FILE * file, array_of_arrays_of_points * clusters){
    int error = 0;
    error += fprintf(file, "\"[") < 0 ? 0 : -1 ; 
    for (size_t i = 0; i < clusters->size; i++)
    {
        error += arrayOfPoints_writeToFILE(file, clusters->array + i, false);
        if (i < clusters->size-1)
        {
            error += fprintf(file, ", ") < 0 ? 0 : -1 ; 
        }
    }
    error += fprintf(file, "]\"") < 0 ? 0 : -1 ; 
    return error < 0 ? 0 : -1 ; 
}