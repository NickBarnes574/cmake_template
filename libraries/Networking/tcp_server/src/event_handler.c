#include <arpa/inet.h> //inet_ntop()
#include <errno.h>     // errno
#include <poll.h>      // poll()
#include <pthread.h>   // pthread_mutext_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy()
#include <unistd.h> // close()

#include "event_handler.h"
#include "opcodes.h"
#include "request_handler.h" // process_client_request()
#include "server_utils.h"
#include "socket_io.h"
#include "socket_manager.h"
#include "utilities.h" // get_in_addr()

#define POLL_ERROR_EVENTS (POLLERR | POLLHUP | POLLNVAL)

static int recv_opcode(uint8_t * opcode, int client_fd);

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
            exit_code = handle_client_event(server, idx);
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

    exit_code = set_fd_non_blocking(client.fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("register_client(): Unable to set fd to non-blocking.");
        close(client.fd);
        goto END;
    }

    exit_code = sock_fd_add(server->sock_mgr, client.fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("register_client(): Unable to add fd to array.");
        close(client.fd);
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

int handle_client_event(server_context_t * server, int index)
{
    int         exit_code = E_FAILURE;
    int         client_fd = 0;
    uint8_t     opcode    = -1;
    job_arg_t * job_args  = NULL;

    if ((NULL == server->sock_mgr) || (NULL == server->sock_mgr->fd_arr) ||
        (NULL == server))
    {
        print_error("handle_client_event(): NULL argument passed.");
        goto END;
    }

    client_fd = server->sock_mgr->fd_arr[index].fd;

    exit_code = recv_opcode(&opcode, client_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_event(): Error receiving opcode.");
        goto END;
    }

    exit_code = create_job_args(
        client_fd, opcode, &server->sock_mgr->mutex_arr[index], &job_args);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_event(): Unable to create job args.");
        goto END;
    }

    exit_code = threadpool_add_job(
        server->thread_pool, process_client_request, free_job_args, job_args);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_event(): Cannot add job to thread pool.");
        free(job_args);
        job_args = NULL;
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int recv_opcode(uint8_t * opcode, int client_fd)
{
    int     exit_code = E_FAILURE;
    uint8_t result    = -1;

    if (NULL == opcode)
    {
        print_error("recv_opcode(): NULL argument passed.");
        goto END;
    }

    exit_code = recv_all_data(client_fd, &result, sizeof(uint8_t));
    if (E_SUCCESS != exit_code)
    {
        goto END;
    }

    *opcode = result;
END:
    return exit_code;
}

/*** end of file ***/
