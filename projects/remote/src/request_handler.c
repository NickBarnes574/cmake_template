#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "job_handler.h"
#include "lights.h"
#include "opcodes.h"
#include "packets.h"
#include "socket_io.h"
#include "utilities.h"

int process_job(int client_fd)
{
    int     exit_code = E_FAILURE;
    uint8_t opcode    = -1;

    printf("DEBUG 2: INSIDE PROCESS JOB\n");

    exit_code = recv_all_data(client_fd, &opcode, sizeof(uint8_t));
    if (E_SUCCESS != exit_code)
    {
        goto END;
    }

    switch (opcode)
    {
        case GREEN_ON:
            exit_code = light_green_on();
            if (E_SUCCESS != exit_code)
            {
                print_error("process_job(): failed to turn green light on.");
                goto END;
            }
            break;

        case GREEN_OFF:
            exit_code = light_green_off();
            if (E_SUCCESS != exit_code)
            {
                print_error("process_job(): failed to turn green light off.");
                goto END;
            }
            break;

        case RED_ON:
            exit_code = light_red_on();
            if (E_SUCCESS != exit_code)
            {
                print_error("process_job(): failed to turn red light on.");
                goto END;
            }
            break;

        case RED_OFF:
            exit_code = light_red_off();
            if (E_SUCCESS != exit_code)
            {
                print_error("process_job(): failed to turn red light off.");
                goto END;
            }
            break;

        case YELLOW_ON:
            exit_code = light_yellow_on();
            if (E_SUCCESS != exit_code)
            {
                print_error("process_job(): failed to turn yellow light on.");
                goto END;
            }
            break;

        case YELLOW_OFF:
            exit_code = light_yellow_off();
            if (E_SUCCESS != exit_code)
            {
                print_error("process_job(): failed to turn yellow light off.");
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
