#ifndef _STACK_H
#define _STACK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief structure of a stack node
 *
 * @param data void pointer to whatever data that stack points to
 */
typedef struct stack_node_t
{
    void *data;
} stack_node_t;

/**
 * @brief A function pointer to a custom-defined delete function
 *        required to support deletion/memory deallocation of
 *        arbitrary data types. For simple data types, this function
 *        pointer can simply point to the free function. For more complex data
 *        types, this function should free all of the sub items that are
 *        allocated in the data.
 *
 */
typedef void (*FREE_F)(void *);

/**
 * @brief structure of a stack object
 *
 * @param capacity is the number of nodes the stack can hold
 * @param currentsz is the number of nodes the stack is currently storing
 * @param arr is the array containing the stack node pointers
 * @param customfree pointer to the user defined free function
 * @param compare_function pointer to the user defined compare function
 */
typedef struct stack_t
{
    uint32_t capacity;
    uint32_t currentsz;
    stack_node_t **arr;
    FREE_F customfree;
} stack_t;

/**
 * @brief creates a new stack
 *
 * @param capacity max number of nodes the stack will hold
 * @param customfree pointer to the free function to be used with that list
 * @returns pointer to allocated stack on SUCCESS, NULL on failure
 */
stack_t *stack_init(uint32_t capacity, FREE_F customfree);

/**
 * @brief verifies that stack isn't full
 *
 * @param stack pointer stack object
 * @return int exit code of the function
 */
int stack_is_full(stack_t *stack);

/**
 * @brief verifies that stack isn't empty
 *
 * @param stack pointer stack object
 * @return int exit code of the function
 */
int stack_is_empty(stack_t *stack);

/**
 * @brief pushes a new node into the stack
 *
 * @param stack pointer to stack pointer to push the node into
 * @param data data to be pushed into node
 * @return 0 on success, non-zero value on failure
 */
int stack_push(stack_t *stack, void *data);

/**
 * @brief pops the front node out of the stack
 *
 * @param stack pointer to stack pointer to pop the node off of
 * @return pointer to popped node on SUCCESS, NULL on failure
 */
void *stack_pop(stack_t *stack);

/**
 * @brief get the data from the node at the front of the stack without popping
 *
 * @param stack pointer to stack pointer to peek
 * @return pointer to peeked node on SUCCESS, NULL on failure
 */
void *stack_peek(stack_t *stack);

/**
 * @brief clear all nodes out of a stack
 *
 * @param stack pointer to stack pointer to clear out
 * @return 0 on success, non-zero value on failure
 */
int stack_clear(stack_t *stack);

/**
 * @brief delete a stack
 *
 * @param stack pointer to stack pointer to be destroyed
 * @return 0 on success, non-zero value on failure
 */
int stack_destroy(stack_t **stack);

#endif
