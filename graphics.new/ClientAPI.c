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

//#define DEBUG

int NetworkNonBlockMode(int fd)
{
#ifdef SUN
  int result = fcntl(fd, F_SETFL, FNDELAY);
#else
  int result = fcntl(fd, F_SETFL, O_NDELAY);
#endif
  if (result < 0)
    return -1;

  return 1;
}

int NetworkBlockMode(int fd)
{
  int result = fcntl(fd, F_SETFL, 0);
  if (result < 0)
    return -1;

  return 1;
}

char *NetworkPaste(int argc, char **argv)
{
  if (argc < 1)
    return strdup("");

  int size = 0;
  int i;
  for(i = 0; i < argc; i++)
    size += strlen(argv[i]);

  // an extra char for space between each argument
  size += argc - 1;

  // one more for the terminating NULL
  size++;

  char *cmd = new char [size];
  if (!cmd)
    return 0;

  char *ptr = cmd;
  *ptr = 0;
  for(i = 0; i < argc; i++) {
    if (!i) {
      sprintf(ptr, "%s", argv[i]);
      ptr += strlen(argv[i]);
    } else {
      sprintf(ptr, " %s", argv[i]);
      ptr += strlen(argv[i]) + 1;
    }
  }

  if (ptr - cmd != size - 1) {
    fprintf(stderr, "Internal error.\n");
    return 0;
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
  if (fd < 0){
    perror("Cannot create socket");
    return -1;
  }

  struct hostent *servEnt = gethostbyname(servName);
  if (!servEnt) {
    perror("Cannot translate server name");
    close(fd);
    return -1;
  }
  if (servEnt->h_addrtype != AF_INET) {
    fprintf(stderr, "Unsupported address type\n");
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
    		perror("Cannot connect to server");
    		close(fd);
    		return -1;
		}
		else
		if  (mode == CONNECT_ALWAYS)
		{
			sleep(seconds);
			printf("Connecting ...\n");
		}
		close(fd);
  		fd = socket(AF_INET, SOCK_STREAM, 0);
  		if (fd < 0){
    		perror("Cannot create socket");
    		return -1;
  		}
  	}
  }while (result <0);
  
  return fd;
}

//
//extract header information from a header
void NetworkAnalyseHeader(const char *headerbuf, int& numElements, int&tsize)
{
  NetworkHeader *hdr;

  assert(headerbuf!=NULL);
  hdr = (NetworkHeader *)headerbuf;
  numElements = ntohs(hdr->nelem);
  tsize = ntohs(hdr->size);
  return;
}

//

int NetworkReceive(int fd, int block, u_short &flag, int &ac, char **&av)
{
#if defined(DEBUG)
  printf("NetworkReceive()\n");
#endif

  static int recBuffSize = 0;
  static char *recBuff = 0;
  int    numElements; 
  int    tsize;

  if (!block) {
    if (NetworkNonBlockMode(fd) < 0)
      return -1;
  } else {
    if (NetworkBlockMode(fd) < 0)
      return -1;
  }

#ifdef DEBUG
  if (block)
    printf("Getting header\n");
#endif

  NetworkHeader hdr;
  while(1) {
   
	errno = 0;
	int res = recv(fd, (char *)&hdr, sizeof hdr, 0);
	
#if 0
	printf(" Received %d bytes\n", res);
	printf(" errno  = %d\n", errno);
#endif
    
	if (res == (int)sizeof hdr)
      break;
	if (res == 0) {
	  continue;
	}
    if (res < 0 && errno == EINTR) {
#if defined(DEBUG)
      printf("Call to recv interrupted, continuing\n");
#endif
      continue;
    }
    if (block) {
	  printf("Error at %s: %d: ", __FILE__, __LINE__);
      perror("recv");
#if defined(DEBUG)
      printf("errno = %d\n", errno);
      printf("res = %d\n", res);
      printf("sizeof(hdr) = %d\n", sizeof(hdr));
#endif
	}
    return -1;
  }
  flag = ntohs(hdr.flag);
  NetworkAnalyseHeader((char*)&hdr,numElements, tsize);

#ifdef DEBUG
  printf("Got flag %u, numElements = %u, size = %u\n",flag,numElements,tsize);
#endif

  if (!recBuff || tsize >= recBuffSize) {
    delete recBuff;
    recBuffSize = tsize + 1;
#ifdef DEBUG
    printf("Increasing size of recBuff to %d bytes\n", recBuffSize);
#endif
    recBuff = new char [recBuffSize];
    if (!recBuff) {
      fprintf(stderr, "Out of memory\n");
      return -1;
    }
  }

  // first receive message in chunks of some size
  char *ptr = recBuff;
  int totrem = tsize;
  while(totrem > 0) {
    int res = 0;
    while(1) {
	  errno = 0;
      res = recv(fd, ptr, totrem, 0);
      if (!res)
        return 0;
      if (res > 0)
        break;
      if (errno == EINTR) {
#ifdef DEBUG
        printf("Call to recv interrupted, continuing\n");
#endif
        continue;
      }
	  printf("Error at %s: %d: ", __FILE__, __LINE__);
      perror("recv");
      return -1;
    }
    ptr += res;
    totrem -= res;
#ifdef DEBUG
    printf("Got %d bytes of message, %d remaining\n", res, totrem);
#endif
  }

  if (totrem != 0) {
    fprintf(stderr, "Invalid protocol message.\n");
    return -1;
  }
  ac = numElements;
  return NetworkParse((const char*)recBuff, ac,av);
}
//
// now break buffer into individual elements
int NetworkParse(const char *recBuff, int numElements, char **&av)
{
  static char **argv = 0;
  static int argc = 0;
  const char *ptr;

  if (numElements > argc) {
    if (argv != 0)
      delete argv;
    argc = numElements;
#ifdef DEBUG
    printf("Increasing size of argv to %d elements\n", argc);
#endif
    argv = new char * [argc];
    if (!argv) {
      fprintf(stderr, "Out of memory\n");
      return -1;
    }
  }
  av = argv;
  ptr = recBuff;

  for(int i = 0; i < numElements; i++) {
    u_short size;
    memcpy(&size, ptr, sizeof size);
    size = ntohs(size);
    ptr += sizeof size;
#ifdef DEBUG
    printf("Element %d is %u bytes\n", i, size);
#endif

    if (ptr[size - 1]) {
      // argument must be terminated with NULL
	  printf(" proper vales = %c %c %c \n",ptr[size -2],ptr[size-1],ptr[size ]);
      fprintf(stderr, "Invalid procotol argument.\n");
      return -1;
    }

#ifdef DEBUG
    printf("Element %d is \"%s\"\n", i, ptr);
#endif
    // Note: casting 'const char *' to 'char *' here to get rid of compiler
    // complaint; that's not really the right thing to do, but I don't want
    // to delve into this more at the present time.  RKW Feb. 17, 1998.
    argv[i] = (char *) ptr;

    ptr += size;
  }

#ifdef DEBUG
  printf("Parsed complete message\n\n");
#endif

  return 1;
}

