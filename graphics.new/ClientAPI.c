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

#include "ClientAPI.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

static void DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  exit(1);
}

int DeviseOpen(char *servName, int portNum, int control)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0){
    perror("Cannot create socket");
    return -1;
  }

  struct hostent *servEnt = gethostbyname(servName);
  if (!servEnt) {
    perror("Cannot translate server name");
    return -1;
  }
  if (servEnt->h_addrtype != AF_INET) {
    fprintf(stderr, "Unsupported address type\n");
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
    return -1;
  }
  
  // First send the XDisplay name so server knows where to display

  char *displayName = getenv("DISPLAY");
  if (!displayName) {
    fprintf(stderr, "Cannot get DISPLAY variable");
    return -1;
  }

  u_short size = htons(strlen(displayName) + 1);
  result = send(fd, (char *)&size, sizeof size, 0);
  if (result < (int)sizeof size) {
    perror("Cannot send DISPLAY size to server");
    return -1;
  }

  size = ntohs(size);
  result = send(fd, displayName, size, 0);
  if (result < size) {
    perror("Cannot send display name to server");
    return -1;
  }

  return fd;
}

int DeviseReceive(int fd, char *result, u_short &flag, char *errorMsg)
{
  static u_short oldsize = 0;
  static char *buff = 0;

  result[0] = 0;

#ifdef DEBUG
  printf("Getting flag\n");
#endif

  int res = recv(fd, (char *)&flag, sizeof flag, 0);
  if (res < (int)sizeof flag) {
    perror("recv");
    sprintf(result, "Receive failed in %s", errorMsg);
    return -1;
  }

  flag = ntohs(flag);

#ifdef DEBUG
  printf("Getting bracket\n");
#endif

  u_short bracket;
  res = recv(fd, (char *)&bracket, sizeof bracket, 0);
  if (res < (int)sizeof bracket) {
    perror("recv");
    sprintf(result, "Receive failed in %s", errorMsg);
    return -1;
  }
  bracket = ntohs(bracket);

#ifdef DEBUG
  printf("Getting numElements\n");
#endif

  u_short numElements;
  res = recv(fd, (char *)&numElements, sizeof numElements, 0);
  if (res < (int)sizeof numElements) {
    perror("recv");
    sprintf(result, "Receive failed in %s", errorMsg);
    return -1;
  }

  numElements = ntohs(numElements);

#ifdef DEBUG
  printf("Got numElements = %d\n", numElements);
#endif

  for(int i = 0; i < numElements; i++) {
#ifdef DEBUG
    printf("Getting size of element %d\n", i);
#endif
    u_short size;
    res = recv(fd, (char *)&size, sizeof size, 0);
    if (res < (int)sizeof size) {
      perror("recv");
      sprintf(result, "Receive failed in %s", errorMsg);
      return -1;
    }
    size = ntohs(size);
#ifdef DEBUG
    printf("Getting element %d (%d bytes)\n", i, size);
#endif
    if (!buff || size > oldsize) {
      delete buff;
      buff = new char [size];
      DOASSERT(buff, "Out of memory");
      oldsize = size;
    }

    if (size > 0) {
      res = recv(fd, buff, size, 0);
      if (res < size) {
	perror("recv");
	sprintf(result, "Receive failed in %s", errorMsg);
	return -1;
      }
#ifdef DEBUG
      printf("Got element \"%s\"\n", buff);
#endif
      if (bracket)
	strcat(result, "{");
      strcat(result, buff);
      if (bracket)
	strcat(result, "} ");
    }
  }

#ifdef DEBUG
  printf("Received complete message\n\n");
#endif

  return 1;
}

int DeviseSend(int fd, char **argv, int num)
{
  if (num <= 0)
    return 1;

#ifdef DEBUG
  printf("Sending flag\n");
#endif

  u_short flag = htons(API_CMD);
  int result = send(fd, (char *)&flag, sizeof flag, 0);
  if (result < (int)sizeof flag) {
    perror("send");
    return -1;
  }

#ifdef DEBUG
  printf("Sending bracket flag\n");
#endif

  u_short bracket = htons(0);
  result = send(fd, (char *)&bracket, sizeof bracket, 0);
  if (result < (int)sizeof bracket) {
    perror("send");
    return -1;
  }

#ifdef DEBUG
  printf("Sending number of elements %d\n", num);
#endif

  u_short size = htons((u_short)num);
  result = send(fd, (char *)&size, sizeof size, 0);
  if (result < (int)sizeof size) {
    perror("send");
    return -1;
  }

  for(int i = 0; i < num; i++) {
    size = strlen(argv[i]) + 1;
#ifdef DEBUG
    printf("Sending size of element %d: %d\n", i, size);
#endif
    size = htons(size);
    result = send(fd, (char *)&size, sizeof size, 0);
    if (result < (int)sizeof size) {
      perror("send");
      return -1;
    }
    size = ntohs(size);
    if (size > 0) {
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
