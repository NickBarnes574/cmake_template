#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "job_handler.h"
#include "opcodes.h"
#include "packets.h"
#include "socket_io.h"
#include "utilities.h"

int message(int client_fd);

int process_job(int client_fd)
{
    int     exit_code = E_FAILURE;
    uint8_t opcode    = -1;

    exit_code = recv_all_data(client_fd, &opcode, sizeof(uint8_t));
    if (E_SUCCESS != exit_code)
    {
        goto END;
    }

    switch (opcode)
    {
        case MESSAGE:
            exit_code = message(client_fd);
            if (E_SUCCESS != exit_code)
            {
                print_error("process_job(): Unable to process message.");
                goto END;
            }
            break;

        case CLOSE_CONNECTION:
            exit_code = E_CONNECTION_CLOSED;
            goto END;
            break;

        default:
            print_error("process_job(): Invalid opcode.");
            goto END;
            break;
    }

END:
    return exit_code;
}

int message(int client_fd)
{
    int           exit_code   = E_FAILURE;
    uint32_t      message_len = 0;
    unsigned char padding[3]  = { 0 };
    char *        message     = NULL;

    // Read padding
    exit_code = recv_all_data(client_fd, padding, sizeof(padding));
    if (E_SUCCESS != exit_code)
    {
        print_error("message(): Unable to get padding.");
        goto END;
    }

    // Read message length
    exit_code = recv_all_data(client_fd, &message_len, sizeof(message_len));
    if (E_SUCCESS != exit_code)
    {
        print_error("message(): Unable to get message length.");
        goto END;
    }

    message_len = ntohl(message_len);

    // Verify the message length to ensure it is within a reasonable range
    if (message_len >
        1024) // Assuming 1024 is the maximum reasonable length for a message
    {
        print_error("message(): Message length is unreasonably large.");
        goto END;
    }

    // Allocate memory for the message
    message = calloc(1, message_len + 1);
    if (NULL == message)
    {
        exit_code = E_FAILURE;
        print_error("message(): Memory allocation failure.");
        goto END;
    }

    exit_code = recv_all_data(client_fd, message, message_len);
    if (E_SUCCESS != exit_code)
    {
        print_error("message(): Unable to get message.");
        goto END;
    }

    if (MAX_MSG_SIZE > message_len)
    {
        message_log("INFO",
                    COLOR_YELLOW,
                    LOG_BOTH,
                    "MESSAGE from [%d]: %s",
                    client_fd,
                    message);
    }
    else
    {
        message_log(
            "INFO", COLOR_YELLOW, LOG_BOTH, "message is too long to display");
    }

    exit_code = E_SUCCESS;

END:
    free(message);
    message = NULL;
    return exit_code;
}