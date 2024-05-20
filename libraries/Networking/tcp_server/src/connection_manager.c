#include <arpa/inet.h> //inet_ntop()
#include <errno.h>     // errno
#include <poll.h>      // poll()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // close()

#include "connection_manager.h"
#include "server_utils.h"
#include "socket_io.h"
#include "socket_manager.h"
#include "string_operations.h" // copy_string()
#include "utilities.h"         // get_in_addr()

#define POLL_ERROR_EVENTS  (POLLERR | POLLHUP | POLLNVAL)
#define CLIENT_BUFFER_SIZE 256

typedef struct job_arg
{
    char data[CLIENT_BUFFER_SIZE]; // Data to process
    int  client_fd;                // Client file descriptor
} job_arg_t;

int handle_connections(server_context_t * server)
{
    int             exit_code = E_FAILURE;
    struct pollfd * fd_entry  = NULL;

    if ((NULL == server) || (NULL == server->sock_mgr))
    {
        print_error("handle_connections(): NULL argument passed.");
        return E_FAILURE;
    }

    for (int idx = 0; idx < server->sock_mgr->fd_count; idx++)
    {
        fd_entry = &server->sock_mgr->fd_arr[idx];

        if (fd_entry->revents & POLL_ERROR_EVENTS)
        {
            print_error("handle_connections(): Error on socket.");
            close(fd_entry->fd);
            fd_entry->fd = -1;
            continue; // Skip if we get a socket error
        }

        if (0 == (fd_entry->revents & POLLIN))
        {
            continue; // Skip if there's no data to read
        }

        if (fd_entry->fd == server->fd)
        {
            exit_code = register_client(server);
            if (E_SUCCESS != exit_code)
            {
                // Continue processing even when a new connection fails
                continue;
            }
        }
        else
        {
            exit_code = handle_client_activity(server, idx);
            if (E_SUCCESS != exit_code)
            {
                continue;
            }
        }
    }

    return E_SUCCESS;
}

int register_client(server_context_t * server)
{
    int              exit_code = E_FAILURE;
    client_context_t client    = { 0 };

    if (NULL == server)
    {
        print_error("register_client(): NULL argument passed.");
        goto END;
    }

    client.addr_len = sizeof(client.addr);

    errno = 0;
    client.fd =
        accept(server->fd, (struct sockaddr *)&client.addr, &client.addr_len);
    if (-1 == client.fd)
    {
        perror("register_client(): accept() failed.");
        goto END;
    }

    exit_code = sock_fd_add(server->sock_mgr, client.fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("register_client(): Unable to add fd to array.");
        goto END;
    }

    printf("pollserver: new connection from %s on socket %d\n",
           inet_ntop(client.addr.ss_family,
                     get_in_addr((struct sockaddr *)&client.addr),
                     client.IP_addr,
                     INET6_ADDRSTRLEN),
           client.fd);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * process_client_request(void * arg)
{
    int         exit_code = E_FAILURE;
    job_arg_t * job_args  = NULL;

    if (NULL == arg)
    {
        print_error("process_client_request(): NULL argument passed.");
        goto END;
    }

    job_args = (job_arg_t *)arg;

    // Process data
    printf("Processing data: %s\n", job_args->data);

    exit_code = send_data(job_args->client_fd, job_args->data);
    if (E_SUCCESS != exit_code)
    {
        print_error("process_client_request(): Unable to send data.");
        goto END;
    }

END:
    return NULL;
}

int handle_client_activity(server_context_t * server, int index)
{
    int  exit_code                  = E_FAILURE;
    char buffer[CLIENT_BUFFER_SIZE] = { 0 };
    int  client_fd                  = 0;

    if ((NULL == server->sock_mgr) || (NULL == server->sock_mgr->fd_arr) ||
        (NULL == server))
    {
        print_error("handle_client_activity(): NULL argument passed.");
        goto END;
    }

    client_fd = server->sock_mgr->fd_arr[index].fd;

    exit_code =
        receive_data_from_client(client_fd, buffer, server->sock_mgr, index);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): Unable to receive data.");
        goto END;
    }

    job_arg_t * job_args = calloc(1, sizeof(job_arg_t));
    if (NULL == job_args)
    {
        print_error("handle_client_activity(): CMR failure - job_args.");
        goto END;
    }

    job_args->client_fd = client_fd;
    exit_code =
        copy_string(buffer, &(job_args->data), CLIENT_BUFFER_SIZE - 1, false);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): Failed to copy buffer.");
        free(job_args);
        job_args = NULL;
        goto END;
    }

    exit_code = threadpool_add_job(
        server->thread_pool, process_client_request, free, job_args);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): Cannot add job to thread pool.");
        free(job_args->data);
        free(job_args);
        job_args = NULL;
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int receive_data_from_client(int                client_fd,
                             char *             buffer,
                             socket_manager_t * sock_mgr,
                             int                index)
{
    int exit_code = E_FAILURE;

    if ((NULL == buffer) || (NULL == sock_mgr))
    {
        print_error("receive_data_from_client(): NULL argument passed.");
        goto END;
    }

    exit_code = recv_data(client_fd, buffer);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): recv_data() failed.");
        close(client_fd);
        exit_code = sock_fd_remove(sock_mgr, index);
        if (E_SUCCESS != exit_code)
        {
            print_error("handle_client_activity(): Unable to remove fd.");
        }
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
