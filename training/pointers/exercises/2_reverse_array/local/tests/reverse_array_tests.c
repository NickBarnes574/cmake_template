#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>

#include "reverse_array.h"
#include "utilities.h"

#define ARR_LENGTH 5

void reverse_array_arr_null(void)
{
    int exit_code = E_FAILURE;

    exit_code = reverse_array(NULL, ARR_LENGTH);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void reverse_array_0_length(void)
{
    int exit_code = E_FAILURE;
    int array[ARR_LENGTH] = {1, 2, 3, 4, 5};

    exit_code = reverse_array(array, 0);
    CU_ASSERT_NOT_EQUAL(exit_code, E_SUCCESS);
}

void reverse_array_valid(void)
{
    int exit_code = E_FAILURE;
    int array[ARR_LENGTH] = {1, 2, 3, 4, 5};

    exit_code = reverse_array(array, ARR_LENGTH);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(array[0], 5);
    CU_ASSERT_EQUAL(array[1], 4);
    CU_ASSERT_EQUAL(array[2], 3);
    CU_ASSERT_EQUAL(array[3], 2);
    CU_ASSERT_EQUAL(array[4], 1);
}

static CU_TestInfo reverse_array_tests[] = {
    {"Reverse Array - Array is NULL", reverse_array_arr_null},
    {"Reverse Array - Length is 0", reverse_array_0_length},
    {"Reverse Array - Valid", reverse_array_valid},
    CU_TEST_INFO_NULL};

CU_SuiteInfo reverse_array_test_suite = {
    "Pointers - Exercise 1 - Swap: Tests", NULL, NULL, NULL, NULL, reverse_array_tests};

/*** end of file ***/
