/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2002
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
   Revision 1.19.6.1  2002/05/27 18:15:59  wenger
   Got DEVise to compile with gcc 2.96 (so I can compile it at NRG).

   Revision 1.19  2001/11/07 22:31:36  wenger
   Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
   js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
   accidentally merged onto the bmrb_dist_br branch previously).
   (These changes include JS heartbeat improvements and the fix to get
   CGI mode working again.)

   Revision 1.18.2.1  2001/11/07 17:22:26  wenger
   Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
   handle it; got CGI mode working again (bug 723).  (Changed JS version
   to 5.0 and protocol version to 9.0.)

   Revision 1.18  2001/10/12 18:43:56  wenger
   Changed JavaScreen ID to 8 bytes; changed JS protocol version to 8.0
   (changes to match the latest JavaScreen code).

   Revision 1.17  2001/10/12 18:30:26  wenger
   Fixed problems with network header reading/writing code (old code
   assumed that the NetworkHeader struct would never contain any extra
   space for alignment, other problems).

   Revision 1.16  2001/01/08 20:32:52  wenger
   Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
   back onto the trunk.

   Revision 1.14.10.3  2000/12/14 00:44:14  wenger
   Devised, devisec, and deviseb now send/receive JavaScreen ID and CGI
   flag with all commands (matching the latest JavaScreen/jspop code).

   Revision 1.14.10.2  2000/11/22 18:18:08  wenger
   Made changes for DEVise server and client to read/write JS ID and CGI
   flag in every command (currently disabled until the Java side is ready).

   Revision 1.14.10.1  2000/09/01 18:26:43  wenger
   Merged changes from js_cgi_base to fixed_bug_616 onto the branch.

   Revision 1.15  2000/09/01 17:33:28  wenger
   Fixed bug 616 (problem with socket-reading code causing heavy CPU usage
   in certain cases).

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
  // Note: if you add a new member to this struct, you MUST!!! add a
  // corresponding member to NetworkHeaderBuf below.

  u_short msgType;                      // type of message (see API_CMD, etc.
  					// in ParseAPI.h)
  int jsId;				// JavaScreen ID
  u_short useCgi;			// (boolean) whether to use CGI
  u_short nelem;                        // number of elements in message
  u_short size;                         // total size of message
} NetworkHeader;

typedef struct {
  // Note: this purpose of this struct is to provide an equivalent of
  // Network Header that is guaranteed to have no "padding", so we can
  // read and write it without problems. RKW 2001-10-12.

  unsigned char		msgType[sizeof(u_short)];
  unsigned char		jsId[sizeof(int)];
  unsigned char		useCgi[sizeof(u_short)];
  unsigned char		nelem[sizeof(u_short)];
  unsigned char		size[sizeof(u_short)];
} NetworkHeaderBuf;

// Do *NOT* use sizeof(NetworkHeader) to read/write to/from sockets.
const int NetworkHeaderSize = sizeof(NetworkHeaderBuf);

typedef enum{
	CONNECT_ONCE, CONNECT_ALWAYS
}ConnectMode;

// Open a connection to a DEVise server on the given host and port;
// returns non-negative socket fd on success, -1 on failure.
extern int NetworkOpen(char *hostName, int port);
extern int NetworkModedOpen(char *hostName, int port, 
		ConnectMode mode, int seconds);

// Close network connection.  Returns 1.
extern int NetworkClose(int fd);

// Send a message on the socket with the given file descriptor.
// If bracket is non-zero, curly brackets are added around each argument.
// Returns 1 on success, -1 on failure.
extern int NetworkSend(int fd, u_short flag, u_short bracket,
		       int argc, char **argv);

// Receive a message.  Returns 1 on success, -1 on failure, 0 if no message
// is available.  argv points into a static buffer.
extern int NetworkReceive(int fd, int block, u_short &flag,
			  int &argc, char **&argv,
			  int maxRetries = 0);

// Returns a dynamically-allocated string containing all of the specified
// arguments, separated with spaces.  Returns NULL on failure.
extern char *NetworkPaste(int argc, char **argv);

// Set the given connection to non-blocking mode.  Returns 1 on success,
// -1 on failure.
extern int NetworkNonBlockMode(int fd);

// Set the given connection to blocking mode.  Returns 1 on success,
// -1 on failure.
extern int NetworkBlockMode(int fd);

// Extract information from a network-formatted header buffer.
extern void NetworkAnalyzeHeader(const char *headerbuf, int &numElements,
    int &tsize); 

// Parse the given buffer into arguments (recBuff should contain the
// received bytes *without* the header).  Returns 1 on success, -1 on
// failure.  numElements is *input*; av points into recBuff.
extern int NetworkParse(const char *recBuff, int numElements, char **&av);

// Assemble the given arguments into "DEVise message" format.  recBufferp
// will contain a pointer to a *static* buffer containing the formatted
// bytes.  Returns the number of messages bytes on success, -1 on failure.
extern int NetworkPrepareMsg(u_short flag, u_short bracket,
		       int argc, char **argv, char** recBufferp);

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
