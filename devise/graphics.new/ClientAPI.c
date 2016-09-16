/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.25.8.3  2005/09/28 22:29:47  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.25.8.2  2005/09/28 17:14:49  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.25.8.1  2005/09/15 19:23:33  wenger
  ClientAPI code now logs errors via the DevError class (added
  DebugLogDummy to the client library so this works for devise
  and collaborator, which don't link the full graphics.new lib).

  Revision 1.25  2001/10/12 18:30:21  wenger
  Fixed problems with network header reading/writing code (old code
  assumed that the NetworkHeader struct would never contain any extra
  space for alignment, other problems).

  Revision 1.24  2001/01/08 20:32:52  wenger
  Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
  back onto the trunk.

  Revision 1.22.2.2  2000/11/22 18:18:07  wenger
  Made changes for DEVise server and client to read/write JS ID and CGI
  flag in every command (currently disabled until the Java side is ready).

  Revision 1.22.2.1  2000/09/01 18:26:42  wenger
  Merged changes from js_cgi_base to fixed_bug_616 onto the branch.

  Revision 1.23  2000/09/01 17:33:26  wenger
  Fixed bug 616 (problem with socket-reading code causing heavy CPU usage
  in certain cases).

  Revision 1.22  1999/11/01 17:53:35  wenger
  More debug info.

  Revision 1.21  1999/05/20 19:32:04  wenger
  Minor improvement to debug output.

  Revision 1.20  1998/11/24 19:31:08  wenger
  Fixed problem with soil science sessions sometimes locking up the
  JavaScreen by disallowing input from file descriptors while waiting for
  queries to finish; JavaScreen support code now omits sending windows for
  which the print exclusion flag is set (allows "control windows") to not
  be displayed in the JavaScreen.

  Revision 1.19  1998/08/17 17:12:01  wenger
  Devised now responds to KeyAction commands from JavaScreen.

  Revision 1.18  1998/08/10 13:48:01  wenger
  Possible (not completely tested yet) fix to interruped system call problems
  being seen in Italy.

  Revision 1.17  1998/05/21 18:18:45  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.16  1998/02/19 23:25:10  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.15  1998/02/12 17:16:09  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.14.6.1  1998/01/28 22:43:42  taodb
  Added support for group communicatoin

  Revision 1.14  1997/09/26 02:10:24  taodb
  Seperated message preparation from send

  Revision 1.13  1996/12/12 22:02:31  jussi
  Disabled debugging messages.

  Revision 1.12  1996/11/01 19:28:17  kmurli
  Added DQL sources to include access to TDataDQL. This is equivalent to
  TDataAscii/TDataBinary. The DQL type in the Tcl/Tk corresponds to this
  class.

  Revision 1.11  1996/08/02 00:13:49  jussi
  Improved performance of network code by preparing data to be
  sent in a buffer and then issuing a single send() call.

  Revision 1.10  1996/06/27 16:40:19  jussi
  The networking code now handles interrupted (EINTR) recv() calls
  properly.

  Revision 1.9  1996/06/27 15:53:43  jussi
  Changed a debugging statement.

  Revision 1.8  1996/05/20 18:42:26  jussi
  Merged with ClientServer library code.

  Revision 1.7  1996/05/15 16:40:59  jussi
  Moved all networking code from ServerAPI.c to ClientAPI.c.
  Improved support for bracketed or non-bracketed arguments
  passed in commands.

  Revision 1.6  1996/05/13 18:10:16  jussi
  All client-server communication is now through a single socket pair.
  Control channel sockets were removed. ClientAPI no longer stores
  sockets in static variables but instead lets the caller of
  these routines to maintain them.

  Revision 1.5  1996/05/11 19:09:18  jussi
  Added error flag and bracket to the client->server communication.

  Revision 1.4  1996/05/11 17:24:09  jussi
  Removed DeviseHost() function which used to read host name and
  port number information from a file. These parameters are
  typically read from the command line instead.

  Revision 1.3  1996/05/11 01:52:17  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>

#include "ClientAPI.h"
#include "Util.h"
#include "DevError.h"

//#define DEBUG

// Convert a NetworkHeader to the corresponding buffer.
static void Header2Buf(const NetworkHeader &hdr, NetworkHeaderBuf &buf);

// Convert a buffer to the corresponding NetworkHeader.
static void Buf2Header(const NetworkHeaderBuf &buf, NetworkHeader &hdr);

// Read a buffer from the file descriptor, retrying if specified.
// Returns 1 on success, -1 on failure.
static int ReadWithRetries(int fd, char *buf, int bufSize, int block,
    int maxRetries);

// Copy bytes to a pointer and increment the pointer by the number of
// bytes copied.
inline void
CopyIncPtr(char *&to, const void *from, int size)
{
  memcpy(to, from, size);
  to += size;
}

// -----------------------------------------------------------------------

int NetworkNonBlockMode(int fd)
{
#ifdef SUN
  int result = fcntl(fd, F_SETFL, FNDELAY);
#else
  int result = fcntl(fd, F_SETFL, O_NDELAY);
#endif
  if (result < 0) {
	reportErrSys("Error setting non-blocking mode");
    return -1;
  }

  return 1;
}

int NetworkBlockMode(int fd)
{
  int result = fcntl(fd, F_SETFL, 0);
  if (result < 0) {
	reportErrSys("Error setting blocking mode");
    return -1;
  }

  return 1;
}

char *NetworkPaste(int argc, const char * const *argv)
{
  if (argc < 1) {
    return strdup("");
  }

  int size = 0;
  int i;
  for(i = 0; i < argc; i++) {
    size += strlen(argv[i]);
  }

  // an extra char for space between each argument
  size += argc - 1;

  // one more for the terminating NULL
  size++;

  char *cmd = new char [size];
  if (!cmd) {
    reportErrNosys("Out of memory\n");
    return NULL;
  }

  char *ptr = cmd;
  *ptr = 0;
  for(i = 0; i < argc; i++) {
    if (i == 0) {
      sprintf(ptr, "%s", argv[i]);
      ptr += strlen(argv[i]);
    } else {
      sprintf(ptr, " %s", argv[i]);
      ptr += strlen(argv[i]) + 1;
    }
  }

  if (ptr - cmd != size - 1) {
    reportErrNosys("Internal error.");
    return NULL;
  }

  return cmd;
}

int NetworkOpen(char *servName, int portNum)
{
	return	NetworkModedOpen(servName, portNum, CONNECT_ONCE,0);
}

int NetworkModedOpen(char *servName, int portNum, ConnectMode mode, int seconds)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
	reportErrSys("Cannot create socket");
    return -1;
  }

  struct hostent *servEnt = gethostbyname(servName);
  if (!servEnt) {
    reportErrSys("Cannot translate server name");
    close(fd);
    return -1;
  }

  if (servEnt->h_addrtype != AF_INET) {
    reportErrNosys("Unsupported address type");
    close(fd);
    return -1;
  }

  struct in_addr *ptr = (struct in_addr *)*servEnt->h_addr_list;

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
	
  servAddr.sin_family = AF_INET;
  servAddr.sin_port   = htons(portNum);
  servAddr.sin_addr   = *ptr;

  int result;
  do {
	void (*func)(int);
	func = signal(SIGPIPE, SIG_IGN);
  	result = connect(fd,(struct sockaddr *)&servAddr,
		       sizeof(struct sockaddr));
	signal(SIGPIPE, func); 
  	if (result < 0) {
		if (mode == CONNECT_ONCE)
		{
    		reportErrSys("Cannot connect to server");
    		close(fd);
    		return -1;
		}
		else if  (mode == CONNECT_ALWAYS)
		{
			sleep(seconds);
			printf("Connecting ...\n");
		}

		close(fd);
  		fd = socket(AF_INET, SOCK_STREAM, 0);
  		if (fd < 0){
    		reportErrSys("Cannot create socket");
    		return -1;
  		}
  	}
  } while (result <0);
  
  return fd;
}

