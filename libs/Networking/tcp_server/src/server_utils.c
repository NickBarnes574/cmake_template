#include <netdb.h>

#include "server_utils.h"

void * get_in_addr(struct sockaddr * sa_p)
{
    void * address = NULL;

    if (sa_p == NULL)
    {
        goto END;
    }

    if (sa_p->sa_family == AF_INET)
    {
        struct sockaddr_in * ipv4_addr = (struct sockaddr_in *)sa_p;
        address                        = &(ipv4_addr->sin_addr);
    }
    else
    {
        struct sockaddr_in6 * ipv6_addr = (struct sockaddr_in6 *)sa_p;
        address                         = &(ipv6_addr->sin6_addr);
    }

END:
    return address;
}

/*** end of file ***/
