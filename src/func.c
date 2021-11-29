#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "point.h"
#include "distance.h"
#include "func.h"
#include "filehandler.h"
#include "distance.h"
#include "argumentsparser.h"


/**
 * These structures are used only once. In this source file.
*/
typedef struct {
    int value;
    array_of_clusters *clusters;
}value_and_clusters;

//extern squared_distance_func_t FORMULA_CHOOSED;

/**
 * This function computes the new centroids ( a centroid_t ) from the current cluster_t.
 * @param clusters (array_of_arrays_of_points *) : A list of array_of_arrays_of_points.
 * @param K (uint32_t *) : A pointer, to the variable K.
 * @param DIMENSION (uint32_t *) : A pointer, to the dimension of points.
 * @return (centroid_t *) A pointer to a centroid_t. In case of an error, NULL is returned.
 */
array_of_centroids * update_centroids(array_of_arrays_of_points* clusters, uint32_t K, uint32_t DIMENSION){
    
    array_of_centroids * centroids = (array_of_centroids *) malloc( sizeof(array_of_centroids) );
    point_t * tempPoint;
    // Array of points in the cluster_t at index i (from 0 to k-1)
    array_of_points * cluster_k;

    if ( centroids == NULL ){ return NULL; } 

    centroids->points = (point_t *) malloc( sizeof(point_t) * K );
    centroids->size = K;

    if (centroids->points == NULL){ 
        free(centroids);
        return NULL; 
    }

    for (uint64_t k = 0; k < K; k++)
    {   
        tempPoint = centroids->points + k;
        if ( tempPoint == NULL) {
            free(centroids->points); 
            free(centroids);
            return NULL;
        }
        
        tempPoint->dimension = DIMENSION;
        tempPoint->values = (int64_t *) calloc(DIMENSION, sizeof(int64_t) );
        if ( tempPoint->values == NULL) {
            free(tempPoint);
            free(centroids->points); 
            free(centroids);
            return NULL;
        }
        
        cluster_k = (clusters->array + k); 
        for( uint64_t j = 0; j < cluster_k->size ; j++ )
        {   
            for (uint64_t m = 0; m < DIMENSION; m++){
                *(tempPoint->values + m) = *(tempPoint->values + m) + *((cluster_k->points+j)->values+m);
            }
        }

        for (uint64_t n = 0; n < DIMENSION; n++){
            if (cluster_k->size != 0)
            {
                *(tempPoint->values + n) = (int64_t) (*(tempPoint->values + n) / (cluster_k->size));
            }
        }
    }

    return centroids;
}

/**
 * Assign vectors to centroids.
 *
 * @param centroids(array_of_centroids *) : The array of centroids.
 * @param cluster (array_of_clusters *) : The current clusters.
 * @param K (uint32_T) : The number of clusters.
 * @param DIMENSION (uint32_t) : The dimension of points.
 * 
 * @return (value_and_clusters *) : The result of the holding a signal to tel if the clusters has changed and the new clusters.
 *          In case of an error NULL.
 */
