#include <arpa/inet.h> // bind(), accept()
#include <errno.h>     // Access 'errno' global variable
#include <netdb.h>     // getaddrinfo() struct
#include <stdio.h>     // printf(), fprintf()
#include <stdlib.h>    // calloc(), free()
#include <string.h>    // strerror()
#include <unistd.h>    // close()

#include "signal_handler.h"
#include "socket_io.h"
#include "tcp_server.h"
#include "utilities.h"

#define INVALID_SOCKET (-1)         // Indicates an invalid socket descriptor
#define BACKLOG_SIZE 10             // Maximum number of pending client connections
#define MAX_CLIENT_ADDRESS_SIZE 100 // Size for storing client address strings

/**
 * @struct config
 * @brief  Holds the configuration settings for the server.
 *
 * This structure stores all the necessary information for setting up
 * and running the server, such as the address to bind to, the
 * listening socket, and client-specific details.
 */
struct config
{
    struct addrinfo hints;         // Specifies criteria for address selection
    struct addrinfo *address_list; // Pointer to linked list of address info
    struct addrinfo *
        current_address;                    // Pointer used to traverse 'address_list'
    int listening_socket;                   // Listening socket for the server
    struct sockaddr_storage client_address; // Stores client address
    int client_fd;                          // Socket for accepting connections
    socklen_t client_len;                   // Length of client address structure
};

/**
 * @brief Configures the server's address and creates a listening socket.
 *
 * This function sets up the server's address for a TCP/IPV4 connection and
 * binds it to a listening socket. It tries each address obtained from
 * getaddrinfo() until it successfully creates and binds a socket.
 *
 * @param config_p Pointer to the config_t structure that holds the server
 *        configuration details.
 * @param port_p Pointer to a string representing the port number on which the
 *        server will listen.
 *
 * @return Returns E_SUCCESS if the server address is configured and the socket
 *         is bound successfully, or E_FAILURE on error.
 */
static int configure_server_address(config_t *config_p, char *port_p);

/**
 * @brief Creates a listening socket for the server.
 *
 * This function creates a socket using the specified address family, socket
 * type, and protocol from the current address in the config_t structure. It
 * also sets the SO_REUSEADDR socket option to allow the server to bind to a
 * port in TIME_WAIT state.
 *
 * @param config_p Pointer to the config_t structure containing the server
 *        configuration and current address information.
 *
 * @return Returns E_SUCCESS if the socket is created and configured
 * successfully, or E_FAILURE on error.
 */
static int create_listening_socket(config_t *config_p);

/**
 * @brief Accepts a new client connection on the server's listening socket.
 *
 * This function waits for a client to connect to the server's listening socket.
 * Upon a successful connection, it stores the client's file descriptor and
 * address information in the provided config_t structure. The function also
 * handles shutdown signals during its wait.
 *
 * @param config_p Pointer to the config_t structure containing the server's
 *        listening socket and client address information.
 *
 * @return Returns E_SUCCESS if a new connection is successfully accepted,
 *         SIG_SHUTDOWN if a shutdown signal is received, or E_FAILURE on error.
 */
static int accept_new_connection(config_t *config_p);

/**
 * @brief Prints the address of a connected client.
 *
 * This function uses getnameinfo() to convert the client's address in the
 * sockaddr structure to a human-readable IP address and prints it. It's used
 * to log or display the address of each client that establishes a connection
 * with the server.
 *
 * @param config_p Pointer to the config_t structure containing the client's
 *        address information.
 */
static void print_client_address(config_t *config_p);

/**
 * @brief Handles a client request in a server application.
 *
 * This function is intended to be executed as a job in a thread pool. It takes
 * a tcp_server_job_t structure as an argument, which contains the client's
 * request handling function and the data associated with the client. After
 * processing the request, it ensures that any allocated resources are freed,
 * and the client's socket is closed.
 *
 * @param args_p Pointer to tcp_server_job_t structure containing the client's
 * data and the function to handle the request.
 *
 * @return Returns the result from executing the client's request handling
 * function, or NULL in case of an error or if there is no return value.
 */
