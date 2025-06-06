// NOLINTNEXTLINE
#define _GNU_SOURCE

#include <errno.h> // errno
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "socket_io.h"
#include "threadsafe_io.h"
#include "utilities.h"

/**
 * @brief Calculates the chunk size to be processed, based on the remaining
 * bytes and a defined maximum.
 *
 * This function computes the size of the next chunk to be processed in data
 * transfer. The size will be the lesser of the remaining bytes and the defined
 * maximum (MAX_BYTES).
 *
 * @param bytes_to_process The total number of bytes to process.
 * @param bytes_processed The number of bytes already processed.
 * @return The size of the next chunk to be processed.
 */
static size_t calculate_chunk(size_t bytes_to_process, size_t bytes_processed);

int create_socket(int * fd_ptr, struct addrinfo * addr)
{
    int exit_code = E_FAILURE;
    int optval    = 1; // Enable SO_REUSEADDR
    int sock_fd   = 0;

    if (NULL == addr)
    {
        PRINT_DEBUG("create_socket(): NULL argument passed.");
        goto END;
    }

    errno   = 0;
    sock_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (-1 == sock_fd)
    {
        perror("create_socket(): socket() failed.");
        goto END;
    }

    // Enable the SO_REUSEADDR option to reuse the local address
    exit_code =
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (E_SUCCESS != exit_code)
    {
        perror("create_socket(): setsockopt() failed.");
        close(sock_fd);
        goto END;
    }

    *fd_ptr = sock_fd;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int send_data(int socket, void * buffer_p)
{
    int     exit_code   = E_FAILURE;
    ssize_t byte_result = 0;

    if (NULL == buffer_p)
    {
        PRINT_DEBUG("send_data(): NULL argument passed.\n");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        PRINT_DEBUG("send_data(): Invalid socket.\n");
        goto END;
    }

    byte_result = send(socket, buffer_p, sizeof(buffer_p), 0);
    if (E_FAILURE == byte_result)
    {
        PRINT_DEBUG("send_data(): Error sending data.\n");
        goto END;
    }

    if (0 == byte_result)
    {
        exit_code =
            fprintf(stderr, "send_data(): socket %d hung up.\n", socket);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("send_data(): fprintf() error.\n");
        }
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int recv_data(int socket, void * buffer_p)
{
    int     exit_code   = E_FAILURE;
    ssize_t byte_result = 0;

    if (NULL == buffer_p)
    {
        PRINT_DEBUG("recv_data(): NULL argument passed.\n");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        PRINT_DEBUG("recv_data(): Invalid socket.\n");
        goto END;
    }

    byte_result = recv(socket, buffer_p, sizeof(buffer_p), 0);
    if (E_FAILURE == byte_result)
    {
        PRINT_DEBUG("recv_data(): Error receiving data.\n");
        goto END;
    }

    if (0 == byte_result)
    {
        exit_code =
            fprintf(stderr, "recv_data(): socket %d hung up.\n", socket);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("recv_data(): fprintf() error.\n");
        }
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int send_all_data(int socket, void * buffer_p, size_t bytes_to_send)
{
    int       exit_code        = E_FAILURE;
    size_t    chunk            = 0;
    ssize_t   byte_result      = 0;
    size_t    total_bytes_sent = 0;
    uint8_t * byte_buffer      = NULL;
    uint8_t * position         = NULL;

    if (NULL == buffer_p)
    {
        PRINT_DEBUG("send_all_data(): NULL argument passed.\n");
        goto END;
    }

    if (0 == bytes_to_send)
    {
        PRINT_DEBUG("send_all_data(): Nothing to send.\n");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        PRINT_DEBUG("send_all_data(): Invalid socket.\n");
        goto END;
    }

    while (total_bytes_sent < bytes_to_send)
    {
        // Only allow MAX_BYTES to be sent at a time
        chunk = calculate_chunk(bytes_to_send, total_bytes_sent);

        byte_buffer = (uint8_t *)buffer_p;
        position    = byte_buffer + total_bytes_sent;

        byte_result = send(socket, position, chunk, 0);
        if (E_FAILURE == byte_result)
        {
            PRINT_DEBUG("send_all_data(): Error sending data.\n");
            goto END;
        }

        // Connection closed by the other side
        if (0 == byte_result)
        {
            // Check if data finished transferring
            if (total_bytes_sent < bytes_to_send)
            {
                PRINT_DEBUG(
                    "send_all_data(): Connection closed unexpectedly.\n");
                goto END;
            }

            break;
        }

        total_bytes_sent += byte_result;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int recv_all_data(int socket, void * buffer_p, size_t bytes_to_recv)
{
    int       exit_code            = E_FAILURE;
    size_t    chunk                = 0;
    ssize_t   byte_result          = 0;
    size_t    total_bytes_received = 0;
    uint8_t * byte_buffer          = NULL;
    uint8_t * position             = NULL;

    if (NULL == buffer_p)
    {
        PRINT_DEBUG("recv_all_data(): NULL argument passed.\n");
        goto END;
    }

    if (0 == bytes_to_recv)
    {
        PRINT_DEBUG("recv_all_data(): Nothing to receive.\n");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        PRINT_DEBUG("recv_all_data(): Invalid socket.\n");
        goto END;
    }

    while (total_bytes_received < bytes_to_recv)
    {
        // Only allow MAX_BYTES to be received at a time
        chunk = calculate_chunk(bytes_to_recv, total_bytes_received);

        byte_buffer = (uint8_t *)buffer_p;
        position    = byte_buffer + total_bytes_received;

        byte_result = recv(socket, position, chunk, 0);
        if (E_FAILURE == byte_result)
        {
            PRINT_DEBUG("recv_all_data(): Error receiving data.\n");
            goto END;
        }

        // Connection closed by the other side
        if (0 == byte_result)
        {
            // Check if data finished transferring
            if (total_bytes_received < bytes_to_recv)
            {
                exit_code =
                    fprintf(stderr, "client [%d] was closed.\n", socket);
                if (E_SUCCESS != exit_code)
                {
                    PRINT_DEBUG("recv_all_data(): fprintf() error.\n");
                }
                PRINT_DEBUG(
                    "recv_all_data(): Connection closed unexpectedly.\n");
                exit_code = E_CONNECTION_CLOSED;
                goto END;
            }

            break;
        }

        total_bytes_received += byte_result;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int set_fd_non_blocking(int socket_fd)
{
    int exit_code    = E_FAILURE;
    int flags        = 0;
    int fcntl_result = 0;

    // Get the current file descriptor flags
    flags = fcntl(socket_fd, F_GETFL, 0);
    if (-1 == flags)
    {
        PRINT_DEBUG("set_fd_non_blocking(): fcntl(F_GETFL) failed.\n");
        goto END;
    }

    fcntl_result = fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    if (-1 == fcntl_result)
    {
        PRINT_DEBUG("set_fd_non_blocking(): fcntl(F_SETFL) failed.\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static size_t calculate_chunk(size_t bytes_to_process, size_t bytes_processed)
{
    size_t chunk           = 0;
    size_t bytes_remaining = (bytes_to_process - bytes_processed);

    if (MAX_BYTES < bytes_remaining)
    {
        chunk = MAX_BYTES;
    }
    else
    {
        chunk = bytes_remaining;
    }

    return chunk;
}
