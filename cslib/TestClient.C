/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Client for testing DEVise data socket.
*/

/*
  $Id$

  $Log$
  Revision 1.1.2.1  1998/01/07 15:59:00  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Client.h"
#include "ClientAPI.h"

static char *_progName = 0;

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }

class TestClient : public Client {
public:
  TestClient(char *name, char *hostname, int serverPort, int dataPort) :
      Client(name, hostname, serverPort)
  {
    _dataPort = dataPort;
    _listenFd = -1;
  }
  virtual ~TestClient() {}

  virtual void MainLoop() {}
  virtual int ServerCmd(int argc, char **argv);
  virtual void WaitForConnection();

protected:
  void InitializeListenFd();

  int _listenFd;
  int _dataPort;
};

int
TestClient::ServerCmd(int argc, char **argv)
{
#ifdef DEBUG	
  printf("ServerCmd: %s, %d args\n", argv[0], argc);
#endif

  if (NetworkSend(_serverFd, API_CMD, 0, argc, argv) < 0) {
    fprintf(stderr, "Server has terminated. Client exits.\n");
    exit(1);
  }

  if (!strcmp(argv[0], "testDataSock")) {
    WaitForConnection();
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

  delete _result;
  _result = NetworkPaste(rargc, rargv);
  DOASSERT(_result, "Out of memory");
#ifdef DEBUG
  printf("Received reply: \"%s\"\n", _result);
#endif

  int status;
  if (flag == API_NAK) {
    status = -1;
  } else {
    status = 1;
  }

  printf("Command result: <%s>\n", _result);
  delete _result;
  _result = NULL;

  if (status <= 0) {
    fprintf(stderr, "Server error.\n");
  }

  return status;
}

void TestClient::WaitForConnection()
{
    int dataFd;
    if (_listenFd < 0)
    {
	InitializeListenFd();
    }
    printf("Client waiting for data connection on port %d\n", _dataPort);

    struct sockaddr_in tempaddr;
    int len = sizeof(tempaddr);
    dataFd = accept(_listenFd, (struct sockaddr *)&tempaddr, &len);
    if (dataFd < 0)
    {
	perror("accept() failed");
	if (errno == EINTR)
	{
	    fprintf(stderr, "Client exits.\n");
	    exit(1);
	}
	DOASSERT(0, "Error in network interface");
    }
    printf("Data connection established to server\n");

    char buf[256];
    int bytesRead = read(dataFd, buf, sizeof(buf));
    printf("Bytes read: %d\n", bytesRead);
    printf("Data: <%s>\n", buf);
    close(dataFd);
}

void TestClient::InitializeListenFd()
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
    servAddr.sin_port = htons(_dataPort);
#ifdef DEBUG
    printf("Client listening at port %u.\n", _dataPort);
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

/*======================================================================*/

void Usage()
{
  fprintf(stderr, "Usage: %s [-h host] [-p port]\n", _progName);
}

int main(int argc, char **argv)
{
  _progName = argv[0];

  char *hostname = "localhost";
  int portnum = DefaultNetworkPort;

  for(int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No host name specified with -h option.\n");
	Usage();
	exit(1);
      }
      hostname = argv[i + 1];
      i++;
    } else if (!strcmp(argv[i], "-p")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No port number specified with -p option.\n");
	Usage();
	exit(1);
      }
      portnum = atoi(argv[i + 1]);
      i++;
    } else {
      Usage();
      exit(1);
    }
  }

  char *dataPortStr = "6111";
  TestClient client("Client", hostname, portnum, atoi(dataPortStr));

  int cmdArgc;
  char *cmdArgv[10];

  cmdArgc = 1;
  cmdArgv[0] = "version";
  client.ServerCmd(cmdArgc, cmdArgv);

  cmdArgc = 2;
  cmdArgv[0] = "testDataSock";
  cmdArgv[1] = dataPortStr;
  client.ServerCmd(cmdArgc, cmdArgv);

  return 0;
}
