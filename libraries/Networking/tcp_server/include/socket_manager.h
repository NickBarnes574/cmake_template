/**
 * @file socket_manager.h
 *
 * @brief
 */
#ifndef _SOCKET_MANAGER_H
#define _SOCKET_MANAGER_H

#include "server_structs.h"

int fd_add(socket_manager_t * sock_mgr, int new_fd);
int fd_remove(socket_manager_t * sock_mgr, int index);
int increase_capacity(socket_manager_t * sock_mgr);
int setup_poll(socket_manager_t * context, int fd);

#endif /* _SOCKET_MANAGER_H */

/*** end of file ***/
