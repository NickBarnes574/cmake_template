/**
 * @file thread_routines.h
 *
 * @brief Declares the main thread function used by the thread pool to handle
 * incoming client sessions, from handshake to full file transfer.
 */
#ifndef _THREAD_ROUTINES_H
#define _THREAD_ROUTINES_H

void * client_session_worker(void * arg_p);

#endif /* _THREAD_ROUTINES_H */

/*** end of file ***/
