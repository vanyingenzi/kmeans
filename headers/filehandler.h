/*****
 *
 * This header contains functions that will be used for file handling
 *
 *****/
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include "arrayofclusters.h"
#include "arrayofpoints.h"
#include "point.h"
#include "circularbuffer.h"

/**
 * This structure represents the binary input file.
 * 
 * - ptrToPoints ( point_t * ) : An array of point_t structures.
 * - dimension ( uint32_t * ) : The pointer to the dimension.
 * - nbOfPoints ( uint64_t * ) : The pointer to the nb of points in the file.
 */ 
typedef struct fileStruct{
    point_t * ptrToPoints;
    uint32_t dimension;
    uint64_t nbOfPoints;
} file_t ;

/**
 *  This structure of arguments to be given to the output-writer thread.
 *  
 *  @param buff (circular_buf *) : The buffer that will containg the strings representing each a row to write in the output file.
 *  @param outPutFile (FILE *) : The opened outfile to write to.
 *  @param quietMode (bool) : The boolean for quiet. Given as parameter to the program.
 */
typedef struct 
{
    circular_buf * buff;
    FILE * outPutFile;
    bool quietMode;
} writerThreadArgs_t;

int fileRead(file_t * theStruct, const char * filePathName);
void freeFileStruct(file_t * inputFile);
void * writeToCSVFromBuffer(void * argT);

#endif //FILEHANDLER_H