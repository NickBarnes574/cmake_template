/**
 * @file number_converter.h
 *
 * @brief Module to convert numbers
 */
#ifndef _NUMBER_CONVERTER_H
#define _NUMBER_CONVERTER_H

#include <stdint.h>

/**
 * @brief Converts a string to a signed 32-bit integer.
 *
 * This function attempts to convert a string pointed to by 'input_p' to a
 * signed 32-bit integer, and stores the result in the location pointed to by
 * 'output_p'. The function checks for NULL pointers, valid integer ranges, and
 * invalid characters in the string. In case of an error, it prints a message
 * and returns E_FAILURE. The BASE macro is used to define the numeral system
 * (e.g., 10 for decimal).
 *
 * @param input_p Pointer to the input string to be converted.
 * @param output_p Pointer to the 32-bit signed integer where the result will be
 * stored.
 * @return int Returns E_SUCCESS if conversion is successful, otherwise returns
 * E_FAILURE.
 */
int str_to_int32(const char * input_p, int32_t * output_p);

/**
 * @brief Converts a string to an unsigned 32-bit integer.
 *
 * This function attempts to convert a string pointed to by 'input_p' to an
 * unsigned 32-bit integer, and stores the result in the location pointed to by
 * 'output_p'. The function checks for NULL pointers, valid integer ranges, and
 * invalid characters in the string. In case of an error, it prints a message
 * and returns E_FAILURE. The BASE macro is used to define the numeral system
 * (e.g., 10 for decimal).
 *
 * @param input_p Pointer to the input string to be converted.
 * @param output_p Pointer to the 32-bit unsigned integer where the result will
 * be stored.
 * @return int Returns E_SUCCESS if conversion is successful, otherwise returns
 * E_FAILURE.
 */
int str_to_uint32(const char * input_p, uint32_t * output_p);

/**
 * @brief Converts a string to an unsigned 16-bit integer.
 *
 * This function attempts to convert a string pointed to by 'input_p' to an
 * unsigned 16-bit integer, and stores the result in the location pointed to by
 * 'output_p'. The function checks for NULL pointers, valid integer ranges, and
 * invalid characters in the string. In case of an error, it prints a message
 * and returns E_FAILURE. The BASE macro is used to define the numeral system
 * (e.g., 10 for decimal).
 *
 * @param input_p Pointer to the input string to be converted.
 * @param output_p Pointer to the 16-bit unsigned integer where the result will
 * be stored.
 * @return int Returns E_SUCCESS if conversion is successful, otherwise returns
 * E_FAILURE.
 */
int str_to_uint16(const char * input_p, uint16_t * output_p);

/**
 * @brief Converts a uint32_t type to a string
 *
 * @param input a uint32_t number to convert to a string
 * @param output_p the result in string form
 * @return int Returns E_SUCCESS on success, E_FAILURE on failure
 */
int uint32_to_str(uint32_t input, char ** output_p);

#endif /* _NUMBER_CONVERTER_H */

/*** end of file ***/
