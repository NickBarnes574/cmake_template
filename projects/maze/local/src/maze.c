#include <stdbool.h>

#include "comparisons.h"
#include "file_io.h"
#include "linked_list.h"
#include "maze.h"
#include "number_generator.h"
#include "stack.h"
#include "utilities.h"

#define NORTH 1
#define EAST  2
#define SOUTH 3
#define WEST  4

void placeholder_free(void * data_p)
{
    return;
}

struct cell_t
{
    int row;
    int col;
};

struct maze_t
{
    cell_t   start_p;
    cell_t   end_p;
    uint32_t width;
    uint32_t height;
    uint32_t total_cells;
    char     maze_graph[MAX_ROWS][MAX_COLS];
};

struct path_t
{
    cell_t   cells[MAX_ROWS * MAX_COLS];
    uint32_t path_length;
};

// const cell_t NORTH = { -1, 0 };
// const cell_t EAST  = { 0, 1 };
// const cell_t SOUTH = { 1, 0 };
// const cell_t WEST  = { 0, -1 };

static int  validate_perimeter(maze_t * maze_p);
static int  validate_path(maze_t * maze_p);
static int  is_valid_move(maze_t * maze_p, cell_t * cell_p, bool * is_valid_p);
static int  verify_maze_integrity(maze_t * maze_p);
static void maze_print(maze_t * maze_p);
static int  choose_direction(maze_t * maze_p,
                             cell_t * current_cell_p,
                             int *    direction_p);

int process_maze(const char * filepath_p)
{
    int      exit_code = E_FAILURE;
    maze_t * maze_p    = NULL;

    if (NULL == filepath_p)
    {
        print_error("process_maze(): NULL argument passed.");
        goto END;
    }

    maze_p = calloc(1, sizeof(maze_t));
    if (NULL == maze_p)
    {
        print_error("process_maze(): CMR failure.");
        goto END;
    }

    exit_code = read_maze_from_file(filepath_p, maze_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("process_maze(): Unable to read maze from file.");
        goto END;
    }

    exit_code = verify_maze_integrity(maze_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("process_maze(): Unable to verify maze integrity.");
        goto END;
    }

    maze_print(maze_p);

    exit_code = E_SUCCESS;
END:
    free(maze_p);
    maze_p = NULL;
    return exit_code;
}