value_and_clusters *  assign_vectors_to_centroids( 
                                                    array_of_centroids * centroids, array_of_clusters * clusters,
                                                    uint32_t K, uint32_t DIMENSION
                                                 )
{
    int unchanged = 1; //1 is True and 0 is False
    array_of_clusters * new_clusters;
    array_of_points * cluster_in_clusters_at_idx;
    cluster_t * current_cluster_in_new_clusters;
    point_t * pointInClustersArray;
    uint64_t closest_centroid_idx;
    uint64_t closest_centroid_distance;
    point_t * pointInCentroids;
    int64_t distance;

    value_and_clusters * toHoldResult = (value_and_clusters *) malloc( sizeof(value_and_clusters) );

    if (toHoldResult == NULL ){ return NULL; }

    new_clusters = (array_of_clusters *) malloc( sizeof(array_of_clusters));

    if (new_clusters == NULL)
    {
        free(toHoldResult);
        return NULL;
    }

    new_clusters->size = K;
    new_clusters->array = (array_of_points *) malloc( sizeof(array_of_points) * K);

    if (new_clusters->array == NULL)
    {
        free(toHoldResult);
        free(new_clusters);
        return NULL;
    }

    // Initiliasing the clusters with the maximum 
    for (size_t currentClusterIdx = 0; currentClusterIdx < K; currentClusterIdx++)
    {   
        arrayOfPoints_init((new_clusters->array + currentClusterIdx ), 0);
    }
    
    for ( uint32_t current_centroid_idx = 0; current_centroid_idx < K; current_centroid_idx++){ // Ask: K is uint32t while current... is uint64_t

        cluster_in_clusters_at_idx = clusters->array + current_centroid_idx;
        
        for (uint64_t i = 0; i < cluster_in_clusters_at_idx->size; i++){
            // Equivalent of vector in the Python code
            pointInClustersArray = cluster_in_clusters_at_idx->points + i;
            pointInClustersArray->dimension = DIMENSION;
            closest_centroid_idx = 0;
            closest_centroid_distance = (int64_t) INFINITY;

            // Here I am going to find the closest centroid for the given "pointInClustersArray" 
            for (uint64_t centroid_idx = 0; centroid_idx < (centroids->size); centroid_idx++){
                
                pointInCentroids = centroids->points + centroid_idx;
                distance = FORMULA_CHOOSED(pointInClustersArray, pointInCentroids);

                if (distance < closest_centroid_distance){
                    closest_centroid_idx = centroid_idx;
                    closest_centroid_distance = distance;
                }
            }

            // Append point to the corresponding new cluster
            current_cluster_in_new_clusters = (new_clusters->array + closest_centroid_idx);
            void * tempHolder;
            arrayOfPoints_append(current_cluster_in_new_clusters, pointInClustersArray, & tempHolder);
        
            unchanged = (unchanged && closest_centroid_idx == current_centroid_idx);
        }
    }

    toHoldResult->value = (unchanged == 1) ? 0 : 1;
    toHoldResult->clusters = new_clusters;
    return toHoldResult;
}

/**
 * Creates clusters according to the initial centroids given.
 *  
 * @param resultHolder (list_of_centroids_and_clusters_only * ) : Holds the result.
 * @param initial_centroids (array_of_centroids *) : Inititial array of K centroids.
 * @param K (uint32_k) : The number of clusters wanted.
 * @param inputFile (file_t *) : The structure containing the input file data.
 * 
 * @return 0 upon successful completition else -1.
 */
int k_means(list_of_centroids_and_clusters_only * ptr,
    array_of_centroids * initial_centroids, uint32_t K, file_t * inputFile)
{
    value_and_clusters * holdsResult;
    array_of_clusters * clusters;
    int nbOfIterations = 0; 
    // Creates an new array of points for the finalCentroids to return 
    array_of_centroids * finalCentroids = initial_centroids;

    //Creates an empty array of clusters
    clusters = (array_of_clusters *) malloc( sizeof(array_of_clusters) );
    clusters->size = K;
    clusters->array = (cluster_t *) malloc( sizeof(cluster_t) * K );

    // We initiate all clusters in clusters to an empty except the first one
    for (size_t i = 0; i < K; i++)
    {
        if (i == 0)
        {
            (clusters->array + i)->size = inputFile->nbOfPoints;
            (clusters->array + i)->points = (point_t *) malloc( sizeof(point_t) * inputFile->nbOfPoints );
            if (((clusters->array + i)->points ) == NULL )
            {
                fprintf(stderr, "[func.c] Failed malloc when initating clusters in kmeans\n");
                return -1;
            }
            memcpy( (clusters->array + i)->points, inputFile->ptrToPoints, sizeof(point_t) * inputFile->nbOfPoints );
        } else{
            (clusters->array + i)->size = 0;
            (clusters->array + i)->points = NULL;
        }
    }

    int changed = 1;
    while (changed)
    {
        holdsResult = assign_vectors_to_centroids(finalCentroids, clusters, K, inputFile->dimension);
        
        arrayOfClusters_destroy(clusters, false);
        free(clusters);

        if (holdsResult == NULL)
        {
            fprintf(stderr, "[func.c] An error occured when assigning vectors to centroids\n");
            return -1;
        }
        
        changed = holdsResult->value;
        clusters = holdsResult->clusters;
        free(holdsResult);

        if (nbOfIterations > 0)
        {   // If the iteration is equal to zero that means finalCentroids points to initial centroids.
            arrayOfPoints_destroy(finalCentroids);
            free(finalCentroids);
        }
        finalCentroids = update_centroids(clusters, K, inputFile->dimension);
        nbOfIterations++;
    }

    if (ptr == NULL){ return -1; }
    
    ptr->finalCentroids = finalCentroids;
    ptr->finalClusters = clusters;

    return 0;
}