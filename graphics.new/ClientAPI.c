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

static int _socketFd = -1;
static int _controlFd = -1;

static void DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  close(_socketFd);
  close(_controlFd);
  exit(1);
}

static int OpenControlSocket()
{
  _controlFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_controlFd < 0) {
    perror("Cannot create socket");
    exit(1);
  }

  struct sockaddr_in cliAddr;
  memset(&cliAddr, 0, sizeof(cliAddr));

  cliAddr.sin_family = AF_INET;
  cliAddr.sin_port = htons(0);
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  int result = bind(_controlFd, (struct sockaddr *)&cliAddr,
		    sizeof(struct sockaddr));
  if (result < 0) {
    perror("Cannot bind to address: bind");
    exit(1);
  }

  // Get port number of control socket
  
  struct sockaddr_in tempAddr;	
  memset(&tempAddr, 0, sizeof(struct sockaddr));
  int len = sizeof(struct sockaddr);
  
  result = getsockname(_controlFd, (sockaddr *)&tempAddr, &len);
  if (result < 0) {
    perror("Cannot get address of socket");
    exit(1);
  }
  
  u_short port = ntohs(tempAddr.sin_port);
#ifdef DEBUG
  printf("Client waiting at port %u\n", port);
#endif

  // We have to enable listening device before we send port number
  // to server

  result = listen(_controlFd, 5);
  if (result < 0) {
    perror("Cannot listen on socket");
    exit(1);
  }

  return port;
}

int DeviseOpen(char *servName, int portNum, int control)
{
  DOASSERT(_socketFd < 0 && _controlFd < 0, "Invalid socket");

  _socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_socketFd < 0){
    perror("Cannot create socket");
    exit(1);
  }

  struct hostent *servEnt = gethostbyname(servName);
  if (!servEnt) {
    perror("Cannot translate server name");
    exit(1);
  }
  if (servEnt->h_addrtype != AF_INET) {
    fprintf(stderr, "Unsupported address type\n");
    exit(1);
  }

  struct in_addr *ptr = (struct in_addr *)*servEnt->h_addr_list;

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
	
  servAddr.sin_family = AF_INET;
  servAddr.sin_port   = htons(portNum);
  servAddr.sin_addr   = *ptr;

  int result = connect(_socketFd,(struct sockaddr *)&servAddr,
		       sizeof(struct sockaddr));
  if (result < 0) {
    perror("Cannot connect to server");
    exit(1);
  }
  
  u_short port = 0;

  if (control)
    port = OpenControlSocket();

  port = htons(port);
  result = send(_socketFd, (char *)&port, sizeof port, 0);
  if (result < (int)sizeof port) {
    perror("Cannot send port number to server");
    exit(1);
  }
	
  // Now send the XDisplay name so server knows where to display

  char *displayName = getenv("DISPLAY");
  if (!displayName) {
    fprintf(stderr, "Cannot get DISPLAY variable");
    exit(1);
  }

  u_short size = htons(strlen(displayName) + 1);
  result = send(_socketFd, (char *)&size, sizeof size, 0);
  if (result < (int)sizeof size) {
    perror("Cannot send DISPLAY size to server");
    exit(1);
  }

  size = ntohs(size);
  result = send(_socketFd, displayName, size, 0);
  if (result < size) {
    perror("Cannot send display name to server");
    exit(1);
  }

  if (control) {
    // Now wait for server to connect back to us

    printf("Waiting for server to establish control channel.\n");

    struct sockaddr_in tempAddr;	
    memset(&tempAddr, 0, sizeof(struct sockaddr));
    int len = sizeof(struct sockaddr);
    int tempFd = accept(_controlFd, (struct sockaddr *)&tempAddr, &len);
    if (tempFd < 0) {
      perror("Cannot wait for server connection");
      exit(1);
    }

    close(_controlFd);
    _controlFd = tempFd;
  }
	
  return _controlFd;
}

int DeviseReceive(char *result, int &flag, char *errorMsg)
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  static u_short oldsize = 0;
  static char *buff = 0;

  result[0] = 0;

#ifdef DEBUG
  printf("Getting error flag\n");
#endif

  u_short errflag;
  int res = recv(_socketFd, (char *)&errflag, sizeof errflag, 0);
  if (res < (int)sizeof errflag) {
    perror("recv");
    sprintf(result, "Receive failed in %s", errorMsg);
    return -1;
  }

  flag = ntohs(errflag);

#ifdef DEBUG
  printf("Getting bracket\n");
#endif

  u_short bracket;
  res = recv(_socketFd, (char *)&bracket, sizeof bracket, 0);
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
  res = recv(_socketFd, (char *)&numElements, sizeof numElements, 0);
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
    res = recv(_socketFd, (char *)&size, sizeof size, 0);
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
      res = recv(_socketFd, buff, size, 0);
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

int DeviseSend(char **argv, int num)
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  if (num <= 0)
    return 1;

#ifdef DEBUG
  printf("Sending error flag\n");
#endif

  u_short errflag = htons(0);
  int result = send(_socketFd, (char *)&errflag, sizeof errflag, 0);
  if (result < (int)sizeof errflag) {
    perror("send");
    return -1;
  }

#ifdef DEBUG
  printf("Sending bracket flag\n");
#endif

  u_short bracket = htons(0);
  result = send(_socketFd, (char *)&bracket, sizeof bracket, 0);
  if (result < (int)sizeof bracket) {
    perror("send");
    return -1;
  }

#ifdef DEBUG
  printf("Sending number of elements %d\n", num);
#endif

  u_short size = htons((u_short)num);
  result = send(_socketFd, (char *)&size, sizeof size, 0);
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
    result = send(_socketFd, (char *)&size, sizeof size, 0);
    if (result < (int)sizeof size) {
      perror("send");
      return -1;
    }
    size = ntohs(size);
    if (size > 0) {
#ifdef DEBUG
      printf("Sending element %d: \"%s\"\n", i, argv[i]);
#endif
      result = send(_socketFd, argv[i], size, 0);
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

int DeviseClose()
{ 
  DOASSERT(_socketFd >= 0, "Invalid socket");

  close(_socketFd);
  close(_controlFd);

  return 1;
}
