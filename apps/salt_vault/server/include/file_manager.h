/**
 * @file file_manager.h
 *
 * @brief Provides functions for securely storing decrypted file data,
 * verifying chunk order, managing temporary buffers, and ensuring filesystem
 * integrity.
 */
#ifndef _FILE_MANAGER_H
#define _FILE_MANAGER_H

#include <stddef.h>
#include <stdint.h>

int  file_manager_init(const char * base_dir);
int  store_chunk(const char *    filename,
                 uint32_t        chunk_index,
                 const uint8_t * data,
                 size_t          data_len);
int  finalize_file(const char * filename);
void file_manager_cleanup(void);

#endif /* _FILE_MANAGER_H */

/*** end of file ***/
