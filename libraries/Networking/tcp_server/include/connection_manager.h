/**
 * @file connection_manager.h
 *
 * @brief
 */
#ifndef _CONNECTION_MANAGER_H
#define _CONNECTION_MANAGER_H

#include "server_structs.h"

int handle_client_activity(server_context_t * server, int index);
int handle_connections(server_context_t * server);
int register_client(server_context_t * server);
int receive_data_from_client(int                client_fd,
                             char *             buffer,
                             socket_manager_t * sock_mgr,
                             int                index);

#endif /* _CONNECTION_MANAGER_H */

/*** end of file ***/