//
//extract header information from a header
void NetworkAnalyzeHeader(const char *headerbuf, int& numElements, int& tsize)
{
  assert(headerbuf!=NULL);

  NetworkHeaderBuf *buf = (NetworkHeaderBuf *)headerbuf;
  NetworkHeader hdr;
  Buf2Header(*buf, hdr);

  numElements = hdr.nelem;
  tsize = hdr.size;

  return;
}

int NetworkReceive(int fd, int block, u_short &msgType, int &ac, char **&av,
	int maxRetries)
{
#if defined(DEBUG)
  printf("NetworkReceive(%d, %d)\n", fd, block);
#endif

  // Note: non-blocking mode doesn't seem to be used.  RKW 2000-09-01.
  if (!block) {
    if (NetworkNonBlockMode(fd) < 0) {
      return -1;
    }
  } else {
    if (NetworkBlockMode(fd) < 0) {
      return -1;
    }
  }

  //
  // Get the message header.
  //
#if defined(DEBUG)
  if (block) {
    printf("Getting header\n");
  }
#endif

  NetworkHeaderBuf hdrBuf;
  if (ReadWithRetries(fd, (char *)&hdrBuf, NetworkHeaderSize, block,
      maxRetries) < 0) {
    return -1;
  }

  NetworkHeader hdr;
  Buf2Header(hdrBuf, hdr);
  msgType = hdr.msgType;

  int numElements = hdr.nelem;
  int tsize = hdr.size;

#if defined(DEBUG)
  printf("  Got msgType %u, numElements = %u, size = %u\n", msgType,
      numElements, tsize);
  cout << "  jsId = " << hdr.jsId << endl;
#endif

  //
  // We've gotten the header, now get the body of the message.
  //
  static char *recBuff = NULL;
  static int recBuffSize = 0;

  // Enlarge the record buffer if necessary.
  if (!recBuff || tsize >= recBuffSize) {
    delete recBuff;
    recBuffSize = tsize + 1;
#if defined(DEBUG)
    printf("Increasing size of recBuff to %d bytes\n", recBuffSize);
#endif
    recBuff = new char [recBuffSize];
    if (!recBuff) {
      reportErrNosys("Out of memory\n");
      return -1;
    }
  }

  // first receive message in chunks of some size
  char *ptr = recBuff;
  int totrem = tsize;
  while(totrem > 0) {
    int res = 0;
    while (true) {
	  errno = 0;
      res = recv(fd, ptr, totrem, 0);
      if (res == 0) {
		// Got nothing.  Note: I'm not sure why here we give up totally if
		// we got nothing, whereas when we're getting the header we keep
		// trying if we got nothing.  RKW 2000-09-01.
        return 0;
	  }
      if (res > 0) {
		// Got at least part of the message.
        break;
	  }

      if (errno == EINTR) {
#if defined(DEBUG)
        printf("Call to recv interrupted, continuing\n");
#endif
        continue;
      }

	  // An error other than EINTR.
	  reportErrSys("Error receiving message body from socket");
      return -1;
    }
    ptr += res;
    totrem -= res;
#if defined(DEBUG)
    printf("Got %d bytes of message, %d remaining\n", res, totrem);
#endif
  }

  if (totrem != 0) {
    reportErrNosys("Invalid protocol message.");
    return -1;
  }

  ac = numElements;
  int result = NetworkParse((const char*)recBuff, ac, av);

#if defined(DEBUG)
  printf("  Message is: ");

  // Note: not using PrintArgs() here because that's not linked into the
  // client.
  int index;
  char *prefix = "";
  for (index = 0; index < ac; index++) {
    printf("%s<%s>", prefix, av[index]);
    prefix = ", ";
  }
  printf("\n");
#endif

  return result;
}

