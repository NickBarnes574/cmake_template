#include "stack.h"
#include "utilities.h"

stack_t * stack_init(uint32_t capacity, FREE_F customfree)
{
    stack_t * stack = calloc(1, sizeof(stack_t));
    if (NULL == stack)
    {
        print_error("CMR failure.");
        goto END;
    }

    stack->capacity  = capacity;
    stack->currentsz = 0;
    stack->arr       = calloc(capacity, sizeof(stack_node_t *));
    if (NULL == stack->arr)
    {
        print_error("CMR failure.");
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
        print_error("NULL argument passed.");
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
        print_error("NULL argument passed.");
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
        print_error("NULL argument passed.");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    if (0 == stack_is_full(stack))
    {
        print_error("Stack is full.");
        goto END;
    }

    new_element = calloc(1, sizeof(stack_node_t));
    if (NULL == new_element)
    {
        print_error("CMR failure.");
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
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == stack_is_empty(stack))
    {
        print_error("Stack is empty.");
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

    if (NULL == stack)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == stack_is_empty(stack))
    {
        print_error("Stack is empty.");
        goto END;
    }

    element = stack->arr[stack->currentsz - 1];

END:
    return element->data;
}

// Covers 4.1.11: Make use of a function pointer to call another function
int stack_clear(stack_t * stack)
{
    int exit_code = E_FAILURE;

    if (NULL == stack)
    {
        print_error("NULL argument passed.");
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
        print_error("NULL argument passed.");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    exit_code = stack_clear(*stack);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to clear stack.");
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
