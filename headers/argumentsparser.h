#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>

#include "distance.h"
#include "point.h"
#include "arrayofpoints.h"
#include "arrayofclusters.h"
#include "func.h"

extern squared_distance_func_t FORMULA_CHOOSED;

/**
 * This structure holds the arguments given by the user to the program.
 * 
 * @param input_pathName (char *) : The pathname to the input file.
 * @param output_pathName (char *) : The pathname to the output file, to write the result of calculations to.
 * @param n_threads (uint32_t) : The number of threads to execute the LLoyd Algorithm.
 * @param k (uint32_t) : The size of clusters.
 * @param n_first_initialization_points (uint32_t) : The number of first initialization.
 * @param quiet (bool) : The argument passed to know if the clusters have be to be written in the output file.
 * @param squared_distance_func (squared_distance_func_t) : The function for calculting the distance chose.
 */ 
typedef struct {
    char * input_pathName;
    char * output_pathName;
    uint32_t n_threads;
    uint32_t k;
    uint32_t n_first_initialization_points;
    bool quiet;
    squared_distance_func_t squared_distance_func;
}args_t;

void usage(char *);

int parse_args(args_t *, int, char **);

#endif // ARGUMENT_PARSER_H