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
  Revision 1.1  1996/10/17 20:42:16  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <tcl.h>
#include <tk.h>

#include "ClientServer.h"

//#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

char *CompDate::Get()
{
  return __DATE__;
}

Client::Client(char *name, char *hostname, int port)
{
  _name = strdup(name);
  _hostname = strdup(hostname);
  DOASSERT(_name && _hostname, "Out of memory");
  _port = port;

  printf("Connecting to server %s:%d.\n", _hostname, _port);

  _serverFd = NetworkOpen(_hostname, _port);
  if (_serverFd < 0)
    exit(1);

  printf("Connection established.\n\n");
}

Client::~Client()
{
  (void)NetworkClose(_serverFd);
  delete _name;
  delete _hostname;
}

void Client::DoAbort(char *reason)
{
  fprintf(stderr, "%s\n", reason);
  (void)NetworkClose(_serverFd);
  exit(1);
}

int Client::ServerCmd(int argc, char **argv)
{
#ifdef DEBUG	
  printf("Function %s, %d args\n", argv[0], argc);
#endif

  if (NetworkSend(_serverFd, API_CMD, 0, argc, argv) < 0) {
    fprintf(stderr, "Server has terminated. Client exits.\n");
    exit(1);
  }

  u_short flag;
  int rargc;
  char **rargv;
  do {
    if (NetworkReceive(_serverFd, 1, flag, rargc, rargv) <= 0) {
      fprintf(stderr, "Server has terminated. Client exits.\n");
      exit(1);
    }
    if (flag == API_CTL)
      ControlCmd(rargc, rargv);
  } while (flag != API_ACK && flag != API_NAK);

  char *result = NetworkPaste(rargc, rargv);
  DOASSERT(result, "Out of memory");
#ifdef DEBUG
  printf("Received reply: \"%s\"\n", result);
#endif
  delete result;

  if (flag == API_NAK)
    return -1;

  return 1;
}

void Client::ReadServer()
{
#ifdef DEBUG
  printf("Receiving command from server\n");
#endif

  u_short flag;
  int argc;
  char **argv;
  if (NetworkReceive(_serverFd, 1, flag, argc, argv) <= 0)
    return;

  if (flag != API_CTL) {
    fprintf(stderr, "Ignoring unexpected message type: %u\n", flag);
    return;
  }

  ControlCmd(argc, argv);
}

void Client::ControlCmd(int argc, char **argv)
{
  char *cmd = NetworkPaste(argc, argv);
  DOASSERT(cmd, "Out of memory");
  printf("Received command from server: \"%s\"\n", cmd);
  delete cmd;
}

static int _serverCmd(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  TclClient *client = (TclClient *)cd;
  return client->ServerCmd(argc - 1, &argv[1]);
}

static void _readServer(ClientData cd, int mask)
{
  TclClient *client = (TclClient *)cd;
  client->ReadServer();
}

TclClient::TclClient(char *name, char *hostname, int port, char *script) :
	Client(name, hostname, port)
{
  _script = strdup(script);
  DOASSERT(_script, "Out of memory");

  _interp = Tcl_CreateInterp();
  DOASSERT(_interp, "Cannot create Tcl interpreter");
  if (Tcl_Init(_interp) == TCL_ERROR)
    DOASSERT(0, "Cannot initialize Tcl");

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
  _mainWindow = Tk_CreateMainWindow(_interp, NULL, _name, _name);
  DOASSERT(_mainWindow, _interp->result);
  Tk_MoveWindow(_mainWindow, 0, 0);
  Tk_GeometryRequest(_mainWindow, 100, 200);
#endif

  if (Tk_Init(_interp) == TCL_ERROR)
    DOASSERT(0, "Cannot initialize Tk");

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION > 0
  _mainWindow = Tk_MainWindow(_interp);
#endif

  Tcl_CreateCommand(_interp, "Server", _serverCmd, (ClientData)this, 0);

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
  Tk_CreateFileHandler(_serverFd, TK_READABLE | TK_EXCEPTION,
		       _readServer, (ClientData)this);
#else
  Tcl_CreateFileHandler(Tcl_GetFile((void *)_serverFd, TCL_UNIX_FD),
                        TCL_READABLE, _readServer, 0);
#endif
	
  int code = Tcl_EvalFile(_interp, _script);
  if (code != TCL_OK) {
    fprintf(stderr, "Cannot start up Tcl code.\n");
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }
}

TclClient::~TclClient()
{
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
  Tk_DeleteFileHandler(_serverFd);
#else
  Tcl_DeleteFileHandler(Tcl_GetFile((void *)_serverFd, TCL_UNIX_FD));
#endif

  delete _script;
}

void TclClient::MainLoop()
{
  Tk_MainLoop();
#ifdef DEBUG
  printf("Returned from Tk loop\n");
#endif
}

int TclClient::ServerCmd(int argc, char **argv)
{
#ifdef DEBUG	
  printf("Function %s, %d args\n", argv[0], argc);
#endif

  if (NetworkSend(_serverFd, API_CMD, 0, argc, argv) < 0) {
    fprintf(stderr, "Server has terminated. Client exits.\n");
    exit(1);
  }

  u_short flag;
  int rargc;
  char **rargv;
  do {
    if (NetworkReceive(_serverFd, 1, flag, rargc, rargv) <= 0) {
      fprintf(stderr, "Server has terminated. Client exits.\n");
      exit(1);
    }
    if (flag == API_CTL)
      ControlCmd(rargc, rargv);
  } while (flag != API_ACK && flag != API_NAK);

  char *result = NetworkPaste(rargc, rargv);
  DOASSERT(result, "Out of memory");
#ifdef DEBUG
  printf("Received reply: \"%s\"\n", result);
#endif
  Tcl_SetResult(_interp, result, TCL_VOLATILE);
  delete result;

  if (flag == API_NAK)
    return TCL_ERROR;

  return TCL_OK;
}

