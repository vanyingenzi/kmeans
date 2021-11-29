/********************************************
 *
 * This contains the CUnit tests for the file "src/argumentsParser.c" and header "headers/argumentsParser.h"
 *
 * For documentation and better understanding check the following website:
 * www.cunit.sourceforge.net/doc/index.html
 *
 *******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "CUnit/Util.h"

#include "argumentsparser.h"

/****
 * It's main purpose is to parse the user input arguments.
 * 
 * Since the arguments given to a program are in an array, therefore we are going to store the arguments, 
 * in arrays and pass the array to the function responsible for parsing.
 *
 ****/
void test_parse_args_silent_on()
{
    char * argv[13] = {"./kmeans", "-q", "-k", "3", "-p", "12", "-n", "4", "-d", "euclidean",
                       "-f", "output_files/test.csv", "input_binary/spreadPoints.bin"};
    args_t argument_holder;
    int errorSignal;
    errorSignal = parse_args(&argument_holder, 13, argv);
    if (errorSignal == 0)
    {
        CU_ASSERT_EQUAL(argument_holder.k, 3);
        CU_ASSERT_EQUAL(argument_holder.input_pathName, "input_binary/spreadPoints.bin");
        CU_ASSERT_EQUAL(argument_holder.n_threads, 4);
        CU_ASSERT_TRUE(argument_holder.quiet);
        CU_ASSERT_EQUAL(argument_holder.squared_distance_func, squared_euclidean_distance);
        CU_ASSERT_EQUAL(argument_holder.n_first_initialization_points, 12);
        CU_ASSERT_EQUAL(argument_holder.output_pathName, "output_files/test.csv");
    }

    optind = 1;
    char * argv1[11] = {"./kmeans", "-q", "-k", "3", "-p", "6", "-n", "4",
                        "-f", "output_files/test.csv", "input_binary/spreadPoints.bin"};
    errorSignal = parse_args(&argument_holder, 11, argv1);
    if (errorSignal == 0)
    {
        CU_ASSERT_EQUAL(argument_holder.k, 3);
        CU_ASSERT_EQUAL(argument_holder.input_pathName, "input_binary/spreadPoints.bin");
        CU_ASSERT_EQUAL(argument_holder.n_threads, 4);
        CU_ASSERT_TRUE(argument_holder.quiet);
        CU_ASSERT_EQUAL(argument_holder.squared_distance_func, squared_manhattan_distance);
        CU_ASSERT_EQUAL(argument_holder.n_first_initialization_points, 6);
        CU_ASSERT_EQUAL(argument_holder.output_pathName, "output_files/test.csv");
    }

    optind = 1;
    char * argv2[11] = {"./kmeans", "-q", "-k", "10", "-p", "60", "-n", "200",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 11, argv2);
    if (errorSignal == 0)
    {
        CU_ASSERT_EQUAL(argument_holder.k, 10);
        CU_ASSERT_EQUAL(argument_holder.input_pathName, "otherinput.bin");
        CU_ASSERT_EQUAL(argument_holder.n_threads, 200);
        CU_ASSERT_TRUE(argument_holder.quiet);
        CU_ASSERT_EQUAL(argument_holder.squared_distance_func, squared_manhattan_distance);
        CU_ASSERT_EQUAL(argument_holder.n_first_initialization_points, 60);
        CU_ASSERT_EQUAL(argument_holder.output_pathName, "outhertest.csv");
    }

    optind = 1;
    char * argv3[11] = {"./kmeans", "-q", "-k", "-10", "-p", "60", "-n", "200",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 11, argv3);
    CU_ASSERT_EQUAL(errorSignal, -1);

    optind = 1;
    char * argv4[11] = {"./kmeans", "-q", "-k", "10", "-p", "60", "-n", "-200",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 11, argv4);
    CU_ASSERT_EQUAL(errorSignal, -1);

    optind = 1;
    char * argv5[11] = {"./kmeans", "-q", "-k", "-10", "-p", "60", "-n", "20",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 11, argv5);
    CU_ASSERT_EQUAL(errorSignal, -1);
}

