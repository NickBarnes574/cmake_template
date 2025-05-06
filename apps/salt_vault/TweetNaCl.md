Given the design specification for **Project 3 – Salt Vault**, here’s how I would utilize **TweetNaCl** in the C server to meet the security and functional requirements:

---

## **1. Key Cryptographic Features Using TweetNaCl**

TweetNaCl will be used for:

- **Authenticated encryption** (`crypto_secretbox_easy`) for encrypting file chunks.
- **Authentication** via pre-shared keys or **challenge-response** mechanisms.
- **Session key derivation** for forward secrecy using **Ephemeral Key Exchange** (`crypto_box`).
- **Integrity verification** using the authentication tag (MAC) that comes with `crypto_secretbox_easy`.

---

## **2. Server Implementation Breakdown**

The C server will handle:

1. **Client authentication** (using pre-shared keys or challenge-response).
2. **Session key derivation** (using ephemeral Diffie-Hellman keys with `crypto_box`).
3. **Encrypted file reception** (decrypting chunks using `crypto_secretbox_open_easy`).
4. **Secure file storage** (writing decrypted files to disk).

---

## **3. Implementing Key Components in C**

### **a) Server: Handling Client Authentication**

To ensure only authorized clients can connect, a **pre-shared key (PSK)** or **challenge-response authentication** method will be used.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tweetnacl.h>

#define CHALLENGE_SIZE 32
#define RESPONSE_SIZE crypto_hash_BYTES

void generate_challenge(unsigned char challenge[CHALLENGE_SIZE]) {
    for (int i = 0; i < CHALLENGE_SIZE; i++) {
        challenge[i] = rand() % 256;
    }
}

void hash_challenge(const unsigned char *challenge, unsigned char response[RESPONSE_SIZE]) {
    crypto_hash(response, challenge, CHALLENGE_SIZE);
}

int verify_client_response(const unsigned char *received_response, const unsigned char *expected_response) {
    return memcmp(received_response, expected_response, RESPONSE_SIZE) == 0;
}
```

- The server generates a **random challenge**.
- The client hashes the challenge using `crypto_hash` and sends it back.
- The server **verifies** the response.

---

### **b) Session Key Derivation with Ephemeral Key Exchange**

Using **NaCl’s public-key cryptography (`crypto_box`)**, we generate a **session key** per connection.

```c
void derive_session_key(unsigned char *client_pub, unsigned char *server_priv, unsigned char *session_key) {
    unsigned char shared_secret[crypto_box_BEFORENMBYTES];
    
    // Compute shared secret using client's public key and server's private key
    crypto_box_beforenm(shared_secret, client_pub, server_priv);
    
    // Hash it to derive a symmetric key
    crypto_hash(session_key, shared_secret, crypto_box_BEFORENMBYTES);
}
```

- The client sends its **ephemeral public key**.
- The server computes a **shared secret**.
- The secret is **hashed** to derive a **session key**.

---

### **c) Encrypted File Transfer**

Each file chunk will be:

- **Encrypted** using `crypto_secretbox_easy`.
- **Sent with an authentication tag** to verify integrity.
- **Decrypted on the server** before storage.

#### **Encrypting a Chunk (Client Side)**

```c
void encrypt_chunk(const unsigned char *plaintext, size_t length,
                   const unsigned char *nonce, const unsigned char *key,
                   unsigned char *ciphertext) {
    crypto_secretbox_easy(ciphertext, plaintext, length, nonce, key);
}
```

#### **Decrypting a Chunk (Server Side)**

```c
int decrypt_chunk(const unsigned char *ciphertext, size_t length,
                  const unsigned char *nonce, const unsigned char *key,
                  unsigned char *plaintext) {
    if (crypto_secretbox_open_easy(plaintext, ciphertext, length, nonce, key) != 0) {
        printf("Decryption failed!\n");
        return -1;
    }
    return 0;
}
```

- Each **chunk has a nonce**, which prevents replay attacks.
- The **authentication tag ensures integrity**.
- If the MAC check fails, the server **rejects the chunk**.

---

## **4. Server Workflow**

1. **Client Authentication**
   - Server sends a **random challenge**.
   - Client **responds with a hashed challenge**.
   - Server **verifies the response**.

2. **Session Key Exchange**
   - Client sends an **ephemeral public key**.
   - Server generates a **shared secret**.
   - Both derive a **session key**.

3. **Encrypted File Transfer**
   - Client encrypts file chunks.
   - Each chunk has:
     - **Nonce**
     - **Ciphertext**
     - **MAC (auth tag)**
   - Server **decrypts** and **stores** chunks.

4. **File Integrity & Storage**
   - Server **validates authentication tags**.
   - If any chunk is tampered with, **reject the transfer**.

---

## **5. Communication Protocol Example**

### **Client (Python) → Server (C)**

#### **1. Handshake**

```
Client → Server: "HELLO" + ephemeral_pub_key
Server → Client: "OK" + server_ephemeral_pub_key
Client → Server: Encrypted test message (using session key)
Server → Client: "VERIFIED"
```

#### **2. File Transfer**

```
Client → Server: Encrypted chunk 1 (ciphertext, nonce, MAC)
Client → Server: Encrypted chunk 2 (ciphertext, nonce, MAC)
...
Client → Server: EOF
```

---

## **6. Concurrency Considerations**

Since the server must handle **multiple clients**, it should:

- Use **threads** or `poll()` to manage multiple connections.
- Store a **unique session key per client**.

```c
typedef struct {
    int client_socket;
    unsigned char session_key[crypto_secretbox_KEYBYTES];
} client_session_t;
```

Each **client gets a separate session key**, ensuring **forward secrecy**.

---

## **7. Summary**

| **Requirement**      | **TweetNaCl Function Used**       |
|---------------------|--------------------------------|
| **Confidentiality** | `crypto_secretbox_easy()`      |
| **Integrity**       | `crypto_secretbox_open_easy()` |
| **Authentication**  | `crypto_hash()` for challenge-response |
| **Forward Secrecy** | `crypto_box()` for ephemeral keys |

---

## **8. Next Steps**

1. **Implement TCP socket server** in C.
2. **Handle multiple clients** with threads.
3. **Implement error handling** for dropped connections.
4. **Test interoperability** with PyNaCl on the Python client.

Would you like help structuring the TCP server with TweetNaCl?
