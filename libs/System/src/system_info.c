// NOLINTNEXTLINE
#define _GNU_SOURCE // gethostname

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "file_io.h"
#include "system_info.h"
#include "threadsafe_io.h"
#include "utilities.h"

#define BUFFER_SIZE     1020
#define PRETTY_NAME_LEN 12
#define MODEL_NAME_LEN  10
#define BASE_TEN        10
#define KILOBYTE        1024

static char * get_os_name(char * buffer);
static char * parse_cpu_model(char * buffer);
static char * parse_memory_info(char * buffer);

char * get_hostname()
{
    int    exit_code             = E_FAILURE;
    char   hostname[BUFFER_SIZE] = { 0 };
    char * result                = calloc(1, BUFFER_SIZE);
    if (NULL == result)
    {
        PRINT_DEBUG("get_hostname(): CMR failure - result");
        result = NULL;
        goto END;
    }

    if (0 == gethostname(hostname, sizeof(hostname)))
    {
        exit_code = ts_snprintf(result, BUFFER_SIZE, "%s", hostname);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("get_hostname(): snprintf() error.");
            free(result);
            result = NULL;
            goto END;
        }
    }
    else
    {
        perror("gethostname");
        free(result);
        result = NULL;
    }

END:
    return result;
}

char * get_operating_system()
{
    int    exit_code           = E_FAILURE;
    FILE * os_release          = NULL;
    char * result              = NULL;
    char * os_name             = NULL;
    char   buffer[BUFFER_SIZE] = { 0 };

    os_release = fopen("/etc/os-release", "re");
    if (NULL == os_release)
    {
        PRINT_DEBUG("get_operating_system(): Unable to open file.");
        goto END;
    }

    while (NULL != fgets(buffer, sizeof(buffer), os_release))
    {
        os_name = get_os_name(buffer);
        if (NULL != os_name) // Successfully parsed the os name
        {
            result = os_name;
            break;
        }
    }

    exit_code = safe_fclose(os_release);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("get_operating_system(): Error closing file.");
        goto END;
    }

END:
    return result;
}

char * get_cpu_info()
{
    int    exit_code           = E_FAILURE;
    FILE * cpuinfo             = NULL;
    char * result              = NULL;
    char   buffer[BUFFER_SIZE] = { 0 };

    cpuinfo = fopen("/proc/cpuinfo", "re");
    if (NULL == cpuinfo)
    {
        PRINT_DEBUG("get_cpu_info(): Unable to open file.");
        goto END;
    }

    while (NULL != fgets(buffer, sizeof(buffer), cpuinfo))
    {
        result = parse_cpu_model(buffer);
        if (NULL != result) // Successfully parsed the cpu model
        {
            break;
        }
    }

    exit_code = safe_fclose(cpuinfo);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("get_cpu_info(): Error closing file.");
        goto END;
    }

END:
    return result;
}

char * get_cpu_architecture()
{
    int            exit_code = E_FAILURE;
    char *         result    = NULL;
    struct utsname buffer;

    if (0 != uname(&buffer))
    {
        perror("uname");
        goto END;
    }

    result = calloc(1, BUFFER_SIZE);
    if (NULL == result)
    {
        PRINT_DEBUG("get_cpu_architecture(): CMR failure - result.");
        goto END;
    }

    exit_code = ts_snprintf(result, BUFFER_SIZE, "%s", buffer.machine);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("get_cpu_architecture(): snprintf() error.");
        free(result);
        result = NULL;
        goto END;
    }

END:
    return result;
}

char * get_memory_info()
{
    int    exit_code           = E_FAILURE;
    FILE * meminfo             = NULL;
    char * result              = NULL;
    char   buffer[BUFFER_SIZE] = { 0 };

    meminfo = fopen("/proc/meminfo", "re");
    if (NULL == meminfo)
    {
        PRINT_DEBUG("get_memory_info(): Unable to open file.");
        goto END;
    }

    while (NULL != fgets(buffer, sizeof(buffer), meminfo))
    {
        result = parse_memory_info(buffer);
        if (NULL != result) // Successfully parsed memory info
        {
            break;
        }
    }

    exit_code = safe_fclose(meminfo);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("get_memory_info(): Error closing file.");
        goto END;
    }

END:
    return result;
}

