#include <stdlib.h> // qsort()
#include <string.h> // memmove()

#include "utilities.h"
#include "vector.h"

#define LEFT  0 // Used for shifting elements left
#define RIGHT 1 // Used for shifting elements right

typedef int (*VECTOR_CMP)(const void *, const void *);

/**
 * @brief Resizes the vector, doubling its capacity.
 *
 * @param vector Pointer to the vector to be resized.
 * @return Status code indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
static int vector_resize(vector_t * vector);

/**
 * @brief Shifts elements in the vector either to the right or left from a given
 * index.
 *
 * @param vector Pointer to the vector.
 * @param index Index from where the shift begins.
 * @param direction Direction of the shift (RIGHT or LEFT).
 * @return Status code indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
static int vector_shift_elements(vector_t * vector, int index, int direction);

/**
 * @brief Shifts elements in the vector to the right from a given index.
 *
 * @param vector Pointer to the vector.
 * @param index Index from where the shift to the right begins.
 * @return Status code indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
static int vector_shift_elements_right(vector_t * vector, int index);

/**
 * @brief Shifts elements in the vector to the left from a given index.
 *
 * @param vector Pointer to the vector.
 * @param index Index from where the shift to the left begins.
 * @return Status code indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
static int vector_shift_elements_left(vector_t * vector, int index);

vector_t * vector_new(FREE_F free_func, CMP_F comp_func, int initial_capacity)
{
    vector_t * new_vector = NULL;

    if ((NULL == free_func) || (NULL == comp_func))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Create the vector
    new_vector = calloc(1, sizeof(vector_t));
    if (NULL == new_vector)
    {
        print_error("CMR failure.");
        goto END;
    }

    // Allocate space for each element
    new_vector->elements = calloc(initial_capacity, sizeof(void *));
    if (NULL == new_vector->elements)
    {
        free(new_vector);
        new_vector = NULL;
        goto END;
    }

    new_vector->capacity     = initial_capacity;
    new_vector->size         = 0;
    new_vector->custom_free  = free_func;
    new_vector->compare_func = comp_func;

END:
    return new_vector;
}

int vector_append(vector_t * vector, void * data)
{
    int exit_code = E_FAILURE;

    if ((NULL == vector) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Insert data at the end of the vector
    exit_code = vector_insert(vector, data, vector->size);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to insert element.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int vector_insert(vector_t * vector, void * data, int index)
{
    int exit_code = E_FAILURE;

    if ((NULL == vector) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if ((0 > index) || index > vector->size)
    {
        print_error("Position out of bounds.");
        goto END;
    }

    if (vector->size == vector->capacity)
    {
        exit_code = vector_resize(vector);
        if (E_SUCCESS != exit_code)
        {
            goto END;
        }
    }

    exit_code = vector_shift_elements_right(vector, index);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to shift elements right.");
        goto END;
    }

    vector->elements[index] = data;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

bool vector_is_empty(vector_t * vector)
{
    int is_empty = false;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == vector->size)
    {
        is_empty = true;
    }

END:
    return is_empty;
}

void * vector_pop(vector_t * vector)
{
    int    exit_code = E_FAILURE;
    void * temp      = NULL;
    void * element   = NULL;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    temp = vector->elements[vector->size - 1];

    exit_code = vector_shift_elements_left(vector, vector->size);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to shift elements left.");
        goto END;
    }

    element = temp;

END:
    return element;
}

int vector_remove(vector_t * vector, int index)
{
    int    exit_code = E_FAILURE;
    void * temp      = NULL;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if ((0 > index) || index > vector->size)
    {
        print_error("Index out of bounds.");
        goto END;
    }

    temp = vector->elements[index];

    exit_code = vector_shift_elements_left(vector, index);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to shift elements left.");
        goto END;
    }

    vector->custom_free(temp);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * vector_get_element(vector_t * vector, int index)
{
    void * element = NULL;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if ((0 > index) || index > vector->size)
    {
        print_error("Index out of bounds.");
        goto END;
    }

    element = vector->elements[index];

END:
    return element;
}

int vector_set_element(vector_t * vector, void * data, int index)
{
    int exit_code = E_FAILURE;

    if ((NULL == vector) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if ((0 > index) || index > vector->size)
    {
        print_error("Index out of bounds.");
        goto END;
    }

    vector->elements[index] = data;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int vector_size(vector_t * vector)
{
    int size = -1;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    size = vector->size;
END:
    return size;
}

int vector_capacity(vector_t * vector)
{
    int capacity = -1;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    capacity = vector->capacity;
END:
    return capacity;
}

int vector_iterate(vector_t * vector, ACT_F action_function)
{
    int exit_code = E_FAILURE;

    if ((NULL == vector) || (NULL == action_function))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Iterate through each element and call the action function
    for (int idx = 0; idx < vector->size; idx++)
    {
        action_function(vector->elements[idx]);
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * vector_find_first_occurrence(vector_t * vector, void ** search_data)
{
    void * found_element = NULL;

    if ((NULL == vector) || (NULL == search_data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    for (int idx = 0; idx < vector->size; idx++)
    {
        if (EQUAL == vector->compare_func(search_data, vector->elements[idx]))
        {
            found_element = vector->elements[idx];
            goto END;
        }
    }

END:
    return found_element;
}

vector_t * vector_find_all_occurrences(vector_t * vector, void ** search_data)
{
    vector_t * result_vector = NULL;
    int        exit_code     = E_FAILURE;

    if ((NULL == vector) || (NULL == search_data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == vector->size)
    {
        print_error("Empty vector.");
        goto END;
    }

    result_vector =
        vector_new(vector->custom_free, vector->compare_func, vector->size);
    if (NULL == result_vector)
    {
        print_error("Unable to create result vector.");
        goto END;
    }

    for (int idx = 0; idx < vector->size; idx++)
    {
        if (EQUAL == vector->compare_func(search_data, vector->elements[idx]))
        {
            exit_code = vector_append(result_vector, vector->elements[idx]);
            if (E_SUCCESS != exit_code)
            {
                print_error("Unable to append element.");
                vector_delete(&vector);
                goto END;
            }
        }
    }

    // If no occurrences were found, free the result vector and return NULL
    if (0 == result_vector->size)
    {
        vector_delete(&result_vector);
    }

END:
    return result_vector;
}

int vector_sort(vector_t * vector)
{
    int exit_code = E_FAILURE;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    qsort(vector->elements,
          vector->size,
          sizeof(void *),
          (VECTOR_CMP)vector->compare_func);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int vector_clear(vector_t * vector)
{
    int exit_code = E_FAILURE;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Call the custom free function on each element
    for (int idx = 0; idx < vector->size; idx++)
    {
        vector->custom_free(vector->elements[idx]);
    }

    // Reset the size
    vector->size = 0;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void vector_delete(vector_t ** vector)
{
    int exit_code = E_FAILURE;

    if (NULL == vector || NULL == *vector)
    {
        print_error("NULL argument passed.");
        return;
    }

    // Clear all elements
    exit_code = vector_clear(*vector);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to clear vector.");
        goto END;
    }

    // Free the elements array
    free((*vector)->elements);

    // Free the vector itself
    free(*vector);

    // Set the vector pointer to NULL
    *vector = NULL;

END:
    return;
}

/***********************************************************************
 * NOTE: STATIC FUNCTIONS LISTED BELOW
 ***********************************************************************/

