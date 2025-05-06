#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encryption.h"
#include "utilities.h"

unsigned char challenge[CHALLENGE_SIZE];
unsigned char response[RESPONSE_SIZE];
unsigned char public_key[PUBLIC_KEY_SIZE];
unsigned char secret_key[SECRET_KEY_SIZE];
unsigned char session_key[SESSION_KEY_SIZE];

void setup(void)
{
    memset(challenge, 0, CHALLENGE_SIZE);
    memset(response, 0, RESPONSE_SIZE);
    memset(public_key, 0, PUBLIC_KEY_SIZE);
    memset(secret_key, 0, SECRET_KEY_SIZE);
    memset(session_key, 0, SESSION_KEY_SIZE);
}

void teardown(void)
{
    // No heap allocations in encryption, so no explicit cleanup needed.
}

void test_encryption_generate_challenge_null_argument(void)
{
    int exit_code = encryption_generate_challenge(NULL);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);
}

void test_encryption_generate_challenge_valid(void)
{
    int exit_code = encryption_generate_challenge(challenge);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_encryption_hash_challenge_null_arguments(void)
{
    int exit_code = encryption_hash_challenge(NULL, response);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    exit_code = encryption_hash_challenge(challenge, NULL);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);
}

void test_encryption_hash_challenge_valid(void)
{
    encryption_generate_challenge(challenge);
    int exit_code = encryption_hash_challenge(challenge, response);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_encryption_generate_key_pair_null_arguments(void)
{
    int exit_code = encryption_generate_key_pair(NULL, secret_key);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    exit_code = encryption_generate_key_pair(public_key, NULL);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);
}

void test_encryption_generate_key_pair_valid(void)
{
    int exit_code = encryption_generate_key_pair(public_key, secret_key);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_encryption_derive_session_key_null_arguments(void)
{
    int exit_code =
        encryption_derive_session_key(NULL, secret_key, session_key);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    exit_code = encryption_derive_session_key(public_key, NULL, session_key);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    exit_code = encryption_derive_session_key(public_key, secret_key, NULL);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);
}

void test_encryption_derive_session_key_valid(void)
{
    encryption_generate_key_pair(public_key, secret_key);
    int exit_code =
        encryption_derive_session_key(public_key, secret_key, session_key);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_encryption_encrypt_chunk_null_arguments(void)
{
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    unsigned char cipher_text[64];
    unsigned char plain_text[64] = "Hello, World!";

    int exit_code = encryption_encrypt_chunk(
        NULL, sizeof(plain_text), nonce, session_key, cipher_text);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    exit_code = encryption_encrypt_chunk(
        plain_text, sizeof(plain_text), NULL, session_key, cipher_text);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    exit_code = encryption_encrypt_chunk(
        plain_text, sizeof(plain_text), nonce, NULL, cipher_text);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);

    exit_code = encryption_encrypt_chunk(
        plain_text, sizeof(plain_text), nonce, session_key, NULL);
    CU_ASSERT_EQUAL(exit_code, E_FAILURE);
}

void test_encryption_encrypt_decrypt_valid(void)
{
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    unsigned char cipher_text[64];
    unsigned char decrypted_text[64];
    unsigned char plain_text[64] = "Hello, World!";

    memset(nonce, 0, sizeof(nonce));

    int exit_code = encryption_encrypt_chunk(
        plain_text, sizeof(plain_text), nonce, session_key, cipher_text);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = encryption_decrypt_chunk(
        cipher_text, sizeof(plain_text), nonce, session_key, decrypted_text);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_NSTRING_EQUAL(plain_text, decrypted_text, sizeof(plain_text));
}

static CU_TestInfo encryption_tests[] = {
    { "generate_challenge_null_argument",
      test_encryption_generate_challenge_null_argument },
    { "generate_challenge_valid", test_encryption_generate_challenge_valid },
    { "hash_challenge_null_arguments",
      test_encryption_hash_challenge_null_arguments },
    { "hash_challenge_valid", test_encryption_hash_challenge_valid },
    { "generate_key_pair_null_arguments",
      test_encryption_generate_key_pair_null_arguments },
    { "generate_key_pair_valid", test_encryption_generate_key_pair_valid },
    { "derive_session_key_null_arguments",
      test_encryption_derive_session_key_null_arguments },
    { "derive_session_key_valid", test_encryption_derive_session_key_valid },
    { "encrypt_chunk_null_arguments",
      test_encryption_encrypt_chunk_null_arguments },
    { "encrypt_decrypt_valid", test_encryption_encrypt_decrypt_valid },
    CU_TEST_INFO_NULL
};

CU_SuiteInfo encryption_test_suite = {
    "encryption Tests",
    NULL,            // Suite initialization function
    NULL,            // Suite cleanup function
    setup,           // Suite setup function
    teardown,        // Suite teardown function
    encryption_tests // The array of test cases
};

/*** end of file ***/