static void *handle_client_request(void *args_p);

/**
 * @brief Initializes a server with specified configuration.
 *
 * This function creates and configures a server instance. It allocates memory
 * for server configuration, initializes a thread pool for handling client
 * requests, and sets up the server's listening socket. The server is configured
 * to listen on the provided port and can handle a specified maximum number of
 * concurrent connections.
 *
 * @param port_p Pointer to a string representing the port number on which the
 * server will listen for incoming connections.
 * @param max_connections The maximum number of concurrent connections the
 * server should handle.
 * @param client_request_handler Pointer to the function that will handle client
 *        requests. This function should be of the form `void *(*)(void *)`.
 * @param client_data_free_func Pointer to the function used to free client
 * data; this function should be of the form `void (*)(void *)`.
 *
 * @return On success, returns a pointer to the initialized server_t structure.
 * On failure, returns NULL.
 */
static server_t *init_server(char *port_p,
                             size_t max_connections,
                             client_request_handler_t client_request_handler,
                             client_data_free_func_t client_data_free_func);

/**
 * @brief Destroys the server and releases allocated resources.
 *
 * This function is responsible for cleaning up and releasing all resources
 * allocated to the server. It destroys the thread pool, closes the listening
 * socket, and frees memory allocated to the server structure and its settings.
 *
 * @param server_p Double pointer to the server_t structure that needs to be
 * destroyed.
 *
 * @return Returns E_SUCCESS after successful cleanup or E_FAILURE if an error
 * occurs.
 */
static int destroy_server(server_t **server_p);

static server_t *init_server(char *port_p,
                             size_t max_connections,
                             client_request_handler_t client_request_handler,
                             client_data_free_func_t client_data_free_func)
{
    int exit_code = E_FAILURE;
    threadpool_t *threadpool_p = NULL;
    config_t *config_p = NULL;
    server_t *server_p = NULL;

    if ((NULL == port_p) || (NULL == client_request_handler))
    {
        print_error("init_server(): NULL argument passed.");
        goto END;
    }

    if (2 > max_connections)
    {
        print_error("init_server(): Max connections must be 2 or more.");
        goto END;
    }

    threadpool_p = threadpool_create(max_connections);
    if (NULL == threadpool_p)
    {
        print_error("init_server(): Unable to create threadpool.");
        goto END;
    }

    config_p = calloc(1, sizeof(config_t));
    if (NULL == config_p)
    {
        print_error("init_server(): config_p - CMR failure.");
        goto END;
    }

    exit_code = configure_server_address(config_p, port_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("init_server(): Unable to configure server address.");
        goto END;
    }

    // Activate listening mode for the server's socket, allowing it to queue up
    // to 'BACKLOG_SIZE' connection requests at a time.
    errno = 0;
    exit_code = listen(config_p->listening_socket, BACKLOG_SIZE);
    if (E_SUCCESS != exit_code)
    {
        print_strerror("init_server(): listen() failed.");
        goto END;
    }

    server_p = calloc(1, sizeof(server_t));
    if (NULL == server_p)
    {
        print_error("init_server(): server_p - CMR failure.");
        exit_code = E_FAILURE;
        goto END;
    }

    server_p->listening_socket = config_p->listening_socket;
    server_p->threadpool_p = threadpool_p;
    server_p->client_request_handler = client_request_handler;
    server_p->client_data_free_func = client_data_free_func;
    server_p->settings_p = config_p;

END:
    if (E_SUCCESS != exit_code)
    {
        free(config_p);
        config_p = NULL;

        threadpool_destroy(&threadpool_p);
        threadpool_p = NULL;

        free(server_p);
        server_p = NULL;
    }

    return server_p;
}