static int validate_perimeter(maze_t * maze_p)
{
    int      exit_code = E_FAILURE;
    uint32_t row       = 0;
    uint32_t col       = 0;

    if (NULL == maze_p)
    {
        print_error("validate_perimeter(): NULL argument passed.");
        goto END;
    }

    for (row = 0; row < maze_p->height; ++row)
    {
        for (col = 0; col < maze_p->width; ++col)
        {
            if ((0 == row) || ((maze_p->height - 1) == row) || (0 == col) ||
                (col == (maze_p->width - 1)))
            {
                if ('#' != (maze_p->maze_graph[row][col]))
                {
                    print_error("validate_perimeter(): Perimeter is invalid.");
                    goto END;
                }
            }
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int is_valid_move(maze_t * maze_p, cell_t * cell_p, bool * is_valid_p)
{
    int  exit_code = E_FAILURE;
    bool is_valid  = false;

    if ((NULL == maze_p) || (NULL == cell_p) || (NULL == is_valid_p))
    {
        print_error("is_valid_move(): NULL argument passed.");
        goto END;
    }

    if ((0 > cell_p->col) || (0 > cell_p->row) ||
        ((maze_p->height - 1) < cell_p->row) ||
        (maze_p->width - 1) < cell_p->col)
    {
        print_error("is_valid_move(): Invalid move. Out of bounds.");
        exit_code = E_SUCCESS;
        goto END;
    }

    if (maze_p->maze_graph[cell_p->row][cell_p->col] == '#')
    {
        print_error(
            "is_valid_move(): Invalid move. Wall is obstructing the path.");
        exit_code = E_SUCCESS;
        goto END;
    }

    is_valid  = true;
    exit_code = E_SUCCESS;
END:
    *is_valid_p = is_valid;
    return exit_code;
}

static int choose_direction(maze_t * maze_p,
                            cell_t * current_cell_p,
                            int *    direction_p)
{
    int      exit_code        = E_FAILURE;
    list_t * direction_list_p = NULL;
    bool     is_valid         = false;
    cell_t   temp_cell        = { 0 };

    int directions[4] = { NORTH, EAST, SOUTH, WEST };

    if ((NULL == maze_p) || (NULL == current_cell_p))
    {
        print_error("choose_direction(): NULL argument passed.");
        goto END;
    }

    direction_list_p = list_new(placeholder_free, int_comp);
    if (NULL == direction_list_p)
    {
        print_error("choose_direction(): CMR failure.");
        goto END;
    }

    // Check North
    temp_cell.row = current_cell_p->row;
    temp_cell.col = current_cell_p->col;
    exit_code     = is_valid_move(maze_p, &temp_cell, &is_valid);
    if (E_SUCCESS != exit_code)
    {
        print_error("choose_direction(): Error validating move.");
        goto END;
    }
    if (true == is_valid)
    {
        exit_code = list_push_head(direction_list_p, &directions[0]);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "choose_direction(): Unable to push direction onto list.");
            goto END;
        }
    }
    is_valid = false;

    // Check East
    temp_cell.row = current_cell_p->row;
    temp_cell.col = current_cell_p->col;
    exit_code     = is_valid_move(maze_p, &temp_cell, &is_valid);
    if (E_SUCCESS != exit_code)
    {
        print_error("choose_direction(): Error validating move.");
        goto END;
    }
    if (true == is_valid)
    {
        exit_code = list_push_head(direction_list_p, &directions[1]);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "choose_direction(): Unable to push direction onto list.");
            goto END;
        }
    }
    is_valid = false;

    // Check South
    temp_cell.row = current_cell_p->row;
    temp_cell.col = current_cell_p->col;
    exit_code     = is_valid_move(maze_p, &temp_cell, &is_valid);
    if (E_SUCCESS != exit_code)
    {
        print_error("choose_direction(): Error validating move.");
        goto END;
    }
    if (true == is_valid)
    {
        exit_code = list_push_head(direction_list_p, &directions[2]);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "choose_direction(): Unable to push direction onto list.");
            goto END;
        }
    }
    is_valid = false;

    // Check West
    temp_cell.row = current_cell_p->row;
    temp_cell.col = current_cell_p->col;
    exit_code     = is_valid_move(maze_p, &temp_cell, &is_valid);
    if (E_SUCCESS != exit_code)
    {
        print_error("choose_direction(): Error validating move.");
        goto END;
    }
    if (true == is_valid)
    {
        exit_code = list_push_head(direction_list_p, &directions[3]);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "choose_direction(): Unable to push direction onto list.");
            goto END;
        }
    }
    is_valid = false;

    *direction_p = list_pick_random_item(direction_list_p);

    exit_code = E_SUCCESS;
END:
    list_delete(&direction_list_p);
    return exit_code;
}

static int validate_path(maze_t * maze_p)
{
    int       exit_code             = E_FAILURE;
    int       is_empty              = E_FAILURE;
    stack_t * stack_p               = NULL;
    list_t *  visted_p              = NULL;
    cell_t *  current_cell_p        = NULL;
    cell_t *  next_cell_p           = NULL;
    int       direction             = 0;
    bool      move_is_valid         = false;
    bool      cell_has_been_visited = false;

    if (NULL == maze_p)
    {
        print_error("validate_path(): NULL argument passed.");
        goto END;
    }

    stack_p = stack_init(maze_p->total_cells, placeholder_free);
    if (NULL == stack_p)
    {
        print_error("validate_path(): Unable to create stack.");
        goto END;
    }

    visted_p = list_new(placeholder_free, cell_comp);
    if (NULL == visted_p)
    {
        print_error("validate_path(): Unable to create visited list.");
        goto END;
    }

    current_cell_p = calloc(1, sizeof(current_cell_p));
    if (NULL == current_cell_p)
    {
        print_error("validate_path(): CMR failure.");
        goto END;
    }

    *current_cell_p = maze_p->start_p;

    exit_code = stack_push(stack_p, &current_cell_p);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "validate_path(): Unable to add starting cell to the stack.");
        goto END;
    }

    exit_code = list_push_head(visted_p, &current_cell_p);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "validate_path(): Unable to add starting cell to visited "
            "list.");
        goto END;
    }

    int iteration = 0;
    while (E_SUCCESS != stack_emptycheck(stack_p))
    {
        // DEBUG
        printf("\nITERATION [%d]:\n", iteration);
        // DEBUG

        exit_code = choose_direction(maze_p, current_cell_p, &direction);
        if (E_SUCCESS != exit_code)
        {
            print_error("validate_path(): Unable to choose direction.");
            goto END;
        }

        // DEBUG
        printf("1. Chose a random direction.\n");
        // DEBUG

        next_cell_p = calloc(1, sizeof(cell_t));
        switch (direction)
        {
            case NORTH:
                printf("NORTH\n");
                next_cell_p->row = current_cell_p->row - 1;
                next_cell_p->col = current_cell_p->col;
                break;

            case EAST:
                printf("EAST\n");
                next_cell_p->row = current_cell_p->row;
                next_cell_p->col = current_cell_p->col + 1;
                break;

            case SOUTH:
                printf("SOUTH\n");
                next_cell_p->row = current_cell_p->row + 1;
                next_cell_p->col = current_cell_p->col;
                break;

            case WEST:
                printf("WEST\n");
                next_cell_p->row = current_cell_p->row;
                next_cell_p->col = current_cell_p->col - 1;
                break;

            default:
                break;
        }

        cell_has_been_visited = list_contains(visted_p, &next_cell_p);

        if (cell_has_been_visited && move_is_valid)
        {
            stack_push(stack_p, &next_cell_p);
            list_push_head(visted_p, &next_cell_p);
            // DEBUG
            printf("8a. Node has not been visted, so add it to the list.\n");
            // DEBUG
        }
        else
        {
            free(next_cell_p);
            next_cell_p = NULL;
            // DEBUG
            printf("8b. Node has already been visited so free it.\n");
            // DEBUG
        }

        // Check to see if the exit has been found
        if ((current_cell_p->row == maze_p->end_p.row) &&
            (current_cell_p->col == maze_p->end_p.col))
        {
            exit_code = E_SUCCESS;
            goto END;
        }
        // DEBUG
        printf("4. Checked to see if the exit has been found.\n");
        // DEBUG

        // Mark the path
        printf("maze[%d][%d] = %c\n",
               current_cell_p->row,
               current_cell_p->col,
               maze_p->maze_graph[current_cell_p->row][current_cell_p->col]);
        maze_p->maze_graph[current_cell_p->row][current_cell_p->col] = '*';
        // DEBUG
        printf("5. Marked the path.\n");
        // DEBUG
        iteration++;
    }

    exit_code = E_SUCCESS;