void TclClient::ReadServer()
{
#ifdef DEBUG
  printf("Receiving command from server\n");
#endif

  u_short flag;
  int argc;
  char **argv;
  if (NetworkReceive(_serverFd, 1, flag, argc, argv) <= 0) {
    fprintf(stderr, "Server has terminated. Client exits.\n");
    exit(1);
  }

  if (flag != API_CTL) {
    fprintf(stderr, "Ignoring unexpected message type: %u\n", flag);
    return;
  }

  ControlCmd(argc, argv);
}

void TclClient::ControlCmd(int argc, char **argv)
{
  char *cmd = NetworkPaste(argc, argv);
  DOASSERT(cmd, "Out of memory");
#ifdef DEBUG
  printf("Executing control command: \"%s\"\n", cmd);
#endif
  (void)Tcl_Eval(_interp, cmd);
  delete cmd;
}

Server::Server(char *name, int port)
{
  _name = strdup(name);
  DOASSERT(_name, "Out of memory");
  _port = port;

  _listenFd = -1;
  _clientFd = -1;
}

Server::~Server()
{
  CloseClient();
  delete _name;
}

void Server::DoAbort(char *reason)
{
  fprintf(stderr, "%s\n", reason);
  char *args[] = { "AbortProgram", reason };
  (void)NetworkSend(_clientFd, API_CTL, 0, 2, args);
  fprintf(stderr, "Server aborts.\n");
  exit(0);
}

void Server::MainLoop()
{
  while(1) {
    WaitForConnection();
    while(_clientFd >= 0)
      ReadCmd();
  }
}

void Server::WaitForConnection()
{
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
  servAddr.sin_port = htons(_port);
#ifdef DEBUG
  printf("Server listening at port %u.\n", _port);
#endif
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  result = bind(_listenFd, (struct sockaddr *)&servAddr,
		sizeof(struct sockaddr));
  if (result < 0)
    perror("bind");
  DOASSERT(result >= 0, "Cannot bind to address");

  result = listen(_listenFd, 5);
  if (result < 0)
    perror("listen");
  DOASSERT(result >= 0, "Cannot listen");

  printf("\nServer waiting for client connection.\n");

  struct sockaddr_in tempaddr;
  int len = sizeof(tempaddr);
  _clientFd = accept(_listenFd, (struct sockaddr *)&tempaddr, &len);
  if (_clientFd < 0) {
    perror("accept");
    if (errno == EINTR) {
      fprintf(stderr, "Server exits.\n");
      exit(1);
    }
    DOASSERT(0, "Error in network interface");
  }
  
  close(_listenFd);
  _listenFd = -1;

  printf("Client connection established.\n\n");
  BeginConnection();
}

void Server::ReadCmd()
{
  DOASSERT(_clientFd >= 0, "Invalid socket");

  u_short flag;
  int argc;
  char **argv;
  int result = NetworkReceive(_clientFd, 1, flag, argc, argv);
  if (result < 0) {
    perror("recv");
    goto error;
  }

  if (!result) {
#ifdef DEBUG
    printf("End of client data.\n");
#endif
    goto error;
  }

  if (flag != API_CMD) {
    fprintf(stderr, "Received unexpected type of message: %u\n", flag);
    goto error;
  }

#ifdef DEBUG
  printf("Executing command\n");
#endif

  ProcessCmd(argc, argv);

#ifdef DEBUG
  printf("Done executing command\n");
#endif

  return;

 error:
  CloseClient();
}

void Server::ProcessCmd(int argc, char **argv)
{
  char *cmd = NetworkPaste(argc, argv);
  DOASSERT(cmd, "Out of memory");
  printf("Received command from client: \"%s\"\n", cmd);
  delete cmd;

  char *args[] = { "done" };
  if (NetworkSend(_clientFd, API_ACK, 0, 1, args) < 0) {
    fprintf(stderr, "Client error.\n");
    CloseClient();
    return;
  }

  if (argc == 1 && !strcmp(argv[0], "exit"))
    CloseClient();
}

void Server::CloseClient()
{
  if (_clientFd >= 0) {
    EndConnection();
    printf("Closing client connection.\n");
    (void)NetworkClose(_clientFd);
  }
  _clientFd = -1;
}

WinServer::WinServer(char *name, int port) : Server(name, port)
{
  _display = DeviseDisplay::DefaultDisplay();
  DOASSERT(_display, "Out of memory");
}

WinServer::~WinServer()
{
  delete _display;
}

void WinServer::MainLoop()
{
  while(1) {
    WaitForConnection();

    while(_clientFd >= 0) {
      int wfd = _display->fd();
      fd_set fdset;
      memset(&fdset, 0, sizeof fdset);
      FD_SET(wfd, &fdset);
      FD_SET(_clientFd, &fdset);
      
      int maxFdCheck = (wfd > _clientFd ? wfd : _clientFd);
      int numFds = select(maxFdCheck + 1, &fdset, 0, 0, 0);
      if (numFds < 0)
	perror("select");
      DOASSERT(numFds > 0, "Internal error");

      if (FD_ISSET(_clientFd, &fdset))
	ReadCmd();
      else
	_display->InternalProcessing();
    }
  }
}
