/**
 * @file system_info.h
 *
 * @brief
 */
#ifndef _SYSTEM_INFO_H
#define _SYSTEM_INFO_H

#include <stdio.h>
#include <stdlib.h>

char * get_hostname();
char * get_operating_system();
char * get_cpu_info();
char * get_cpu_architecture();
char * get_memory_info();
char * get_network_interfaces();

#endif /* _SYSTEM_INFO_H */

/*** end of file ***/
