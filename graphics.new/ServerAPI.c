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
  Revision 1.7  1996/05/13 18:07:11  jussi
  Changed interpretation of the "flag" argument to Send().
  It is no longer simply an error flag but instead indicates
  the type of a message: API_CMD, API_ACK, API_NAK, or API_CTL.
  Removed control channel from client-server communication;
  all communication goes through a single socket pair.

  Revision 1.6  1996/05/11 20:44:19  jussi
  Moved output statement for "exit" command from ParseAPI.C to
  ServerAPI.c.

  Revision 1.5  1996/05/11 19:08:24  jussi
  Added replica management.

  Revision 1.4  1996/05/11 17:25:39  jussi
  Moved all command parsing to ParseAPI.C so that TkControl.c
  would not have to duplicate the code.

  Revision 1.3  1996/05/11 02:01:01  jussi
  Fixed typo.

  Revision 1.2  1996/05/11 01:53:56  jussi
  Condensed the code by removing some unnecessary Tcl/Tk parts
  which are not used by the server. Changed the client-server
  protocol somewhat.
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "ServerAPI.h"
#include "QueryProc.h"
#include "ParseCat.h"

#ifdef SUN
#include "missing.h"
#endif

//#define SERV_ANYPORT
//#define DEBUG
//#define DEBUG9

ControlPanel::Mode ServerAPI::_mode = ControlPanel::DisplayMode;
MapInterpClassInfo *ServerAPI::_interpProto = 0;

ControlPanel *GetTkControl()
{
  return new ServerAPI();
}

ServerAPI::ServerAPI()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");

  ControlPanel::_controlPanel = (ControlPanel *)this;
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);
	
  _busy = false;
  _listenFd = _socketFd = -1;

  _replicate = 0;

  // see if any replicas are defined

  FILE *fp = fopen("devise.rep", "r");
  if (fp) {
    char hostName[64];
    int port;
    while(fscanf(fp, "%s%d", hostName, &port) == 2)
      (void)AddReplica(hostName, port);
    fclose(fp);
  }

  RestartSession();
}

void ServerAPI::DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  SendControl(API_CTL, reason);
  fprintf(stderr, "Server aborts.\n");
  exit(0);
}

int ServerAPI::AddReplica(char *hostName, int port)
{
  if (_replicate >= _maxReplicas) {
    fprintf(stderr, "Replica limit exceeded. Cannot add %s:%d.\n",
	    hostName, port);
    return -1;
  }
  
  int i;
  for(i = 0; i < _replicate; i++) {
    if (!strcmp(_replicas[i].host, hostName) && 
	_replicas[i].port == port)
      break;
  }

  if (i < _replicate) {
    fprintf(stderr, "Replica already defined.\n");
    return -1;
  }

  struct hostent *hostEnt = gethostbyname(hostName);
  if (!hostEnt) {
    perror("Cannot translate replica host name");
    return -1;
  }
  if (hostEnt->h_addrtype != AF_INET) {
    fprintf(stderr, "Unsupported replica address type\n");
    return -1;
  }

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0){
    perror("Cannot create socket");
    return -1;
  }

  struct in_addr *ptr = (struct in_addr *)*hostEnt->h_addr_list;

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
	
  servAddr.sin_family = AF_INET;
  servAddr.sin_port   = htons(port);
  servAddr.sin_addr   = *ptr;

  int result = connect(fd,(struct sockaddr *)&servAddr,
		       sizeof(struct sockaddr));
  if (result < 0) {
    fprintf(stderr, "Cannot connect to replica server %s:%d.\n",
	    hostName, port);
    perror("connect");
    close(fd);
    return -1;
  }
  
  u_short size = htons(0);
  result = send(fd, (char *)&size, sizeof size, 0);
  if (result < (int)sizeof size) {
    perror("Cannot send DISPLAY size to replica server");
    close(fd);
    return -1;
  }

  _replicas[_replicate].host = CopyString(hostName);
  _replicas[_replicate].port = port;
  _replicas[_replicate].fd = fd;
 
  _replicate++;

  printf("Added %s:%d as a replica server.\n", hostName, port);

  return 1;
}