static int vector_resize(vector_t * vector)
{
    int     exit_code     = E_FAILURE;
    void ** resized_array = NULL;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Attempt to double the capacity of the array vector
    resized_array =
        realloc(vector->elements, (vector->capacity * 2) * (sizeof(void *)));
    if (NULL == resized_array)
    {
        print_error("Failed to reallocate array vector.");
        goto END;
    }

    vector->capacity *= 2;
    vector->elements = resized_array;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int vector_shift_elements(vector_t * vector, int index, int direction)
{
    int     exit_code         = E_FAILURE;
    void ** source            = NULL;
    void ** destination       = NULL;
    int     elements_to_shift = 0;
    int     num_bytes         = 0;

    if (NULL == vector)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if ((0 > index) || index > vector->size)
    {
        print_error("Position out of bounds.");
        goto END;
    }

    elements_to_shift = vector->size - index;
    num_bytes         = elements_to_shift * sizeof(*vector->elements);

    // Determine whether to shift elements right or left
    switch (direction)
    {
        case RIGHT:
            source      = &vector->elements[index];
            destination = &vector->elements[index + 1];
            break;

        case LEFT:
            source      = &vector->elements[index + 1];
            destination = &vector->elements[index];
            break;

        default:
            print_error("Invalid direction.");
            goto END;
    }

    // Perform the shift
    memmove(destination, source, num_bytes);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int vector_shift_elements_right(vector_t * vector, int index)
{
    int exit_code = E_FAILURE;

    exit_code = vector_shift_elements(vector, index, RIGHT);
    if (E_SUCCESS != exit_code)
    {
        goto END;
    }

    // Increase the size of the vector
    vector->size++;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int vector_shift_elements_left(vector_t * vector, int index)
{
    int exit_code = E_FAILURE;

    exit_code = vector_shift_elements(vector, index, LEFT);
    if (E_SUCCESS != exit_code)
    {
        goto END;
    }

    // Decrease the size of the vector
    vector->size--;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}