#include "queue.h"
#include "utilities.h"

queue_t * queue_init(uint32_t capacity, FREE_F customfree)
{
    queue_t * queue = calloc(1, sizeof(queue_t));
    if (NULL == queue)
    {
        print_error("CMR failure.");
        goto END;
    }

    queue->capacity  = capacity;
    queue->currentsz = 0;
    queue->arr       = calloc(capacity, sizeof(queue_node_t *));
    if (NULL == queue->arr)
    {
        print_error("CMR failure.");
        free(queue);
        queue = NULL;
        goto END;
    }

    queue->customfree = (NULL == customfree) ? free : customfree;

END:
    return queue;
}

int queue_fullcheck(queue_t * queue)
{
    int exit_code = E_FAILURE;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (queue->currentsz == queue->capacity)
    {
        exit_code = E_SUCCESS;
    }

END:
    return exit_code;
}

int queue_emptycheck(queue_t * queue)
{
    int exit_code = E_FAILURE;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == queue->currentsz)
    {
        exit_code = E_SUCCESS;
    }

END:
    return exit_code;
}

int queue_enqueue(queue_t * queue, void * data)
{
    int            exit_code = E_FAILURE;
    queue_node_t * new_node  = NULL;

    if ((NULL == queue) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == queue_fullcheck(queue))
    {
        print_error("Queue is full.");
        goto END;
    }

    new_node = calloc(1, sizeof(queue_node_t));
    if (NULL == new_node)
    {
        print_error("CMR failure.");
        goto END;
    }

    new_node->data = data;

    queue->arr[queue->currentsz++] = new_node;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

queue_node_t * queue_dequeue(queue_t * queue)
{
    queue_node_t * node = NULL;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == queue_emptycheck(queue))
    {
        print_error("Queue is empty.");
        goto END;
    }

    node = queue->arr[0];

    for (size_t idx = 0; idx < (queue->currentsz - 1); idx++)
    {
        queue->arr[idx] = queue->arr[idx + 1];
    }

    queue->arr[queue->currentsz - 1] = NULL;

    queue->currentsz--;

END:
    return node;
}

queue_node_t * queue_peek(queue_t * queue)
{
    queue_node_t * node = NULL;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    node = queue->arr[0];

END:
    return node;
}

int queue_clear(queue_t * queue)
{
    int exit_code = E_FAILURE;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    while (-1 == queue_emptycheck(queue))
    {
        queue->customfree(queue->arr[queue->currentsz - 1]->data);
        queue->arr[queue->currentsz - 1]->data = NULL;
        free(queue->arr[queue->currentsz - 1]);
        queue->arr[queue->currentsz - 1] = NULL;
        queue->currentsz--;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int queue_destroy(queue_t ** queue_addr)
{
    int exit_code = E_FAILURE;

    if (NULL == *queue_addr)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    exit_code = queue_clear(*queue_addr);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to clear queue.");
        goto END;
    }

    free((*queue_addr)->arr);
    (*queue_addr)->arr = NULL;
    free(*queue_addr);
    *queue_addr = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
