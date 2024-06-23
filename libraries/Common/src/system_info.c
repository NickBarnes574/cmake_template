#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "system_info.h"

#define BUFFER_SIZE 1024

char * get_hostname()
{
    char   hostname[BUFFER_SIZE];
    char * result = (char *)malloc(BUFFER_SIZE);
    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        snprintf(result, BUFFER_SIZE, "%s", hostname);
    }
    else
    {
        perror("gethostname");
        free(result);
        return NULL;
    }
    return result;
}

char * get_operating_system()
{
    FILE * os_release = fopen("/etc/os-release", "r");
    char   buffer[BUFFER_SIZE];
    char * result = (char *)malloc(BUFFER_SIZE);
    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    if (os_release != NULL)
    {
        while (fgets(buffer, sizeof(buffer), os_release) != NULL)
        {
            if (strncmp(buffer, "PRETTY_NAME=", 12) == 0)
            {
                char * name = strchr(buffer, '"');
                if (name != NULL)
                {
                    name++;
                    char * end = strchr(name, '"');
                    if (end != NULL)
                    {
                        *end = '\0';
                        snprintf(result, BUFFER_SIZE, "%s", name);
                        fclose(os_release);
                        return result;
                    }
                }
            }
        }
        fclose(os_release);
    }

    perror("fopen");
    free(result);
    return NULL;
}

char * get_cpu_info()
{
    FILE * cpuinfo = fopen("/proc/cpuinfo", "r");
    char   buffer[BUFFER_SIZE];
    char * result = (char *)malloc(BUFFER_SIZE);
    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    if (cpuinfo != NULL)
    {
        while (fgets(buffer, sizeof(buffer), cpuinfo) != NULL)
        {
            if (strncmp(buffer, "model name", 10) == 0)
            {
                // Remove newline character from the buffer if present
                char * newline = strchr(buffer, '\n');
                if (newline)
                {
                    *newline = '\0';
                }

                snprintf(result, BUFFER_SIZE, "%s", strchr(buffer, ':') + 2);
                fclose(cpuinfo);
                return result;
            }
        }
        fclose(cpuinfo);
    }

    perror("fopen");
    free(result);
    return NULL;
}

char * get_cpu_architecture()
{
    struct utsname buffer;
    char *         result = (char *)malloc(BUFFER_SIZE);
    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    if (uname(&buffer) == 0)
    {
        snprintf(result, BUFFER_SIZE, "%s", buffer.machine);
    }
    else
    {
        perror("uname");
        free(result);
        return NULL;
    }
    return result;
}

char * get_memory_info()
{
    FILE * meminfo = fopen("/proc/meminfo", "r");
    char   buffer[BUFFER_SIZE];
    char * result = (char *)malloc(BUFFER_SIZE);
    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    if (meminfo != NULL)
    {
        while (fgets(buffer, sizeof(buffer), meminfo) != NULL)
        {
            if (strncmp(buffer, "MemTotal", 8) == 0)
            {
                char * mem_kb_str = strchr(buffer, ':') + 1;
                long   mem_kb     = strtol(mem_kb_str, NULL, 10);
                long   mem_mb     = mem_kb / 1024;
                snprintf(result, BUFFER_SIZE, "%ld MB", mem_mb);
                fclose(meminfo);
                return result;
            }
        }
        fclose(meminfo);
    }

    perror("fopen");
    free(result);
    return NULL;
}

char * get_network_interfaces()
{
    FILE * netdev = fopen("/proc/net/dev", "r");
    char   buffer[BUFFER_SIZE];
    char * result = (char *)malloc(BUFFER_SIZE);
    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    if (netdev != NULL)
    {
        // Skip the first two lines of headers
        fgets(buffer, sizeof(buffer), netdev);
        fgets(buffer, sizeof(buffer), netdev);

        snprintf(result, BUFFER_SIZE, "Network Interfaces:\n");
        char * result_ptr = result + strlen(result);

        while (fgets(buffer, sizeof(buffer), netdev) != NULL)
        {
            char * interface = strtok(buffer, " :");
            result_ptr += snprintf(result_ptr,
                                   BUFFER_SIZE - (result_ptr - result),
                                   "  %s\n",
                                   interface);
        }
        fclose(netdev);
    }
    else
    {
        perror("fopen");
        free(result);
        return NULL;
    }
    return result;
}