#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @NOTES:
 *
 * [1] - It's good practice to initialize all your variables at the top of
 * the function. Additionally, set them to a value that makes sense. For
 * integers, this is typically 0 or -1. For pointers, it's usually NULL. In
 * this case, we have a start position for the light of power and Thor.
 *
 * [2] - It's best practice to always check pointer for NULL whenever they are
 * being passed as parameters into a function.
 *
 * [3] - I prefer using `fgets()` over `scanf()` for the following reasons:
 * 1. Safety: Reduces risk of buffer overflow attacks
 * 2. Automatically handles newline characters
 * 3. Better error handling
 *
 * [4] - Using `strncmp()` here as opposed to the more widely used `strcmp()`,
 * because `strcmp()` has the potential risk of buffer overflow attacks.
 * Specifying the maximum number of bytes to compare against is a safe way to
 * ensure you won't overflow the buffer.
 *
 * [5] - `main()` is the entry point to your program. It shouldn't handle all
 * the logic for your game, but rather call a series of functions that handle
 * all the game logic. Additionally, it's good practice to write your `main()`
 * function in such a way that it can give other programmers a 10,000 meter view
 * of how your code works, in an easy, digestable way. In other words, it should
 * be easy to understand what the program is doing at a high-level, just by
 * looking at `main()`.
 *
 * [6] - Using return codes or "exit_codes" (as I call them) is a great way to
 * introduce error handling into your programs. If the function succeeds at what
 * it was supposed to do, return SUCCESS (0). If it didn't, return FAILURE (-1).
 *
 * [7] - At work we use gotos and END labels. This is optional, I wouldn't
 * really worry about using them if you don't want to. The reason we do is so
 * that we only have one single return per function. It helps to clean up the
 * code, but you'll almost never see this when looking at code on the internet,
 * so I'll leave it up to you to decide whether or not you want to use GOTOs.
 *
 */

#define FAILURE              (-1) // Used to signify a success state
#define SUCCESS              0    // Used to signify a failure state
#define MAX_INPUT_LENGTH     50   // A reasonable maximum string length
#define MAX_DIRECTION_LENGTH 3    // The max length a direction can be
#define MAP_WIDTH            40
#define MAP_HEIGHT           18
#define GOAL_X               26 // X position of the light of power
#define GOAL_y               15 // Y position of the light of power
#define START_X              14 // X coordinate for Thor's starting position
#define START_Y              18 // Y coordinate for Thor's starting position

typedef enum direction
{
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST,
} direction_t;

/**
 * @brief Determines the direction to move
 *
 * @param direction A pointer to a direction enum type
 * @return int Returns SUCCESS on success, otherwise Failure
 */
static int determine_direction_to_move(direction_t * direction);

/**
 * @brief Moves Thor in a specified direction
 *
 * @param direction The desired direction to move
 * @param thor_x A pointer to Thor's current x position
 * @param thor_y A pointer to Thor's current y position
 * @return int Returns SUCCESS on success, otherwise FAILURE
 */
static int move_direction(direction_t direction, int * thor_x, int * thor_y);

/**
 * @brief Checks to see if Thor has found the light of power
 *
 * @param thor_x Thor's x-position
 * @param thor_y Thor's y-position
 * @param light_x Light x-position
 * @param light_y Light y-position
 * @return bool Returns true if win condition was met
 */
static bool check_win_condition(int thor_x,
                                int thor_y,
                                int light_x,
                                int light_y);

int main(void)
{
    /* NOTE: [6]*/
    /* NOTE: [5]*/
    /* NOTE: [1]*/
    int         exit_code   = FAILURE;
    int         light_x     = GOAL_X;
    int         light_y     = GOAL_y;
    int         thor_x      = START_X;
    int         thor_y      = START_Y;
    int         steps_taken = 0;
    direction_t direction   = -1;
    bool        has_won     = false;

    printf("Starting Game...\n");

    while (has_won == false)
    {
        printf("Thor is at: X[%d] Y[%d]\n", thor_x, thor_y);
        printf("Goal is at: X[%d] Y[%d]\n", light_x, light_y);
        printf("Steps taken: %d\n", steps_taken);
        printf("--------------------------\n");

        exit_code = determine_direction_to_move(&direction);
        if (FAILURE == exit_code)
        {
            fprintf(stderr, "ERROR: Unable to determine direction.\n");
            continue;
        }

        exit_code = move_direction(direction, &thor_x, &thor_y);
        if (FAILURE == exit_code)
        {
            fprintf(stderr, "ERROR: Unable to move direction.\n");
            continue;
        }

        has_won = check_win_condition(thor_x, thor_y, light_x, light_y);
        if (has_won == true)
        {
            printf("Congratulations! You won!\n");
        }

        steps_taken += 1;
    }

END:
    return exit_code;
}

