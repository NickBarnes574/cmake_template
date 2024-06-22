/**
 * @file event_handler.h
 *
 * @brief
 */
#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "server_structs.h"

/**
 * @brief Handles all connections for the server.
 *
 * This function iterates through the file descriptors managed by the socket
 * manager and processes the connections. It registers new clients or handles
 * events for existing clients based on the state of the file descriptors.
 *
 * @param server Pointer to the server context.
 * @return int E_SUCCESS on success, E_FAILURE on failure.
 */
int handle_connections(server_context_t * server);

/**
 * @brief Registers a new client connection.
 *
 * This function accepts a new client connection, sets the client socket to
 * non-blocking mode, and adds the client file descriptor to the socket manager.
 * It also logs the new connection.
 *
 * @param server Pointer to the server context.
 * @return int E_SUCCESS on success, E_FAILURE on failure.
 */
int register_client(server_context_t * server);

/**
 * @brief Handles events for a specific client.
 *
 * This function processes the events for a client by creating job arguments
 * and adding the job to the thread pool for handling. It also removes the
 * client's file descriptor from the socket manager.
 *
 * @param server Pointer to the server context.
 * @param index Index of the client file descriptor in the socket manager.
 * @return int E_SUCCESS on success, E_FAILURE on failure.
 */
int handle_client_event(server_context_t * server, int index);

#endif /* _EVENT_HANDLER_H */

/*** end of file ***/
