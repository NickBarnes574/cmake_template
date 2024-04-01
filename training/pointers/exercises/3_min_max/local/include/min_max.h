/**
 * @file min_max.h
 *
 * @brief
 */
#ifndef _MIN_MAX_H
#define _MIN_MAX_H

/**
 * @brief Finds the minimum and maximum values in an integer array.
 *
 * @param p_arr Pointer to the array of integers.
 * @param length The number of elements in the array.
 * @param p_min Pointer to store the minimum value found.
 * @param p_max Pointer to store the maximum value found.
 * @return int Returns E_SUCCESS on success, and E_FAILURE on failure.
 */
int find_min_max(const int *p_arr, int length, int *p_min, int *p_max);

#endif /* _MIN_MAX_H */

/*** end of file ***/
