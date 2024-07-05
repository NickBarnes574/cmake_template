#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>

#include "comparisons.h"
#include "linked_list.h"
#include "utilities.h"

#define DATA_ARR_LENGTH 10

list_t * test_list = NULL;

int data[DATA_ARR_LENGTH] = { 44, 51, 77, 14, 68, 41, 46, 18, 72, 53 };

void custom_action(void * data)
{
    int * value = (int *)data;
    (*value)++;
}

void custom_free(void * data)
{
    (void)data;
    return;
}

void setup(void)
{
    test_list = list_new(custom_free, int_comp);
}

void teardown(void)
{
    if (NULL != test_list)
    {
        list_delete(&test_list);
    }
}

void test_list_new(void)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(test_list);
    CU_ASSERT_EQUAL(test_list->size, 0);
    CU_ASSERT_PTR_EQUAL(test_list->head, NULL);
    CU_ASSERT_PTR_EQUAL(test_list->tail, NULL);
}

void test_list_push_head_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_head(NULL, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_push_head_null_data(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_head(test_list, NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_push_head_single(void)
{
    int           exit_code = E_FAILURE;
    list_node_t * node      = NULL;

    exit_code = list_push_head(test_list, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    node = test_list->head;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    CU_ASSERT_EQUAL(*(int *)node->data, data[0]);
}

void test_list_push_head_multiple(void)
{
    int           exit_code = E_FAILURE;
    list_node_t * node      = NULL;

    // Push multiple items to the head
    for (int i = 0; i < DATA_ARR_LENGTH; i++)
    {
        exit_code = list_push_head(test_list, &data[i]);
        CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    }

    // Check that all items are present and in correct order
    CU_ASSERT_EQUAL(test_list->size, DATA_ARR_LENGTH);
    node = test_list->head;
    for (int i = DATA_ARR_LENGTH - 1; i >= 0; i--)
    {
        CU_ASSERT_PTR_NOT_NULL_FATAL(node);
        CU_ASSERT_EQUAL(*(int *)node->data, data[i]);
        node = node->next;
    }
}

void test_list_push_tail_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_tail(NULL, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_push_tail_null_data(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_tail(test_list, NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_push_tail_single(void)
{
    int           exit_code = E_FAILURE;
    list_node_t * node      = NULL;

    exit_code = list_push_tail(test_list, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    node = test_list->tail;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    CU_ASSERT_EQUAL(*(int *)node->data, data[0]);
}

void test_list_push_tail_multiple(void)
{
    int           exit_code = E_FAILURE;
    list_node_t * node      = NULL;

    // Push multiple items to the tail
    for (int i = 0; i < DATA_ARR_LENGTH; i++)
    {
        exit_code = list_push_tail(test_list, &data[i]);
        CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    }

    // Check that all items are present and in correct order
    CU_ASSERT_EQUAL(test_list->size, DATA_ARR_LENGTH);
    node = test_list->head;
    for (int i = 0; i < DATA_ARR_LENGTH; i++)
    {
        CU_ASSERT_PTR_NOT_NULL_FATAL(node);
        CU_ASSERT_EQUAL(*(int *)node->data, data[i]);
        node = node->next;
    }
}

void test_list_push_position_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_position(NULL, &data[0], 0);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_push_position_null_data(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_position(test_list, NULL, 0);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_push_position_out_of_bounds(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_position(test_list, &data[0], DATA_ARR_LENGTH);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);
}

void test_list_push_position_single(void)
{
    int           exit_code = E_FAILURE;
    list_node_t * node      = NULL;

    exit_code = list_push_position(test_list, &data[0], 0);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    node = test_list->tail;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    CU_ASSERT_EQUAL(*(int *)node->data, data[0]);
}

void test_list_push_position_multiple(void)
{
    int           exit_code = E_FAILURE;
    list_node_t * node      = NULL;

    // [44]
    exit_code = list_push_position(test_list, &data[0], 0);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 77]
    exit_code = list_push_position(test_list, &data[2], 1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77]
    exit_code = list_push_position(test_list, &data[1], 1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77, 14]
    exit_code = list_push_position(test_list, &data[3], 3);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77, 14, 53]
    exit_code = list_push_position(test_list, &data[9], 4);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77, 14, 72, 53]
    exit_code = list_push_position(test_list, &data[8], 4);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77, 14, 18, 72, 53]
    exit_code = list_push_position(test_list, &data[7], 4);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77, 14, 46, 18, 72, 53]
    exit_code = list_push_position(test_list, &data[6], 4);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77, 14, 41, 46, 18, 72, 53]
    exit_code = list_push_position(test_list, &data[5], 4);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // [44, 51, 77, 14, 68, 41, 46, 18, 72, 53]
    exit_code = list_push_position(test_list, &data[4], 4);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    // Check that all items are present and in correct order
    CU_ASSERT_EQUAL(test_list->size, DATA_ARR_LENGTH);
    node = test_list->head;
    for (int i = 0; i < DATA_ARR_LENGTH; i++)
    {
        CU_ASSERT_PTR_NOT_NULL_FATAL(node);
        CU_ASSERT_EQUAL(*(int *)node->data, data[i]);
        node = node->next;
    }
}

void test_list_is_empty_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_is_empty(NULL);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);
}

