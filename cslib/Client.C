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
  Implementation of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
  Revision 1.4  1998/02/19 23:24:18  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.3  1998/02/12 17:15:26  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.2.2.1  1998/01/28 22:43:10  taodb
  Added support for group communicatoin

  Revision 1.2  1998/01/07 19:27:45  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.2  1998/01/07 15:58:56  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.1.2.1  1997/12/09 19:03:20  wenger
  deviseb now uses client/server library.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Client.h"
#include "ClientAPI.h"
#define DOASSERT(cond, str)\
	if ((cond))\
	{\
		fprintf(stderr,str);\
	}
//#include "Exit.h"
//#define DEBUG

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }

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
  _result = NULL;
  _cmd = NULL;

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
  delete _result;
  delete _cmd;
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

  delete _result;
  _result = NetworkPaste(rargc, rargv);
  DOASSERT(_result, "Out of memory");
#ifdef DEBUG
  printf("Received reply: \"%s\"\n", _result);
#endif

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

void Client::ControlCmd(int argc, char **argv)
{
  delete _cmd;
  _cmd = NetworkPaste(argc, argv);
  DOASSERT(_cmd, "Out of memory");
#if defined(DEBUG)
  printf("Received command from server: \"%s\"\n", _cmd);
#endif
}
