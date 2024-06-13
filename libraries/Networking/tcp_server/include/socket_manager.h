/**
 * @file socket_manager.h
 *
 * @brief
 */
#ifndef _SOCKET_MANAGER_H
#define _SOCKET_MANAGER_H

#include "server_structs.h"

int  sock_mgr_init(socket_manager_t * sock_mgr,
                   int                server_fd,
                   int                max_fds,
                   int                fd_capacity);
int  sock_fd_add(socket_manager_t * sock_mgr, int new_fd);
int  sock_fd_remove(socket_manager_t * sock_mgr, int index);
int  sock_fd_increase_capacity(socket_manager_t * sock_mgr);
int  sock_fd_arr_init(socket_manager_t * sock_mgr, int server_fd);
int  close_all_sockets(socket_manager_t * sock_mgr);
void print_fd_array(socket_manager_t * sock_mgr);

#endif /* _SOCKET_MANAGER_H */

/*** end of file ***/
