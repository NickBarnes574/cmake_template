#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>

#include "swap.h"
#include "utilities.h"

void swap_first_parameter_null(void)
{
    int exit_code = E_FAILURE;
    int num_2 = 30;

    exit_code = swap(NULL, &num_2);
    CU_ASSERT_NOT_EQUAL(exit_code, E_SUCCESS);
}

void swap_second_parameter_null(void)
{
    int exit_code = E_FAILURE;
    int num_1 = 10;

    exit_code = swap(&num_1, NULL);
    CU_ASSERT_NOT_EQUAL(exit_code, E_SUCCESS);
}

void swap_10_and_30(void)
{
    int exit_code = E_FAILURE;
    int num_1 = 10;
    int num_2 = 30;

    exit_code = swap(&num_1, &num_2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(num_1, 30);
    CU_ASSERT_EQUAL(num_2, 10);
}

static CU_TestInfo swap_tests[] = {
    {"Swap - First Parameter is NULL", swap_first_parameter_null},
    {"Swap - Second Paramater is NULL", swap_second_parameter_null},
    {"Swap - 10 and 30", swap_10_and_30},
    CU_TEST_INFO_NULL};

CU_SuiteInfo swap_test_suite = {
    "Pointers - Exercise 1 - Swap: Tests", NULL, NULL, NULL, NULL, swap_tests};

/*** end of file ***/
