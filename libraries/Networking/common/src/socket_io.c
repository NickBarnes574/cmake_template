#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "socket_io.h"
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

int send_data(int socket, void * buffer_p)
{
    int     exit_code   = E_FAILURE;
    ssize_t byte_result = 0;

    if (NULL == buffer_p)
    {
        print_error("send_data(): NULL argument passed.");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        print_error("send_data(): Invalid socket.");
        goto END;
    }

    byte_result = send(socket, buffer_p, sizeof(buffer_p), 0);
    if (E_FAILURE == byte_result)
    {
        print_error("send_data(): Error sending data.");
        goto END;
    }

    if (0 == byte_result)
    {
        fprintf(stderr, "send_data(): socket %d hung up.\n", socket);
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
        print_error("recv_data(): NULL argument passed.");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        print_error("recv_data(): Invalid socket.");
        goto END;
    }

    byte_result = recv(socket, buffer_p, sizeof(buffer_p), 0);
    if (E_FAILURE == byte_result)
    {
        print_error("recv_data(): Error receiving data.");
        goto END;
    }

    if (0 == byte_result)
    {
        fprintf(stderr, "recv_data(): socket %d hung up.\n", socket);
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
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == bytes_to_send)
    {
        print_error("Nothing to send.");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        print_error("Invalid socket.");
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
            print_error("Error sending data.");
            goto END;
        }

        // Connection closed by the other side
        if (0 == byte_result)
        {
            // Check if data finished transferring
            if (total_bytes_sent < bytes_to_send)
            {
                print_error("Connection closed unexpectedly.");
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
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == bytes_to_recv)
    {
        print_error("Nothing to receive.");
        goto END;
    }

    if (MIN_SOCKET > socket)
    {
        print_error("Invalid socket.");
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
            print_error("Error receiving data.");
            goto END;
        }

        // Connection closed by the other side
        if (0 == byte_result)
        {
            // Check if data finished transferring
            if (total_bytes_received < bytes_to_recv)
            {
                print_error("Connection closed unexpectedly.");
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
