/**
 * @file concatenation.h
 *
 * @brief
 */
#ifndef _CONCATENATION_H
#define _CONCATENATION_H

#define MAX_STR_LEN 100

/**
 * @file str_concat.h
 *
 * @brief Concatenates two strings into a new string.
 *
 * NOTE:The new string should be dynamically allocated from inside the function,
 * and must be freed by the caller.
 *
 * @param p_str1 Pointer to the first null-terminated string.
 * @param p_str2 Pointer to the second null-terminated string.
 * @param pp_result Double pointer to store the address of the concatenated string.
 * @return int Returns E_SUCCESS on success, and E_FAILURE on failure.
 */
int str_concat(const char *p_str1, const char *p_str2, char **pp_result);

#endif /* _CONCATENATION_H */

/*** end of file ***/
