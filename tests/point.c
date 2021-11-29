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
#include <stdint.h>
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

#include "point.h" 

void test_for_pointToString_1Dimension(){
    uint32_t dim1 = 1;
    int64_t p0_arr = {1};
    point_t p1_0 = { dim1, &p0_arr };
    int64_t p1_arr = {0};
    point_t p1_1 = { dim1, &p1_arr };
    int64_t p2_arr = {-10};
    point_t p1_2 = { dim1, &p2_arr};
    
    char * fromPt0 = pointToString(&p1_0);
    char corrPt0[] = "(1)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt0, fromPt0));
    free(fromPt0);
    char * fromPt1 = pointToString(&p1_1);
    char corrPt1[] = "(0)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt1, fromPt1));
    free(fromPt1);
    char * fromPt2 = pointToString(&p1_2);
    char corrPt2[] = "(-10)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt2, fromPt2));
    free(fromPt2);
}

void test_for_pointToString_2Dimension(){
    uint32_t dim2 = 2;
    int64_t p0_arr[] = {1, -1};
    point_t p1_0 = { dim2, p0_arr };
    int64_t p1_arr[] = {0, 0};
    point_t p1_1 = { dim2, p1_arr };
    int64_t p2_arr[] = {-10, -1110};
    point_t p1_2 = { dim2, p2_arr};
    
    char * fromPt0 = pointToString(&p1_0);
    char corrPt0[] = "(1, -1)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt0, fromPt0));
    free(fromPt0);
    char * fromPt1 = pointToString(&p1_1);
    char corrPt1[] = "(0, 0)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt1, fromPt1));
    free(fromPt1);
    char * fromPt2 = pointToString(&p1_2);
    char corrPt2[] = "(-10, -1110)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt2, fromPt2));
    free(fromPt2);
}

void test_for_pointToString_3Dimension(){
    uint32_t dim3 = 3;
    int64_t p0_arr[] = {56, -1, 0};
    point_t p1_0 = { dim3, p0_arr };
    int64_t p1_arr[] = {2300, 0, -2001};
    point_t p1_1 = { dim3, p1_arr };
    int64_t p2_arr[] = {-10, -1110, 91};
    point_t p1_2 = { dim3, p2_arr};
    
    char * fromPt0 = pointToString(&p1_0);
    char corrPt0[] = "(56, -1, 0)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt0, fromPt0));
    free(fromPt0);
    char * fromPt1 = pointToString(&p1_1);
    char corrPt1[] = "(2300, 0, -2001)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt1, fromPt1));
    free(fromPt1);
    char * fromPt2 = pointToString(&p1_2);
    char corrPt2[] = "(-10, -1110, 91)";
    CU_ASSERT_EQUAL( 0, strcmp(corrPt2, fromPt2));
    free(fromPt2);
}

void test_for_pointArrayToString_with_1Dimension_points(){
    uint32_t dim1 = 1;
    int64_t p0_arr = {1};
    point_t p0 = { dim1, &p0_arr };
    int64_t p1_arr = {0};
    point_t p1 = { dim1, &p1_arr };
    int64_t p2_arr = {-10};
    point_t p2 = { dim1, &p2_arr};

    point_t arr_1[] = { p0 };
    char * ansForArr_1 = pointArrayToString(arr_1, 1);
    char corrForArr_1[] = "[(1)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_1, corrForArr_1));
    free(ansForArr_1);

    point_t arr_2[] = { p2 };
    char * ansForArr_2 = pointArrayToString(arr_2, 1);
    char corrForArr_2[] = "[(-10)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_2, corrForArr_2));
    free(ansForArr_2);

    point_t arr_3[] = { p0, p1 };
    char * ansForArr_3 = pointArrayToString(arr_3, 2);
    char corrForArr_3[] = "[(1), (0)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_3, corrForArr_3));
    free(ansForArr_3);

    point_t arr_4[] = { p0, p2 };
    char * ansForArr_4 = pointArrayToString(arr_4, 2);
    char corrForArr_4[] = "[(1), (-10)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_4, corrForArr_4));
    free(ansForArr_4);
}

void test_for_pointArrayToString_with_2Dimension_points(){
    uint32_t dim2 = 2;
    int64_t p0_arr[] = {1, -1};
    point_t p0 = { dim2, p0_arr };
    int64_t p1_arr[] = {0, 0};
    point_t p1 = { dim2, p1_arr };
    int64_t p2_arr[] = {-10, -1110};
    point_t p2 = { dim2, p2_arr};

    point_t arr_1[] = { p0 };
    char * ansForArr_1 = pointArrayToString(arr_1, 1);
    char corrForArr_1[] = "[(1, -1)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_1, corrForArr_1));
    free(ansForArr_1);

    point_t arr_2[] = { p2 };
    char * ansForArr_2 = pointArrayToString(arr_2, 1);
    char corrForArr_2[] = "[(-10, -1110)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_2, corrForArr_2));
    free(ansForArr_2);

    point_t arr_3[] = { p0, p1 };
    char * ansForArr_3 = pointArrayToString(arr_3, 2);
    char corrForArr_3[] = "[(1, -1), (0, 0)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_3, corrForArr_3));
    free(ansForArr_3);

    point_t arr_4[] = { p0, p2 };
    char * ansForArr_4 = pointArrayToString(arr_4, 2);
    char corrForArr_4[] = "[(1, -1), (-10, -1110)]";
    CU_ASSERT_EQUAL(0, strcmp(ansForArr_4, corrForArr_4));
    free(ansForArr_4);
}

int main(int argc, char const *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
 
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("Tests for local header <point.h>", NULL, NULL );

    if ( (NULL == CU_add_test(pSuite, "for pointToString with 1 Dimension Points", test_for_pointToString_1Dimension )) ||
         (NULL == CU_add_test(pSuite, "for pointToString with 2 Dimension Points", test_for_pointToString_2Dimension )) || 
         (NULL == CU_add_test(pSuite, "for pointToString with 3 Dimension Points", test_for_pointToString_3Dimension )) ||
         (NULL == CU_add_test(pSuite, "for pointArrayToString with 1 Dimension Points", test_for_pointArrayToString_with_1Dimension_points ))
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
