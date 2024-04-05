#include "linked_list.h"
#include "comparisons.h"
#include "number_generator.h"
#include "utilities.h"

/**
 * @brief Create a new `list_node_t`
 *
 * @param data The data to store in the node
 * @return list_node_t*
 */
static list_node_t * list_node_new(void * data);

/**
 * @brief Finds a node in the linked list that matches the given data.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be matched.
 * @return Pointer to the node containing the data if found, NULL otherwise.
 */
static list_node_t * find_node(list_t * list, void * data);

/**
 * @brief Removes a given node from the linked list.
 *
 * This function updates the linked list to exclude the given node, freeing
 * the memory associated with the node and decrementing the list size.
 *
 * @param list Pointer to the linked list.
 * @param node Pointer to the node to be removed.
 */
static void remove_node(list_t * list, list_node_t * node);

/**
 * @brief Splits a list into two equal halves
 *
 * @param head Pointer to the head of the list to be split.
 * @param first_half Pointer to store the head of the first half.
 * @param second_half Pointer to store the head of the second half.
 */
static void split(list_node_t *  head,
                  list_node_t ** first_half,
                  list_node_t ** second_half);

/**
 * @brief Merges two sorted linked lists into a single sorted linked list.
 *
 * @param first Pointer to the head of the first sorted list.
 * @param second Pointer to the head of the second sorted list.
 * @param compare_func Pointer to the comparison function used to sort the list.
 * @return Pointer to the head of the merged sorted list.
 */
static list_node_t * merge(list_node_t * first,
                           list_node_t * second,
                           CMP_F         compare_func);

/**
 * @brief Internal recursive function to implement merge sort on linked list.
 *
 * @param head_ref Pointer to the head of the list to be sorted.
 * @param compare_func Pointer to the comparison function used to sort the list.
 */
static void merge_sort(list_node_t ** head_ref, CMP_F compare_func);

list_t * list_new(FREE_F free_func, CMP_F comp_func)
{
    list_t * new_list = NULL;

    new_list = calloc(1, sizeof(list_t));
    if (NULL == new_list)
    {
        print_error("CMR failure.");
        goto END;
    }

    new_list->size         = 0;
    new_list->head         = NULL;
    new_list->tail         = NULL;
    new_list->custom_free  = (NULL == free_func) ? free : free_func;
    new_list->compare_func = (NULL == comp_func) ? int_comp : comp_func;

END:
    return new_list;
}

