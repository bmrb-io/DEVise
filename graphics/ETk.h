
#ifndef _ETKCLIENT_h_
#define _ETKCLIENT_h_

#define ETK_MAX_STR_LENGTH 200
#define ETK_PORT 6700

/* Embedded Tk client: opens a connection to the ETk server on the
 *   server "servName" and returns a file descriptor. In case of
 *   errors, it will return a negative value, and a brief description
 *   of the error in "errmsg".  errmsg must point to a buffer of
 *   ETK_MAX_STR_LENGTH bytes.  */

extern int ETkClient(const char *servName, char *errmsg,
		     int port = ETK_PORT);

#endif

