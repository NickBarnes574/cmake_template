/**
 * @file packets.h
 *
 * @brief Contains all the packet definitions that are sent and received by the
 * server
 */
#ifndef _PACKETS_H
#define _PACKETS_H

#include <stdint.h> // uint8_t, uint16_t, uint32_t, size_t
#include <stdio.h>

/**
 * @brief Linked list of session IDs
 *
 */
typedef struct session_id_node
{
    char *                   session_id;
    struct session_id_node * next;
} session_id_node_t;

/**
 * @brief Holds information about individual users
 *
 */
typedef struct user_info
{
    uint16_t            username_len;
    uint16_t            password_len;
    char *              username;
    char *              password;
    uint32_t            privilege;
    session_id_node_t * session_head; // Linked list of SessionIDs
} user_info_t;

//
// -------------------------------AUTHENTICATION-------------------------------
//

// REQUEST

typedef struct __attribute__((__packed__)) authentication_request
{
    uint8_t  opcode;
    uint8_t  user_flag;
    uint32_t session_id;
    uint16_t username_len;
    uint16_t password_len;
    char *   username;
    char *   password;
} authentication_request_t;

typedef struct __attribute__((__packed__)) message_request
{
    uint8_t       opcode;
    uint32_t      session_id;
    unsigned char padding[3];
    uint32_t      message_len;
    char *        message;
} message_request_t;

typedef struct __attribute__((__packed__)) general_request
{
    uint8_t  opcode;
    uint32_t session_id;
} general_request_t;

// RESPONSE

typedef struct __attribute__((__packed__)) authentication_response
{
    uint8_t  return_code;
    uint32_t session_id;
} authentication_response_t;

typedef struct __attribute__((__packed__)) message_response
{
    uint8_t  return_code;
    uint32_t message_len;
    char *   message;
} message_response_t;

typedef struct __attribute__((__packed__)) general_response
{
    uint8_t return_code;
} general_response_t;

#endif /* _PACKETS_H */

/*** end of file ***/
