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
  Revision 1.12  1996/05/15 19:00:15  jussi
  Return value of recv() on a socket in non-blocking mode is
  EWOULDBLOCK when there is no data, although on Intel/Solaris
  testing the return value against EAGAIN worked just fine.
  Anyway, I fixed this so that the server works on other platforms
  as well.

  Revision 1.11  1996/05/15 16:41:08  jussi
  Moved all networking code from ServerAPI.c to ClientAPI.c.
  Improved support for bracketed or non-bracketed arguments
  passed in commands.

  Revision 1.10  1996/05/14 19:18:41  jussi
  Changed memory allocation in ReadSocket(). Rather than
  malloc a chunk of memory for each element every time
  a new command arrives, new memory is allocated only
  when the number of elements increases. Also ReadSocket()
  is called successively until no more commands exist
  rather than just a single time and then returning to
  the dispatcher.

  Revision 1.9  1996/05/13 21:55:43  jussi
  Moved initialization of _mode to Control.c.

  Revision 1.8  1996/05/13 20:25:03  jussi
  Improved support for replicas. Commands are echoed to replicas
  before being executed locally. See comments in ReadSocket()
  for an explanation.

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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "ServerAPI.h"
#include "QueryProc.h"
#include "ParseCat.h"

#ifdef SUN
#include "missing.h"
#endif

//#define SERV_ANYPORT
//#define DEBUG
//#define DEBUG9

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
  char *args[] = { "AbortProgram", reason };
  SendControl(2, args);
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

  int fd = NetworkOpen(hostName, port);
  if (fd < 0) {
    fprintf(stderr, "Cannot connect to replica server %s:%d.\n",
	    hostName, port);
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

  NetworkClose(_replicas[i].fd);

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
    if (NetworkSend(_replicas[i].fd, API_CMD, 0, argc, argv) < 0) {
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
    while(_socketFd >= 0) {
      int result = ReadCommand();
      if (!result)
	break;
      if (result < 0) {
	fprintf(stderr,
		"Cannot communicate with client. Closing connection.\n");
	RestartSession();
      }
    }
    return;
  }

  printf("\n");
  printf("Server waiting for client connection.\n");

  struct sockaddr_in tempaddr;
  int len = sizeof(tempaddr);
  _socketFd = accept(_listenFd, (struct sockaddr *)&tempaddr, &len);
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

int ServerAPI::ReadCommand()
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  // If the Dispacher uses select(), then we will be called only
  // when there's new data to be read from _socketFd; otherwise
  // the Dispatcher calls us constantly, so we want to read
  // the socket in a non-blocking mode.

  u_short flag;
  int argc;
  char **argv;
#ifdef USE_SELECT
  int result = NetworkReceive(_socketFd, 1, flag, argc, argv);
#else
  int result = NetworkReceive(_socketFd, 0, flag, argc, argv);
#endif
  if (result < 0) {
#ifndef USE_SELECT
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return 0;
#endif
    perror("recv");
    return -1;
  }

  if (!result) {
#ifdef DEBUG
    printf("End of client data.\n");
#endif
    return -1;
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

  Replicate(argc, argv);

#ifdef DEBUG
  printf("Executing command\n");
#endif

  if (ParseAPI(argc, argv, this) < 0)
    fprintf(stderr, "Devise API command error\n");

#ifdef DEBUG
  printf("Done executing command\n");
#endif

#ifndef USE_SELECT
  // go back to non-blocking mode
  (void)NetworkNonBlockMode(_socketFd);
#endif

  return 1;

 error:
#ifndef USE_SELECT
  // go back to non-blocking mode
  (void)NetworkNonBlockMode(_socketFd);
#endif

  return -1;
}

int ServerAPI::GotoConnectedMode()
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  printf("Setting up client connection.\n");
    
  // no action needed at this time

  printf("Client connection established.\n");
    
  return 1;
}

void ServerAPI::RestartSession()
{
  Dispatcher::Current()->Unregister(this);

  DestroySessionData();

  if (_socketFd >= 0) {
    printf("Closing client connection.\n");
    NetworkClose(_socketFd);
    _socketFd = -1;
  }

  for(int i = 0; i < _replicate; i++) {
    printf("Closing replica connection to %s:%d\n", _replicas[i].host,
	   _replicas[i].port);
    NetworkClose(_replicas[i].fd);
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
  servAddr.sin_port = htons(DefaultNetworkPort);
#ifdef DEBUG
  printf("Server listening at port %u.\n", DefaultNetworkPort);
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

void ServerAPI::SetBusy()
{
  if (++_busy == 1) {
    char *args[] = { "ChangeStatus", "1" };
    SendControl(2, args);
  }
}

void ServerAPI::SetIdle()
{
  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0) {
    char *args[] = { "ChangeStatus", "0" };
    SendControl(2, args);
  }
}

Boolean ServerAPI::IsBusy()
{
  return (_busy > 0);
}

void ServerAPI::FilterChanged(View *view, VisualFilter &filter,
				   int flushed)
{
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
  
  char *args[] = { "ProcessViewFilterChange", view->GetName(),
		   (flushed ? "1" : "0"), xLowBuf, yLowBuf,
		   xHighBuf, yHighBuf, "0" };
  SendControl(8, args);

  char *rep[] = { "setFilter", view->GetName(), xLowBuf,
		  yLowBuf, xHighBuf, yHighBuf };
  Replicate(6, rep);
}

void ServerAPI::SelectView(View *view)
{
  char *args[] = { "ProcessViewSelected", view->GetName() };
  SendControl(2, args);
}

void ServerAPI::SyncNotify()
{
  SendControl(API_CTL, "SyncDone");
  ClearSyncNotify();
}
