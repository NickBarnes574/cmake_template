/**
 * @file tcp_server.h
 *
 * @brief
 */
#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <stdlib.h>

#include "server_structs.h" // server_context_t, socket_manager_t

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
int initialize_server(server_context_t * server);
int run_server_loop(server_context_t * server);
int handle_client_activity(socket_manager_t * sock_mgr, int index);

#endif /* _TCP_SERVER_H */

/*** end of file ***/
