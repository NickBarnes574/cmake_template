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
int process_job(int client_fd);

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

    exit_code = process_job(job_args->client_fd);
    if (E_SUCCESS != exit_code)
    {
        if (E_CONNECTION_CLOSED == exit_code)
        {
            close(job_args->client_fd);
            goto END;
        }
        print_error("process_client_request(): Unable to process job.");
    }

    // Determine whether or not to add back to fd array and/or close client
    exit_code = sock_fd_add(job_args->sock_mgr, job_args->client_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("free_job_args(): Unable to add fd to array.");
    }

    // print_fd_array(job_args->sock_mgr);

END:
    return NULL;
}

int process_job(int client_fd)
{
    int     exit_code = E_FAILURE;
    uint8_t opcode    = -1;

    fprintf(stderr, "GETTING OPCODE [%d]\n", client_fd);
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
            printf("CLOSING CONNECTION\n");
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
    // fprintf(stderr, "GETTING PADDING [%d]\n", client_fd);
    exit_code = recv_all_data(client_fd, padding, sizeof(padding));
    if (E_SUCCESS != exit_code)
    {
        print_error("message(): Unable to get padding.");
        goto END;
    }
    // printf("Padding: [0x%02x 0x%02x 0x%02x]\n",
    //        padding[0],
    //        padding[1],
    //        padding[2]);

    // Read message length
    // fprintf(stderr, "GETTING MESSAGE LENGTH [%d]\n", client_fd);
    exit_code = recv_all_data(client_fd, &message_len, sizeof(message_len));
    if (E_SUCCESS != exit_code)
    {
        print_error("message(): Unable to get message length.");
        goto END;
    }

    // printf("Message length before ntohl: %u\n", message_len);

    // Print raw bytes received for message length
    // unsigned char * raw_bytes = (unsigned char *)&message_len;
    // printf("Raw message length bytes: [0x%02x 0x%02x 0x%02x 0x%02x]\n",
    //        raw_bytes[0],
    //        raw_bytes[1],
    //        raw_bytes[2],
    //        raw_bytes[3]);

    message_len = ntohl(message_len);
    // printf("Message length after ntohl: %u\n", message_len);

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

    // printf("Receiving message of length [%u]\n", message_len);
    // fprintf(stderr, "GETTING MESSAGE [%d]\n", client_fd);
    exit_code = recv_all_data(client_fd, message, message_len);
    if (E_SUCCESS != exit_code)
    {
        print_error("message(): Unable to get message.");
        goto END;
    }

    // printf("[MESSAGE RECEIVED]: %s\n", message);

    exit_code = E_SUCCESS;

END:
    free(message);
    message = NULL;
    return exit_code;
}