/**
 * @file server_ops.h
 *
 * @brief
 */
#ifndef _SERVER_OPS_H
#define _SERVER_OPS_H

#include "server_structs.h"

int create_socket(int * fd, struct addrinfo * addr);
int initialize_server(server_context_t * server);
int run_server_loop(server_context_t * server);
int handle_client_activity(socket_manager_t * sock_mgr, int index);

#endif /* _SERVER_OPS_H */

/*** end of file ***/