int start_tcp_server(char *port_p,
                     size_t max_connections,
                     client_request_handler_t client_request_handler,
                     client_data_free_func_t client_data_free_func,
                     void *user_data_p)
{
    int exit_code = E_FAILURE;
    server_t *server_p = NULL;
    client_data_t *client_data_p = NULL;
    tcp_server_job_t *new_job_p = NULL;

    if ((NULL == port_p) || (NULL == client_request_handler) ||
        (NULL == user_data_p))
    {
        print_error("start_server(): NULL argument passed.");
        goto END;
    }

    if (2 > max_connections)
    {
        print_error("start_server(): Max connections must be 2 or more.");
        goto END;
    }

    server_p = init_server(
        port_p, max_connections, client_request_handler, client_data_free_func);
    if (NULL == server_p)
    {
        print_error("start_server(): Failed to initialize server.");
        goto END;
    }

    printf("Waiting for client connections...\n");

    // Main loop to handle incoming client connections
    for (;;)
    {
        if ((signal_flag_g == SIGINT) || (signal_flag_g) == SIGUSR1)
        {
            printf("\nShutdown signal received.\n");
            goto SHUTDOWN;
        }

        exit_code = accept_new_connection(server_p->settings_p);
        if (E_SUCCESS != exit_code)
        {
            print_error("start_server(): Unable to accept new connection.");
            goto SHUTDOWN;
        }

        client_data_p = calloc(1, sizeof(client_data_t));
        if (NULL == client_data_p)
        {
            print_error("start_server(): client_data_p - CMR failure.");
            goto SHUTDOWN;
        }

        // Set up client data
        client_data_p->client_fd = server_p->settings_p->client_fd;
        client_data_p->user_data_p = user_data_p;

        new_job_p = calloc(1, sizeof(tcp_server_job_t));
        if (NULL == new_job_p)
        {
            print_error("start_server(): new_job_p - CMR failure.");
            free(client_data_p);
            client_data_p = NULL;
            goto SHUTDOWN;
        }

        // Initialize the new job
        new_job_p->client_function = client_request_handler;
        new_job_p->free_function = client_data_free_func;
        new_job_p->args_p = client_data_p;

        // Add new job to the thread pool
        exit_code = threadpool_add_job(
            server_p->threadpool_p, handle_client_request, NULL, new_job_p);
        if (E_SUCCESS != exit_code)
        {
            print_error("start_server(): Unable to add job to threadpool.");
            goto SHUTDOWN;
        }

        // Print client address for logging
        print_client_address(server_p->settings_p);
    }

SHUTDOWN:
    exit_code = destroy_server(&server_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_server(): Failed to destroy server.");
        goto END;
    }

END:
    return exit_code;
}

static int destroy_server(server_t **server_p)
{
    int exit_code = E_FAILURE;
    if ((NULL == server_p) || (NULL == (*server_p)))
    {
        print_error("destroy_server(): NULL argument passed.");
        goto END;
    }

    exit_code = threadpool_destroy(&(*server_p)->threadpool_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("destroy_server(): Unable to destroy threadpool.");
    }

    close((*server_p)->listening_socket);

    free((*server_p)->settings_p);
    (*server_p)->settings_p = NULL;

    free(*server_p);
    *server_p = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int accept_new_connection(config_t *config_p)
{
    int exit_code = E_FAILURE;
    config_p->client_len = sizeof(config_p->client_address);

    // Attempt to accept a new client connection using the listening socket.
    // If the accept() function returns an invalid client fd, check for shutdown
    // signals (SIGINT or SIGUSR1).
    errno = 0;
    config_p->client_fd = accept(config_p->listening_socket,
                                 (struct sockaddr *)&config_p->client_address,
                                 &config_p->client_len);
    if (INVALID_SOCKET >= config_p->client_fd)
    {
        if ((signal_flag_g == SIGINT) || (signal_flag_g == SIGUSR1))
        {
            printf("\nShutdown signal received.\n");
            exit_code = SIG_SHUTDOWN;
            goto END;
        }

        perror("accept_new_connection(): accept() failed.");
        exit_code = E_FAILURE;
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int configure_server_address(config_t *config_p, char *port_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == config_p) || (NULL == port_p))
    {
        print_error("configure_server_address(): NULL argument passed.");
        goto END;
    }

    // Setup a TCP server address using IPV4
    config_p->hints = (struct addrinfo){.ai_family = AF_INET,       // IPV4
                                        .ai_socktype = SOCK_STREAM, // TCP
                                        .ai_flags = AI_PASSIVE,     // For binding
                                        .ai_protocol = 0,
                                        .ai_canonname = NULL,
                                        .ai_addr = NULL,
                                        .ai_next = NULL};

    // Get a list of address structures and store them in 'config->address_list'
    exit_code =
        getaddrinfo(NULL, port_p, &config_p->hints, &config_p->address_list);
    if (E_SUCCESS != exit_code)
    {
        /* [ACTION]: Suppressing fprintf() error.
         * [JUSTIFICATION]: Result inconsequential to any further operations.
         */
        // NOLINTNEXTLINE
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(exit_code));
        goto END;
    }

    // Try each address, attempting to create a socket and bind
    for (config_p->current_address = config_p->address_list;
         config_p->current_address != NULL;
         config_p->current_address = config_p->current_address->ai_next)
    {
        exit_code = create_listening_socket(config_p);
        if (E_SUCCESS != exit_code)
        {
            /**
             * NOTE: Barr-C generally discourages the use of the `continue`
             * keyword, but I think it makes sense in this case. Using flags and
             * additional conditional statements in order to factor out the
             * `continue` keyword could diminish readability.
             */
            continue;
        }

        errno = 0;
        exit_code = bind(config_p->listening_socket,
                         config_p->current_address->ai_addr,
                         config_p->current_address->ai_addrlen);
        if (E_SUCCESS == exit_code)
        {
            break; // Successfully bound socket
        }

        // Close the socket if unsuccessful
        close(config_p->listening_socket);
        perror("configure_server_address(): bind() failed.");
    }

    if (NULL == config_p->current_address)
    {
        // None of the addresses succeeded
        exit_code = E_FAILURE;
    }
    else
    {
        exit_code = E_SUCCESS;
    }

END:
    freeaddrinfo(config_p->address_list);
    return exit_code;
}

