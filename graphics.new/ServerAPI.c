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
  _listenFd = _socketFd = _controlFd = -1;

  _replicate = 0;

  RestartSession();
}

void ServerAPI::DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  SendControl(API_OK, reason);
  fprintf(stderr, "Server aborts.\n");
  exit(0);
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

  DOASSERT((_socketFd < 0 && _controlFd < 0 && _listenFd >= 0) ||
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

int ServerAPI::ReadSocket()
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  static u_short maxSize = 0;
  static char **buff = 0;

#ifdef DEBUG9
  printf("Getting numElements\n");
#endif

  u_short numElements;
  int result = recv(_socketFd, (char *)&numElements, sizeof numElements, 0);
  if (result < (int)sizeof numElements) {
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

  numElements = ntohs(numElements);

  // set socket to blocking mode

  result = fcntl(_socketFd, F_SETFL, 0);
  if (result < 0) {
    perror("fcntl");
    return -1;
  }
    
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

  for(int i = 0; i < numElements;i++) {
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

#ifdef DEBUG
  printf("Executing command\n");
#endif

  if (ParseAPI(numElements, buff, this) < 0)
    fprintf(stderr, "Devise API command error\n");

#ifdef DEBUG
  printf("Done executing command\n");
#endif

  if (_replicate) {
#ifdef DEBUG
    printf("Replicating command to other servers\n");
#endif

#ifdef DEBUG
    printf("Done replicating command\n");
#endif
  }

  for(int i = 0; i < numElements; i++)
    delete buff[i];

  // When client executes an exit command, _socketFd will be
  // closed in DEViseCmd()

  if (_socketFd >= 0) {
    // set socket back to non-blocking mode

#ifdef SUN
    result = fcntl(_socketFd, F_SETFL, FNDELAY);
#else
    result = fcntl(_socketFd, F_SETFL, O_NDELAY);
#endif
    if (result < 0) {
      perror("fcntl");
      return -1;
    }
  }

  return 1;

 error:
  // set socket back to non-blocking mode

#ifdef SUN
  result = fcntl(_socketFd, F_SETFL, FNDELAY);
#else
  result = fcntl(_socketFd, F_SETFL, O_NDELAY);
#endif
  if (result < 0)
    perror("fcntl");

  return -1;
}

int ServerAPI::GotoConnectedMode()
{
  DOASSERT(_socketFd >= 0 && _controlFd < 0, "Invalid sockets");

  printf("Setting up client connection.\n");
    
  // we will receive the port number of the clients back channel

  u_short port;
  int result = recv(_socketFd, (char *)&port, sizeof port, 0);
  if (result < (int)sizeof port) {
    perror("recv");
    return -1;
  }
  port = ntohs(port);

  // next we receive the name of the display at the client (DISPLAY)

  u_short size;
  result = recv(_socketFd, (char *)&size, sizeof size, 0);
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

  if (port > 0) {
    _controlFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_controlFd < 0) {
      perror("socket");
      return -1;
    }

    struct sockaddr_in backChannel;
    memcpy(&backChannel, &_client_addr, sizeof backChannel);
    backChannel.sin_port = htons(port);
    
#ifdef DEBUG
    printf("Connecting to client port %u\n", port);
#endif

    result = connect(_controlFd, (struct sockaddr *)&backChannel,
		     sizeof(struct sockaddr));
    if (result < 0) {
      perror("connect");
      return -1;
    }
  } else {
#ifdef DEBUG
    printf("No control channel.\n");
#endif
  }

  // now set socket to non-blocking mode

#ifdef SUN
  result = fcntl(_socketFd, F_SETFL, FNDELAY);
#else
  result = fcntl(_socketFd, F_SETFL, O_NDELAY);
#endif
  if (result < 0) {
    perror("fcntl");
    return -1;
  }
    
  printf("Client connection established.\n");
    
  return 1;
}

void ServerAPI::RestartSession()
{
  Dispatcher::Current()->Unregister(this);

  DestroySessionData();

  if (_socketFd >= 0) {
    close(_socketFd);
    _socketFd = -1;
  }

  if (_controlFd >= 0) {
    close(_controlFd);
    _controlFd = -1;
  }

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

int ServerAPI::Send(int fd, int flag, int bracket,
		    int numArgs, char **argv)
{
  u_short errflag = (flag == API_OK ? 0 : 1);

#ifdef DEBUG
  printf("Sending error flag %d\n", errflag);
#endif

  errflag = htons(errflag);
  int result = send(fd, (char *)&errflag, sizeof errflag, 0);
  if (result < (int)sizeof errflag) {
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
    SendControl(API_OK, "ChangeStatus 1");
}

void ServerAPI::SetIdle()
{
  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0)
    SendControl(API_OK, "ChangeStatus 0");
}

Boolean ServerAPI::IsBusy()
{
  return (_busy > 0);
}

void ServerAPI::ExecuteScript(char *script)
{
  char cmd[256];
  sprintf(cmd, "ExecuteScript %s", script);
  SendControl(API_OK, cmd);
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
  SendControl(API_OK, cmd);
}

void ServerAPI::ViewCreated(View *view)
{
#if 0
  char cmd[256];
  sprintf(cmd, "ProcessViewCreated {%s}", view->GetName());
  SendControl(API_OK, cmd);
#endif
}

void ServerAPI::ViewDestroyed(View *view)
{
#if 0
  char cmd[256];
  sprintf(cmd, "ProcessViewDestroyed {%s}", view->GetName());
  SendControl(API_OK, cmd);
#endif
}

/* Make view the current view in the control panel */

void ServerAPI::SelectView(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewSelected {%s}", view->GetName());
  SendControl(API_OK, cmd);
}