char * get_network_interfaces()
{
    int    exit_code           = E_FAILURE;
    FILE * netdev              = NULL;
    char * result              = NULL;
    char * fgets_check         = NULL;
    char * result_ptr          = NULL;
    char * interface           = NULL;
    char   buffer[BUFFER_SIZE] = { 0 };

    netdev = fopen("/proc/net/dev", "re");
    if (NULL == netdev)
    {
        PRINT_DEBUG("get_network_interfaces(): Unable to open file.");
        goto END;
    }

    result = calloc(1, BUFFER_SIZE);
    if (NULL == result)
    {
        PRINT_DEBUG("get_network_interfaces(): Memory allocation failure.");
        goto END;
    }

    // Skip the first two lines of headers
    fgets_check = safe_fgets(buffer, sizeof(buffer), netdev);
    if (NULL == fgets_check)
    {
        PRINT_DEBUG(
            "get_network_interfaces(): Error reading first header line.");
        free(result);
        result = NULL;
        goto END;
    }

    fgets_check = safe_fgets(buffer, sizeof(buffer), netdev);
    if (NULL == fgets_check)
    {
        PRINT_DEBUG(
            "get_network_interfaces(): Error reading second header line.");
        free(result);
        result = NULL;
        goto END;
    }

    exit_code = ts_snprintf(result, BUFFER_SIZE, "Network Interfaces:\n");
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("get_network_interfaces(): snprintf() error.");
        free(result);
        result = NULL;
        goto END;
    }

    result_ptr = result + strlen(result);
    while (NULL != safe_fgets(buffer, sizeof(buffer), netdev))
    {
        interface = strtok(buffer, " :");
        result_ptr += snprintf(result_ptr,
                               BUFFER_SIZE - (result_ptr - result),
                               "  %s\n",
                               interface);
    }

    exit_code = safe_fclose(netdev);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("get_network_interfaces(): Error closing file.");
        free(result);
        result = NULL;
    }

END:
    return result;
}

static char * get_os_name(char * buffer)
{
    int    exit_code = E_FAILURE;
    char * result    = NULL;
    char * name      = NULL;
    char * end       = NULL;

    if (NULL == buffer)
    {
        PRINT_DEBUG("get_os_name(): NULL argument passed.");
        goto END;
    }

    result = calloc(1, BUFFER_SIZE);
    if (result == NULL)
    {
        PRINT_DEBUG("get_operating_system(): CMR failure - result.");
        goto END;
    }

    if (0 == strncmp(buffer, "PRETTY_NAME=", PRETTY_NAME_LEN))
    {
        name = strchr(buffer, '"');
        if (name != NULL)
        {
            name++;
            end = strchr(name, '"');
            if (end != NULL)
            {
                *end      = '\0';
                exit_code = ts_snprintf(result, BUFFER_SIZE, "%s", name);
                if (E_SUCCESS != exit_code)
                {
                    PRINT_DEBUG("get_os_name(): snprintf() error.");
                    free(result);
                    result = NULL;
                }
                goto END;
            }
        }
    }

END:
    return result;
}

static char * parse_cpu_model(char * buffer)
{
    int    exit_code = E_FAILURE;
    char * result    = NULL;
    char * newline   = NULL;

    if (NULL == buffer)
    {
        PRINT_DEBUG("parse_cpu_model(): NULL argument passed.");
        goto END;
    }

    if (0 == strncmp(buffer, "model name", MODEL_NAME_LEN))
    {
        result = calloc(1, BUFFER_SIZE);
        if (NULL == result)
        {
            PRINT_DEBUG("parse_cpu_model(): Memory allocation failure.");
            goto END;
        }

        // Remove newline character from buffer if present
        newline = strchr(buffer, '\n');
        if (NULL != newline)
        {
            *newline = '\0';
        }

        // (void)snprintf(result, BUFFER_SIZE, "%s", strchr(buffer, ':') + 2);
        exit_code =
            ts_snprintf(result, BUFFER_SIZE, "%s", strchr(buffer, ':') + 2);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("parse_cpu_model(): snprintf() error.");
            goto END;
        }
    }

END:
    return result;
}

static char * parse_memory_info(char * buffer)
{
    int    exit_code  = E_FAILURE;
    char * result     = NULL;
    char * mem_kb_str = NULL;
    long   mem_kb     = 0;
    long   mem_mb     = 0;

    if (NULL == buffer)
    {
        PRINT_DEBUG("parse_memory_info(): NULL argument passed.");
        goto END;
    }

    if (0 == strncmp(buffer, "MemTotal", 8))
    {
        result = calloc(1, BUFFER_SIZE);
        if (NULL == result)
        {
            PRINT_DEBUG("parse_memory_info(): Memory allocation failure.");
            goto END;
        }

        mem_kb_str = strchr(buffer, ':') + 1;
        mem_kb     = strtol(mem_kb_str, NULL, BASE_TEN);
        mem_mb     = mem_kb / KILOBYTE;
        exit_code  = ts_snprintf(result, BUFFER_SIZE, "%ld MB", mem_mb);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("parse_memory_info(): snprintf() error.");
            free(result);
            result = NULL;
            goto END;
        }
    }

END:
    return result;
}