int ServerAPI::RemoveReplica(char *hostName, int port)
{
  int i;
  for(i = 0; i < _replicate; i++) {
    if (!strcmp(_replicas[i].host, hostName) && 
	_replicas[i].port == port)
      break;
  }

  if (i >= _replicate) {
    fprintf(stderr, "No such replica server.\n");
    return -1;
  }

  close(_replicas[i].fd);

  for(int j = i + 1; j < _replicate; j++) {
    _replicas[j - 1].host = _replicas[j].host;
    _replicas[j - 1].port = _replicas[j].port;
    _replicas[j - 1].fd = _replicas[j].fd;
  }

  _replicate--;

  printf("Removed %s:%d as a replica server.\n", hostName, port);

  return 1;
}

void ServerAPI::Replicate(int argc, char **argv)
{
  if (!_replicate)
    return;

  if (argc > 0 &&
      (!strcmp(argv[0], "addReplicaServer")
       || !strcmp(argv[0], "removeReplicaServer")))
    return;

#ifdef DEBUG
  printf("Replicating command to other servers\n");
#endif

  for(int i = 0; i < _replicate; i++) {
    if (Send(_replicas[i].fd, API_CMD, 0, argc, argv) < 0) {
      fprintf(stderr,
	      "Failed to replicate command to %s:%d. Disconnecting.\n",
	      _replicas[i].host, _replicas[i].port);
      RemoveReplica(_replicas[i].host, _replicas[i].port);
    }
  }

#ifdef DEBUG
  printf("Done replicating command\n");
#endif
}

void ServerAPI::DestroySessionData()
{
#ifdef DEBUG
  printf("Destroying session data\n");
#endif

  ClassDir *classDir = GetClassDir();

  // destroy tdata, gdata, cursor, view link, win, axis, action
  classDir->DestroyAllInstances();

  // clearQP
  classDir->DestroyTransientClasses();
  QueryProc::Instance()->ClearQueries();
  ClearCats();

  // clearTopGroups
  delete gdir;
  gdir = new GroupDir();
}

void ServerAPI::Run()
{
#if DEBUG9
  printf("In ServerAPI::Run\n");
#endif

  DOASSERT((_socketFd < 0 && _listenFd >= 0) ||
	   (_socketFd >= 0 && _listenFd < 0), "Invalid sockets");

  if (_socketFd >= 0) {
    // We are connected so just keep reading from the socket
    if (ReadSocket() < 0) {
      fprintf(stderr, "Cannot communicate with client. Closing connection.\n");
      RestartSession();
    }
    return;
  }

  printf("\n");
  printf("Server waiting for client connection.\n");

  int len = sizeof(_client_addr);
  _socketFd = accept(_listenFd, (struct sockaddr *)&_client_addr, &len);
  if (_socketFd < 0) {
    perror("accept");
    if (errno == EINTR) {
      fprintf(stderr, "Server exits.\n");
      exit(1);
    }
    DOASSERT(0, "Error in network interface");
  }
  
  close(_listenFd);
  _listenFd = -1;

  if (GotoConnectedMode() < 0) {
    fprintf(stderr, "Bad client connection. Closing connection.\n");
    RestartSession();
    return;
  }

  Dispatcher::Current()->Unregister(this);
  Dispatcher::Current()->Register(this, 10, AllState, true, _socketFd);
}

int ServerAPI::NonBlockingMode(int fd)
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

int ServerAPI::BlockingMode(int fd)
{
  int result = fcntl(fd, F_SETFL, 0);
  if (result < 0)
    return -1;

  return 1;
}

int ServerAPI::ReadSocket()
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  static u_short maxSize = 0;
  static char **buff = 0;

#ifdef DEBUG9
  printf("Getting flag\n");
#endif

  u_short flag;
  int result = recv(_socketFd, (char *)&flag, sizeof flag, 0);
  if (result < (int)sizeof flag) {
#ifdef DEBUG9
    perror("recv");
#endif
    if (errno == EAGAIN)
      return 1;
    return -1;
  }

  if (!result) {
#ifdef DEBUG
    printf("End of client data.\n");
#endif
    return -1;
  }

  flag = ntohs(flag);

  // set socket to blocking mode
  (void)BlockingMode(_socketFd);

#ifdef DEBUG
  printf("Getting bracket\n");
#endif

  u_short bracket;
  result = recv(_socketFd, (char *)&bracket, sizeof bracket, 0);
  if (result < (int)sizeof bracket) {
    perror("recv");
    goto error;
  }
  bracket = ntohs(bracket);

#ifdef DEBUG
  printf("Getting numElements\n");
