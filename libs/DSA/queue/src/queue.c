#include "queue.h"
#include "default_free.h"
#include "signal_handler.h"
#include "utilities.h"

queue_t * queue_init(FREE_F customfree, uint32_t max_size)
{
    int       exit_code = E_FAILURE;
    queue_t * queue     = NULL;

    queue = calloc(1, sizeof(queue_t));
    if (NULL == queue)
    {
        PRINT_DEBUG("queue_init(): CMR failure.\n");
        goto END;
    }

    queue->current_size = 0;
    queue->max_size     = max_size;
    queue->head         = NULL;
    queue->tail         = NULL;
    queue->customfree   = (NULL == customfree) ? default_free : customfree;

    // Initialize the mutex
    exit_code = pthread_mutex_init(&queue->mutex, NULL);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("queue_init(): Failed to initialize mutex.\n");
        goto CLEANUP_QUEUE;
    }

    exit_code = pthread_cond_init(&queue->not_empty, NULL);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("queue_init(): Failed to initialize mutex.\n");
        goto CLEANUP_MUTEX;
    }

    exit_code = pthread_cond_init(&queue->not_full, NULL);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("queue_init(): Failed to initialize mutex.\n");
        goto CLEANUP_COND1;
    }

    goto END;

CLEANUP_COND1:
    pthread_cond_destroy(&queue->not_empty);
CLEANUP_MUTEX:
    pthread_mutex_destroy(&queue->mutex);
CLEANUP_QUEUE:
    free(queue);
    queue = NULL;
END:
    return queue;
}

bool queue_is_empty(queue_t * queue)
{
    bool result = false;

    pthread_mutex_lock(&queue->mutex);
    result = (queue->current_size == 0);
    pthread_mutex_unlock(&queue->mutex);

    return result;
}

bool queue_is_full(queue_t * queue)
{
    bool result = false;

    pthread_mutex_lock(&queue->mutex);
    if (QUEUE_UNBOUNDED != queue->max_size)
    {
        result = (queue->current_size >= queue->max_size);
    }
    pthread_mutex_unlock(&queue->mutex);

    return result;
}

int queue_enqueue(queue_t * queue, void * data)
{
    int            exit_code = E_FAILURE;
    queue_node_t * new_node  = NULL;

    if ((NULL == queue) || (NULL == data))
    {
        PRINT_DEBUG("queue_enqueue(): NULL argument passed.\n");
        goto END;
    }

    new_node = calloc(1, sizeof(queue_node_t));
    if (NULL == new_node)
    {
        PRINT_DEBUG("queue_enqueue(): CMR failure - new_node.\n");
        goto END;
    }

    new_node->data = data;
    new_node->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    while ((QUEUE_UNBOUNDED != queue->max_size) &&
           (queue->current_size >= queue->max_size))
    {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }

    if (queue->current_size == 0)
    {
        queue->head = new_node;
        queue->tail = new_node;
    }
    else
    {
        queue->tail->next = new_node;
        queue->tail       = new_node;
    }

    queue->current_size++;
    pthread_cond_signal(&queue->not_empty);
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
        PRINT_DEBUG("queue_dequeue(): NULL argument passed.\n");
        goto END;
    }

    pthread_mutex_lock(&queue->mutex);

    while (queue->current_size == 0)
    {
        if (signal_flag == SHUTDOWN)
        {
            pthread_mutex_unlock(&queue->mutex);
            goto END;
        }

        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    node_to_remove = queue->head;
    data           = node_to_remove->data;
    queue->head    = queue->head->next;

    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }

    queue->current_size--;
    pthread_cond_signal(&queue->not_full);
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
        PRINT_DEBUG("queue_peek(): NULL argument passed.\n");
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
        PRINT_DEBUG("queue_clear(): NULL argument passed.\n");
        goto END;
    }

    while (queue->current_size > 0)
    {
        void * data = queue_dequeue(queue);
        queue->customfree(data);
    }

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

void queue_destroy(queue_t ** queue_addr)
{
    if (NULL == *queue_addr)
    {
        PRINT_DEBUG("queue_destroy(): NULL argument passed.\n");
        return;
    }

    queue_clear(*queue_addr);

    // Destroy the mutex
    pthread_mutex_destroy(&(*queue_addr)->mutex);
    pthread_cond_destroy(&(*queue_addr)->not_empty);
    pthread_cond_destroy(&(*queue_addr)->not_full);

    free(*queue_addr);
    *queue_addr = NULL;
}
