#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "socket_manager.h"
#include "utilities.h"

static int  mutex_arr_init(int max_fds, pthread_mutex_t ** mutex_arr);
static void mutex_arr_destroy(int max_fds, pthread_mutex_t * mutex_arr);

int sock_mgr_init(socket_manager_t * sock_mgr,
                  int                server_fd,
                  int                max_fds,
                  int                fd_capacity)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("sock_mgr_init(): NULL argument passed.");
        goto END;
    }

    if (0 >= max_fds)
    {
        print_error("sock_fd_arr_init(): 'max_fds' must be greater than 0");
        goto END;
    }

    exit_code = sock_fd_arr_init(sock_mgr, server_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("sock_mgr_init(): Unable to setup fd array.");
        goto END;
    }

    exit_code = mutex_arr_init(max_fds, &sock_mgr->mutex_arr);
    if (E_SUCCESS != exit_code)
    {
        print_error("sock_mgr_init(): Unable to initialize mutex array.");
        goto END;
    }

    sock_mgr->fd_count    = 1; // For the server fd
    sock_mgr->fd_capacity = fd_capacity;
    sock_mgr->max_fds     = max_fds;

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(sock_mgr->fd_arr);
        sock_mgr->fd_arr = NULL;

        mutex_arr_destroy(max_fds, sock_mgr->mutex_arr);
        free(sock_mgr->mutex_arr);
        sock_mgr->mutex_arr = NULL;
    }
    return exit_code;
}

int sock_fd_add(socket_manager_t * sock_mgr, int new_fd)
{
    int exit_code = E_FAILURE;

    if ((NULL == sock_mgr))
    {
        print_error("sock_fd_add(): NULL argument passed.");
        goto END;
    }

    if (sock_mgr->fd_count == sock_mgr->fd_capacity)
    {
        exit_code = sock_fd_increase_capacity(sock_mgr);
        if (E_SUCCESS != exit_code)
        {
            print_error("sock_fd_add(): Unable to check capacity.");
            goto END;
        }
    }

    // Add new client fd to the array
    sock_mgr->fd_arr[sock_mgr->fd_count].fd     = new_fd;
    sock_mgr->fd_arr[sock_mgr->fd_count].events = POLLIN;
    sock_mgr->fd_count++;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int sock_fd_remove(socket_manager_t * sock_mgr, int index)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("sock_fd_remove(): NULL argument passed.");
        goto END;
    }

    sock_mgr->fd_count--;
    sock_mgr->fd_arr[index].fd = sock_mgr->fd_arr[sock_mgr->fd_count].fd;
    sock_mgr->fd_arr[index].events =
        sock_mgr->fd_arr[sock_mgr->fd_count].events;
    sock_mgr->fd_arr[index].revents =
        sock_mgr->fd_arr[sock_mgr->fd_count].revents;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int sock_fd_increase_capacity(socket_manager_t * sock_mgr)
{
    int             exit_code    = E_FAILURE;
    int             new_capacity = 0;
    struct pollfd * new_array    = NULL;

    if (NULL == sock_mgr)
    {
        print_error("sock_fd_increase_capacity(): NULL argument passed.");
        goto END;
    }

    new_capacity = sock_mgr->fd_capacity * 2;
    new_array =
        realloc(sock_mgr->fd_arr, sizeof(*sock_mgr->fd_arr) * new_capacity);
    if (NULL == new_array)
    {
        print_error("sock_fd_increase_capacity(): CMR failure - 'fd_arr'.");
        goto END;
    }

    sock_mgr->fd_arr      = new_array;
    sock_mgr->fd_capacity = new_capacity;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int sock_fd_arr_init(socket_manager_t * sock_mgr, int server_fd)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("sock_fd_arr_init(): NULL argument passed.");
        goto END;
    }

    sock_mgr->fd_arr = calloc(DEFAULT_FD_CAPACITY, sizeof(struct pollfd));
    if (NULL == sock_mgr->fd_arr)
    {
        print_error("sock_fd_arr_init(): CMR failure - 'fd_arr'.");
        goto END;
    }

    sock_mgr->fd_arr[0].fd     = server_fd; // Set up the server socket
    sock_mgr->fd_arr[0].events = POLLIN;    // Check for read events

    // Initialize the rest of the array
    for (int idx = 1; idx < sock_mgr->max_fds; idx++)
    {
        sock_mgr->fd_arr[idx].fd     = -1; // Unused slots are set to `-1`
        sock_mgr->fd_arr[idx].events = 0;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int close_all_sockets(socket_manager_t * sock_mgr)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("sock_fd_arr_init(): NULL argument passed.");
        goto END;
    }

    // Close any open socket fds
    for (int idx = 0; idx < sock_mgr->fd_count; idx++)
    {
        close(sock_mgr->fd_arr[idx].fd);
    }

    free(sock_mgr->fd_arr);
    sock_mgr->fd_arr = NULL;

    mutex_arr_destroy(sock_mgr->max_fds, sock_mgr->mutex_arr);
    free(sock_mgr->mutex_arr);
    sock_mgr->mutex_arr = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int mutex_arr_init(int max_fds, pthread_mutex_t ** mutex_arr)
{
    int               exit_code = E_FAILURE;
    pthread_mutex_t * new_array = NULL;

    if (NULL == mutex_arr)
    {
        print_error("mutex_arr_init(): NULL argument passed.");
        goto END;
    }

    new_array = calloc(max_fds, sizeof(pthread_mutex_t));
    if (NULL == new_array)
    {
        print_error("mutex_arr_init(): CMR failure - new_array");
        goto END;
    }

    for (int idx = 0; idx < max_fds; idx++)
    {
        pthread_mutex_init(&new_array[idx], NULL);
    }

    *mutex_arr = new_array;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void mutex_arr_destroy(int max_fds, pthread_mutex_t * mutex_arr)
{
    if (NULL == mutex_arr)
    {
        print_error("mutex_arr_destroy(): NULL argument passed.");
        goto END;
    }

    for (int idx = 0; idx < max_fds; idx++)
    {
        pthread_mutex_destroy(&mutex_arr[idx]);
    }

END:
    return;
}

/*** end of file ***/