void test_list_is_empty(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_is_empty(test_list);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_list_pop_head_null_list(void)
{
    void * result = NULL;

    result = list_pop_head(NULL);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_pop_head_empty(void)
{
    void * result = NULL;

    result = list_pop_head(test_list);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_pop_head_single(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_head(test_list, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_pop_head(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);
}

void test_list_pop_head_multiple(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_head(test_list, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_push_head(test_list, &data[1]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_push_head(test_list, &data[2]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_pop_head(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[2]);

    result = list_pop_head(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[1]);

    result = list_pop_head(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);
}

void test_list_pop_tail_null_list(void)
{
    void * result = NULL;

    result = list_pop_tail(NULL);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_pop_tail_empty(void)
{
    void * result = NULL;

    result = list_pop_tail(test_list);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_pop_tail_single(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_head(test_list, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_pop_tail(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);
}

void test_list_pop_tail_multiple(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_head(test_list, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_push_head(test_list, &data[1]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_push_head(test_list, &data[2]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_pop_tail(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);

    result = list_pop_tail(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[1]);

    result = list_pop_tail(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[2]);
}

void test_list_pop_position_null_list(void)
{
    void * result = NULL;

    result = list_pop_position(NULL, 0);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_pop_position(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_position(test_list, &data[0], 0); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_position(test_list, &data[1], 1); // [44, 51]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_position(test_list, &data[2], 2); // [44, 51, 77]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_pop_position(test_list, 1); // [44, 77]
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(*(int *)result, data[1]);

    result = list_pop_position(test_list, 0); // [77]
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);

    result = list_pop_position(test_list, 0); // []
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(*(int *)result, data[2]);
}

void test_list_remove_head_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_remove_head(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_remove_head(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_head(test_list, &data[0]); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_head(test_list, &data[1]); // [51, 44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_remove_head(test_list); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(*(int *)list_peek_head(test_list), data[0]);

    exit_code = list_remove_head(test_list); // []
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_PTR_NULL(list_peek_head(test_list));
}

void test_list_remove_tail_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_remove_tail(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_remove_tail(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_head(test_list, &data[0]); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_head(test_list, &data[1]); // [51, 44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_remove_tail(test_list); // [51]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(*(int *)list_peek_tail(test_list), data[1]);

    exit_code = list_remove_tail(test_list); // []
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_PTR_NULL(list_peek_tail(test_list));
}

void test_list_remove_position_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_remove_position(NULL, 0);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_remove_position(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_position(test_list, &data[0], 0); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_position(test_list, &data[1], 1); // [44, 51]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_position(test_list, &data[2], 2); // [44, 51, 77]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_remove_position(test_list, 1); // [44, 77]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(*(int *)list_peek_position(test_list, 1), data[2]);

    exit_code = list_remove_position(test_list, 0); // [77]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(*(int *)list_peek_position(test_list, 0), data[2]);

    exit_code = list_remove_position(test_list, 0); // []
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_PTR_NULL(list_peek_position(test_list, 0));
}

void test_list_peek_head_null_list(void)
{
    void * result = NULL;

    result = list_peek_head(NULL);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_peek_head(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_head(test_list, &data[0]); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_peek_head(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);
}

void test_list_peek_tail_null_list(void)
{
    void * result = NULL;

    result = list_peek_tail(NULL);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_peek_tail(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_head(test_list, &data[0]); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_peek_tail(test_list);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);
}

void test_list_peek_position_null_list(void)
{
    void * result = NULL;

    result = list_peek_position(NULL, 0);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_peek_position(void)
{
    int    exit_code = E_FAILURE;
    void * result    = NULL;

    exit_code = list_push_position(test_list, &data[0], 0); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_position(test_list, &data[1], 1); // [44, 51]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_position(test_list, &data[2], 2); // [44, 51, 77]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    result = list_peek_position(test_list, 1);
    CU_ASSERT_EQUAL(*(int *)result, data[1]);

    result = list_peek_position(test_list, 0);
    CU_ASSERT_EQUAL(*(int *)result, data[0]);

    result = list_peek_position(test_list, 2);
    CU_ASSERT_EQUAL(*(int *)result, data[2]);
}

void test_list_remove_data_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_remove_data(NULL, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_remove_data_null_data_p(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_remove_data(test_list, NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_remove_data(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_head(test_list, &data[0]); // [44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = list_push_head(test_list, &data[1]); // [51, 44]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_remove_data(test_list, &data[0]); // [51]
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(*(int *)list_peek_head(test_list), data[1]);

    exit_code = list_remove_data(test_list, &data[1]); // []
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_PTR_NULL(list_peek_head(test_list));
}

void test_list_foreach_call_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_foreach_call(NULL, NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_foreach_call(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_push_head(test_list, &data[0]);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = list_foreach_call(test_list, custom_action);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(data[0], 45);
}

void test_list_contains_null_list(void)
{
    bool result = false;

    result = list_contains(NULL, &data[0]);
    CU_ASSERT_FALSE(result);
}

void test_list_contains_null_data_p(void)
{
    bool result = false;

    result = list_contains(test_list, NULL);
    CU_ASSERT_FALSE(result);
}

void test_list_contains(void)
{
    bool result = false;

    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[1]);

    result = list_contains(test_list, &data[0]);
    CU_ASSERT_TRUE(result);

    result = list_contains(test_list, &data[2]);
    CU_ASSERT_FALSE(result);
}

void test_list_pick_random_item_null_list(void)
{
    void * result = NULL;

    result = list_pick_random_item(NULL);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_pick_random_item(void)
{
    void * result = NULL;

    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[1]);

    result = list_pick_random_item(test_list);
    CU_ASSERT_PTR_NOT_NULL(result);
}

void test_list_find_first_occurrence_null_list(void)
{
    void * result = NULL;

    result = list_find_first_occurrence(NULL, &data[0]);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_find_first_occurrence_null_search_data(void)
{
    void * result = NULL;

    result = list_find_first_occurrence(test_list, NULL);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_find_first_occurrence(void)
{
    void * result = NULL;

    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[1]);

    result = list_find_first_occurrence(test_list, &data[0]);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(*(int *)(((list_node_t *)result)->data), data[0]);
}

void test_list_find_all_occurrences_null_list(void)
{
    list_t * result = NULL;

    result = list_find_all_occurrences(NULL, &data[0]);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_find_all_occurrences_null_search_data(void)
{
    list_t * result = NULL;

    result = list_find_all_occurrences(test_list, NULL);
    CU_ASSERT_PTR_EQUAL(result, NULL);
}

void test_list_find_all_occurrences(void)
{
    list_t * result = NULL;

    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[1]);

    result = list_find_all_occurrences(test_list, &data[0]);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(result->size, 2);

    list_delete(&result);
}

void test_list_sort_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_sort(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_sort(void)
{
    int exit_code = E_FAILURE;

    list_push_head(test_list, &data[2]);
    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[1]);

    exit_code = list_sort(test_list);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(*(int *)list_peek_head(test_list), data[0]);
    CU_ASSERT_EQUAL(*(int *)list_peek_tail(test_list), data[2]);
}

void test_list_clear_null_list(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_clear(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_clear(void)
{
    int exit_code = E_FAILURE;

    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[1]);

    exit_code = list_clear(test_list);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(test_list->size, 0);
    CU_ASSERT_PTR_NULL(list_peek_head(test_list));
}

void test_list_delete_null_list_address(void)
{
    int exit_code = E_FAILURE;

    exit_code = list_delete(NULL);
    CU_ASSERT_EQUAL(exit_code, E_NULL_POINTER);
}

void test_list_delete(void)
{
    int exit_code = E_FAILURE;

    list_push_head(test_list, &data[0]);
    list_push_head(test_list, &data[1]);

    exit_code = list_delete(&test_list);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_PTR_NULL(test_list);
}

static CU_TestInfo linked_list_tests[] = {
    { "new_list", test_list_new },
    { "push_head_null_list", test_list_push_head_null_list },
    { "push_head_null_data", test_list_push_head_null_data },
    { "push_head_single", test_list_push_head_single },
    { "push_head_multiple", test_list_push_head_multiple },
    { "push_tail_null_list", test_list_push_tail_null_list },
    { "push_tail_null_data", test_list_push_tail_null_data },
    { "push_tail_single", test_list_push_tail_single },
    { "push_tail_multiple", test_list_push_tail_multiple },
    { "push_position_null_list", test_list_push_position_null_list },
    { "push_position_null_data", test_list_push_position_null_data },
    { "push_position_out_of_bounds", test_list_push_position_out_of_bounds },
    { "push_position_single", test_list_push_position_single },
    { "push_position_multiple", test_list_push_position_multiple },
    { "is_empty_null_list", test_list_is_empty_null_list },
    { "is_empty", test_list_is_empty },
    { "pop_head_null_list", test_list_pop_head_null_list },
    { "pop_head_empty", test_list_pop_head_empty },
    { "pop_head_single", test_list_pop_head_single },
    { "pop_head_multiple", test_list_pop_head_multiple },
    { "pop_tail_null_list", test_list_pop_tail_null_list },
    { "pop_tail_empty", test_list_pop_tail_empty },
    { "pop_tail_single", test_list_pop_tail_single },
    { "pop_tail_multiple", test_list_pop_tail_multiple },
    { "pop_position_null_list", test_list_pop_position_null_list },
    { "pop_position", test_list_pop_position },
    { "remove_head_null_list", test_list_remove_head_null_list },
    { "remove_head", test_list_remove_head },
    { "remove_tail_null_list", test_list_remove_tail_null_list },
    { "remove_tail", test_list_remove_tail },
    { "remove_position_null_list", test_list_remove_position_null_list },
    { "remove_position", test_list_remove_position },
    { "peek_head_null_list", test_list_peek_head_null_list },
    { "peek_head", test_list_peek_head },
    { "peek_tail_null_list", test_list_peek_tail_null_list },
    { "peek_tail", test_list_peek_tail },
    { "peek_position_null_list", test_list_peek_position_null_list },
    { "peek_position", test_list_peek_position },
    { "remove_data_null_list", test_list_remove_data_null_list },
    { "remove_data_null_data_p", test_list_remove_data_null_data_p },
    { "remove_data", test_list_remove_data },
    { "foreach_call_null_list", test_list_foreach_call_null_list },
    { "foreach_call", test_list_foreach_call },
    { "contains_null_list", test_list_contains_null_list },
    { "contains_null_data_p", test_list_contains_null_data_p },
    { "contains", test_list_contains },
    { "pick_random_item_null_list", test_list_pick_random_item_null_list },
    { "pick_random_item", test_list_pick_random_item },
    { "find_first_occurrence_null_list",
      test_list_find_first_occurrence_null_list },
    { "find_first_occurrence_null_search_data",
      test_list_find_first_occurrence_null_search_data },
    { "find_first_occurrence", test_list_find_first_occurrence },
    { "find_all_occurrences_null_list",
      test_list_find_all_occurrences_null_list },
    { "find_all_occurrences_null_search_data",
      test_list_find_all_occurrences_null_search_data },
    { "find_all_occurrences", test_list_find_all_occurrences },
    { "sort_null_list", test_list_sort_null_list },
    { "sort", test_list_sort },
    { "clear_null_list", test_list_clear_null_list },
    { "clear", test_list_clear },
    { "delete_null_list_address", test_list_delete_null_list_address },
    { "delete", test_list_delete },
    CU_TEST_INFO_NULL
};

CU_SuiteInfo linked_list_test_suite = {
    "linked_list Tests",
    NULL,             // Suite initialization function
    NULL,             // Suite cleanup function
    setup,            // Suite setup function
    teardown,         // Suite teardown function
    linked_list_tests // The combined array of all tests
};

/*** end of file ***/
