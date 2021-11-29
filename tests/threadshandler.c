/********************************************
 * 
 * This contains the CUnit tests for the file "src/threadsHandler.c" and header "src/threadsHandler.h" and main.c.
 * 
 * These tests work better for on multicores processor. Here we are testing to see if the code is faster when we increase the number of threads
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
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "CUnit/Util.h"

#include "threadshandler.h" 

void test_with_2_threads()
{    
    // Checking signal
    int checking;
    // Check to see if the number of processors >2
    checking = get_nprocs() >= 2;
    if (checking == 0){ return; }
    // Check to see if the file on which we want to test on exist 
    checking = access("input_binary/spreadPoints.bin", F_OK) == 0;
    if (checking == 0){ return; }

    checking = access("kmeans", F_OK) == 0;

    // Once done checking we can now execute the tests
    if (checking == 0)
    {
        system("make kmeans");
    }
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    system("./kmeans -q -k 5 -p 11 -n 1 -f output_files/test.csv input_binary/spreadPoints.bin");
    gettimeofday(&tv2, NULL);
    double timeWithOneThread = (double) tv2.tv_sec - (double) tv1.tv_sec;

    gettimeofday(&tv1, NULL);
    system("./kmeans -q -k 5 -p 11 -n 2 -f output_files/test.csv input_binary/spreadPoints.bin");
    gettimeofday(&tv2, NULL);
    double timeWithTwoThread = (double) tv2.tv_sec - (double) tv1.tv_sec;

    printf("The time with one thread %f, two threads %f", timeWithOneThread, timeWithTwoThread);

    CU_ASSERT_TRUE(timeWithTwoThread < timeWithOneThread);
}

int main(int argc, char const *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
 
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("Tests for local header <threadsHandler.h> and main.c", NULL, NULL );

    if ( NULL == CU_add_test(pSuite, "for 2 Threads", test_with_2_threads ) )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();
    CU_cleanup_registry();
    printf("\n");

    return 0;
}
