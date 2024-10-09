# Common Library TODOs

## File I/O

### Function Prototypes

#### write_line()

```c
/**
 * @brief Writes a line to a file.
 *
 * This function writes a given string (line) to a specified file stream. The
 * string does not need to contain a newline character at the end as the
 * function appends it automatically. If 'write_fd' is NULL or the write
 * operation fails, an error is reported.
 *
 * @param write_fd The file stream to write to.
 * @param line The string to write to the file.
 * @return int Returns E_SUCCESS if the line is written successfully, or
 * E_FAILURE on failure.
 */
int write_line(FILE * write_fd, const char * line);
```

#### open_file()

```c
/**
 * @brief Opens a file with the specified mode.
 *
 * This function opens a file at the given 'filepath' with the specified 'mode'
 * (e.g., "r", "w", "a") and stores the file pointer in '*file'. It provides
 * detailed error reporting and integrates with the module's error handling
 * scheme.
 *
 * @param filepath The path to the file to be opened.
 * @param mode The mode in which to open the file.
 * @param file Double pointer to the FILE object to store the opened file
 * pointer.
 * @return int Returns E_SUCCESS if the file is opened successfully, or
 * E_FAILURE on failure.
 */
int open_file(const char * filepath, const char * mode, FILE ** file);
```

#### close_file()

```c
/**
 * @brief Closes a file.
 *
 * This function closes an opened file pointed to by 'file_fd'. It performs
 * necessary cleanup operations and error checking. If 'file_fd' is NULL or
 * closing fails, an error is reported.
 *
 * @param file_fd The file pointer to the file to be closed.
 * @return int Returns E_SUCCESS if the file is closed successfully, or
 * E_FAILURE on failure.
 */
int close_file(FILE * file_fd);
```

#### read_file_into_buffer()

```c
/**
 * @brief Reads an entire file into a buffer.
 *
 * This function reads the contents of a file specified by 'read_fd' into a
 * dynamically allocated buffer. The address of the buffer is stored into
 * '*buffer', and the size of the buffer is stored into '*size'. It is the
 * caller's responsibility to free the buffer. If 'read_fd' is NULL, or any
 * reading or allocation fails, an error is reported.
 *
 * @param read_fd The file stream to read from.
 * @param buffer Double pointer to the buffer where file contents will be
 * stored.
 * @param size Pointer to a variable where the size of the buffer will be
 * stored.
 * @return int Returns E_SUCCESS if the file is read successfully, or E_FAILURE
 * on failure.
 */
int read_file_into_buffer(FILE * read_fd, char ** buffer, size_t * size);
```

#### delete_file()

```c
/**
 * @brief Deletes a file at the specified path.
 *
 * This function attempts to delete a file located at 'filepath'. If 'filepath'
 * is NULL or the file cannot be deleted, an error is reported.
 *
 * @param filepath The path to the file to be deleted.
 * @return int Returns E_SUCCESS if the file is deleted successfully, or
 * E_FAILURE if it cannot be deleted or an error occurs.
 */
int delete_file(const char * filepath);
```

#### get_file_size()

```c
/**
 * @brief Gets the size of a file.
 *
 * This function determines the size of the file specified by 'file_fd' and
 * stores the size in '*size'. If 'file_fd' is NULL or the file size cannot be
 * determined, an error is reported.
 *
 * @param file_fd The file stream of the file whose size is to be determined.
 * @param size Pointer to a variable where the file size will be stored.
 * @return int Returns E_SUCCESS if the file size is determined successfully, or
 * E_FAILURE on failure.
 */
int get_file_size(FILE * file_fd, size_t * size);
```

#### copy_file()

```c
/**
 * @brief Copies a file from source to destination path.
 *
 * This function copies the contents of the file located at 'src_filepath' to
 * 'dest_filepath'. If either 'src_filepath' or 'dest_filepath' is NULL, or if
 * the copy operation fails for any reason (e.g., source file not found,
 * insufficient permissions, insufficient storage space), an error is reported.
 *
 * @param src_filepath The path to the source file.
 * @param dest_filepath The path to the destination file.
 * @return int Returns E_SUCCESS if the file is copied successfully, or
 * E_FAILURE on failure.
 */
int copy_file(const char * src_filepath, const char * dest_filepath);
```
