
/* Some of the missing function prototypes.
 */

#ifndef MISSING_H
#define MISSING_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" 
{
#endif  

extern int socket (int __family, int __type, int __protocol);
extern int bind (int __sockfd, struct sockaddr *__my_addr,
		 int __addrlen);

extern int select (int __width, fd_set * __readfds,
		   fd_set * __writefds, fd_set * __exceptfds,
		   struct timeval * __timeout);

extern int sendto (int __sockfd, __const void *__buff, int __len,
		   unsigned int __flags, __const struct sockaddr *__to,
		   int __tolen);
extern int recvfrom (int __sockfd, void *__buff, int __len,
		     unsigned int __flags, struct sockaddr *__from,
		     int *__fromlen);

extern int setitimer (int __which, __const struct itimerval *__value,
		      struct itimerval *__ovalue);

#ifdef __cplusplus
}
#endif  


#endif /* MISSING_H */
				  
				  

