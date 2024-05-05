#include <stdio.h>
#include <stdlib.h>

#include "tcp_server.h"
#include "utilities.h"

int main(void)
{
    int exit_code = E_FAILURE;

    exit_code = start_tcp_server("17337");
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): start_tcp_server() failed.");
        goto END;
    }

END:
    return exit_code;
}
