/**
 * @file session_manager.h
 *
 * @brief Handles ephemeral key generation, key exchange, and management of
 * per-client cryptographic session state including public keys, shared secrets,
 * and nonces.
 */
#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#include "config.h"

typedef struct session_ctx
{
    uint8_t client_pubkey[SESSION_KEY_BYTES];
    uint8_t server_pubkey[SESSION_KEY_BYTES];
    uint8_t server_seckey[SESSION_KEY_BYTES];
    uint8_t shared_key[SESSION_KEY_BYTES];
    uint8_t nonce[NONCE_BYTES];
    int     authenticated;
} session_ctx_t;

int  establish_session(int client_fd, session_ctx_t * session_p);
void clear_session(session_ctx_t * session_p);

#endif /* _SESSION_MANAGER_H */

/*** end of file ***/
