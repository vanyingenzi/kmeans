#include <errno.h>
#include <endian.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "filehandler.h"
#include "point.h"
#include "arrayofclusters.h"
#include "arrayofpoints.h"
#include "threadshandler.h"

/**
 * Frees the fileStrcuture according to the number of initiated points.
 * 
 * @param inputFile (file_t *) : The file structure.
 * @param initiatedPoints (size_t) : The number of correclty initiated points.
 */ 
void freeDynamicFileStruct(file_t * inputFile, size_t initiatedPoints)
{
    for (size_t i = 0; i < initiatedPoints; i++)
    {
        free((inputFile->ptrToPoints + i)->values);
    }
    free(inputFile->ptrToPoints);
}

/**
 * Reads the binary file, and initialize the file_t structure given in the parameters.
 * 
 * @param theStruct (file_t *) : The structure to initialize.
 * @param filePathName (const char *) : The pathName to the binary file.
 * 
 * @return (int). 0 Upon success else -1.
 */
int fileRead(file_t * theStruct, const char * filePathName)
{
    // Check if the pointer is NULL
    if(theStruct == NULL){ return -1; }

    FILE* file;
    point_t * temp_point;
    uint64_t alreadyInitiatedPoints = 0;

    // Opens the binary file
    file = fopen(filePathName,"rb");
    if (file == NULL)
    {
        fprintf(stderr,"[src/filehandler.c] Error when opening the input file < %s >:\n\t%s\n", filePathName, strerror(errno) );
        return -1;
    }

    // Reads the first 4 bytes which is the dimension
    if (fread( &(theStruct->dimension) ,sizeof(uint32_t), 1, file) != 1)
    { 
        fprintf(stderr, "[filehandler.c] Error reading the dimension. It seems that the input file doesn't respect the specification\n");
        fclose(file);
        return -1; 
    } 
    // Reads the next 4 bytes which is the number of points
    if (fread( &(theStruct->nbOfPoints) ,sizeof(uint64_t), 1, file) != 1)
    { 
        fprintf(stderr, "[filehandler.c] Error reading the number of point. It seems that the input file doesn't respect the specification\n");
        fclose(file);
        return -1; 
    } 
    
    // We ensure that we use the same integer encoding : big endian
    theStruct->dimension = be32toh( theStruct->dimension ); 
    theStruct->nbOfPoints = be64toh( theStruct->nbOfPoints );
    theStruct->ptrToPoints = (point_t *) malloc(sizeof(point_t) * theStruct->nbOfPoints );

    if (theStruct->ptrToPoints == NULL )
    {
        fprintf(stderr, "[filehandler.c] Failed malloc when initiating the pointer to hold the points\n");
        fclose(file);
        return -1;
    }

    // We start reading the points now
    for (uint64_t i = 0; i < theStruct->nbOfPoints; i++)
    {   
        temp_point = theStruct->ptrToPoints + i;
        temp_point->values = (int64_t *) malloc( sizeof(int64_t) * theStruct->dimension );

        if(temp_point->values == NULL)
        {
            fprintf(stderr, "[filehandler.c] Failed malloc when reading input points. Number of points that were already initiated : %lld \n", (long long int) alreadyInitiatedPoints);
            fclose(file);
            freeDynamicFileStruct(theStruct, alreadyInitiatedPoints);
            return -1;
        }

        temp_point->dimension = theStruct->dimension;
        for (uint64_t j = 0; j < theStruct->dimension; j++)
        {
            fread(temp_point->values + j,sizeof(int64_t),1,file);
            *(temp_point->values + j) = be64toh( *(temp_point->values + j) );
        }
        alreadyInitiatedPoints+=1;
    }
    // Close the file
    fclose(file);
    return 0;
}

/**
 * Free the content of the file structure, and the content of each point.
 * 
 * @param (file_t *) : The file structure.
 */
void freeFileStruct(file_t * inputFile)
{
    freeDynamicFileStruct(inputFile, inputFile->nbOfPoints);
}

/**
 * Writes the content of a calculation resukt holder to a file in a (CSV) format
 * 
 * @param file (FILE *) : The file to write to
 * @param holder (calculation_result_holder * ) : The holder of results.
 * @param quiet (bool) : To know if the quiet mode is active.
 * 
 * @return int 0 Upon Success, else -1.
 */
int writeCalculationsHolderToCSV(FILE * file, calculation_result_holder * holder, bool quiet)
{
    int error = 0;
    error += arrayOfPoints_writeToFILE(file, holder->initialCentroids, true);
    error += fprintf(file, ",%lld,", (long long int) holder->distortion_distance ) < 0 ? 0 : -1 ; 
    error += arrayOfPoints_writeToFILE(file, holder->finalCentroids, true);
    if (quiet == false)
    {
        error += fprintf(file, ",") < 0 ? 0 : -1 ; 
        error += arrayOfClusters_toFILE(file, holder->finalClusters);
    }
    error += fprintf(file, "\n") < 0 ? 0 : -1 ; 
    return error < 0 ? 0 : -1 ; 
}

/**
 * Writes the content of a buffer in the structure given in the arguments.
 * This function ust be given to the output-writer thread. 
 * 
 * @param argT (void *) : This should be a casted (writerThreadArgs_t *) pointer.  
 * 
 * /!\ Note that this function may loop forever if the buffer in the structure is not well 
 * maintained, meaning that once producers are done they must set a signal and wake all consumers 
 * that might be waiting on the circular buffer.
 * 
 * @return (void *) NULL.
 */
void * writeToCSVFromBuffer(void * argT)
{
    int possibleError = 0;
    writerThreadArgs_t * args = (writerThreadArgs_t *) argT;
    calculation_result_holder * holder;
    int toBeUsedInGet = 0;
    possibleError = circularbuffer_get(args->buff, &toBeUsedInGet, (void **) &holder);

    while (holder != NULL && possibleError == 0)
    {
        possibleError = writeCalculationsHolderToCSV(args->outPutFile, holder, args->quietMode);

        // Free all the resources used in this iteration 
        arrayOfPoints_destroy(holder->initialCentroids);
        free(holder->initialCentroids);

        arrayOfPoints_destroy(holder->finalCentroids);
        free(holder->finalCentroids);
        for (size_t k = 0; k < holder->finalClusters->size; k++)
        {
            free( (holder->finalClusters->array + k)->points );
        }
        free(holder->finalClusters->array);
        free(holder->finalClusters);
        free(holder);

        if(possibleError == 0)
        {
            circularbuffer_get(args->buff, &toBeUsedInGet, (void **) &holder);
        } else {
            fprintf(stderr, "[filehandler.c] An error occured writing to the CSV\n");
        }
    }
    return(NULL);
}
