/**
 * @file socket_manager.h
 * @brief Socket manager module for handling socket operations.
 */

#ifndef _SOCKET_MANAGER_H
#define _SOCKET_MANAGER_H

#include "server_structs.h"

/**
 * @brief Initialize the socket manager.
 *
 * This function initializes the socket manager, setting up the file descriptor
 * array and other necessary configurations.
 *
 * @param sock_mgr Pointer to the socket manager structure.
 * @param server_fd The file descriptor of the server socket.
 * @param max_fds The maximum number of file descriptors.
 * @param fd_capacity The initial capacity of the file descriptor array.
 * @return int Status code, E_SUCCESS on success, or an error code on failure.
 */
int sock_mgr_init(socket_manager_t * sock_mgr,
                  int                server_fd,
                  int                max_fds,
                  int                fd_capacity);

/**
 * @brief Add a new file descriptor to the socket manager.
 *
 * This function adds a new file descriptor to the socket manager's file
 * descriptor array.
 *
 * @param sock_mgr Pointer to the socket manager structure.
 * @param new_fd The new file descriptor to be added.
 * @return int Status code, E_SUCCESS on success, or an error code on failure.
 */
int sock_fd_add(socket_manager_t * sock_mgr, int new_fd);

/**
 * @brief Remove a file descriptor from the socket manager.
 *
 * This function removes a file descriptor from the socket manager's file
 * descriptor array.
 *
 * @param sock_mgr Pointer to the socket manager structure.
 * @param index The index of the file descriptor to be removed.
 * @return int Status code, E_SUCCESS on success, or an error code on failure.
 */
int sock_fd_remove(socket_manager_t * sock_mgr, int index);

/**
 * @brief Increase the capacity of the file descriptor array.
 *
 * This function increases the capacity of the file descriptor array to handle
 * more file descriptors.
 *
 * @param sock_mgr Pointer to the socket manager structure.
 * @return int Status code, E_SUCCESS on success, or an error code on failure.
 */
int sock_fd_increase_capacity(socket_manager_t * sock_mgr);

/**
 * @brief Initialize the file descriptor array.
 *
 * This function initializes the file descriptor array, setting up the server
 * socket and initial configurations.
 *
 * @param sock_mgr Pointer to the socket manager structure.
 * @param server_fd The file descriptor of the server socket.
 * @return int Status code, E_SUCCESS on success, or an error code on failure.
 */
int sock_fd_arr_init(socket_manager_t * sock_mgr, int server_fd);

/**
 * @brief Close all sockets managed by the socket manager.
 *
 * This function closes all sockets in the file descriptor array and cleans up
 * resources.
 *
 * @param sock_mgr Pointer to the socket manager structure.
 * @return int Status code, E_SUCCESS on success, or an error code on failure.
 */
int close_all_sockets(socket_manager_t * sock_mgr);

/**
 * @brief Print the file descriptor array.
 *
 * This function prints the current state of the file descriptor array for
 * debugging purposes.
 *
 * @param sock_mgr Pointer to the socket manager structure.
 */
void print_fd_array(socket_manager_t * sock_mgr);

#endif /* _SOCKET_MANAGER_H */

/*** end of file ***/
