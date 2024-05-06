#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "socket_manager.h"
#include "utilities.h"

int fd_add(socket_manager_t * sock_mgr, int new_fd)
{
    int exit_code = E_FAILURE;

    if ((NULL == sock_mgr))
    {
        print_error("fd_add(): NULL argument passed.");
        goto END;
    }

    if (sock_mgr->fd_count == sock_mgr->fd_capacity)
    {
        exit_code = increase_capacity(sock_mgr);
        if (E_SUCCESS != exit_code)
        {
            print_error("fd_add(): Unable to check capacity.");
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

int fd_remove(socket_manager_t * sock_mgr, int index)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("fd_remove(): NULL argument passed.");
        goto END;
    }

    sock_mgr->fd_arr[index] = sock_mgr->fd_arr[sock_mgr->fd_count - 1];
    sock_mgr->fd_count--;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int increase_capacity(socket_manager_t * sock_mgr)
{
    int             exit_code    = E_FAILURE;
    int             new_capacity = 0;
    struct pollfd * new_array    = NULL;

    if (NULL == sock_mgr)
    {
        print_error("increase_capacity(): NULL argument passed.");
        goto END;
    }

    new_capacity = sock_mgr->fd_capacity * 2;
    new_array =
        realloc(sock_mgr->fd_arr, sizeof(*sock_mgr->fd_arr) * new_capacity);
    if (NULL == new_array)
    {
        print_error("increase_capacity(): CMR failure - 'fd_arr'.");
        goto END;
    }

    sock_mgr->fd_arr      = new_array;
    sock_mgr->fd_capacity = new_capacity;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int setup_poll(socket_manager_t * sock_mgr, int fd)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("setup_poll(): NULL argument passed.");
        goto END;
    }

    sock_mgr->fd_arr = calloc(DEFAULT_FD_CAPACITY, sizeof(struct pollfd));
    if (NULL == sock_mgr->fd_arr)
    {
        print_error("setup_poll(): CMR failure - 'fd_arr'.");
        goto END;
    }

    if (0 >= sock_mgr->max_fds)
    {
        print_error("setup_poll(): 'max_fds' must be greater than 0");
        goto END;
    }

    sock_mgr->fd_arr[0].fd     = fd;     // Set up the server socket
    sock_mgr->fd_arr[0].events = POLLIN; // Check for read events
    sock_mgr->fd_count         = 1;

    // Initialize the rest of the array
    for (int idx = 1; idx < sock_mgr->max_fds; idx++)
    {
        sock_mgr->fd_arr[idx].fd     = -1; // Unused slots are set to `-1`
        sock_mgr->fd_arr[idx].events = 0;
    }

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(sock_mgr->fd_arr);
        sock_mgr->fd_arr = NULL;
    }
    return exit_code;
}

/*** end of file ***/
