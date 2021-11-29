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

#include <stdint.h>
#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "CUnit/Util.h"

void test_compare_python_and_c_csv_files(){
    FILE *fl;
    
    char result[10];
    char c_commands[92];
    char python_commands[127];
    char inputFile[][32] = {"input_binary/centeredPoints.bin", "input_binary/spreadPoints.bin"};
    int k[] = {5, 7};
    int p[] = {7, 9};

    //Check first if the file exists
    int check = (access("input_binary/centeredPoints.bin", F_OK) == 0 && access("input_binary/spreadPoints.bin", F_OK) == 0);
    if (check == 0){ return; }
    //Check if kmeans exists also
    check = access("kmeans", F_OK) == 0;
    if (check == 0){ system("make kmeans"); }

    for (uint32_t i = 0; i < 2; i++){
        fprintf(stdout, "--------- Running with %s ---------------\n", inputFile[i]);
        for (uint32_t K= 0; K < (sizeof(k)/sizeof(int)); K++){
            for (uint32_t P = 0; P < (sizeof(p)/sizeof(int)); P++){
                
                //The command below was inspired from: https://stackoverflow.com/questions/18014626/pass-parameter-using-system-command
                snprintf(c_commands, sizeof(c_commands), "./kmeans -k %d -p %d -n 4 -f output_files/test_c_version.csv %s", k[K], p[P], inputFile[i]);
                system(c_commands);

                snprintf(python_commands, sizeof(python_commands), "python3 ./python_version_folder/k-means.py -f output_files/test_python_version.csv -k %d -p %d  %s", k[K], p[P], inputFile[i]);
                system(python_commands);

                fprintf(stdout,"I am comparing the 2 versions\n");
                fl = popen("python3 ./python_version_folder/compare_solutions.py output_files/test_python_version.csv output_files/test_c_version.csv", "r");
                
                fgets(result, sizeof(result), fl);
                pclose(fl);
                
                CU_ASSERT_STRING_EQUAL(result, "Success !");
            }
            
        }
        
    }
    
    
    
}

int main(int argc, char const *argv[])
{
    CU_pSuite pSuite = NULL;
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    
    pSuite = CU_add_suite("Tests the output file of both version", NULL, NULL);
    if ( NULL == CU_add_test(pSuite, "Checking output files", test_compare_python_and_c_csv_files ) )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();
    CU_cleanup_registry();
    printf("\n");

    
    return 0;
}