static int determine_direction_to_move(direction_t * direction)
{
    int    exit_code               = FAILURE;
    char   input[MAX_INPUT_LENGTH] = { 0 };
    char * result                  = NULL;
    size_t result_length           = 0;

    /* NOTE: [2]*/
    if (direction == NULL)
    {
        fprintf(stderr, "ERROR: NULL pointer argument.\n");
        /*NOTE: [7]*/
        goto END;
    }

    printf("Enter a direction: ");
    /* NOTE: [3]*/
    result = fgets(input, MAX_INPUT_LENGTH, stdin);
    if (NULL == result)
    {
        fprintf(stderr, "ERROR: Input error.\n");
        goto END;
    }

    result_length = strlen(result);
    if (result_length > MAX_DIRECTION_LENGTH)
    {
        fprintf(stderr, "ERROR: Input is too long.\n");
        goto END;
    }

    /* NOTE: [4]*/
    if (0 == strncmp(result, "N", MAX_DIRECTION_LENGTH))
    {
        *direction = NORTH;
    }
    else if (0 == strncmp(result, "NE", MAX_DIRECTION_LENGTH))
    {
        *direction = NORTH_EAST;
    }
    else if (0 == strncmp(result, "E", MAX_DIRECTION_LENGTH))
    {
        *direction = EAST;
    }
    else if (0 == strncmp(result, "SE", MAX_DIRECTION_LENGTH))
    {
        *direction = SOUTH_EAST;
    }
    else if (0 == strncmp(result, "S", MAX_DIRECTION_LENGTH))
    {
        *direction = SOUTH;
    }
    else if (0 == strncmp(result, "SW", MAX_DIRECTION_LENGTH))
    {
        *direction = SOUTH_WEST;
    }
    else if (0 == strncmp(result, "W", MAX_DIRECTION_LENGTH))
    {
        *direction = WEST;
    }
    else if (0 == strncmp(result, "NW", MAX_DIRECTION_LENGTH))
    {
        *direction = NORTH_WEST;
    }
    else
    {
        fprintf(stderr, "ERROR: Invalid direction.\n");
        goto END;
    }

    exit_code = SUCCESS;
END:
    return exit_code;
}

static int move_direction(direction_t direction, int * thor_x, int * thor_y)
{
    int exit_code = FAILURE;
    int current_x = 0;
    int current_y = 0;

    if ((NULL == thor_x) || (NULL == thor_y))
    {
        fprintf(stderr, "ERROR: NULL pointer argument.\n");
        goto END;
    }

    current_x = *thor_x;
    current_y = *thor_y;

    switch (direction)
    {
        case NORTH:
            current_y -= 1;
            break;

        case NORTH_EAST:
            current_x += 1;
            current_y -= 1;
            break;

        case EAST:
            current_x += 1;
            break;

        case SOUTH_EAST:
            current_x += 1;
            current_y += 1;
            break;

        case SOUTH:
            current_y += 1;
            break;

        case SOUTH_WEST:
            current_x -= 1;
            current_y += 1;
            break;

        case WEST:
            current_x -= 1;
            break;

        case NORTH_WEST:
            current_x -= 1;
            current_y -= 1;
            break;

        default:
            break;
    }

    // Check to see if Thor has gone off of the map
    if ((current_x > 40) || (current_x < 0) || (current_y > 18) ||
        (current_y < 0))
    {
        fprintf(stderr, "ERROR: Out of bounds.\n");
        goto END;
    }

    // Update Thor's position
    *thor_x = current_x;
    *thor_y = current_y;

    exit_code = SUCCESS;
END:
    return exit_code;
}

static bool check_win_condition(int thor_x,
                                int thor_y,
                                int light_x,
                                int light_y)
{
    bool result = false;

    if ((thor_x == light_x) && (thor_y == light_y))
    {
        result = true;
    }

    return result;
}