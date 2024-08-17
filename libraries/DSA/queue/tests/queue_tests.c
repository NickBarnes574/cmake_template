#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "queue.h"

#define QUEUE_CAPACITY 10

queue_t * test_queue                = NULL;
int       test_data[QUEUE_CAPACITY] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

void custom_free(void * data)
{
    (void)data;
}

void setup(void)
{
    test_queue = queue_init(custom_free);
}

void teardown(void)
{
    if (NULL != test_queue)
    {
        queue_destroy(&test_queue);
    }
}

void test_queue_init(void)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(test_queue);
}

void test_queue_is_empty(void)
{
    CU_ASSERT_TRUE(queue_is_empty(test_queue));

    queue_enqueue(test_queue, &test_data[0]);
    CU_ASSERT_FALSE(queue_is_empty(test_queue));
}

void test_queue_enqueue(void)
{
    int exit_code = queue_enqueue(test_queue, &test_data[0]);
    CU_ASSERT_EQUAL(exit_code, 0);
    CU_ASSERT_EQUAL(test_queue->currentsz, 1);
}

void test_queue_dequeue(void)
{
    queue_enqueue(test_queue, &test_data[0]);
    queue_enqueue(test_queue, &test_data[1]);
    int * data = (int *)queue_dequeue(test_queue);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data);
    CU_ASSERT_EQUAL(*data, test_data[0]);
    CU_ASSERT_EQUAL(test_queue->currentsz, 1);
}

void test_queue_peek(void)
{
    queue_enqueue(test_queue, &test_data[0]);
    int * data = (int *)queue_peek(test_queue);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data);
    CU_ASSERT_EQUAL(*data, test_data[0]);
    CU_ASSERT_EQUAL(test_queue->currentsz, 1);
}

void test_queue_clear(void)
{
    for (int i = 0; i < QUEUE_CAPACITY; ++i)
    {
        queue_enqueue(test_queue, &test_data[i]);
    }
    int exit_code = queue_clear(test_queue);
    CU_ASSERT_EQUAL(exit_code, 0);
    CU_ASSERT_EQUAL(test_queue->currentsz, 0);
    CU_ASSERT_TRUE(queue_is_empty(test_queue));
}

void test_queue_destroy(void)
{
    queue_destroy(&test_queue);
    CU_ASSERT_PTR_NULL(test_queue);
}

static CU_TestInfo queue_tests[] = { { "queue_init", test_queue_init },
                                     { "queue_is_empty", test_queue_is_empty },
                                     { "queue_enqueue", test_queue_enqueue },
                                     { "queue_dequeue", test_queue_dequeue },
                                     { "queue_peek", test_queue_peek },
                                     { "queue_clear", test_queue_clear },
                                     { "queue_destroy", test_queue_destroy },
                                     CU_TEST_INFO_NULL };

CU_SuiteInfo queue_test_suite = {
    "Queue Tests",
    NULL,       // Suite initialization function
    NULL,       // Suite cleanup function
    setup,      // Suite setup function
    teardown,   // Suite teardown function
    queue_tests // The combined array of all tests
};

/*** end of file ***/
