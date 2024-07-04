#ifndef _SOCKET_IO_H
#define _SOCKET_IO_H

#include <netdb.h> // struct addrinfo
#include <stdint.h>
#include <stdlib.h>

#define MIN_SOCKET 3 // The lowest allowable user-defined socket
#define MAX_BYTES \
    (size_t)1400 // Max number of bytes to send or received at once

int create_socket(int * fd, struct addrinfo * addr);
int send_data(int socket, void * buffer_p);
int recv_data(int socket, void * buffer_p);
int set_fd_non_blocking(int socket_fd);

/**
 * @brief Sends the specified number of bytes to a given socket.
 *
 * This function handles sending data over a specified socket. It ensures
 * that the arguments are valid and then utilizes the socket_io function to
 * manage the actual sending of data, processing it in chunks as defined by
 * MAX_BYTES.
 *
 * @param socket The socket descriptor for sending data.
 * @param buffer_p A pointer to the buffer containing the data to send.
 * @param bytes_to_send The total number of bytes to send.
 * @return E_SUCCESS on successful completion of the send operation or
 * E_FAILURE in case of an error.
 */
int send_all_data(int socket, void * buffer_p, size_t bytes_to_send);

/**
 * @brief Receives the specified number of bytes from a given socket.
 *
 * This function handles receiving data over a specified socket. It ensures that
 * the arguments are valid and then utilizes the socket_io function to manage
 * the actual receiving of data, processing it in chunks as defined by
 * MAX_BYTES.
 *
 * @param socket The socket descriptor for receiving data.
 * @param buffer_p A pointer to the buffer where the received data will be
 * stored.
 * @param bytes_to_recv The total number of bytes to receive.
 * @return E_SUCCESS on successful completion of the receive operation or
 * E_FAILURE in case of an error.
 */
int recv_all_data(int socket, void * buffer_p, size_t bytes_to_recv);

#endif
