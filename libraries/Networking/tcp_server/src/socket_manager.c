#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "socket_manager.h"
#include "utilities.h"

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

    sock_mgr->fd_count    = 1; // For the server fd
    sock_mgr->fd_capacity = fd_capacity;
    sock_mgr->max_fds     = max_fds;

    pthread_mutex_init(&sock_mgr->fd_mutex, NULL);

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(sock_mgr->fd_arr);
        sock_mgr->fd_arr = NULL;
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

    pthread_mutex_lock(&sock_mgr->fd_mutex);

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

    pthread_mutex_unlock(&sock_mgr->fd_mutex);

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

    pthread_mutex_lock(&sock_mgr->fd_mutex);

    if (index < 0 || index >= sock_mgr->fd_count)
    {
        printf("sock_fd_remove(): Invalid index %d\n", index);
        goto END;
    }

    sock_mgr->fd_count--;
    if (index != sock_mgr->fd_count)
    {
        sock_mgr->fd_arr[index].fd = sock_mgr->fd_arr[sock_mgr->fd_count].fd;
        sock_mgr->fd_arr[index].events =
            sock_mgr->fd_arr[sock_mgr->fd_count].events;
        sock_mgr->fd_arr[index].revents =
            sock_mgr->fd_arr[sock_mgr->fd_count].revents;
    }

    // Reset the removed entry (optional but can be helpful for debugging)
    sock_mgr->fd_arr[sock_mgr->fd_count].fd      = -1;
    sock_mgr->fd_arr[sock_mgr->fd_count].events  = 0;
    sock_mgr->fd_arr[sock_mgr->fd_count].revents = 0;

    pthread_mutex_unlock(&sock_mgr->fd_mutex);

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
    pthread_mutex_destroy(&sock_mgr->fd_mutex);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void print_fd_array(socket_manager_t * sock_mgr)
{
    if (sock_mgr == NULL)
    {
        printf("print_fd_array(): NULL socket manager\n");
        return;
    }

    pthread_mutex_lock(&sock_mgr->fd_mutex);
    printf("File Descriptor Array:\n");
    printf("Index | FD   | Events   | Revents\n");
    printf("-----------------------------\n");

    for (int i = 0; i < sock_mgr->fd_count; i++)
    {
        printf("%5d | %4d | %8x | %8x\n",
               i,
               sock_mgr->fd_arr[i].fd,
               sock_mgr->fd_arr[i].events,
               sock_mgr->fd_arr[i].revents);
    }

    printf("\n");
    pthread_mutex_unlock(&sock_mgr->fd_mutex);
}

/*** end of file ***/
