#ifndef TASVIR_PATRON_H
#define TASVIR_PATRON_H

#include "Tasvir.h"

/***********************************************************
**  Tasvir client: opens a connection to the Tasvir server**
**  on the server servName and returns a file descriptor. **
**  In case of errors, it will return a negative value,   **
**  and a brief description of the error in "errmsg".     **
**  errmsg must point to a buffer of TASVIR_MAX_STR_LENGTH**
**  bytes.                                                **
**  If the server was started with the "-port <portnum>"  **
**  option, use that portnum. Otherwise use TASVIR_PORT   **
***********************************************************/

extern int TasvirPatron(char *servName, char *errmsg, int portnum);

#endif

