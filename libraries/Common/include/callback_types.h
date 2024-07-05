/**
 * @file callback_types.h
 *
 * @brief
 */
#ifndef _CALLBACK_TYPES_H
#define _CALLBACK_TYPES_H

#include "comparisons.h"

/**
 * @brief Function pointer type for custom free functions.
 */
typedef void (*FREE_F)(void *);

/**
 * @brief Function pointer type for custom action functions.
 */
typedef void (*ACTION_F)(const void *);

/**
 * @brief Function pointer type for custom compare functions.
 */
typedef comp_rtns_t (*CMP_F)(void *, void *);

#endif /* _CALLBACK_TYPES_H */

/*** end of file ***/