int NetworkSend(int fd, u_short flag, u_short bracket, int argc, char **argv)
{
#if defined(DEBUG)
  printf("NetworkSend(%d, %d, %d, ", fd, flag, bracket);
  printf("args(");
  for (int argNum = 0; argNum < argc; argNum++) {
    printf("<%s>, ", argv[argNum]);
  }
  printf(")\n");
  fflush(stdout);
#endif

  int msgsize;
  char *recBuffer = 0;

  msgsize = NetworkPrepareMsg(flag, bracket, argc, argv, &recBuffer );
  if (msgsize == -1)
    return -1;

  if (recBuffer != 0)
  {
    int result = send(fd, recBuffer, msgsize, 0);
    if (result < (int)sizeof (NetworkHeader)) {
      perror("send");
      return -1;
    }
  }
  recBuffer = 0;

#ifdef DEBUG
  printf("Complete message sent\n\n");
#endif

  return 1;
}

int NetworkPrepareMsg(u_short flag, 
    u_short bracket, int argc, char **argv, char** recBufferp)
{
  static int recBuffSize = 0;
  static char *recBuff = 0;

  NetworkHeader hdr;
  hdr.flag = htons(flag);
  hdr.nelem = htons(argc);

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
  hdr.size = htons(tsize);

  u_short msgsize = sizeof hdr + tsize;
  if (!recBuff || msgsize >= recBuffSize) {
    delete recBuff;
    recBuffSize = msgsize + 1;
#ifdef DEBUG
    printf("Increasing size of recBuff to %d bytes\n", recBuffSize);
#endif
    recBuff = new char [recBuffSize];
    if (!recBuff) {
      fprintf(stderr, "Out of memory\n");
      return -1;
    }
  }

  char *buff = recBuff;

  memcpy(recBuff, &hdr, sizeof hdr);
  buff += sizeof hdr;

  for(i = 0; i < argc; i++) {
    // must send terminating NULL too
    u_short size = strlen(argv[i]) + 1;
    if (bracket) {
      size += 2;
    }
#ifdef DEBUG
    printf("Sending size of element %d: %u\n", i, size);
#endif
    u_short nsize = htons(size);
    memcpy(buff, &nsize, sizeof nsize);
    buff += sizeof nsize;
    if (bracket) {
      size -= 2;
    }

    if (bracket) {
#ifdef DEBUG
      printf("Sending element %d: \"{%s}\"\n", i, argv[i]);
#endif
      *buff++ = '{';
      // don't send null of argument, instead send null after closing brace
      memcpy(buff, argv[i], size - 1);
      buff += size - 1;
      *buff++ = '}';
      *buff++ = 0;
    } else {
#ifdef DEBUG
      printf("Sending element %d: \"%s\"\n", i, argv[i]);
#endif
      memcpy(buff, argv[i], size);
      buff += size;
    }
  }

  assert(buff - recBuff == msgsize);
#if defined(DEBUG)
  printf("Sending message: flag %u, nelem %u, size %u\n", flag, argc, tsize);
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
