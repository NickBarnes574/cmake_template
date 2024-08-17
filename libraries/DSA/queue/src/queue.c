#include "queue.h"
#include "utilities.h"

queue_t * queue_init(FREE_F customfree)
{
    queue_t * queue = calloc(1, sizeof(queue_t));
    if (NULL == queue)
    {
        print_error("CMR failure.");
        goto END;
    }

    queue->currentsz  = 0;
    queue->head       = NULL;
    queue->tail       = NULL;
    queue->customfree = (NULL == customfree) ? free : customfree;

END:
    return queue;
}

bool queue_is_empty(queue_t * queue)
{
    bool result = false;

    if (0 == queue->currentsz)
    {
        result = true;
        goto END;
    }

END:
    return result;
}

int queue_enqueue(queue_t * queue, void * data)
{
    int            exit_code = E_FAILURE;
    queue_node_t * new_node  = NULL;
    bool           is_empty  = false;

    if ((NULL == queue) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    new_node = calloc(1, sizeof(queue_node_t));
    if (NULL == new_node)
    {
        print_error("CMR failure.");
        goto END;
    }

    new_node->data = data;
    new_node->next = NULL;

    is_empty = queue_is_empty(queue);
    if (true == is_empty)
    {
        queue->head = new_node;
        queue->tail = new_node;
    }
    else
    {
        queue->tail->next = new_node;
        queue->tail       = new_node;
    }

    queue->currentsz++;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * queue_dequeue(queue_t * queue)
{
    void *         data           = NULL;
    queue_node_t * node_to_remove = NULL;
    bool           is_empty       = false;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    is_empty = queue_is_empty(queue);
    if (true == is_empty)
    {
        print_error("queue_dequeue(): Queue is empty.");
        goto END;
    }

    node_to_remove = queue->head;
    data           = node_to_remove->data;
    queue->head    = queue->head->next;

    free(node_to_remove);
    node_to_remove = NULL;

    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }

    queue->currentsz--;

END:
    return data;
}

void * queue_peek(queue_t * queue)
{
    void * data     = NULL;
    bool   is_empty = false;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    is_empty = queue_is_empty(queue);
    if (false == is_empty)
    {
        data = queue->head->data;
    }

END:
    return data;
}

int queue_clear(queue_t * queue)
{
    int    exit_code = E_FAILURE;
    void * data      = NULL;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    while (false == queue_is_empty(queue))
    {
        data = queue_dequeue(queue);
        queue->customfree(data);
        data = NULL;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void queue_destroy(queue_t ** queue_addr)
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

    free(*queue_addr);
    *queue_addr = NULL;

END:
    return;
}

/*** end of file ***/
