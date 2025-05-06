// NOLINTNEXTLINE
#define _GNU_SOURCE // accept4()

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
#include "runtime_log.h"
#include "server_utils.h"
#include "socket_io.h"
#include "socket_manager.h"
#include "threadsafe_io.h"
#include "utilities.h" // get_in_addr()

#define POLL_ERROR_EVENTS (POLLERR | POLLHUP | POLLNVAL)
#define ARR_SIZE          256

/**
 * @brief Creates job arguments for handling client requests.
 *
 * This function allocates and initializes the job arguments needed for
 * processing a client request. It includes the client file descriptor, the file
 * descriptor mutex, and the socket manager.
 *
 * @param client_fd Client file descriptor.
 * @param fd_mutex Pointer to the file descriptor mutex.
 * @param sock_mgr Pointer to the socket manager.
 * @param job_args Double pointer to store the allocated job arguments.
 * @return int E_SUCCESS on success, E_FAILURE on failure.
 */
static int create_job_args(int                client_fd,
                           pthread_mutex_t *  fd_mutex,
                           socket_manager_t * sock_mgr,
                           job_arg_t **       job_args);

/**
 * @brief Frees the memory allocated for job arguments.
 *
 * This function releases the memory allocated for the job arguments used
 * in processing a client request.
 *
 * @param arg Pointer to the job arguments to free.
 */
static void free_job_args(void * arg);

int handle_connections(server_context_t * server)
{
    int             exit_code      = E_FAILURE;
    struct pollfd * fd_entry       = NULL;
    int             local_fd_count = 0;

    if ((NULL == server) || (NULL == server->sock_mgr))
    {
        PRINT_DEBUG("handle_connections(): NULL argument passed.\n");
        goto END;
    }

    pthread_mutex_lock(&server->sock_mgr->fd_mutex);
    local_fd_count = server->sock_mgr->fd_count;
    pthread_mutex_unlock(&server->sock_mgr->fd_mutex);

    for (int idx = 0; idx < local_fd_count; idx++)
    {
        pthread_mutex_lock(&server->sock_mgr->fd_mutex);
        fd_entry = &server->sock_mgr->fd_arr[idx];
        pthread_mutex_unlock(&server->sock_mgr->fd_mutex);

        if (fd_entry->revents & POLL_ERROR_EVENTS)
        {
            PRINT_DEBUG("handle_connections(): Error on socket.\n");
            close(fd_entry->fd);
            fd_entry->fd = -1;
            continue; // Skip if we get a socket error.
        }

        if (0 == (fd_entry->revents & POLLIN))
        {
            continue; // Skip if there's no data to read.
        }

        if (fd_entry->fd == server->fd)
        {
            exit_code = register_client(server);
            if (E_SUCCESS != exit_code)
            {
                PRINT_DEBUG(
                    "handle_connections(): Unable to register client.\n");
                continue; // Keep processing even when a new connection fails.
            }
        }
        else
        {
            exit_code = handle_client_event(server, idx);
            if (E_SUCCESS != exit_code)
            {
                PRINT_DEBUG("handle_connections(): Error handling event.\n");
                continue; // Keep processing even if an event cannot be handled.
            }
        }
    }

END:
    return exit_code;
}

