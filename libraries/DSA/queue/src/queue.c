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

    // Initialize the mutex
    if (pthread_mutex_init(&queue->mutex, NULL) != 0)
    {
        print_error("Failed to initialize mutex.");
        free(queue);
        queue = NULL;
        goto END;
    }

END:
    return queue;
}

bool queue_is_empty(queue_t * queue)
{
    bool result = false;

    pthread_mutex_lock(&queue->mutex);
    result = (queue->currentsz == 0);
    pthread_mutex_unlock(&queue->mutex);

    return result;
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

    new_node = calloc(1, sizeof(queue_node_t));
    if (NULL == new_node)
    {
        print_error("CMR failure.");
        goto END;
    }

    new_node->data = data;
    new_node->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    if (queue->currentsz == 0)
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
    pthread_mutex_unlock(&queue->mutex);

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

void * queue_dequeue(queue_t * queue)
{
    void *         data           = NULL;
    queue_node_t * node_to_remove = NULL;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    pthread_mutex_lock(&queue->mutex);

    if (queue->currentsz == 0)
    {
        print_error("queue_dequeue(): Queue is empty.");
        pthread_mutex_unlock(&queue->mutex);
        goto END;
    }

    node_to_remove = queue->head;
    data           = node_to_remove->data;
    queue->head    = queue->head->next;

    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }

    queue->currentsz--;
    pthread_mutex_unlock(&queue->mutex);

    free(node_to_remove);

END:
    return data;
}

void * queue_peek(queue_t * queue)
{
    void * data = NULL;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    pthread_mutex_lock(&queue->mutex);
    if (queue->head != NULL)
    {
        data = queue->head->data;
    }
    pthread_mutex_unlock(&queue->mutex);

END:
    return data;
}

int queue_clear(queue_t * queue)
{
    int exit_code = E_FAILURE;

    if (NULL == queue)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    pthread_mutex_lock(&queue->mutex);
    while (queue->currentsz > 0)
    {
        void * data = queue_dequeue(queue);
        queue->customfree(data);
    }
    pthread_mutex_unlock(&queue->mutex);

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

void queue_destroy(queue_t ** queue_addr)
{
    if (NULL == *queue_addr)
    {
        print_error("NULL argument passed.");
        return;
    }

    queue_clear(*queue_addr);

    // Destroy the mutex
    pthread_mutex_destroy(&(*queue_addr)->mutex);

    free(*queue_addr);
    *queue_addr = NULL;
}