#endif

  u_short numElements;
  result = recv(_socketFd, (char *)&numElements, sizeof numElements, 0);
  if (result < (int)sizeof numElements) {
    perror("recv");
    goto error;
  }
  numElements = ntohs(numElements);

#ifdef DEBUG
  printf("\nGot numElements = %d\n", numElements);
#endif

  if (numElements > maxSize) {
    if (buff) 
      delete buff;
    buff = new char * [numElements];
    DOASSERT(buff, "Out of memory");
    maxSize = numElements;
  }

  int i;
  for(i = 0; i < numElements;i++) {
#ifdef DEBUG
    printf("Getting size of element %d\n", i);
#endif
    u_short size;
    result = recv(_socketFd, (char *)&size, sizeof size, 0);
    if (result < (int)sizeof size) {
      perror("recv");
      goto error;
    }
    size = ntohs(size);
#ifdef DEBUG
    printf("Getting element %d (%d bytes)\n", i, size);
#endif
    buff[i] = new char [size];
    DOASSERT(buff[i], "Out of memory");
    buff[i][0] = 0;
    if (size > 0) {
      result = recv(_socketFd, buff[i], size, 0);
      if (result < size) {
	perror("recv");
	goto error;
      }
#ifdef DEBUG
      printf("Got element \"%s\"\n", buff[i]);
#endif
    }
  }

  if (flag != API_CMD) {
    fprintf(stderr, "Received unexpected type of message: %u\n", flag);
    goto error;
  }

  // a command must first be replicated before executing it locally
  // because the local execution may result in further API_CTL type
  // commands being sent to the replicas before ParseAPI completes

  // for example, a "create view" command causes a "setFilter" control
  // command to be sent to the replicas, and we want the replicas to
  // have already executed the "create view" command before the
  // "setFilter" command is sent

  Replicate(numElements, buff);

#ifdef DEBUG
  printf("Executing command\n");
#endif

  if (ParseAPI(numElements, buff, this) < 0)
    fprintf(stderr, "Devise API command error\n");

#ifdef DEBUG
  printf("Done executing command\n");
#endif

  for(i = 0; i < numElements; i++)
    delete buff[i];

  // go back to non-blocking mode
  (void)NonBlockingMode(_socketFd);

  return 1;

 error:
  // go back to non-blocking mode
  (void)NonBlockingMode(_socketFd);
  return -1;
}

int ServerAPI::GotoConnectedMode()
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  printf("Setting up client connection.\n");
    
  // first we receive the name of the display at the client (DISPLAY)

  u_short size;
  int result = recv(_socketFd, (char *)&size, sizeof size, 0);
  if (result < (int)sizeof size) {
    perror("recv");
    return -1;
  }
  size = ntohs(size);

  char displayName[size];
  result = recv(_socketFd, displayName, size, 0);
  if (result < size) {
    perror("recv");
    return -1;
  }
  
#ifdef DEBUG
  printf("Client sent display name: %s\n", displayName);
#endif

  // set socket to non-blocking mode

  if (NonBlockingMode(_socketFd) < 0)
    return -1;

  printf("Client connection established.\n");
    
  return 1;
}

void ServerAPI::RestartSession()
{
  Dispatcher::Current()->Unregister(this);

  DestroySessionData();

  if (_socketFd >= 0) {
    printf("Closing client connection.\n");
    close(_socketFd);
    _socketFd = -1;
  }

  for(int i = 0; i < _replicate; i++) {
    printf("Closing replica connection to %s:%d\n", _replicas[i].host,
	   _replicas[i].port);
    close(_replicas[i].fd);
  }
  _replicate = 0;

  DOASSERT(_listenFd < 0, "Invalid socket");
  _listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_listenFd < 0)
    perror("socket");
  DOASSERT(_listenFd >= 0, "Cannot create socket");

  // allow listening port number to be reused
  int enable = 1;
  int result = setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR,
		      (char *)&enable, sizeof enable);
  if (result < 0)
    perror("setsockopt");
  DOASSERT(result >= 0, "Cannot set socket options");

  // Now bind these to the address..

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof servAddr);
  servAddr.sin_family = AF_INET;
#ifdef SERV_ANYPORT
  servAddr.sin_port = htons(0);
#else
  servAddr.sin_port = htons(DefaultDevisePort);
#ifdef DEBUG
  printf("Server listening at port %u.\n", DefaultDevisePort);
