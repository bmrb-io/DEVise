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

#include "ClientAPI.h"

//#define DEBUG

typedef struct {
  u_short flag;                         // type of message
  u_short nelem;                        // number of elements in message
  u_short size;                         // total size of message
} DeviseHeader;

int DeviseNonBlockMode(int fd)
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

int DeviseBlockMode(int fd)
{
  int result = fcntl(fd, F_SETFL, 0);
  if (result < 0)
    return -1;

  return 1;
}

char *DevisePaste(int argc, char **argv)
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

int DeviseOpen(char *servName, int portNum)
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

  int result = connect(fd,(struct sockaddr *)&servAddr,
		       sizeof(struct sockaddr));
  if (result < 0) {
    perror("Cannot connect to server");
    close(fd);
    return -1;
  }
  
  return fd;
}

int DeviseReceive(int fd, int block, u_short &flag, int &ac, char **&av)
{
  static int recBuffSize = 0;
  static char *recBuff = 0;
  static int argc = 0;
  static char **argv = 0;

  if (!block) {
    if (DeviseNonBlockMode(fd) < 0)
      return -1;
  }

#ifdef DEBUG
  if (block)
    printf("Getting header\n");
#endif

  DeviseHeader hdr;
  int res = recv(fd, (char *)&hdr, sizeof hdr, 0);
  if (!res)
    return 0;
  if (res < (int)sizeof hdr) {
#ifdef DEBUG
    if (block)
      perror("recv");
#endif
    return -1;
  }

  flag = ntohs(hdr.flag);
  u_short numElements = ntohs(hdr.nelem);
  u_short tsize = ntohs(hdr.size);

#ifdef DEBUG
  printf("Got flag %u, numElements = %u, size = %u\n",
	 flag, numElements, tsize);
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

  if (numElements > argc) {
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

  if (!block) {
    if (DeviseBlockMode(fd) < 0)
      return -1;
  }

#ifdef DEBUG
  printf("Getting message\n");
#endif

  // first receive message in chunks of some size
  char *ptr = recBuff;
  int totrem = tsize;
  while(totrem > 0) {
    res = recv(fd, ptr, totrem, 0);
    if (!res)
      return 0;
    if (res < 0) {
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
  av = argv;

  // now break buffer into individual elements

  ptr = recBuff;
  int totsize = 0;

  for(int i = 0; i < numElements; i++) {
    u_short size;
    memcpy(&size, ptr, sizeof size);
    size = ntohs(size);
    ptr += sizeof size;
    tsize += sizeof size;
#ifdef DEBUG
    printf("Element %d is %u bytes\n", i, size);
#endif

    if (ptr[size - 1]) {
      // argument must be terminated with NULL
      fprintf(stderr, "Invalid procotol argument.\n");
      return -1;
    }

#ifdef DEBUG
    printf("Element %d is \"%s\"\n", i, ptr);
#endif
    argv[i] = ptr;

    ptr += size;
    totsize += size;

    if (totsize > tsize) {
      fprintf(stderr, "Invalid protocol message.\n");
      return -1;
    }
  }

#ifdef DEBUG
  printf("Received complete message\n\n");
#endif

  return 1;
}

int DeviseSend(int fd, u_short flag, u_short bracket, int argc, char **argv)
{
  DeviseHeader hdr;
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

#ifdef DEBUG
  printf("Sending header: flag %u, nelem %u, size %u\n", flag, argc, tsize);
#endif

  int result = send(fd, (char *)&hdr, sizeof hdr, 0);
  if (result < (int)sizeof hdr) {
    perror("send");
    return -1;
  }

  for(i = 0; i < argc; i++) {
    // must send terminating NULL too
    u_short size = strlen(argv[i]) + 1;
    if (bracket)
      size += 2;
#ifdef DEBUG
    printf("Sending size of element %d: %u\n", i, size);
#endif
    size = htons(size);
    result = send(fd, (char *)&size, sizeof size, 0);
    if (result < (int)sizeof size) {
      perror("send");
      return -1;
    }
    size = ntohs(size);

    if (bracket)
      size -= 2;

    if (bracket) {
#ifdef DEBUG
      printf("Sending element %d: \"{%s}\"\n", i, argv[i]);
#endif
      char *obrace = "{";
      result = send(fd, obrace, 1, 0);
      if (result == 1) {
	// don't send null of argument, instead send null after closing brace
	result = send(fd, argv[i], size - 1, 0);
	if (result == size - 1) {
	  char *cbrace = "}";
	  result = send(fd, cbrace, 2, 0);
	  if (result < 2)
	    result = -1;
	} else
	  result = -1;
      } else
	result = -1;

      if (result < 0) {
	perror("send");
	return -1;
      }
    } else {
#ifdef DEBUG
      printf("Sending element %d: \"%s\"\n", i, argv[i]);
#endif
      result = send(fd, argv[i], size, 0);
      if (result < size) {
	perror("send");
	return -1;
      }
    }
  }

#ifdef DEBUG
  printf("Complete message sent\n\n");
#endif

  return 1;
}

int DeviseClose(int fd)
{ 
  close(fd);

  return 1;
}