int list_push_head(list_t * list, void * data)
{
    int           exit_code = E_FAILURE;
    list_node_t * new_node  = NULL;

    if ((NULL == list) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    new_node = list_node_new(data);
    if (NULL == new_node)
    {
        print_error("Unable to create new node.");
        goto END;
    }

    if (NULL == list->head)
    {
        // Establish 'new_node' as the first node of an empty list
        list->head       = new_node;
        list->tail       = new_node;
        list->tail->next = NULL;
        list->head->prev = NULL;
    }
    else
    {
        // Insert 'new_node' at the front of a populated list
        new_node->next   = list->head;
        new_node->prev   = NULL;
        list->head->prev = new_node;
        list->head       = new_node;
    }

    list->size += 1;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int list_push_tail(list_t * list, void * data)
{
    int           exit_code = E_FAILURE;
    list_node_t * new_node  = NULL;
    // list_node_t *current = NULL;

    if ((NULL == list) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    new_node = list_node_new(data);
    if (NULL == new_node)
    {
        print_error("Unable to create new node.");
        goto END;
    }

    if (NULL == list->head)
    {
        // Establish 'new_node' as the first node of an empty list
        list->head       = new_node;
        list->tail       = new_node;
        list->tail->next = NULL;
        list->head->prev = NULL;
    }
    else
    {
        // Insert 'new_node' at the rear of a populated list
        new_node->prev   = list->tail;
        new_node->next   = NULL;
        list->tail->next = new_node;
        list->tail       = new_node;
    }

    list->size += 1;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int list_push_position(list_t * list, void * data, uint32_t position)
{
    int           exit_code    = E_FAILURE;
    list_node_t * new_node     = NULL;
    list_node_t * current_node = NULL;

    if ((NULL == list) || (NULL == data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (position > list->size)
    {
        print_error("Position out of bounds.");
        goto END;
    }

    // Handle special cases for head and tail
    if (0 == position)
    {
        exit_code = list_push_head(list, data);
        goto END;
    }
    if (position == list->size - 1)
    {
        exit_code = list_push_tail(list, data);
        goto END;
    }

    new_node = list_node_new(data);
    if (NULL == new_node)
    {
        print_error("Unable to create new node.");
        goto END;
    }

    // Navigate to the specified position
    current_node = list->head;
    for (uint32_t idx = 0; idx < position - 1; idx++)
    {
        current_node = current_node->next;
    }

    // Insert the new node at the specified position
    new_node->next           = current_node->next;
    new_node->prev           = current_node;
    current_node->next->prev = new_node;
    current_node->next       = new_node;

    list->size += 1;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int list_emptycheck(list_t * list)
{
    int exit_code = E_FAILURE;

    if (NULL == list)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == list->size)
    {
        exit_code = E_SUCCESS;
    }

END:
    return exit_code;
}

void * list_pop_head(list_t * list)
{
    list_node_t * head_node = NULL;

    if ((NULL == list) || (NULL == list->head))
    {
        print_error("List is empty.");
        goto END;
    }

    head_node = list->head;

    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->head       = list->head->next;
        list->head->prev = NULL;
    }

    list->size--;

END:
    return head_node->data;
}

void * list_pop_tail(list_t * list)
{
    list_node_t * tail_node = NULL;

    if ((NULL == list) || (NULL == list->tail))
    {
        print_error("List is empty.");
        goto END;
    }

    tail_node = list->tail;

    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->tail       = list->tail->prev;
        list->tail->next = NULL;
    }

    list->size--;

END:
    return tail_node->data;
}

void * list_pop_position(list_t * list, uint32_t position)
{
    list_node_t * current     = NULL;
    list_node_t * node_to_pop = NULL;

    if (NULL == list)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (position >= list->size)
    {
        print_error("Position out of bounds.");
        goto END;
    }

    if (0 == position)
    {
        node_to_pop = list_pop_head(list);
        goto END;
    }

    if (position == list->size - 1)
    {
        node_to_pop = list_pop_tail(list);
        goto END;
    }

    current = list->head;
    for (uint32_t idx = 0; idx < position; idx++)
    {
        current = current->next;
    }

    node_to_pop         = current;
    current->prev->next = current->next;
    if (NULL != current->next)
    {
        current->next->prev = current->prev;
    }

    node_to_pop->prev = NULL;
    node_to_pop->next = NULL;

    list->size--;

END:
    return node_to_pop->data;
}

int list_remove_head(list_t * list)
{
    int           exit_code      = E_FAILURE;
    list_node_t * node_to_remove = NULL;

    if (NULL == list)
    {
        print_error("List is empty.");
        goto END;
    }

    node_to_remove = list_pop_head(list);
    list->custom_free(node_to_remove->data);
    node_to_remove->data = NULL;
    free(node_to_remove);
    node_to_remove = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int list_remove_tail(list_t * list)
{
    int           exit_code      = E_FAILURE;
    list_node_t * node_to_remove = NULL;

    if (NULL == list)
    {
        print_error("List is empty.");
        goto END;
    }

    node_to_remove = list_pop_tail(list);
    list->custom_free(node_to_remove->data);
    node_to_remove->data = NULL;
    free(node_to_remove);
    node_to_remove = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int list_remove_position(list_t * list, uint32_t position)
{
    int           exit_code      = E_FAILURE;
    list_node_t * node_to_remove = NULL;

    if (NULL == list)
    {
        print_error("List is empty.");
        goto END;
    }

    if (position >= list->size)
    {
        print_error("Position out of bounds.");
        goto END;
    }

    node_to_remove = list_pop_position(list, position);
    list->custom_free(node_to_remove->data);
    node_to_remove->data = NULL;
    free(node_to_remove);
    node_to_remove = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * list_peek_head(list_t * list)
{
    list_node_t * node = NULL;

    if (NULL == list)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    node = list->head;

END:
    return node->data;
}

void * list_peek_tail(list_t * list)
{
    list_node_t * node = NULL;

    if (NULL == list)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    node = list->tail;

END:
    return node->data;
}

void * list_peek_position(list_t * list, uint32_t position)
{
    list_node_t * current_node = NULL;

    if (NULL == list)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (position >= list->size)
    {
        print_error("Position out of bounds.");
        goto END;
    }

    current_node = list->head;
    for (uint32_t idx = 0; idx < position; idx++)
    {
        current_node = current_node->next;
    }

END:
    return current_node->data;
}

int list_remove_data(list_t * list, void * data_p)
{
    int           exit_code      = E_FAILURE;
    list_node_t * node_to_remove = NULL;

    if ((NULL == list) || (NULL == data_p))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    node_to_remove = find_node(list, data_p);
    if (NULL != node_to_remove)
    {
        remove_node(list, node_to_remove);
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int list_foreach_call(list_t * list, ACT_F action_function)
{
    int           exit_code    = E_FAILURE;
    list_node_t * current_node = NULL;

    if ((NULL == list) || (NULL == action_function))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    current_node = list->head;

    for (size_t idx = 0; idx < list->size; idx++)
    {
        action_function(current_node);
        current_node = current_node->next;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void * list_find_first_occurrence(list_t * list, void ** search_data)
{
    list_node_t * current_node = NULL;

    if (NULL == list)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    current_node = list->head;

    for (size_t idx = 0; idx < list->size; idx++)
    {
        if (EQUAL ==
            (list->compare_func(*search_data,
                                ((list_node_t *)(current_node->data))->data)))
        {
            goto END;
        }

        current_node = current_node->next;
    }

    current_node = NULL; // Set back to NULL if no match was found
END:
    return current_node->data;
}

bool list_contains(list_t * list, void * data_p)
{
    bool          result    = false;
    list_node_t * current_p = NULL;

    if ((NULL == list) || (NULL == data_p))
    {
        print_error("list_contains(): NULL argument passed.");
        goto END;
    }

    current_p = list->head;

    while (NULL != current_p)
    {
        if (EQUAL == list->compare_func(current_p->data, data_p))
        {
            result = true;
            goto END;
        }
        current_p = current_p->next;
    }

END:
    return result;
}

void * list_pick_random_item(list_t * list)
{
    int           exit_code    = E_FAILURE;
    list_node_t * node_p       = NULL;
    int           random_index = 0;

    if (NULL == list)
    {
        print_error("list_pick_random_item(): NULL argument passed.");
        goto END;
    }

    if (0 == list->size)
    {
        print_error("list_pick_random_item(): List is empty.");
        goto END;
    }

    exit_code = random_number(0, list->size - 1, &random_index);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "list_pick_random_item(): Unable to generate a random index.");
        goto END;
    }

    node_p = list->head;
    for (int idx = 0; (idx < random_index) && (NULL != node_p); ++idx)
    {
        node_p = node_p->next;
    }

    if (NULL == node_p)
    {
        print_error(
            "list_pick_random_item(): Failed to navigate random index.");
        goto END;
    }

END:
    return node_p->data;
}

list_t * list_find_all_occurrences(list_t * list, void ** search_data)
{
    list_t *      new_list     = NULL;
    list_node_t * current_node = NULL;
    int           check        = E_FAILURE;

    if (NULL == list)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (0 == list->size)
    {
        print_error("Empty list.");
        goto END;
    }

    new_list = list_new(list->custom_free, list->compare_func);
    if (NULL == new_list)
    {
        print_error("Unable to create new list.");
        goto END;
    }

    current_node = list->head;

    // Iterate over the whole list and store any matches in 'new_list'
    for (size_t idx = 0; idx < list->size; idx++)
    {
        if (ERROR != (list->compare_func(search_data, current_node)))
        {
            check = list_push_head(new_list, current_node->data);
            if (E_SUCCESS != check)
            {
                print_error("Unable to push node into list.");
                list_delete(&new_list);
                goto END;
            }
        }
    }

END:
    return new_list;
}

int list_sort(list_t * list)
{
    int exit_code = E_FAILURE;

    if (NULL != list)
    {
        merge_sort(&(list->head), list->compare_func);
        exit_code = E_SUCCESS;
    }

    return exit_code;
}

int list_clear(list_t * list)
{
    int           exit_code    = E_FAILURE;
    list_node_t * current_node = NULL;
    list_node_t * next_node    = NULL;

    if (NULL == list)
    {
        print_error("list_clear(): NULL argument passed.");
        goto END;
    }

    current_node = list->head;
    if (NULL == current_node)
    {
        print_error("list_clear(): Empty list.");
        goto END;
    }

    for (size_t idx = 0; idx < list->size; idx++)
    {
        next_node = current_node->next;

        list->custom_free(current_node->data);
        current_node->data = NULL;
        free(current_node);
        current_node = NULL;
        current_node = next_node;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int list_delete(list_t ** list_address)
{
    int exit_code = E_FAILURE;

    if (NULL == *list_address)
    {
        print_error("list_delete(): NULL argument passed.");
        goto END;
    }

    exit_code = list_clear(*list_address);
    if (E_SUCCESS != exit_code)
    {
        print_error("list_delete(): Unable to clear list.");
        goto END;
    }

    free(*list_address);
    *list_address = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void custom_free(void * mem_addr)
{
    free(mem_addr);
    mem_addr = NULL;
}

/***********************************************************************
 * NOTE: STATIC FUNCTIONS LISTED BELOW
 ***********************************************************************/

static list_node_t * list_node_new(void * data)
{
    list_node_t * new_node = NULL;

    if (NULL == data)
    {
        print_error("NULL argument passed.");
        goto END;
    }

    new_node = calloc(1, sizeof(list_node_t));
    if (NULL == new_node)
    {
        print_error("CMR failure.");
        goto END;
    }

    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = NULL;

END:
    return new_node;
}

static list_node_t * find_node(list_t * list, void * data)
{
    list_node_t * current_node = NULL;

    if ((NULL == list) || (NULL == data))
    {
        print_error("Null argument passed.");
        goto END;
    }

    current_node = list->head;
    while (NULL != current_node)
    {
        if (EQUAL == list->compare_func(data, current_node->data))
        {
            // Success
            goto END;
        }
        current_node = current_node->next;
    }

END:
    return current_node;
}

static void remove_node(list_t * list, list_node_t * node)
{
    if ((NULL == list) || (NULL == node))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (NULL != node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->head = node->next;
    }

    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->tail = node->prev;
    }

    list->custom_free(node->data);
    node->data = NULL;
    free(node);
    node = NULL;

    list->size--;

END:
    return;
}

static void split(list_node_t *  head,
                  list_node_t ** first_half,
                  list_node_t ** second_half)
{
    list_node_t * slow = head;
    list_node_t * fast = head->next;

    // Split the list into two halves by advancing 'fast' two nodes, and 'slow'
    // one node
    while (NULL != fast)
    {
        fast = fast->next;
        if (NULL != fast)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *first_half  = head;
    *second_half = slow->next;
    slow->next   = NULL;
}

static list_node_t * merge(list_node_t * first,
                           list_node_t * second,
                           CMP_F         compare_func)
{
    list_node_t * result = NULL;

    if (NULL == first)
    {
        result = second;
        goto END;
    }

    if (NULL == second)
    {
        result = first;
        goto END;
    }

    // 'first' is less than 'second'
    if (LESS_THAN == compare_func(first->data, second->data))
    {
        result             = first;
        result->next       = merge(first->next, second, compare_func);
        result->next->prev = result;
        result->prev       = NULL;
    }
    // 'first' is greater than or equal to 'second'
    else
    {
        result             = second;
        result->next       = merge(first, second->next, compare_func);
        result->next->prev = result;
        result->prev       = NULL;
    }

END:
    return result;
}

static void merge_sort(list_node_t ** head_ref, CMP_F compare_func)
{
    list_node_t * head = *head_ref;
    list_node_t * first_half;
    list_node_t * second_half;

    if ((NULL == head) || (NULL == head->next))
    {
        return;
    }

    split(head, &first_half, &second_half);

    merge_sort(&first_half, compare_func);
    merge_sort(&second_half, compare_func);

    *head_ref = merge(first_half, second_half, compare_func);
}

/*** end of file ***/