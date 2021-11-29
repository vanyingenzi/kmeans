#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>

#include "argumentsparser.h"
#include "distance.h"
#include "point.h"
#include "arrayofpoints.h"
#include "arrayofclusters.h"
#include "func.h"

squared_distance_func_t FORMULA_CHOOSED;
/**
 * This function prints all the arguments that can be given to the program.
 */ 
void usage(char *prog_name) {
    fprintf(stderr, "USAGE:\n");
    fprintf(stderr, "    %s [-p n_combinations_points] [-n n_threads] [input_filename]\n", prog_name);
    fprintf(stderr, "    -k n_clusters (default value: 2): the number of clusters to compute\n");
    fprintf(stderr, "    -p n_combinations (default value: equal to k): consider the n_combinations first points present in the input to generate possible initializations for the k-means algorithm\n");
    fprintf(stderr, "    -n n_threads (default value: 4): sets the number of computing threads that will be used to execute the k-means algorithm\n");
    fprintf(stderr, "    -f output_file (default value: stdout): sets the filename on which to write the csv result\n");
    fprintf(stderr, "    -q quiet mode: does not output the clusters content (the \"clusters\" column is simply not present in the csv)\n");
    fprintf(stderr, "    -d distance (manhattan by default): can be either \"euclidean\" or \"manhattan\". Chooses the distance formula to use by the algorithm to compute the distance between the points\n");
}

/**
 * This function parses the input arguments into the argv string array.
 * And stores the values into the args_t structure.
 *
 * @param args (args_t *) : The structure that will hold the corresponding parsed values.
 * @param argc (int) : The length of the array (argv), which contains the user input arguments for terminal.
 * @param argv (char *) : The array of the user input arguments.
 *
 * @return signal (int) : Upon success 0 else -1.
 */ 
int parse_args(args_t *args, int argc, char *argv[]) {
    memset(args, 0, sizeof(args_t));    // set everything to 0 by default
    // the default values are the following, they will be changed depending on the arguments given to the program
    args->k = 2;
    args->n_first_initialization_points = args->k;
    args->n_threads = 4;
    args->quiet = false;
    args->squared_distance_func = squared_manhattan_distance;
    FORMULA_CHOOSED = squared_manhattan_distance;
    int opt;
    while ((opt = getopt(argc, argv, "n:p:k:f:d:q")) != -1) {
        switch (opt)
        {
            case 'n':
                if (atoi(optarg) <= 0) {
                    fprintf(stderr, "Wrong number of threads. Needs a positive integer, received \"%s\"\n", optarg);
                    return -1;
                } else {
                    args->n_threads = (uint32_t) atoi(optarg);
                }
                break;
            case 'p':
                if (atoi(optarg) <= 0) {
                    fprintf(stderr, "Wrong number of initialization points. Needs a positive integer, received \"%s\"\n", optarg);
                    return -1;
                } else {
                    args->n_first_initialization_points = (uint32_t) atoi(optarg);
                }
                break;
            case 'k':
                if (atoi(optarg) <= 0) {
                    fprintf(stderr, "Wrong value, -k. Needs a positive integer, received \"%s\"\n", optarg);
                    return -1;
                } else {
                    args->k = (uint32_t) atoi(optarg);
                }
                break;
            case 'f':
                args->output_pathName = optarg;
                if (!args->output_pathName) {
                    fprintf(stderr, "[argumentsparser.c] Could not open output file %s: %s\n", optarg, strerror(errno));
                    return -1;
                }
                break;
            case 'd':
                if (strcmp("euclidean", optarg) == 0) {
                    args->squared_distance_func = squared_euclidean_distance;
                    FORMULA_CHOOSED = squared_euclidean_distance;
                }
                break;
            case 'q':
                args->quiet = true;
                break;
            case '?':
                usage(argv[0]);
                return 1;
            default:
                usage(argv[0]);
        }
    }

    if (optind == argc) {
        usage(argv[0]);
    } else {
        args->input_pathName = argv[optind];
        if (!args->input_pathName) {
            fprintf(stderr, "Could not open file %s: %s\n", argv[optind], strerror(errno));
            return -1;
        }
    }
    if (args->n_first_initialization_points < args->k) 
    {
        fprintf(stderr, "[argumentsparser.c] Cannot generate an instance of k-means with less initialization points than needed clusters: %"PRIu32" < %"PRIu32"\n", args->n_first_initialization_points, args->k);
        return -1;
    }

    return 0;
}
