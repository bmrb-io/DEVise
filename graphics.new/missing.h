/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$*/

/*
   Some of the missing function prototypes.
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

extern int accept(int, struct sockaddr *, int *);
extern int bind(int, struct sockaddr *, int);
extern int connect(int, struct sockaddr *, int);
extern int getpeername(int, struct sockaddr *, int *);
extern int getsockname(int, struct sockaddr *, int *);
extern int getsockopt(int, int, int, char *, int *);
extern int listen(int, int);
extern int recv(int, char *, int, int);
extern int recvfrom(int, char *, int, int, struct sockaddr *, int *);
extern int send(int, const char *, int, int);
extern int sendto(int, const char *, int, int, struct sockaddr *, int);
extern int setsockopt(int, int, int, const char *, int);
extern int socket(int, int, int);
extern int recvmsg(int, struct msghdr *, int);
extern int sendmsg(int, struct msghdr *, int);
extern int shutdown(int, int);
extern int socketpair(int, int, int, int *);

#ifdef __cplusplus
}
#endif  

#endif
