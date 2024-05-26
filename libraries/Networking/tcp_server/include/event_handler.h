/**
 * @file event_handler.h
 *
 * @brief
 */
#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "server_structs.h"

int handle_client_event(server_context_t * server, int index);
int handle_connections(server_context_t * server);
int register_client(server_context_t * server);

#endif /* _EVENT_HANDLER_H */

/*** end of file ***/