#endif
#endif
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  result = bind(_listenFd, (struct sockaddr *)&servAddr,
		sizeof(struct sockaddr));
  if (result < 0)
    perror("bind");
  DOASSERT(result >= 0, "Cannot bind to address");
     
#ifdef SERV_ANYPORT	
  struct sockaddr_in tempAddr;
  memset(&tempAddr, 0, sizeof(struct sockaddr));
  int len = sizeof(struct sockaddr);

  result = getsockname(_listenFd, (sockaddr *)&tempAddr, &len);
  if (result < 0)
    perror("getsockname");
  DOASSERT(result >= 0, "Cannot get sockname");
  printf("Server listening at port %u.\n", ntohs(tempAddr.sin_port);
#endif	

  result = listen(_listenFd, 5);
  if (result < 0)
    perror("listen");
  DOASSERT(result >= 0, "Cannot listen");

  Dispatcher::Current()->Register(this, 10, AllState, true, _listenFd);
}

int ServerAPI::Send(int fd, u_short flag, int bracket,
		    int numArgs, char **argv)
{
#ifdef DEBUG
  printf("Sending flag %d\n", flag);
#endif

  flag = htons(flag);
  int result = send(fd, (char *)&flag, sizeof flag, 0);
  if (result < (int)sizeof flag) {
    perror("send");
    return -1;
  }
 
#ifdef DEBUG
  printf("Sending bracket flag %d\n", bracket);
#endif

  u_short nbracket = htons(bracket);
  result = send(fd, (char *)&nbracket, sizeof nbracket, 0);
  if (result < (int)sizeof nbracket) {
    perror("send");
    return -1;
  }

#ifdef DEBUG
  printf("Sending number of elements %d\n", numArgs);
#endif

  u_short num = htons((u_short)numArgs);
  result = send(fd, (char *)&num, sizeof num, 0);
  if (result < (int)sizeof num) {
    perror("send");
    return -1;
  }

  for(int i = 0; i < numArgs; i++) {
    u_short size;
    if (!argv[i])
      size = 0;
    else
      size = (u_short)(strlen(argv[i]) + 1);
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
      if (result < 0) {
	perror("send");
	return -1;
      }
    }
  }

#ifdef DEBUG
  printf("Complete message sent\n");
#endif

  return 1;
}

void ServerAPI::SetBusy()
{
  if (++_busy == 1)
    SendControl(API_CTL, "ChangeStatus 1");
}

void ServerAPI::SetIdle()
{
  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0)
    SendControl(API_CTL, "ChangeStatus 0");
}

Boolean ServerAPI::IsBusy()
{
  return (_busy > 0);
}

void ServerAPI::ExecuteScript(char *script)
{
  char cmd[256];
  sprintf(cmd, "ExecuteScript %s", script);
  SendControl(API_CTL, cmd);
}

void ServerAPI::FilterChanged(View *view, VisualFilter &filter,
				   int flushed)
{
#ifdef DEBUG
  printf("TkControl: filter changed\n");
#endif

  char cmd[256];
  char xLowBuf[80], yLowBuf[80], xHighBuf[80], yHighBuf[80];
  if (view->GetXAxisAttrType() == DateAttr) {
    sprintf(xLowBuf, "%s", DateString(filter.xLow));
    sprintf(xHighBuf, "%s", DateString(filter.xHigh));
  } else {
    sprintf(xLowBuf, "%.2f", filter.xLow);
    sprintf(xHighBuf, "%.2f", filter.xHigh);
  }
				
  if (view->GetYAxisAttrType() == DateAttr) {
    sprintf(yLowBuf, "%s", DateString(filter.yLow));
    sprintf(yHighBuf, "%s", DateString(filter.yHigh));
  } else {
    sprintf(yLowBuf, "%.2f", filter.yLow);
    sprintf(yHighBuf, "%.2f", filter.yHigh);
  }
  
  sprintf(cmd, "ProcessViewFilterChange {%s} %d {%s} {%s} {%s} {%s} 0",
	  view->GetName(), flushed, xLowBuf, yLowBuf, xHighBuf, yHighBuf);
  SendControl(API_CTL, cmd);

  char *args[] = { "setFilter", view->GetName(), xLowBuf,
		   yLowBuf, xHighBuf, yHighBuf };
  Replicate(6, args);
}

void ServerAPI::SelectView(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewSelected {%s}", view->GetName());
  SendControl(API_CTL, cmd);
}
