#include <string.h>

#include "hash_table.h"
#include "utilities.h"

#define BASE         256
#define PRIME        65521
#define MAX_KEY_SIZE 64
#define MAX_LENGTH   50 // used for strncmp() in hash_table_lookup()

typedef unsigned const char uchar_t;

/**
 * @brief Implements a hashing algorithm used to insert and lookup data
 *
 * @param size The size of the hash table
 * @param p_data The key to use
 * @param p_index A pointer to the hash index
 * @return int 0 for success, anything else results in failure.
 */
static int hash(uint32_t size, void * p_data, uint32_t * p_index);

/**
 * @brief Creates a new node for a hash table
 *
 * @param key The key to use
 * @param data The data to store in the node
 * @return node_t*
 */
static node_t * new_node(char * p_key, void * p_data);

hash_table_t * hash_table_init(uint32_t size, FREE_F customfree)
{
    hash_table_t * p_hash_table = NULL;
    int            mutex_check  = -1;

    if (0 == size)
    {
        print_error("Invalid hash table size of 0");
        goto END;
    }

    p_hash_table = calloc(1, sizeof(hash_table_t));
    if (NULL == p_hash_table)
    {
        print_error("CMR failure.");
        goto END;
    }

    p_hash_table->table = calloc(size, sizeof(node_t *));
    if (NULL == p_hash_table->table)
    {
        print_error("CMR failure.");
        goto END;
    }

    mutex_check = pthread_mutex_init(&p_hash_table->lock, NULL);
    if (E_SUCCESS != mutex_check)
    {
        print_error("Unable to initialize mutex.");
        free(p_hash_table->table);
        p_hash_table->table = NULL;
        free(p_hash_table);
        p_hash_table = NULL;
        goto END;
    }

    p_hash_table->size       = size;
    p_hash_table->customfree = (NULL == customfree) ? free : customfree;

END:
    return p_hash_table;
}

