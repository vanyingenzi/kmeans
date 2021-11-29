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
#include "filehandler.h"
#include "arrayofclusters.h"
#include "arrayofpoints.h"
#include "point.h"

void test_for_fileRead_nbOfPoints(){
    file_t theStruct;

   fileRead(&theStruct ,"input_binary/example.bin");
    CU_ASSERT_TRUE(theStruct.nbOfPoints == (uint64_t) 7);
    fileRead(&theStruct ,"input_binary/manyPoints.bin");
    CU_ASSERT_TRUE(theStruct.nbOfPoints == (uint64_t) 48);
    fileRead(&theStruct ,"input_binary/lotsOfPoints.bin");
    CU_ASSERT_TRUE(theStruct.nbOfPoints == (uint64_t) 33000);
    fileRead(&theStruct ,"input_binary/triPoints.bin");
    CU_ASSERT_TRUE(theStruct.nbOfPoints == (uint64_t) 9);
    
}
/*Finding number of points in any different files*/
/*************************************************************************/

void test_for_fileRead_dim(){
    file_t theStruct;
    
    fileRead(&theStruct ,"input_binary/example.bin");
    CU_ASSERT_TRUE(theStruct.dimension == (uint64_t) 2);
    
    fileRead(&theStruct ,"input_binary/manyPoints.bin");
    CU_ASSERT_TRUE(theStruct.dimension == (uint64_t) 2); 
    
    fileRead(&theStruct ,"input_binary/lotsOfPoints.bin");  
    CU_ASSERT_TRUE(theStruct.dimension == (uint64_t) 2);
    
    fileRead(&theStruct ,"input_binary/3dPoints.bin");  
    CU_ASSERT_TRUE(theStruct.dimension == (uint64_t) 3);
    
}
/* Finding dimension in many files*/
/*************************************************************************/


void test_for_fileRead_point_t(){
    file_t theStruct;
   fileRead(&theStruct ,"input_binary/example.bin");

    int table[7][2];
    int x[7]={1,2,3,5,3,5,4};
    int y[7]={1,2,4,7,5,5,5};
    for(int i=0; i<7; i++)
    {
        table[i][0]=x[i];
        table[i][1]=y[i];
    }

    int b = 1;
    for(int i=0;i<theStruct.nbOfPoints;i++){
        for(int j=0;j<theStruct.dimension;j++){
            if(table [i][j]!=(&theStruct)->ptrToPoints[i].values[j])
            {
                b=0;
            }
        }
    }
    CU_ASSERT_TRUE(b);
/*Iteration of the entire file to test our founction*/
/*************************************************************************/

   fileRead(&theStruct ,"input_binary/lotsOfPoints.bin");
    int a =(&theStruct)->ptrToPoints[23569].values[0];
    int c =theStruct.ptrToPoints[23569].values[1];
    CU_ASSERT_TRUE(a==14);
    CU_ASSERT_TRUE(c==7);


  fileRead(&theStruct ,"input_binary/triPoints.bin");
    int p =(&theStruct)->ptrToPoints[6].values[0];
    int q =theStruct.ptrToPoints[6].values[1];
    int r =theStruct.ptrToPoints[6].values[2];
    CU_ASSERT_TRUE(p==75);
    CU_ASSERT_TRUE(q==89);
    CU_ASSERT_TRUE(r==63);
}
/* Search of specific points  */
/*************************************************************************/

int main(int argc, char const *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
 
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("Tests for local header <filehandler.h>", NULL, NULL );

    if (
         (NULL == CU_add_test(pSuite, "for many different file", test_for_fileRead_dim )) ||
         (NULL == CU_add_test(pSuite, "for many different file", test_for_fileRead_nbOfPoints )) ||
         (NULL == CU_add_test(pSuite, "for many different file", test_for_fileRead_point_t )) 
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