static int create_listening_socket(config_t *config_p)
{
    int exit_code = E_FAILURE;
    int sock_opt_check = 0; // To check the return value of setsockopt
    int optval = 1;         // Enable SO_REUSEADDR

    if (NULL == config_p)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Create a new socket
    errno = 0;
    config_p->listening_socket = socket(config_p->current_address->ai_family,
                                        config_p->current_address->ai_socktype,
                                        config_p->current_address->ai_protocol);
    if (INVALID_SOCKET >= config_p->listening_socket)
    {
        perror("create_listening_socket(): socket() failed.");
        exit_code = E_FAILURE;
        goto END;
    }

    // Enable the SO_REUSEADDR option to reuse the local address
    sock_opt_check = setsockopt(config_p->listening_socket,
                                SOL_SOCKET,
                                SO_REUSEADDR,
                                &optval,
                                sizeof(optval));
    if (0 > sock_opt_check)
    {
        perror("create_listening_socket(): setsockopt() failed.");
        exit_code = E_FAILURE;
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void print_client_address(config_t *config_p)
{
    char address_buffer[MAX_CLIENT_ADDRESS_SIZE];

    // Convert client address to a human-readable form
    getnameinfo((struct sockaddr *)&config_p->client_address,
                config_p->client_len,
                address_buffer,
                sizeof(address_buffer),
                0,
                0,
                NI_NUMERICHOST); // Use numeric host address

    // Print the client's IP address
    printf("Connection established - [%s]\n", address_buffer);
}

static void *handle_client_request(void *args_p)
{
    tcp_server_job_t *job_p = NULL;
    client_data_t *data_p = NULL;
    void *result_p = NULL;

    if (NULL == args_p)
    {
        print_error("handle_client_request(): NULL job passed.");
        goto END;
    }

    job_p = (tcp_server_job_t *)args_p;

    // Execute the custom function
    result_p = job_p->client_function(job_p->args_p);

    // Free any resources if required
    if (NULL != job_p->free_function)
    {
        job_p->free_function(job_p->args_p);
    }

    data_p = (client_data_t *)job_p->args_p;
    close(data_p->client_fd);

    free(data_p);
    data_p = NULL;
    free(job_p);
    job_p = NULL;

END:
    return result_p;
}

/*** end of file ***/
