#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "point.h"
#include "distance.h"

/**
 * Prints the representation to opened file gave in parameter.
 * 
 * @param file (FILE *) : Opened fike.
 * @param point (point_t *) : The point strcuture to write.
 * 
 * @return int. 0 upon success else negative integer.
 */
int pointToFILE(FILE * file, point_t * point){
    int error = 0;
    error += fprintf(file, "(") < 0 ? 0 : -1 ; 
    for (size_t i = 0; i < point->dimension; i++)
    {
        error += fprintf(file, "%lld",(long long int) point->values[i]);
        if (i < point->dimension-1)
        {
            error += fprintf(file, ", ") < 0 ? 0 : -1 ; 
        }
    }
    error += fprintf(file, ")") < 0 ? 0 : -1 ; 
    return error;
}