int hash_table_add(hash_table_t * table, void * data, char * key)
{
    int      exit_code      = E_FAILURE;
    uint32_t index          = 0;
    node_t * p_current_node = NULL;
    node_t * p_new_node     = NULL;

    if ((NULL == table) || (NULL == data) || (NULL == key))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    exit_code = hash(table->size, key, &index);
    if (E_SUCCESS != exit_code)
    {
        print_error("Hashing failure.");
        goto END;
    }

    p_new_node = new_node(key, data);
    if (NULL == p_new_node)
    {
        goto END;
    }

    pthread_mutex_lock(&table->lock);
    p_current_node = table->table[index];
    if (NULL == p_current_node)
    {
        table->table[index] = p_new_node;
    }
    else
    {
        // Handle collisions
        while (NULL != p_current_node->next)
        {
            p_current_node = p_current_node->next;
        }

        p_current_node->next = p_new_node;
    }
    pthread_mutex_unlock(&table->lock);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * hash_table_lookup(hash_table_t * table, char * key)
{
    int      exit_code      = E_FAILURE;
    void *   p_data         = NULL;
    uint32_t index          = 0;
    int      check          = 0;
    node_t * p_current_node = NULL;

    if ((NULL == table) || (NULL == key))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    exit_code = hash(table->size, key, &index);
    if (E_SUCCESS != exit_code)
    {
        print_error("Hashing failure");
        goto END;
    }

    p_current_node = table->table[index];

    while (NULL != p_current_node)
    {
        check = strncmp(p_current_node->key, key, MAX_LENGTH);
        if (0 == check)
        {
            p_data = p_current_node->data;
            goto END;
        }

        p_current_node = p_current_node->next;
    }

END:
    return p_data;
}

int hash_table_find(hash_table_t * table,
                    const char *   search,
                    size_t *       result_count,
                    char ***       results)
{
    int      exit_code = E_FAILURE;
    char **  buffer    = NULL;
    size_t   count     = 0;
    node_t * current   = NULL;
    char *   str_res   = NULL;

    if ((NULL == table) || (NULL == search) || (NULL == result_count) ||
        (NULL == results))
    {
        print_error("hash_table_find(): NULL argument passed.");
        goto END;
    }

    buffer = calloc(table->size, sizeof(char *));

    pthread_mutex_lock(&table->lock);
    for (uint32_t idx = 0; idx < table->size; ++idx)
    {
        current = table->table[idx];
        while (NULL != current)
        {
            str_res = strstr(current->key, search);
            if (NULL != str_res)
            {
                // Duplicate and store the key
                buffer[count++] = strndup(current->key, MAX_KEY_SIZE);
            }
            current = current->next;
        }
    }
    pthread_mutex_unlock(&table->lock);

    *result_count = count;
    *results      = buffer;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int hash_table_list(hash_table_t * table,
                    size_t *       result_count,
                    char ***       results)
{
    int      exit_code = E_FAILURE;
    char **  buffer    = NULL;
    size_t   count     = 0;
    node_t * current   = NULL;

    if ((NULL == table) || (NULL == result_count) || (NULL == results))
    {
        print_error("hash_table_find(): NULL argument passed.");
        goto END;
    }

    buffer = calloc(table->size, sizeof(char *));

    pthread_mutex_lock(&table->lock);
    for (uint32_t idx = 0; idx < table->size; ++idx)
    {
        current = table->table[idx];
        while (NULL != current)
        {
            // Duplicate and store the key
            buffer[count++] = strndup(current->key, MAX_KEY_SIZE);
            current         = current->next;
        }
    }
    pthread_mutex_unlock(&table->lock);

    *result_count = count;
    *results      = buffer;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int hash_table_remove(hash_table_t * table, char * key)
{
    int      exit_code      = E_FAILURE;
    int      check          = E_FAILURE;
    uint32_t index          = 0;
    node_t * p_current_node = NULL;
    node_t * p_prev_node    = NULL;

    if ((NULL == table) || (NULL == key))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    check = hash(table->size, key, &index);
    if (E_SUCCESS != check)
    {
        print_error("Hashing failure.");
        goto END;
    }

    pthread_mutex_lock(&table->lock);
    p_current_node = table->table[index];
    while (NULL != p_current_node)
    {
        check = strncmp(p_current_node->key, key, MAX_LENGTH);
        if (0 != check)
        {
            p_prev_node    = p_current_node;
            p_current_node = p_current_node->next;
        }
        else
        {
            if (NULL == p_prev_node)
            {
                table->table[index] = p_current_node->next;
            }
            else
            {
                p_prev_node->next = p_current_node->next;
            }

            table->customfree(p_current_node->data);
            p_current_node->data = NULL;
            free(p_current_node->key);
            p_current_node->key = NULL;
            free(p_current_node);
            p_current_node = NULL;
            exit_code      = E_SUCCESS;
        }
    }
    pthread_mutex_unlock(&table->lock);

END:
    return exit_code;
}

int hash_table_clear(hash_table_t * table)
{

    int      exit_code      = E_FAILURE;
    node_t * p_current_node = NULL;
    node_t * p_temp_node    = NULL;

    if (NULL == table)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    pthread_mutex_lock(&table->lock);
    for (uint32_t idx = 0; idx < table->size; idx++)
    {
        p_current_node = table->table[idx];

        while (NULL != p_current_node)
        {
            p_temp_node = p_current_node->next;
            free(p_current_node->key);
            p_current_node->key = NULL;
            table->customfree(p_current_node->data);
            free(p_current_node);
            p_current_node = p_temp_node;
        }
        table->table[idx] = NULL;
    }
    pthread_mutex_unlock(&table->lock);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int hash_table_destroy(hash_table_t ** table_addr)
{
    int exit_code = E_FAILURE;

    if (NULL == *table_addr)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    exit_code = hash_table_clear(*table_addr);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to clear hash table.");
        goto END;
    }

    pthread_mutex_destroy(&(*table_addr)->lock);
    free((*table_addr)->table);
    (*table_addr)->table = NULL;
    free(*table_addr);
    *table_addr = NULL;

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

/***********************************************************************
 * NOTE: STATIC FUNCTIONS LISTED BELOW
 ***********************************************************************/

static int hash(uint32_t size, void * p_data, uint32_t * index)
{
    int          exit_code = E_FAILURE;
    uint32_t     target    = 0;
    uchar_t *    letter    = NULL;
    const char * string    = NULL;

    if ((NULL == p_data) || (NULL == index))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    string = (const char *)p_data; // Interpret the key as a string
    letter = (uchar_t *)string;    // Point to the first character

    while ('\0' != *letter)
    {
        target = (((target * BASE) + *letter) % PRIME);
        letter++;
    }

    *index = (target % size);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static node_t * new_node(char * key, void * data)
{
    node_t * new_node  = NULL;
    char *   p_new_key = NULL;

    if ((NULL == key) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    new_node = calloc(1, sizeof(node_t));
    if (NULL == new_node)
    {
        print_error("CMR failure.");
        goto END;
    }

    p_new_key = calloc((MAX_KEY_SIZE + 1), (sizeof(char)));
    if (NULL == p_new_key)
    {
        print_error("CMR failure.");
        free(new_node);
        new_node = NULL;
        goto END;
    }

    new_node->data = data;
    new_node->next = NULL;
    strncpy(p_new_key, key, MAX_KEY_SIZE);
    new_node->key = p_new_key;

END:
    return new_node;
}
