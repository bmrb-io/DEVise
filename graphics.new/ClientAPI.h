/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
   Revision 1.14  1998/05/02 09:00:37  taodb
   Added support for JAVA Screen and command logging

   Revision 1.13  1998/02/12 17:16:10  wenger
   Merged through collab_br_2; updated version number to 1.5.1.

   Revision 1.12.6.1  1998/01/28 22:43:43  taodb
   Added support for group communicatoin

   Revision 1.12  1997/09/26 02:10:25  taodb
   Seperated message preparation from send

   Revision 1.11  1997/01/21 19:01:41  wenger
   Minor changes to get Linux compile to work, updated Linux dependencies.

   Revision 1.10  1997/01/13 18:08:05  wenger
   Fixed bugs 043, 083, 084, 091, 114.

 */

#ifndef ClientAPI_h
#define ClientAPI_h

#include <sys/types.h>

#if defined(SUN) || defined(AIX) || defined(ULTRIX)
#include "missing.h"
#endif

#if !defined(FD_SET) && !defined(LINUX)
#define fd_set int
#define FD_ZERO(fdset) memset(fdset, 0, sizeof fdset)
#define FD_SET(fd,fdset) (*fdset |= 1 << fd);
#define FD_CLR(fd,fdset) (*fdset &= ~(1 << fd));
#define FD_ISSET(fd,fdset) (*fdset & (1 << fd))
#endif

#include "ParseAPI.h"

typedef struct {
  u_short flag;                         // type of message
  u_short nelem;                        // number of elements in message
  u_short size;                         // total size of message
} NetworkHeader;
typedef enum{
	CONNECT_ONCE, CONNECT_ALWAYS
}ConnectMode;

extern char *NetworkPaste(int argc, char **argv);
extern int NetworkNonBlockMode(int fd);
extern int NetworkBlockMode(int fd);
extern int NetworkOpen(char *hostName, int port);
extern int NetworkModedOpen(char *hostName, int port, 
		ConnectMode mode, int seconds);
extern void NetworkAnalyseHeader(const char *headerbuf, int& numElements, int&tsize); 
extern int NetworkParse(const char *recBuff, int numElements, char **&av);
extern int NetworkReceive(int fd, int block, u_short &flag,
			  int &argc, char **&argv,
			  int maxRetries = 0);
extern int NetworkSend(int fd, u_short flag, u_short bracket,
		       int argc, char **argv);
extern int NetworkPrepareMsg(u_short flag, u_short bracket,
		       int argc, char **argv, char** recBufferp);
extern int NetworkClose(int fd);

//
// Dataport is for one-time connection
// Image port is for dedicated connection
const int DefaultNetworkPort = 6100;
const int DefaultDataPort = 6200;
const int DefaultSwitchPort = 6300;
const int DefaultImagePort = 6400; 

const int DefaultMaxClients = 10;
#define DefaultSwitchName  ("NULL")
const int BUF_SIZE = 1024;

#endif
