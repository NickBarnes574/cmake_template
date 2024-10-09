#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "utilities.h"

void custom_free(void * data)
{
    (void)data;
    return;
}

void test_stack_init(void)
{
    uint32_t capacity = 10;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);
    CU_ASSERT_EQUAL(stack->capacity, capacity);
    CU_ASSERT_EQUAL(stack->currentsz, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack->arr);
    CU_ASSERT_PTR_NULL(stack->arr[0]); // Initially, all should be NULL
    stack_destroy(&stack);
}

// IS FULL

void test_stack_is_full__NULL_ARG__(void)
{
    int exit_code = E_FAILURE;

    exit_code = stack_is_full(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_stack_is_full__FALSE__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    exit_code = stack_is_full(stack);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    stack_destroy(&stack);
}

void test_stack_is_full__TRUE__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;
    uint32_t result    = 0;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    for (uint32_t idx = 0; idx < capacity; idx++)
    {
        exit_code = stack_push(stack, &idx);
        CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

        result = *(uint32_t *)stack_peek(stack);
        CU_ASSERT_EQUAL(result, idx);
        printf("result = [%d]\n", result);
    }

    exit_code = stack_is_full(stack);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    stack_destroy(&stack);
}

// IS EMPTY

void test_stack_is_empty__NULL_ARG__(void)
{
    int exit_code = E_FAILURE;

    exit_code = stack_is_full(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_stack_is_empty__FALSE__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;
    uint32_t result    = 0;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    for (uint32_t idx = 0; idx < capacity; idx++)
    {
        exit_code = stack_push(stack, &idx);
        CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

        result = *(uint32_t *)stack_peek(stack);
        CU_ASSERT_EQUAL(result, idx);
    }

    exit_code = stack_is_empty(stack);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    stack_destroy(&stack);
}

void test_stack_is_empty__TRUE__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    exit_code = stack_is_empty(stack);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    stack_destroy(&stack);
}

// PUSH

void test_stack_push__NULL_ARG_STACK__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t data      = 1;

    exit_code = stack_push(NULL, &data);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_stack_push__NULL_ARG_DATA__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    exit_code = stack_push(stack, NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);

    stack_destroy(&stack);
}

void test_stack_push__SINGLE_DATA__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;
    uint32_t data      = 1;
    uint32_t result    = 0;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    exit_code = stack_push(stack, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = *(uint32_t *)stack_peek(stack);
    CU_ASSERT_EQUAL(result, data);

    stack_destroy(&stack);
}

void test_stack_push__MULTI_DATA__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;
    uint32_t result    = 0;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    for (uint32_t idx = 0; idx < capacity; idx++)
    {
        exit_code = stack_push(stack, &idx);
        CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

        result = *(uint32_t *)stack_peek(stack);
        CU_ASSERT_EQUAL(result, idx);
    }

    stack_destroy(&stack);
}

// POP

void test_stack_pop__NULL_ARG_STACK__(void)
{
    void * result = NULL;

    result = stack_pop(NULL);
    CU_ASSERT_PTR_NULL(result);
}

void test_stack_pop__SINGLE_DATA__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;
    uint32_t data      = 1;
    uint32_t result    = 0;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    exit_code = stack_push(stack, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = *(uint32_t *)stack_peek(stack);
    CU_ASSERT_EQUAL(result, data);

    result = *(uint32_t *)stack_pop(stack);
    CU_ASSERT_EQUAL(result, data);

    exit_code = stack_is_empty(stack);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    stack_destroy(&stack);
}

void test_stack_pop__MULTI_DATA__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;
    uint32_t result    = 0;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    for (uint32_t idx = 0; idx < capacity; idx++)
    {
        exit_code = stack_push(stack, &idx);
        CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

        result = *(uint32_t *)stack_peek(stack);
        CU_ASSERT_EQUAL(result, idx);
    }

    for (uint32_t idx = 0; idx < capacity; idx++)
    {
        result = *(uint32_t *)stack_pop(stack);
        CU_ASSERT_EQUAL(result, result);
    }

    exit_code = stack_is_empty(stack);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    stack_destroy(&stack);
}

void test_stack_clear__NULL_ARG_STACK__(void)
{
    int exit_code = E_FAILURE;

    exit_code = stack_clear(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_stack_clear__SUCCESS__(void)
{
    int      exit_code = E_FAILURE;
    uint32_t capacity  = 10;
    uint32_t result    = 0;

    stack_t * stack = stack_init(capacity, custom_free);
    CU_ASSERT_PTR_NOT_NULL_FATAL(stack);

    for (uint32_t idx = 0; idx < capacity; idx++)
    {
        exit_code = stack_push(stack, &idx);
        CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

        result = *(uint32_t *)stack_peek(stack);
        CU_ASSERT_EQUAL(result, idx);
    }

    exit_code = stack_clear(stack);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = stack_is_empty(stack);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    stack_destroy(&stack);
}

static CU_TestInfo stack_tests[] = {
    // Tests for stack initialization
    { "stack_init", test_stack_init },

    // Tests for checking if the stack is full
    { "stack_is_full_null", test_stack_is_full__NULL_ARG__ },
    { "stack_is_full_false", test_stack_is_full__FALSE__ },
    { "stack_is_full_true", test_stack_is_full__TRUE__ },

    // Tests for checking if the stack is empty
    { "stack_is_empty_null", test_stack_is_empty__NULL_ARG__ },
    { "stack_is_empty_false", test_stack_is_empty__FALSE__ },
    { "stack_is_empty_true", test_stack_is_empty__TRUE__ },

    // Tests for pushing onto the stack
    { "stack_push_null_stack", test_stack_push__NULL_ARG_STACK__ },
    { "stack_push_null_data", test_stack_push__NULL_ARG_DATA__ },
    { "stack_push_single", test_stack_push__SINGLE_DATA__ },
    { "stack_push_multi", test_stack_push__MULTI_DATA__ },

    // Tests for popping from the stack
    { "stack_pop_null_stack", test_stack_pop__NULL_ARG_STACK__ },
    { "stack_pop_single", test_stack_pop__SINGLE_DATA__ },
    { "stack_pop_multi", test_stack_pop__MULTI_DATA__ },

    // Tests for clearing the stack
    { "stack_clear_null", test_stack_clear__NULL_ARG_STACK__ },
    { "stack_clear", test_stack_clear__SUCCESS__ },

    CU_TEST_INFO_NULL
};

CU_SuiteInfo stack_test_suite = {
    "Stack Tests",
    NULL,       // Suite initialization function
    NULL,       // Suite cleanup function
    NULL,       // Suite setup function
    NULL,       // Suite teardown function
    stack_tests // The combined array of all tests
};

/*** end of file ***/
