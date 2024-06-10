#include <arpa/inet.h> //inet_ntop()
#include <errno.h>     // errno
#include <poll.h>      // poll()
#include <pthread.h>   // pthread_mutext_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy()
#include <unistd.h> // close()

#include "event_handler.h"
#include "job_handler.h" // process_client_request()
#include "server_utils.h"
#include "socket_io.h"
#include "socket_manager.h"
#include "utilities.h" // get_in_addr()

#define POLL_ERROR_EVENTS (POLLERR | POLLHUP | POLLNVAL)

static int  recv_opcode(uint8_t * opcode, int client_fd);
static int  create_job_args(int                client_fd,
                            int                arr_idx,
                            uint8_t            opcode,
                            pthread_mutex_t *  fd_mutex,
                            socket_manager_t * sock_mgr,
                            job_arg_t **       job_args);
static void free_job_args(void * arg);

int handle_connections(server_context_t * server)
{
    int             exit_code = E_FAILURE;
    struct pollfd * fd_entry  = NULL;

    if ((NULL == server) || (NULL == server->sock_mgr))
    {
        print_error("handle_connections(): NULL argument passed.");
        goto END;
    }

    for (int idx = 0; idx < server->sock_mgr->fd_count; idx++)
    {
        fd_entry = &server->sock_mgr->fd_arr[idx];

        // Clear revents before processing
        if (fd_entry->revents & POLL_ERROR_EVENTS)
        {
            printf("CLIENT HAS CLOSED THE SOCKET.\n");
            print_error("handle_connections(): Error on socket.");
            close(fd_entry->fd);
            fd_entry->fd = -1;
            continue; // Skip if we get a socket error.
        }

        if (0 == (fd_entry->revents &
                  (POLLIN | POLLOUT | POLLHUP | POLLERR | POLLNVAL)))
        {
            printf("Nothing to read on fd: [%d]\n", fd_entry->fd);
            fd_entry->revents = 0; // Clear revents
            continue;              // Skip if there's no data to read.
        }

        if (fd_entry->fd == server->fd)
        {
            printf("Registering new client on fd: [%d]\n", fd_entry->fd);
            exit_code = register_client(server);
            if (E_SUCCESS != exit_code)
            {
                fd_entry->revents = 0; // Clear revents
                print_error("handle_connections(): Unable to register client.");
                continue; // Keep processing even when a new connection fails.
            }
        }
        else
        {
            printf("Handling a client event on fd: [%d]\n", fd_entry->fd);
            exit_code = handle_client_event(server, idx);
            if (E_SUCCESS != exit_code)
            {
                fd_entry->revents = 0; // Clear revents
                print_error("handle_connections(): Error handling event.");
                continue; // Keep processing even if an event cannot be handled.
            }
        }

        fd_entry->revents = 0;
    }

END:
    return exit_code;
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

    printf("Receiving opcode on fd [%d]\n", client_fd);

    exit_code = recv_opcode(&opcode, client_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_event(): Error receiving opcode.");
        goto END;
    }

    exit_code = create_job_args(client_fd,
                                index,
                                opcode,
                                &server->sock_mgr->mutex_arr[index],
                                server->sock_mgr,
                                &job_args);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_event(): Unable to create job args.");
        goto END;
    }

    pthread_mutex_lock(&server->sock_mgr->mutex);
    server->sock_mgr->fd_arr[index].events &= ~POLLIN;
    pthread_mutex_unlock(&server->sock_mgr->mutex);

    exit_code = threadpool_add_job(server->thread_pool,
                                   server->config->client_request,
                                   free_job_args,
                                   job_args);
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
    printf(
        "recv_opcode(): Received opcode %d from fd %d\n", *opcode, client_fd);

END:
    return exit_code;
}

static int create_job_args(int                client_fd,
                           int                arr_idx,
                           uint8_t            opcode,
                           pthread_mutex_t *  fd_mutex,
                           socket_manager_t * sock_mgr,
                           job_arg_t **       job_args)
{
    int         exit_code    = E_FAILURE;
    job_arg_t * new_job_args = NULL;

    if ((NULL == fd_mutex) || (NULL == job_args))
    {
        print_error("create_job_args(): NULL argument passed.");
        goto END;
    }

    new_job_args = calloc(1, sizeof(job_arg_t));
    if (NULL == new_job_args)
    {
        print_error("create_job_args(): CMR failure - new_job_args.");
        goto END;
    }

    new_job_args->client_fd = client_fd;
    new_job_args->arr_idx   = arr_idx;
    new_job_args->opcode    = opcode;
    new_job_args->fd_mutex  = fd_mutex;
    new_job_args->sock_mgr  = sock_mgr;

    *job_args = new_job_args;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void free_job_args(void * arg)
{
    job_arg_t * job_args = NULL;

    if (NULL == arg)
    {
        print_error("free_job_arg(): NULL argument passed.");
        goto END;
    }

    job_args = (job_arg_t *)arg;

    // Re-enable POLLIN
    job_args->sock_mgr->fd_arr[job_args->arr_idx].events |= POLLIN;

    pthread_mutex_unlock(
        &job_args->sock_mgr->mutex); // Unlock the sock_mgr mutex

    free(job_args);

    printf("FINISHED!!\n");

END:
    return;
}

/*** end of file ***/
