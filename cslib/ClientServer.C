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
  Revision 1.7.4.1  1997/12/06 17:42:52  wenger
  Split libcs.a into libdevcs.a and libdevwin.a in preparation for
  incorporating client/server part into DEVise.

  Revision 1.7  1997/08/12 15:32:36  wenger
  Fixed problems with WinServer::SingleStep() function.

  Revision 1.6  1997/08/07 18:42:08  wenger
  Added SingleStep() functions to the Server and WinServer classes (needed
  for Opossum).

  Revision 1.5  1997/05/21 21:05:08  andyt
  Support for multiple clients in client-server library. Single-client mode
  still supported by compiling with -DSINGLE_CLIENT. Client-server library
  can now display EmbeddedTk windows and Tasvir images. Added a sample
  Embedded-Tk script and GIF image to the repository (ETkSample.tcl,
  earth.gif). Modified tarcslib script to include all new files.

  Revision 1.4  1996/12/02 18:38:59  wenger
  Fixed memory leak in client/server library.

  Revision 1.3  1996/11/23 00:24:00  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.2  1996/10/18 15:19:43  jussi
  Added CompDate class.

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

//-----------------------------------------------------------------------
// BEGIN Server class
//-----------------------------------------------------------------------

#if defined(SINGLE_CLIENT)
Server::Server(char *name, int port)
#else
Server::Server(char *name, int port, int maxClients)
#endif
{
    _name = strdup(name);
    DOASSERT(_name, "Out of memory");
    _port = port;
    _listenFd = -1;
#if defined(SINGLE_CLIENT)
    _clientFd = -1;
#else
    _clients = new ClientInfo[maxClients];
    _numClients = 0;
    _maxClients = maxClients;
#endif
}

Server::~Server()
{
#if defined(SINGLE_CLIENT)
    CloseClient();
#else
    for (int i = 0; i < _maxClients; i++)
    {
	CloseClient(i);
    }
    delete [] _clients;
#endif
    delete _name;
}

void Server::DoAbort(char *reason)
{
    fprintf(stderr, "%s\n", reason);
    char *args[] = { "AbortProgram", reason };
    
#if defined(SINGLE_CLIENT)
    (void)NetworkSend(_clientFd, API_CTL, 0, 2, args);
#else
    for (int i = 0; i < _maxClients; i++)
    {
	if (_clients[i].valid)
	{
	    (void)NetworkSend(_clients[i].fd, API_CTL, 0, 2, args);
	}
    }
#endif
    fprintf(stderr, "Server aborts.\n");
    exit(0);
}

void Server::MainLoop()
{
    while (1) {
        WaitForConnection();
#if defined(SINGLE_CLIENT)
        while(_clientFd >= 0)
#else
        while (_numClients > 0)
#endif
        {
	    SingleStep();
        }
    }
}

void Server::SingleStep()
{
    ReadCmd();
}

void Server::InitializeListenFd()
{
    DOASSERT(_listenFd < 0, "Invalid socket");
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenFd < 0)
    {
	perror("socket() failed");
    }
    DOASSERT(_listenFd >= 0, "Cannot create socket");
    
    // allow listening port number to be reused
    int enable = 1;
    int result = setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR,
			    (char *)&enable, sizeof enable);
    if (result < 0)
    {
	perror("setsockopt() failed");
    }
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
    {
	perror("bind() failed");
    }
    DOASSERT(result >= 0, "Cannot bind to address");
    
    result = listen(_listenFd, 5);
    if (result < 0)
    {
	perror("listen() failed");
    }
    DOASSERT(result >= 0, "Cannot listen");
    
    printf("Listening fd has been initialized\n");
    
}

#if !defined(SINGLE_CLIENT)
int Server::FindIdleClientSlot()
{
    for (int i = 0; i < _maxClients; i++)
    {
	if (!_clients[i].valid)
	{
	    return i;
	}
    }
    return -1;
}
#endif

void Server::WaitForConnection()
{
    int clientfd;
#if defined(SINGLE_CLIENT)
    DOASSERT(_listenFd < 0, "Invalid socket");
#endif
    if (_listenFd < 0)
    {
	InitializeListenFd();
    }
    printf("Server waiting for client connection\n");
    struct sockaddr_in tempaddr;
    int len = sizeof(tempaddr);
    clientfd = accept(_listenFd, (struct sockaddr *)&tempaddr, &len);
    if (clientfd < 0)
    {
	perror("accept() failed");
	if (errno == EINTR)
	{
	    fprintf(stderr, "Server exits.\n");
	    exit(1);
	}
	DOASSERT(0, "Error in network interface");
    }
#if defined(SINGLE_CLIENT)
    _clientFd = clientfd;
    close(_listenFd);
    _listenFd = -1;
    printf("Client connection established.\n\n");
    BeginConnection();
#else
    int slot = FindIdleClientSlot();
    if (slot < 0)
    {
	fprintf(stderr, "WARNING: Too many clients. Connection denied\n");
	close(clientfd);
	return;
    }
    _clients[slot].valid = true;
    _clients[slot].fd = clientfd;
    _numClients++;
    printf("Connection established to client %d\n", slot);
    BeginConnection(slot);
#endif
}

