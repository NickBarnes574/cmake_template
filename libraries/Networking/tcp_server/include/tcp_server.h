/**
 * @file tcp_server.h
 *
 * @brief
 */
#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <stdlib.h>

/**
 * @brief Indicator for server shutdown.
 *
 * This define value is returned by certain functions to indicate
 * that a shutdown signal has been received and the server should
 * terminate its operations gracefully.
 */

/**
 * @brief
 *
 * @param port
 * @param max_connections
 * @return int
 */
int start_tcp_server(char * port);

#endif /* _TCP_SERVER_H */

/*** end of file ***/