//
// now break buffer into individual elements
int NetworkParse(const char *recBuff, int numElements, char **&av)
{
  static char **argv = 0;
  static int argc = 0;
  const char *ptr;

  // Add more argument buffers if necessary.
  if (numElements > argc) {
    if (argv != 0) {
      delete argv;
	}
    argc = numElements;
#if defined(DEBUG)
    printf("Increasing size of argv to %d elements\n", argc);
#endif
    argv = new char * [argc];
    if (!argv) {
      reportErrNosys("Out of memory");
      return -1;
    }
  }

  av = argv;
  ptr = recBuff;

  for(int i = 0; i < numElements; i++) {
    u_short size;

    memcpy(&size, ptr, sizeof(size));
    ptr += sizeof size;

    size = ntohs(size);

#if defined(DEBUG)
    printf("Element %d is %u bytes\n", i, size);
#endif

    if (ptr[size - 1]) {
      // argument must be terminated with NULL
	  printf(" proper values = %c %c %c \n", ptr[size-2], ptr[size-1],
	      ptr[size]);
      reportErrNosys("Invalid procotol argument");
      return -1;
    }

#if defined(DEBUG)
    printf("Element %d is \"%s\"\n", i, ptr);
#endif
    // Note: casting 'const char *' to 'char *' here to get rid of compiler
    // complaint; that's not really the right thing to do, but I don't want
    // to delve into this more at the present time.  RKW Feb. 17, 1998.
    argv[i] = (char *) ptr;

    ptr += size;
  }

#if defined(DEBUG)
  printf("Parsed complete message\n\n");
#endif

  return 1;
}

