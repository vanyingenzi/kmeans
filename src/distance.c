#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "distance.h"
#include "point.h"
#include "arrayofpoints.h"
#include "argumentsparser.h"

/**
 * Calculates the squared distance between two points using thje manhattan formula.
 * 
 * @param p1 (point_t *) : A point.
 * @param p2 (point_t *) : And another point.
 * 
 * @return value (int64_t)
 */
int64_t squared_manhattan_distance(const point_t *p1, const point_t *p2) {
    int64_t dim = p1->dimension;
    int64_t sum = 0;
    int64_t *p1_val = p1->values;
    int64_t *p2_val = p2->values;

    for (size_t i = 0; i < dim; i++){
        sum += abs( *(p1_val+i) - *(p2_val+i) );
    }
    return (sum * sum);
}

/**
 * Calculates the squared distance between two points using thje euclidean formula.
 * 
 * @param p1 (point_t *) : A point.
 * @param p2 (point_t *) : And another point.
 * 
 * @return value (int64_t)
 */
int64_t squared_euclidean_distance(const point_t *p1, const point_t *p2) {
    int64_t dim = p1->dimension;
    int64_t sum = 0;
    int64_t *p1_val = p1->values;
    int64_t *p2_val = p2->values;

    for (size_t i = 0; i < dim; i++){
        sum += ( *(p1_val+i) - *(p2_val+i) ) * ( *(p1_val+i) - *(p2_val+i) );
    }
    return sum;
}

/**
 * A function that calculates the distortion distance of centroids to their correspondind clusters
 * 
 * @param centroids (array_of_centroids *) : The strcuture holding centroids.
 * @param clusters (array_of_arrays_of_points *) : The structure holding the clusters.
 * 
 * @return value (int64_t)
 */
int64_t distortion_distance(const array_of_centroids *centroids, const array_of_arrays_of_points *clusters){
    int64_t accum_sum = 0;
    array_of_points * cluster_at_idx;
    point_t * vector;

    for (size_t i = 0; i < (clusters->size); i++){
        cluster_at_idx = clusters->array + i;
        for (size_t j = 0; j < cluster_at_idx->size; j++){
            vector = cluster_at_idx->points + j;
            accum_sum += FORMULA_CHOOSED( vector , ( centroids->points+i ) );
        }
    }
    return accum_sum;
}
