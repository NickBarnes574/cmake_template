#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "callback_types.h"

/**
 * @brief A pointer to a user-defined function that gets called in the
 * foreach_call on each item in the vector.
 */
typedef void (*ACT_F)(void *);

/**
 * @brief A pointer to a user-defined free function. This is used to free
 * memory allocated for list data. For simple data types, this is just a
 * pointer to the standard free function. More complex structs stored in array
 * lists may require a function that calls free on multiple components.
 */
typedef void (*FREE_F)(void *);

typedef struct vector
{
    void ** elements;
    int     size;
    int     capacity;
    FREE_F  custom_free;
    CMP_F   compare_func;
} vector_t;

/**
 * @brief Initializes a new vector with a given capacity.
 * @param custom_free Function pointer to a custom free function for the
 * elements.
 * @param compare_func Function pointer to a comparison function for the
 * elements.
 * @param initial_capacity Initial capacity of the vector.
 * @return A pointer to the newly created vector.
 */
vector_t * vector_new(FREE_F custom_free,
                      CMP_F  compare_func,
                      int    initial_capacity);

/**
 * @brief Appends a data element to the end of the vector.
 * @param vector Pointer to the vector.
 * @param data Pointer to the data to append.
 * @return Status code indicating success or failure.
 */
int vector_append(vector_t * vector, void * data);

/**
 * @brief Inserts a data element at a specific index in the vector.
 * @param vector Pointer to the vector.
 * @param data Pointer to the data to insert.
 * @param index Index at which to insert the data.
 * @return Status code indicating success or failure.
 */
int vector_insert(vector_t * vector, void * data, int index);

/**
 * @brief Checks if the vector is empty.
 * @param vector Pointer to the vector.
 * @return 'true' if the vector is empty, 'false' otherwise.
 */
bool vector_is_empty(vector_t * vector);

/**
 * @brief Removes and returns the last element from the vector.
 * @param vector Pointer to the vector.
 * @return Pointer to the popped element.
 */
void * vector_pop(vector_t * vector);

/**
 * @brief Removes an element at a specific index from the vector.
 * @param vector Pointer to the vector.
 * @param index Index of the element to remove.
 * @return Status code indicating success or failure.
 */
int vector_remove(vector_t * vector, int index);

/**
 * @brief Retrieves an element at a specific index from the vector.
 * @param vector Pointer to the vector.
 * @param index Index of the element to retrieve.
 * @return Pointer to the retrieved element.
 */
void * vector_get_element(vector_t * vector, int index);

/**
 * @brief Sets an element at a specific index in the vector.
 * @param vector Pointer to the vector.
 * @param data Pointer to the data to set.
 * @param index Index of the element to set.
 * @return Status code indicating success or failure.
 */
int vector_set_element(vector_t * vector, void * data, int index);

/**
 * @brief Retrieves the current size of the vector.
 * @param vector Pointer to the vector.
 * @return Current size of the vector.
 */
int vector_size(vector_t * vector);

/**
 * @brief Retrieves the current capacity of the vector.
 * @param vector Pointer to the vector.
 * @return Current capacity of the vector.
 */
int vector_capacity(vector_t * vector);

/**
 * @brief Iterates over the vector and calls a user-provided function on each
 * element.
 * @param vector Pointer to the vector.
 * @param action_function Function to call on each element.
 * @return Status code indicating success or failure.
 */
int vector_iterate(vector_t * vector, ACT_F action_function);

/**
 * @brief Finds the first occurrence of a specific element in the vector.
 * @param vector Pointer to the vector.
 * @param search_data Pointer to the data to search for.
 * @return Pointer to the first occurrence, or NULL if not found.
 */
void * vector_find_first_occurrence(vector_t * vector, void ** search_data);

/**
 * @brief Finds all occurrences of a specific element in the vector.
 * @param vector Pointer to the vector.
 * @param search_data Pointer to the data to search for.
 * @return A new vector containing all occurrences, or NULL if not found.
 */
vector_t * vector_find_all_occurrences(vector_t * vector, void ** search_data);

/**
 * @brief Sorts the elements in the vector.
 * @param vector Pointer to the vector.
 * @return Status code indicating success or failure.
 */
int vector_sort(vector_t * vector);

/**
 * @brief Clears all elements from the vector.
 * @param vector Pointer to the vector.
 * @return Status code indicating success or failure.
 */
int vector_clear(vector_t * vector);

/**
 * @brief Deletes the vector and frees all associated memory.
 * @param vector Pointer to a pointer to the vector.
 * @return Status code indicating success or failure.
 */
void vector_delete(vector_t ** vector);

#endif