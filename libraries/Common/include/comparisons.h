/** @file comparisons.h
 *
 * @brief Module for comparisons for the datastructures.
 */
#ifndef _COMPARISONS_H
#define _COMPARISONS_H

// return values all comparison functions should use for standardizing
// behavior
typedef enum
{
    ERROR = -1,
    EQUAL,
    GREATER_THAN,
    LESS_THAN,
    NOT_EQUAL
} comp_rtns_t;

/**
 * @brief Compare function for node pointers.
 *
 * Compares two node pointers for equality. Returns EQUAL if the pointers are
 * the same, and NOT_EQUAL otherwise.
 *
 * @param ptr_1 Pointer to the first node.
 * @param ptr_2 Pointer to the second node.
 * @return EQUAL if the pointers are the same, NOT_EQUAL otherwise.
 */
comp_rtns_t node_ptr_comp(void * ptr_1, void * ptr_2);

/**
 * @brief Fuction to compare integer data inside two void sources
 *
 * @param p_node_one_data pointer to the data from the first source
 * @param p_node_two_data pointer to the data from the second source
 * @return EQUAL (if equal), GREATER_THAN (if first argument is larger),
 * LESS_THAN (if first_argument is less), ERROR (on error)
 */
comp_rtns_t int_comp(void * p_node_one_data, void * p_node_two_data);

/**
 * @brief Fuction to compare string data inside two void sources
 *
 * @param p_node_one_data pointer to the data from the first source
 * @param p_node_two_data pointer to the data from the second source
 * @return EQUAL (if equal), GREATER_THAN (if first argument is larger),
 * LESS_THAN (if first_argument is less), ERROR (on error)
 */
comp_rtns_t str_comp(void * p_node_one_data, void * p_node_two_data);

comp_rtns_t cell_comp(void * p_node_one_data, void * p_node_two_data);

#endif /* _COMPARISONS_H */

/*** end of file ***/