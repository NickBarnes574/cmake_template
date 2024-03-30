/**
 * @file maze.h
 *
 * @brief
 */
#ifndef _MAZE_H
#define _MAZE_H

#define MAX_ROWS   100
#define MAX_COLS   100
#define MAZE_START '@'
#define MAZE_END   '>'

typedef struct cell_t cell_t;
typedef struct maze_t maze_t;
typedef struct path_t path_t;

int process_maze(const char * filepath_p);
int read_maze_from_file(const char * filepath_p, maze_t * maze_p);

#endif /* _MAZE_H */

/*** end of file ***/
