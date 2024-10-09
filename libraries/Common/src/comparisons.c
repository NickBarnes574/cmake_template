#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comparisons.h"
#include "utilities.h"

typedef struct
{
    int row;
    int col;
} cell_t;

comp_rtns_t node_ptr_comp(void * ptr_1, void * ptr_2)
{
    comp_rtns_t result = ERROR;

    if ((NULL == ptr_1) || (NULL == ptr_2))
    {
        print_error("node_ptr_comp(): NULL argument passed.");
        goto END;
    }

    if (ptr_1 == ptr_2)
    {
        result = EQUAL;
    }

    result = NOT_EQUAL;

END:
    return result;
}

comp_rtns_t int_comp(void * p_node_one_data, void * p_node_two_data)
{
    comp_rtns_t result   = ERROR;
    int         data_one = 0;
    int         data_two = 0;

    if ((NULL == p_node_one_data) || (NULL == p_node_two_data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    data_one = *(int *)p_node_one_data;
    data_two = *(int *)p_node_two_data;

    if (data_one == data_two)
    {
        result = EQUAL;
        goto END;
    }

    if (data_one > data_two)
    {
        result = GREATER_THAN;
        goto END;
    }

    if (data_one < data_two)
    {
        result = LESS_THAN;
        goto END;
    }

END:
    return result;
}

comp_rtns_t str_comp(void * p_node_one_data, void * p_node_two_data)
{
    comp_rtns_t result            = ERROR;
    char *      data_one_p        = NULL;
    char *      data_two_p        = NULL;
    int         comparison_result = 0;

    if ((NULL == p_node_one_data) || (NULL == p_node_two_data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    data_one_p = (char *)p_node_one_data;
    data_two_p = (char *)p_node_two_data;

    comparison_result =
        strncmp(data_one_p, data_two_p, strnlen(data_one_p, 1000));

    if (0 == comparison_result)
    {
        result = EQUAL;
        goto END;
    }

    if (0 > comparison_result)
    {
        result = LESS_THAN;
        goto END;
    }

    if (0 < comparison_result)
    {
        result = GREATER_THAN;
        goto END;
    }

END:
    return result;
}

comp_rtns_t cell_comp(void * p_node_one_data, void * p_node_two_data)
{
    comp_rtns_t result   = ERROR;
    cell_t *    cell_one = NULL;
    cell_t *    cell_two = NULL;

    if ((NULL == p_node_one_data) || (NULL == p_node_two_data))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    cell_one = (cell_t *)p_node_one_data;
    cell_two = (cell_t *)p_node_two_data;

    // First compare by row
    if (cell_one->row < cell_two->row)
    {
        result = LESS_THAN;
        goto END;
    }
    else if (cell_one->row > cell_two->row)
    {
        result = GREATER_THAN;
        goto END;
    }
    else
    {
        // If rows are equal, compare by column
        if (cell_one->col < cell_two->col)
        {
            result = LESS_THAN;
            goto END;
        }
        else if (cell_one->col > cell_two->col)
        {
            result = GREATER_THAN;
            goto END;
        }
        else
        {
            result = EQUAL;
            goto END;
        }
    }

END:
    return result;
}

/*** end of file ***/