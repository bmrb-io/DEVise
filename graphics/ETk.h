
#ifndef _ETKCLIENT_H_
#define _ETKCLIENT_H_

#define ETK_MAX_STR_LENGTH 200
#define ETK_PORT 6700

#define BEGIN_ETK_TRACE(s)
#define END_ETK_TRACE(s)
//#define BEGIN_ETK_TRACE(s) fprintf(stderr, "BEGIN %s\n", s)
//#define END_ETK_TRACE(s) fprintf(stderr, "END %s\n", s)

const int ETK_INVALID_HANDLE = -1;

/* EmbeddedTk client: opens a connection to the ETk server on the
   server "servName" and returns a file descriptor. In case of errors,
   it will return a negative value, and a brief description of the
   error in "errmsg".  errmsg must point to a buffer of
   ETK_MAX_STR_LENGTH bytes. */

extern int ETkClient(const char *servName, char *errmsg,
		     int port = ETK_PORT);

#endif

