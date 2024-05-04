/**
 * @file tcp_server.h
 *
 * @brief
 */
#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <stdlib.h>

/**
 * @brief
 *
 * @param port
 * @param max_connections
 * @return int
 */
int start_tcp_server(char * port, size_t max_connections);

#endif /* _TCP_SERVER_H */

/*** end of file ***/