/****
 * It's main purpose is to parse the user input arguments.
 * 
 * Since the arguments given to a program are in an array, therefore we are going to store the arguments, 
 * in arrays and pass the array to the function responsible for parsing.
 *
 ****/
void test_parse_args_silent_off()
{
    char * argv[12] = {"./kmeans", "-k", "3", "-p", "12", "-n", "4", "-d", "euclidean",
                       "-f", "output_files/test.csv", "input_binary/spreadPoints.bin"};
    args_t argument_holder;
    int errorSignal;

    errorSignal = parse_args(&argument_holder, 12, argv);
    if (errorSignal == 0)
    {
        CU_ASSERT_EQUAL(argument_holder.k, 3);
        CU_ASSERT_EQUAL(argument_holder.input_pathName, "input_binary/spreadPoints.bin");
        CU_ASSERT_EQUAL(argument_holder.n_threads, 4);
        CU_ASSERT_FALSE(argument_holder.quiet);
        CU_ASSERT_EQUAL(argument_holder.squared_distance_func, squared_euclidean_distance);
        CU_ASSERT_EQUAL(argument_holder.n_first_initialization_points, 12);
        CU_ASSERT_EQUAL(argument_holder.output_pathName, "output_files/test.csv");
    }

    optind = 1;
    char * argv1[10] = {"./kmeans", "-k", "3", "-p", "6", "-n", "4",
                        "-f", "output_files/test.csv", "input_binary/spreadPoints.bin"};
    errorSignal = parse_args(&argument_holder, 10, argv1);
    if (errorSignal == 0)
    {
        CU_ASSERT_EQUAL(argument_holder.k, 3);
        CU_ASSERT_EQUAL(argument_holder.input_pathName, "input_binary/spreadPoints.bin");
        CU_ASSERT_EQUAL(argument_holder.n_threads, 4);
        CU_ASSERT_FALSE(argument_holder.quiet);
        CU_ASSERT_EQUAL(argument_holder.squared_distance_func, squared_manhattan_distance);
        CU_ASSERT_EQUAL(argument_holder.n_first_initialization_points, 6);
        CU_ASSERT_EQUAL(argument_holder.output_pathName, "output_files/test.csv");
    }

    optind = 1;
    char * argv2[10] = {"./kmeans", "-k", "10", "-p", "60", "-n", "200",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 10, argv2);
    if (errorSignal == 0)
    {
        CU_ASSERT_EQUAL(argument_holder.k, 10);
        CU_ASSERT_EQUAL(argument_holder.input_pathName, "otherinput.bin");
        CU_ASSERT_EQUAL(argument_holder.n_threads, 200);
        CU_ASSERT_FALSE(argument_holder.quiet);
        CU_ASSERT_EQUAL(argument_holder.squared_distance_func, squared_manhattan_distance);
        CU_ASSERT_EQUAL(argument_holder.n_first_initialization_points, 60);
        CU_ASSERT_EQUAL(argument_holder.output_pathName, "outhertest.csv");
    }

    optind = 1;
    char * argv3[10] = {"./kmeans", "-k", "-10", "-p", "60", "-n", "200",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 10, argv3);
    CU_ASSERT_EQUAL(errorSignal, -1);


    optind = 1;
    char * argv4[10] = {"./kmeans", "-k", "10", "-p", "60", "-n", "-200",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 10, argv4);
    CU_ASSERT_EQUAL(errorSignal, -1);

    optind = 1;
    char * argv5[11] = {"./kmeans", "-q", "-k", "-10", "-p", "60", "-n", "20",
                        "-f", "outhertest.csv", "otherinput.bin"};
    errorSignal = parse_args(&argument_holder, 11, argv5);
    CU_ASSERT_EQUAL(errorSignal, -1);
}

int main(int argc, char const *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
 
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("Tests for local header <argumentsParser.h>", NULL, NULL );

    if ( NULL == CU_add_test(pSuite, "for many different file", test_parse_args_silent_on) ) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();
    CU_cleanup_registry();
    printf("\n");
    return 0;
}