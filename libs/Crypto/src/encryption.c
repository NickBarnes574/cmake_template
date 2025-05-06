#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "encryption.h"
#include "utilities.h"

int encryption_generate_challenge(unsigned char challenge[CHALLENGE_SIZE])
{
    int     exit_code  = E_FAILURE;
    int     file_fd    = -1;
    ssize_t bytes_read = -1;

    if (NULL == challenge)
    {
        PRINT_DEBUG("encryption_generate_challenge(): NULL argument passed.");
        goto END;
    }

    file_fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if (0 > file_fd)
    {
        PRINT_DEBUG(
            "encryption_generate_challenge(): Failed to open /dev/urandom.");
        goto END;
    }

    bytes_read = read(file_fd, challenge, CHALLENGE_SIZE);
    if (CHALLENGE_SIZE != bytes_read)
    {
        PRINT_DEBUG(
            "encryption_generate_challenge(): Failed to read from "
            "/dev/urandom.");
        close(file_fd);
        goto END;
    }

    close(file_fd);
    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int encryption_hash_challenge(const unsigned char * challenge,
                              unsigned char         response[RESPONSE_SIZE])
{
    int exit_code = E_FAILURE;

    if ((NULL == challenge) || (NULL == response))
    {
        PRINT_DEBUG("encryption_generate_challenge(): NULL argument passed.");
        goto END;
    }

    crypto_hash(response, challenge, CHALLENGE_SIZE);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}
int encryption_generate_key_pair(unsigned char public_key[PUBLIC_KEY_SIZE],
                                 unsigned char secret_key[SECRET_KEY_SIZE])
{
    int exit_code = E_FAILURE;

    if ((NULL == public_key) || (NULL == secret_key))
    {
        PRINT_DEBUG("encryption_generate_keypair(): NULL argument passed.");
        goto END;
    }

    crypto_box_keypair(public_key, secret_key);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}
int encryption_derive_session_key(unsigned char * client_public,
                                  unsigned char * server_private,
                                  unsigned char * session_key)
{
    int           exit_code = E_FAILURE;
    unsigned char shared_secret[crypto_box_BEFORENMBYTES];

    if ((NULL == client_public) || (NULL == server_private) ||
        (NULL == session_key))
    {
        PRINT_DEBUG("encryption_derive_session_key(): NULL argument passed.");
        goto END;
    }

    crypto_box_beforenm(shared_secret, client_public, server_private);
    crypto_hash(session_key, shared_secret, crypto_box_BEFORENMBYTES);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}
int encryption_encrypt_chunk(const unsigned char * plain_text,
                             size_t                length,
                             const unsigned char * nonce,
                             const unsigned char * key,
                             unsigned char *       cipher_text)
{
    int exit_code = E_FAILURE;

    if ((NULL == plain_text) || (NULL == nonce) || (NULL == key) ||
        (NULL == cipher_text))
    {
        PRINT_DEBUG("encryption_encrypt_chunk(): NULL argument passed.");
        goto END;
    }

    crypto_secretbox(cipher_text, plain_text, length, nonce, key);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}
int encryption_decrypt_chunk(const unsigned char * cipher_text,
                             size_t                length,
                             const unsigned char * nonce,
                             const unsigned char * key,
                             unsigned char *       plain_text)
{
    int exit_code = E_FAILURE;

    if ((NULL == cipher_text) || (NULL == nonce) || (NULL == key) ||
        (NULL == plain_text))
    {
        PRINT_DEBUG("encryption_decrypt_chunk(): NULL argument passed.");
        goto END;
    }

    exit_code =
        crypto_secretbox_open(plain_text, cipher_text, length, nonce, key);
    if (0 != exit_code)
    {
        PRINT_DEBUG("encryption_decrypt_chunk(): Decryption failure!");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
