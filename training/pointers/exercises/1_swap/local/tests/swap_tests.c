#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>

#include "swap.h"
#include "utilities.h"

void swap_first_parameter_null(void)
{
    int exit_code = E_FAILURE;
    int num_2     = 30;

    exit_code = swap(NULL, &num_2);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void swap_second_parameter_null(void)
{
    int exit_code = E_FAILURE;
    int num_1     = 10;

    exit_code = swap(&num_1, NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void swap_10_and_30(void)
{
    int exit_code = E_FAILURE;
    int num_1     = 10;
    int num_2     = 30;

    exit_code = swap(&num_1, &num_2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(num_1, 30);
    CU_ASSERT_EQUAL(num_2, 10);
}

void swap_20_and_50(void)
{
    int exit_code = E_FAILURE;
    int num_1     = 20;
    int num_2     = 50;

    exit_code = swap(&num_1, &num_2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(num_1, 50);
    CU_ASSERT_EQUAL(num_2, 20);
}

void swap_neg19_and_87(void)
{
    int exit_code = E_FAILURE;
    int num_1     = (-19);
    int num_2     = 87;

    exit_code = swap(&num_1, &num_2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(num_1, 87);
    CU_ASSERT_EQUAL(num_2, (-19));
}

static CU_TestInfo swap_tests[] = {
    { "Swap - First Parameter is NULL", swap_first_parameter_null },
    { "Swap - Second Paramater is NULL", swap_second_parameter_null },
    { "Swap - 10 and 30", swap_10_and_30 },
    { "Swap - 20 and 50", swap_20_and_50 },
    { "Swap - (-19) and 87", swap_neg19_and_87 },
    CU_TEST_INFO_NULL
};

CU_SuiteInfo swap_test_suite = {
    "Pointers - Exercise 1 - Swap: Tests", NULL, NULL, NULL, NULL, swap_tests
};

/*** end of file ***/