#if defined(SINGLE_CLIENT)
void Server::ProcessCmd(int argc, char **argv)
#else
void Server::ProcessCmd(ClientID clientID, int argc, char **argv)
#endif
{
    char *cmd = NetworkPaste(argc, argv);
    DOASSERT(cmd, "Out of memory");
    printf("Received command from client: \"%s\"\n", cmd);
    delete cmd;
    int result;
    bool doClose = false;
    char *args[] = { "done" };
#if defined(SINGLE_CLIENT)
    result = NetworkSend(_clientFd, API_ACK, 0, 1, args);
#else
    result = NetworkSend(_clients[clientID].fd, API_ACK, 0, 1, args);
#endif

    if (result < 0)
    {
	fprintf(stderr, "Client error.\n");
	doClose = true;
    }
    else if (argc == 1 && !strcmp(argv[0], "exit"))
    {
	doClose = true;
    }
    
    if (doClose)
    {
#if defined(SINGLE_CLIENT)
	CloseClient();
#else
	CloseClient(clientID);
#endif
    }

}

#if defined(SINGLE_CLIENT)
void Server::CloseClient()
#else
void Server::CloseClient(ClientID clientID)
#endif
{
#if defined(SINGLE_CLIENT)
    if (_clientFd >= 0) {
	EndConnection();
	printf("Closing client connection.\n");
	(void)NetworkClose(_clientFd);
    }
    _clientFd = -1;
#else
    if (!_clients[clientID].valid)
    {
	return;
    }
    if (_clients[clientID].fd >= 0)
    {
	EndConnection(clientID);
	printf("Closing client connection.\n");
	(void)NetworkClose(_clients[clientID].fd);
    }
    _clients[clientID].fd = -1;
    _clients[clientID].valid = false;
    _numClients--;
#endif
}

#if defined(SINGLE_CLIENT)
//
// Reading client commands when only a SINGLE client is allowed
//
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
#else
//
// Reading client commands when MULTIPLE clients are allowed
//
void Server::ReadCmd()
{
    fd_set fdset;
    int maxFdCheck;
    int numFds;
    bool error = false;
    //
    // Initialize the fd set
    //
    memset(&fdset, 0, sizeof fdset);
    FD_SET(_listenFd, &fdset);
    maxFdCheck = _listenFd;
    for (int i = 0; i < _maxClients; i++)
    {
	if (_clients[i].valid)
	{
	    FD_SET(_clients[i].fd, &fdset);
	    if (_clients[i].fd > maxFdCheck)
	    {
		maxFdCheck = _clients[i].fd;
	    }
	}
    }
    //
    // select()
    //
    numFds = select(maxFdCheck + 1, &fdset, 0, 0, 0);
    if (numFds < 0)
    {
	perror("select() failed");
    }
    DOASSERT(numFds > 0, "Internal error");
    //
    // Handle a new connection request
    //
    if (FD_ISSET(_listenFd, &fdset))
    {
	WaitForConnection();
    }
    //
    // Process incoming commands on all client fds
    //
    ExecClientCmds(&fdset);
}
void Server::ExecClientCmds(fd_set *fdset)
{
    u_short flag;
    int result;
    int argc;
    char **argv;
    for (int i = 0; i < _maxClients; i++)
    {
	if (!_clients[i].valid || !FD_ISSET(_clients[i].fd, fdset))
	{
	    continue;
	}
	result = NetworkReceive(_clients[i].fd, 1, flag, argc, argv);
	if (result < 0)
	{
	    CloseClient(i);
	}
	else if (!result)
	{
#ifdef DEBUG
	    printf("End of client data.\n");
#endif
	    CloseClient(i);
	}
	else if (flag != API_CMD)
	{
	    fprintf(stderr, "Received unexpected type of message: %u\n", flag);
	    CloseClient(i);
	}
	else
	{
#ifdef DEBUG
	    printf("Executing command\n");
#endif
	    ProcessCmd(i, argc, argv);
#ifdef DEBUG
	    printf("Done executing command\n");
#endif
	}
    }
}
#endif
