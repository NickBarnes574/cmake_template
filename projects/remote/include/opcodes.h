/**
 * @file opcodes.h
 *
 * @brief Contains the operation codes and return codes for the client/server
 */
#ifndef _OPCODES_H
#define _OPCODES_H

#define LOGIN     0x00 // Standard login
#define NORML_USR 0x01 // Create a normal user
#define ADMIN_USR 0x02 // Create an admin user

//
// ------------------------------OPERATION CODES------------------------------
//
#define AUTHENTICATION 0x01
#define MESSAGE        0x02
#define CLOSE_CONN     0x03

//
// --------------------------------RETURN CODES--------------------------------
//
#define SUCCESS       0x01 // General success message
#define SESSION_ERROR 0x02 // Invalid/expired session - reauthenticate
#define PERM_ERROR    0x03 // Non-permitted action
#define FAILURE       0xff // General failure message

#endif /* _OPCODES_H */

/*** end of file ***/
