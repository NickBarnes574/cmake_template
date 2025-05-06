/**
 * @file encryption.h
 *
 * @brief
 */
#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

#include "tweetnacl.h"
#include <stdlib.h> // size_t

#define CHALLENGE_SIZE   32
#define RESPONSE_SIZE    crypto_hash_BYTES
#define PUBLIC_KEY_SIZE  crypto_box_PUBLICKEYBYTES
#define SECRET_KEY_SIZE  crypto_box_SECRETKEYBYTES
#define SESSION_KEY_SIZE crypto_secretbox_KEYBYTES

int encryption_generate_challenge(unsigned char challenge[CHALLENGE_SIZE]);
int encryption_hash_challenge(const unsigned char * challenge,
                              unsigned char         response[RESPONSE_SIZE]);
int encryption_generate_key_pair(unsigned char public_key[PUBLIC_KEY_SIZE],
                                 unsigned char secret_key[SECRET_KEY_SIZE]);
int encryption_derive_session_key(unsigned char * client_public,
                                  unsigned char * server_private,
                                  unsigned char * session_key);
int encryption_encrypt_chunk(const unsigned char * plain_text,
                             size_t                length,
                             const unsigned char * nonce,
                             const unsigned char * key,
                             unsigned char *       cipher_text);
int encryption_decrypt_chunk(const unsigned char * cipher_text,
                             size_t                length,
                             const unsigned char * nonce,
                             const unsigned char * key,
                             unsigned char *       plain_text);
#endif /* _ENCRYPTION_H */

/*** end of file ***/
