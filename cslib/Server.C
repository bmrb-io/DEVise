/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of Server class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/01/07 19:27:51  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.5  1998/01/09 16:33:37  wenger
  Updated copyright date and version number; minor mods to compile for
  hp and sun; fixed problem with _batchMode flag getting improperly
  reset in the ControlPanel class (prevented using pixmaps instead of
  X windows).

  Revision 1.1.2.4  1998/01/07 15:58:58  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.1.2.3  1997/12/30 16:47:55  wenger
  Removed single-client compile option to simplify the code.

  Revision 1.1.2.2  1997/12/17 17:30:11  wenger
  Got cslib to compile for Linux.

  Revision 1.1.2.1  1997/12/09 19:03:24  wenger
  deviseb now uses client/server library.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/param.h> // For MAXPATHLEN.

#if defined(LINUX)
#include <sys/time.h>
#endif

#include "ClientAPI.h"
#include "Server.h"
#include "machdep.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

Server::Server(char *name, int port, int maxClients)
{
    _name = strdup(name);
    DOASSERT(_name, "Out of memory");
    _port = port;
    _listenFd = -1;
    _clients = new ClientInfo[maxClients];
    _numClients = 0;
    _maxClients = maxClients;
    _cmd = NULL;
}

Server::~Server()
{
    for (int i = 0; i < _maxClients; i++)
    {
	CloseClient(i);
    }
    delete [] _clients;
    delete _name;
    delete _cmd;
}

void Server::DoAbort(char *reason)
{
    fprintf(stderr, "%s\n", reason);
    char *args[] = { "AbortProgram", reason };
    SendControl(API_CTL, 2, args);
    fprintf(stderr, "Server aborts.\n");
    exit(0);
}

void Server::MainLoop()
{
    while (1) {
        WaitForConnection();
        while (_numClients > 0)
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

void Server::WaitForConnection()
{
    int clientfd;
    if (_listenFd < 0)
    {
	InitializeListenFd();
    }
    printf("%s server waiting for client connection on port %d\n",
      _name, _port);
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
}

void Server::ProcessCmd(ClientID clientID, int argc, char **argv)
{
    GetCmd(argc, argv);
    printf("Received command from client: \"%s\"\n", _cmd);
    delete _cmd;

    bool doClose = false;
    int result = ReturnVal(clientID, API_ACK, "done");

    if (result < 0)
    {
	doClose = true;
    }
    else if (argc == 1 && !strcmp(argv[0], "exit"))
    {
	doClose = true;
    }
    
    if (doClose)
    {
	CloseClient(clientID);
    }
}

void Server::CloseClient(ClientID clientID)
{
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
}

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
    numFds = select(maxFdCheck + 1,
#if defined(HPUX)
	(int *) &fdset, (int *) 0, (int *) 0,
#else
	&fdset, 0, 0,
#endif
	0);
    if (numFds < 0)
    {
	char errBuf[MAXPATHLEN + 256];
	sprintf(errBuf, "select() failed at %s: %d", __FILE__, __LINE__);
	perror(errBuf);
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

void
Server::GetCmd(int argc, char **argv)
{
    delete _cmd;
    _cmd = NetworkPaste(argc, argv);
    DOASSERT(_cmd, "Out of memory");
#if defined(DEBUG)
    printf("Received command from client: \"%s\"\n", _cmd);
#endif
}

int
Server::ReturnVal(ClientID clientID, u_short flag, int argc, char **argv,
    int addBraces)
{
    if (flag != API_ACK && flag != API_NAK) {
        fprintf(stderr, "Is this really a return value?\n");
    }

    int status = NetworkSend(_clients[clientID].fd, flag, addBraces, argc,
      argv);

    if (status < 0) {
	fprintf(stderr, "Client error.\n");
    }

    return status;
}

int
Server::SendControl(u_short flag, int argc, char **argv, int addBraces)
{
#if defined(DEBUG)
    printf("SendControl(");
    int num;
    for (num = 0; num < argc; num++) {
      printf("<%s> ", argv[num]);
    }
    printf(")\n");
#endif

    int status = 1;

    if (flag != API_CMD && flag != API_CTL) {
        fprintf(stderr, "Is this really a command?\n");
    }

    ClientID clientID;
    for (clientID = 0; clientID < _maxClients; clientID++) {
	if (_clients[clientID].valid) {
#if defined(DEBUG)
            printf("  sending control command to client %d\n", clientID);
#endif
            int tmpStatus = NetworkSend(_clients[clientID].fd, flag, addBraces,
	        argc, argv);
            if (tmpStatus < 0) {
	        fprintf(stderr, "Client error.\n");
	        status = -1;
            }
	}
    }

    return status;
}
