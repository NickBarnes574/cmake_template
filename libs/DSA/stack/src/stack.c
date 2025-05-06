#include "stack.h"
#include "utilities.h"

stack_t * stack_init(uint32_t capacity, FREE_F customfree)
{
    stack_t * stack = calloc(1, sizeof(stack_t));
    if (NULL == stack)
    {
        PRINT_DEBUG("stack_init(): CMR failure - stack.\n");
        goto END;
    }

    stack->capacity  = capacity;
    stack->currentsz = 0;
    stack->arr       = calloc(capacity, sizeof(stack_node_t *));
    if (NULL == stack->arr)
    {
        PRINT_DEBUG("stack_init(): CMR failure - stack->arr.\n");
        free(stack);
        stack = NULL;
        goto END;
    }

    stack->customfree = customfree;

END:
    return stack;
}

int stack_is_full(stack_t * stack)
{
    int exit_code = E_FAILURE;

    if (NULL == stack)
    {
        PRINT_DEBUG("stack_is_full(): NULL argument passed.\n");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    if (stack->currentsz == stack->capacity)
    {
        exit_code = E_SUCCESS;
    }

END:
    return exit_code;
}

int stack_is_empty(stack_t * stack)
{
    int exit_code = E_FAILURE;

    if (NULL == stack)
    {
        PRINT_DEBUG("stack_is_empty(): NULL argument passed.\n");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    if (0 == stack->currentsz)
    {
        exit_code = E_SUCCESS;
    }

END:
    return exit_code;
}

int stack_push(stack_t * stack, void * data)
{
    int            exit_code   = E_FAILURE;
    stack_node_t * new_element = NULL;

    if ((NULL == stack) || (NULL == data))
    {
        PRINT_DEBUG("stack_push(): NULL argument passed.\n");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    if (0 == stack_is_full(stack))
    {
        PRINT_DEBUG("stack_push(): Stack is full.");
        goto END;
    }

    new_element = calloc(1, sizeof(stack_node_t));
    if (NULL == new_element)
    {
        PRINT_DEBUG("stack_push(): CMR failure - new_element.\n");
        goto END;
    }

    new_element->data = data;

    stack->arr[stack->currentsz++] = new_element;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * stack_pop(stack_t * stack)
{
    void * data = NULL;

    if (NULL == stack)
    {
        PRINT_DEBUG("stack_pop(): NULL argument passed.\n");
        goto END;
    }

    if (0 == stack_is_empty(stack))
    {
        PRINT_DEBUG("stack_pop(): Stack is empty.\n");
        goto END;
    }

    data = stack->arr[stack->currentsz - 1]->data;

    free(stack->arr[stack->currentsz - 1]);
    stack->arr[stack->currentsz - 1] = NULL;

    stack->currentsz--;

END:
    return data;
}

void * stack_peek(stack_t * stack)
{
    stack_node_t * element = NULL;
    void *         data    = NULL;

    if (NULL == stack)
    {
        PRINT_DEBUG("stack_peek(): NULL argument passed.\n");
        goto END;
    }

    if (0 == stack_is_empty(stack))
    {
        PRINT_DEBUG("stack_peek(): Stack is empty.\n");
        goto END;
    }

    element = stack->arr[stack->currentsz - 1];
    if (NULL == element)
    {
        PRINT_DEBUG("stack_peek(): Element does not exist.\n");
        goto END;
    }

    data = element->data;

END:
    return data;
}

// Covers 4.1.11: Make use of a function pointer to call another function
int stack_clear(stack_t * stack)
{
    int exit_code = E_FAILURE;

    if (NULL == stack)
    {
        PRINT_DEBUG("stack_clear(): NULL argument passed.\n");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    while (-1 == stack_is_empty(stack))
    {
        stack->customfree(stack->arr[stack->currentsz - 1]->data);
        stack->arr[stack->currentsz - 1]->data = NULL;
        free(stack->arr[stack->currentsz - 1]);
        stack->arr[stack->currentsz - 1] = NULL;
        stack->currentsz--;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int stack_destroy(stack_t ** stack)
{
    int exit_code = E_FAILURE;

    if (NULL == *stack)
    {
        PRINT_DEBUG("stack_destroy(): NULL argument passed.\n");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    exit_code = stack_clear(*stack);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("stack_destroy(): Unable to clear stack.\n");
        goto END;
    }

    free((*stack)->arr);
    (*stack)->arr = NULL;
    free(*stack);
    *stack = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