int register_client(server_context_t * server)
{
    int              exit_code                    = E_FAILURE;
    char             log_new_connection[ARR_SIZE] = { 0 };
    client_context_t client                       = { 0 };

    if (NULL == server)
    {
        PRINT_DEBUG("register_client(): NULL argument passed.\n");
        goto END;
    }

    client.addr_len = sizeof(client.addr);

    errno     = 0;
    client.fd = accept4(
        server->fd, (struct sockaddr *)&client.addr, &client.addr_len, 0);
    if (-1 == client.fd)
    {
        perror("register_client(): accept() failed.\n");
        goto END;
    }

    exit_code = set_fd_non_blocking(client.fd);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("register_client(): Unable to set fd to non-blocking.\n");
        close(client.fd);
        goto END;
    }

    exit_code = sock_fd_add(server->sock_mgr, client.fd);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("register_client(): Unable to add fd to array.\n");
        close(client.fd);
        goto END;
    }

    exit_code = snprintf(log_new_connection,
                         sizeof(log_new_connection),
                         "new connection from port: [%s] on socket: [%d]",
                         inet_ntop(client.addr.ss_family,
                                   get_in_addr((struct sockaddr *)&client.addr),
                                   client.IP_addr,
                                   INET6_ADDRSTRLEN),
                         client.fd);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("register_client(): snprintf() error.\n");
        close(client.fd);
        goto END;
    }

    message_log("INFO", FG_GREEN, BG_NONE, LOG_BOTH, true, log_new_connection);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int handle_client_event(server_context_t * server, int index)
{
    int         exit_code = E_FAILURE;
    int         client_fd = 0;
    job_arg_t * job_args  = NULL;

    if ((NULL == server->sock_mgr) || (NULL == server->sock_mgr->fd_arr) ||
        (NULL == server))
    {
        PRINT_DEBUG("handle_client_event(): NULL argument passed.\n");
        goto END;
    }

    client_fd = server->sock_mgr->fd_arr[index].fd;

    message_log("INFO",
                FG_DEFAULT,
                BG_NONE,
                LOG_FILE,
                true,
                "handling event on client fd [%d]...",
                client_fd);

    exit_code = create_job_args(
        client_fd, &server->sock_mgr->fd_mutex, server->sock_mgr, &job_args);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("handle_client_event(): Unable to create job args.\n");
        goto END;
    }

    exit_code = thread_pool_add_task(server->thread_pool,
                                     server->config->client_request,
                                     free_job_args,
                                     job_args);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("handle_client_event(): Cannot add job to thread pool.\n");
        free(job_args);
        job_args = NULL;
        goto END;
    }

    exit_code = sock_fd_remove(server->sock_mgr, index);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("handle_client_event(): Unable to remove fd from array.\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int process_job(int client_fd)
{
    (void)client_fd;
    return -1;
}

void * process_client_request(void * arg)
{
    int         exit_code = E_FAILURE;
    job_arg_t * job_args  = NULL;

    if (NULL == arg)
    {
        PRINT_DEBUG("process_client_request(): NULL argument passed.\n");
        goto END;
    }

    job_args = (job_arg_t *)arg;

    exit_code = process_job(job_args->client_fd);
    if (E_SUCCESS != exit_code)
    {
        if (E_CONNECTION_CLOSED == exit_code)
        {

            message_log("INFO",
                        FG_RED,
                        BG_NONE,
                        LOG_BOTH,
                        true,
                        "closing connection [%d]",
                        job_args->client_fd);

            close(job_args->client_fd);
            goto END;
        }
        PRINT_DEBUG("process_client_request(): Unable to process job.\n");
    }

    // Determine whether or not to add back to fd array and/or close client
    exit_code = sock_fd_add(job_args->sock_mgr, job_args->client_fd);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("process_client_request(): Unable to add fd to array.\n");
    }

END:
    return NULL;
}

static int create_job_args(int                client_fd,
                           pthread_mutex_t *  fd_mutex,
                           socket_manager_t * sock_mgr,
                           job_arg_t **       job_args)
{
    int         exit_code    = E_FAILURE;
    job_arg_t * new_job_args = NULL;

    if ((NULL == fd_mutex) || (NULL == job_args))
    {
        PRINT_DEBUG("create_job_args(): NULL argument passed.\n");
        goto END;
    }

    new_job_args = calloc(1, sizeof(job_arg_t));
    if (NULL == new_job_args)
    {
        PRINT_DEBUG("create_job_args(): CMR failure - new_job_args.\n");
        goto END;
    }

    new_job_args->client_fd = client_fd;
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
        PRINT_DEBUG("free_job_args(): NULL argument passed.\n");
        goto END;
    }

    job_args = (job_arg_t *)arg;

    free(job_args);
    job_args = NULL;

END:
    return;
}

/*** end of file ***/