int NetworkSend(int fd, u_short msgType, u_short bracket, int argc,
    const char * const *argv)
{
#if defined(DEBUG)
  printf("NetworkSend(%d, %d, %d, ", fd, msgType, bracket);
  printf("args(");
  for (int argNum = 0; argNum < argc; argNum++) {
    printf("<%s>, ", argv[argNum]);
  }
  printf(")\n");
  fflush(stdout);
#endif

  int msgsize;
  char *recBuffer = 0;

  msgsize = NetworkPrepareMsg(msgType, bracket, argc, argv, &recBuffer);
  if (msgsize == -1) {
    return -1;
  }

  if (recBuffer != NULL)
  {
    int result = send(fd, recBuffer, msgsize, 0);
    if (result != msgsize) {
	  reportErrSys("Error sending message");
      return -1;
    }
  }

#if defined(DEBUG)
  printf("Complete message sent\n\n");
#endif

  return 1;
}

int NetworkPrepareMsg(u_short msgType, 
    u_short bracket, int argc, const char * const *argv, char** recBufferp)
{
  static int recBuffSize = 0;
  static char *recBuff = 0;

  //
  // Set up the header.
  //
  NetworkHeader hdr;
  hdr.msgType = msgType;
  hdr.jsId = 0;
  hdr.useCgi = 0;
  hdr.nelem = argc;

  //
  // Calculate the size of the arguments, and put that into the header.
  //
  u_short tsize = 0;
  int i;
  for(i = 0; i < argc; i++) {
    // argument is prefixed with its size and terminated with a NULL
    tsize += sizeof(u_short) + strlen(argv[i]) + 1;
    if (bracket) {
      // if bracketing is needed, an extra set of braces is added
      tsize += 2;
    }
  }
  hdr.size = tsize;

  //
  // "Bufferize" the header.
  //
  NetworkHeaderBuf hdrBuf;
  Header2Buf(hdr, hdrBuf);

  //
  // Calculate the overall message size (header plus arguments).
  //
  u_short msgsize = NetworkHeaderSize + tsize;

  //
  // Enlarge the record buffer if necessary.
  //
  if (!recBuff || msgsize >= recBuffSize) {
    delete recBuff;
    recBuffSize = msgsize + 1;
#if defined(DEBUG)
    printf("Increasing size of recBuff to %d bytes\n", recBuffSize);
#endif
    recBuff = new char [recBuffSize];
    if (!recBuff) {
      reportErrNosys("Out of memory");
      return -1;
    }
  }

  //
  // Copy the header and arguments into the record buffer.
  //
  char *ptr = recBuff;

  CopyIncPtr(ptr, &hdrBuf, NetworkHeaderSize);

  for(i = 0; i < argc; i++) {
    // must send terminating NULL too
    u_short size = strlen(argv[i]) + 1;
    if (bracket) {
      size += 2;
    }
#if defined(DEBUG)
    printf("Sending size of element %d: %u\n", i, size);
#endif
    u_short nsize = htons(size);
    CopyIncPtr(ptr, &nsize, sizeof(nsize));
    if (bracket) {
      size -= 2;
    }

    if (bracket) {
#if defined(DEBUG)
      printf("Sending element %d: \"{%s}\"\n", i, argv[i]);
#endif
      *ptr++ = '{';
      // don't send null of argument, instead send null after closing brace
      CopyIncPtr(ptr, argv[i], size - 1);
      *ptr++ = '}';
      *ptr++ = 0;
    } else {
#if defined(DEBUG)
      printf("Sending element %d: \"%s\"\n", i, argv[i]);
#endif
      CopyIncPtr(ptr, argv[i], size);
    }
  }

  assert(ptr - recBuff == msgsize);

#if defined(DEBUG)
  printf("Sending message: msgType %u, nelem %u, size %u\n", msgType,
      argc, tsize);
  printf("  buffer = <");
  for (int count = 0; count < msgsize; count++) {
    if (isprint(recBuff[count])) {
      printf("%c", recBuff[count]);
    } else {
      printf("0x%x", recBuff[count]);
    }
  }
  printf(">\n");
#endif

  *recBufferp = recBuff;

  return msgsize;
}

