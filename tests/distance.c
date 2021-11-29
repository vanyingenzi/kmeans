/********************************************
 * 
 * This contains the CUnit tests for the file "src/distance.c" and header "src/distance.h"
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

#include "distance.h" 

/**
 * 
 *  The answers for the following tests' inputs, are taken from an online calculatoe:
 * 
 *  translatorscafe.com/unit-converter/en-US/calculator/two-points-distance/
 * 
 */

void test_with_2dimension_points()
{
    uint32_t dim2 = 2;
    int64_t p0_arr[]= {0, 0};
    point_t p0 = { dim2, p0_arr };
    
    int64_t p1_arr[] = {5, 5};
    point_t p1 = { dim2, p1_arr };
    

    int64_t p2_arr[] = {-1, 1};
    point_t p2 = { dim2, p2_arr };

    int64_t p3_arr[] = {-8, -8};
    point_t p3 = { dim2, p3_arr };
    
    CU_ASSERT_EQUAL( (int64_t) 0, squared_euclidean_distance( &p0, &p0) );
    CU_ASSERT_EQUAL( (int64_t) 0, squared_manhattan_distance( &p0, &p0) );


    CU_ASSERT_EQUAL( (int64_t) 50, squared_euclidean_distance( &p0, &p1) );
    CU_ASSERT_EQUAL( (int64_t) 50, squared_euclidean_distance( &p1, &p0) );
    CU_ASSERT_EQUAL( (int64_t) 100, squared_manhattan_distance( &p0, &p1) );
    CU_ASSERT_EQUAL( (int64_t) 100, squared_manhattan_distance( &p1, &p0) );

    CU_ASSERT_EQUAL( (int64_t) 52, squared_euclidean_distance( &p1, &p2) );
    CU_ASSERT_EQUAL( (int64_t) 52, squared_euclidean_distance( &p1, &p2) );
    CU_ASSERT_EQUAL( (int64_t) 100, squared_manhattan_distance( &p1, &p2) );
    CU_ASSERT_EQUAL( (int64_t) 100, squared_manhattan_distance( &p1, &p2) );

    CU_ASSERT_EQUAL( (int64_t) 338, squared_euclidean_distance( &p1, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 338, squared_euclidean_distance( &p3, &p1) );
    CU_ASSERT_EQUAL( (int64_t) 676, squared_manhattan_distance( &p1, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 676, squared_manhattan_distance( &p3, &p1) );

    CU_ASSERT_EQUAL( (int64_t) 128, squared_euclidean_distance( &p0, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 128, squared_euclidean_distance( &p3, &p0) );
    CU_ASSERT_EQUAL( (int64_t) 256, squared_manhattan_distance( &p0, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 256, squared_manhattan_distance( &p3, &p0) );

}

void test_with_3dimension_points()
{
    uint32_t dim3 = 3;
    int64_t p0_arr[3] = {0, 0, 0};
    point_t p0 = { dim3, p0_arr };
    int64_t p1_arr[] = {5, 5, 5};
    point_t p1 = { dim3, p1_arr };
    int64_t p2_arr[] = {-1, 1, -1};
    point_t p2 = { dim3, p2_arr };
    int64_t p3_arr[] = {-8, -8, -8};
    point_t p3 = { dim3, p3_arr };
    
    CU_ASSERT_EQUAL( (int64_t) 0, squared_euclidean_distance( &p0, &p0) );
    CU_ASSERT_EQUAL( (int64_t) 0, squared_manhattan_distance( &p0, &p0) );

    CU_ASSERT_EQUAL( (int64_t) 75, squared_euclidean_distance( &p0, &p1) );
    CU_ASSERT_EQUAL( (int64_t) 75, squared_euclidean_distance( &p1, &p0) );
    CU_ASSERT_EQUAL( (int64_t) 225, squared_manhattan_distance( &p0, &p1) );
    CU_ASSERT_EQUAL( (int64_t) 225, squared_manhattan_distance( &p1, &p0) );

    CU_ASSERT_EQUAL( (int64_t) 88, squared_euclidean_distance( &p1, &p2) );
    CU_ASSERT_EQUAL( (int64_t) 88, squared_euclidean_distance( &p1, &p2) );
    CU_ASSERT_EQUAL( (int64_t) 256, squared_manhattan_distance( &p1, &p2) );
    CU_ASSERT_EQUAL( (int64_t) 256, squared_manhattan_distance( &p1, &p2) );

    CU_ASSERT_EQUAL( (int64_t) 507, squared_euclidean_distance( &p1, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 507, squared_euclidean_distance( &p3, &p1) );
    CU_ASSERT_EQUAL( (int64_t) 1521, squared_manhattan_distance( &p1, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 1521, squared_manhattan_distance( &p3, &p1) );

    CU_ASSERT_EQUAL( (int64_t) 192, squared_euclidean_distance( &p0, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 192, squared_euclidean_distance( &p3, &p0) );
    CU_ASSERT_EQUAL( (int64_t) 576, squared_manhattan_distance( &p0, &p3) );
    CU_ASSERT_EQUAL( (int64_t) 576, squared_manhattan_distance( &p3, &p0) );
}

int main(int argc, char const *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
 
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("Tests for local header <distance.h>", NULL, NULL );

    if ( (NULL == CU_add_test(pSuite, "for 2 Dimension Points", test_with_2dimension_points )) ||
         (NULL == CU_add_test(pSuite, "for 3 Dimension Points", test_with_3dimension_points ))
       ) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();
    CU_cleanup_registry();
    printf("\n");

    return 0;
}
