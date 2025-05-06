/**
 * @file system_info.h
 *
 * @brief Provides system information queries such as hostname, OS, CPU, memory,
 * and network interfaces.
 */

#ifndef _SYSTEM_INFO_H
#define _SYSTEM_INFO_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Retrieves the hostname of the system.
 *
 * @return A dynamically allocated string containing the hostname,
 *         or NULL if an error occurs. Caller must free the string.
 */
char * get_hostname();

/**
 * @brief Retrieves the operating system name.
 *
 * @return A dynamically allocated string containing the OS name,
 *         or NULL if an error occurs. Caller must free the string.
 */
char * get_operating_system();

/**
 * @brief Retrieves the CPU model information.
 *
 * @return A dynamically allocated string containing the CPU model name,
 *         or NULL if an error occurs. Caller must free the string.
 */
char * get_cpu_info();

/**
 * @brief Retrieves the CPU architecture type (e.g., x86_64, armv7l).
 *
 * @return A dynamically allocated string containing the CPU architecture,
 *         or NULL if an error occurs. Caller must free the string.
 */
char * get_cpu_architecture();

/**
 * @brief Retrieves the total memory available on the system (in megabytes).
 *
 * @return A dynamically allocated string containing the total memory (e.g.,
 * "8000 MB"), or NULL if an error occurs. Caller must free the string.
 */
char * get_memory_info();

/**
 * @brief Retrieves a list of network interfaces.
 *
 * @return A dynamically allocated string listing the available network
 * interfaces, or NULL if an error occurs. Caller must free the string.
 */
char * get_network_interfaces();

#endif /* _SYSTEM_INFO_H */

/*** end of file ***/
