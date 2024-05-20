/**
 * @file string_operations.h
 *
 * @brief
 */
#ifndef _STRING_OPERATIONS_H
#define _STRING_OPERATIONS_H

#define MAX_LABEL_SIZE 20

/**
 * @brief Removes the newline character from the end of a string if present.
 *
 * This function checks if the last character of the input string 'input_str_p'
 * is a newline. If so, it replaces the newline character with a null terminator
 * to effectively strip it from the string. The operation is performed in place
 * and affects the original string.
 *
 * @param input_str_p The string from which the newline will be removed.
 * @return int Returns E_SUCCESS on success, or E_FAILURE on failure.
 */
int strip_newline(char * input_str_p);

/**
 * @brief Checks if two strings are equal to each other
 *
 * @param str1_p the first string
 * @param str2_p the second string
 * @param result_p an int pointer, representing the result (1 for true, 0 for
 * false)
 * @return int Returns E_SUCCESS on success, or E_FAILURE on failure.
 */
int strings_are_equal(const char * str1_p, const char * str2_p, int * result_p);

int copy_string(const char * input,
                char **      output,
                size_t       character_limit,
                bool         allow_truncation);

#endif /* _STRING_OPERATIONS_H */

/*** end of file ***/