int NetworkClose(int fd)
{ 
  close(fd);

  return 1;
}

// -----------------------------------------------------------------------
// Static utility functions.

static void
Header2Buf(const NetworkHeader &hdr, NetworkHeaderBuf &buf)
{
  NetworkHeader myHdr;

  myHdr.msgType = htons(hdr.msgType);

  //TEMP -- we should convert jsId to network order here, but I'm not
  // sure how to do that for a long long; it's not critical because
  // the devised doesn't pay any attention to the jsId right now.
  // RKW 2001-10-12.
  myHdr.jsId = hdr.jsId;

  myHdr.useCgi = htons(hdr.useCgi);
  myHdr.nelem = htons(hdr.nelem);
  myHdr.size = htons(hdr.size);

  memcpy(&buf.msgType, &myHdr.msgType, sizeof(buf.msgType));
  memcpy(&buf.jsId, &myHdr.jsId, sizeof(buf.jsId));
  memcpy(&buf.useCgi, &myHdr.useCgi, sizeof(buf.useCgi));
  memcpy(&buf.nelem, &myHdr.nelem, sizeof(buf.nelem));
  memcpy(&buf.size, &myHdr.size, sizeof(buf.size));
}

static void
Buf2Header(const NetworkHeaderBuf &buf, NetworkHeader &hdr)
{
  NetworkHeader myHdr;

  memcpy(&myHdr.msgType, &buf.msgType, sizeof(buf.msgType));
  memcpy(&myHdr.jsId, &buf.jsId, sizeof(buf.jsId));
  memcpy(&myHdr.useCgi, &buf.useCgi, sizeof(buf.useCgi));
  memcpy(&myHdr.nelem, &buf.nelem, sizeof(buf.nelem));
  memcpy(&myHdr.size, &buf.size, sizeof(buf.size));

  hdr.msgType = ntohs(myHdr.msgType);

  //TEMP -- we should convert jsId to host order here, but I'm not
  // sure how to do that for a long long; it's not critical because
  // the devised doesn't pay any attention to the jsId right now.
  // RKW 2001-10-12.
  hdr.jsId = myHdr.jsId;

  hdr.useCgi = ntohs(myHdr.useCgi);
  hdr.nelem = ntohs(myHdr.nelem);
  hdr.size = ntohs(myHdr.size);
}

static int
ReadWithRetries(int fd, char *buf, int bufSize, int block, int maxRetries)
{
  int tries = 0;
  while (true) {
	errno = 0;
	int res = recv(fd, buf, bufSize, 0);
	
#if 0
	// Warning: turning this on may generate an awful lot of output.
	printf(" Received %d bytes\n", res);
	printf(" errno  = %d\n", errno);
#endif
    
	if (res == bufSize) {
	  // We got the whole buffer.
      break;
	}

	if (res == 0) {
	  // Got nothing.
	  if (maxRetries >= 0 && tries >= maxRetries) {
		// Note: if select() reported the fd is ready for reading, but there
		// is no data, that probably means that the process on the other
		// end of the socket died without closing the socket.  RKW 2000-09-01.
	    reportErrNosys("No data available on socket");
	    return -1;
	  } else {
		tries++;
	    // Sleep here so we don't suck up loads of CPU.
	    sleep(1);
	    continue;
	  }
	}

    if (res < 0 && errno == EINTR) {
#if defined(DEBUG)
      printf("Call to recv interrupted, continuing\n");
#endif
      continue;
    }

	// An error other than EINTR, or got part of the buffer.
    if (block) {
	  reportErrSys("Error receiving message from socket");
#if defined(DEBUG)
      printf("errno = %d\n", errno);
      printf("res = %d\n", res);
      printf("bufSize = %d\n", bufSize);
#endif
	}
    return -1;
  }

  return 1;
}
