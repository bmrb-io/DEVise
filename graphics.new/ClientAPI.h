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

   $Log$
 */

#ifndef ClientAPI_h
#define ClientAPI_h

#include <sys/types.h>

#if defined(SUN) || defined(AIX) || defined(ULTRIX)
#include "missing.h"
#endif

//TEMPTEMP -- Is this really needed?  It causes compile errors in some
//versions of Linux.  RKW 1/13/97.
#ifndef FD_SET
#define fd_set int
#define FD_ZERO(fdset) memset(fdset, 0, sizeof fdset)
#define FD_SET(fd,fdset) (*fdset |= 1 << fd);
#define FD_CLR(fd,fdset) (*fdset &= ~(1 << fd));
#define FD_ISSET(fd,fdset) (*fdset & (1 << fd))
#endif

#include "ParseAPI.h"

extern char *NetworkPaste(int argc, char **argv);
extern int NetworkNonBlockMode(int fd);
extern int NetworkBlockMode(int fd);
extern int NetworkOpen(char *hostName, int port);
extern int NetworkReceive(int fd, int block, u_short &flag,
			  int &argc, char **&argv);
extern int NetworkSend(int fd, u_short flag, u_short bracket,
		       int argc, char **argv);
extern int NetworkClose(int fd);

const int DefaultNetworkPort = 6100;
const int DefaultDataPort = 6200;
const int BUF_SIZE = 1024;

#endif