END:
    free(stack_p);
    stack_p = NULL;
    free(visted_p);
    visted_p = NULL;
    return exit_code;
}

static int verify_maze_integrity(maze_t * maze_p)
{
    int exit_code = E_FAILURE;

    if (NULL == maze_p)
    {
        print_error("verify_maze_integrity(): NULL argument passed.");
        goto END;
    }

    exit_code = validate_perimeter(maze_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("verify_maze_integrity(): Unable to validate perimeter.");
        goto END;
    }

    exit_code = validate_path(maze_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("verify_maze_integrity(): Unable to validate path.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int read_maze_from_file(const char * filepath_p, maze_t * maze_p)
{
    int    exit_code = E_FAILURE;
    FILE * file_p    = NULL;
    char * line_p    = NULL;
    int    row       = 0;

    if ((NULL == filepath_p) || (NULL == maze_p))
    {
        print_error("read_maze_from_file(): NULL argument passed.");
        goto END;
    }

    file_p = fopen(filepath_p, "r");
    if (NULL == file_p)
    {
        print_error("read_maze_from_file(): Unable to open file.");
        goto END;
    }

    do
    {
        exit_code = read_line(file_p, &line_p);
        if (E_FAILURE == exit_code)
        {
            print_error(
                "read_maze_from_file(): Unable to read line from file.");
            goto END;
        }

        for (int col = 0; (line_p[col] != '\n') && (line_p[col] != '\0'); ++col)
        {
            // Store each character in a cell
            maze_p->maze_graph[row][col] = line_p[col];

            // Find the start and end points
            if (line_p[col] == MAZE_START)
            {
                maze_p->start_p.row = row;
                maze_p->start_p.col = col;
            }
            if (line_p[col] == MAZE_END)
            {
                maze_p->end_p.row = row;
                maze_p->end_p.col = col;
            }

            // Update the height and width
            if ((row + 1) > maze_p->height)
            {
                maze_p->height = (row + 1);
            }
            if ((col + 1) > maze_p->width)
            {
                maze_p->width = (col + 1);
            }
        }
        ++row;

    } while (E_SUCCESS == exit_code);

    // Calculate the total number of cells in the maze
    maze_p->total_cells = (maze_p->height * maze_p->width);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void maze_print(maze_t * maze_p)
{
    uint32_t col = 0;
    uint32_t row = 0;

    if (NULL == maze_p)
    {
        print_error("maze_print(): NULL argument passed.");
        goto END;
    }

    for (row = 0; row < maze_p->height; ++row)
    {
        for (col = 0; col < maze_p->width; ++col)
        {
            printf("%c", maze_p->maze_graph[row][col]);
        }
        printf("\n");
    }

END:
    return;
}

/*** end of file ***/
