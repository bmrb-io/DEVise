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
  Implementation of TkClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/01/07 19:27:53  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.1  1997/12/09 19:03:26  wenger
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
#include <tcl.h>
#include <tk.h>

#include "TkClient.h"
#include "ClientAPI.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

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

TkClient::TkClient(char *name, char *hostname, int port, char *script) :
	TclClient(name, hostname, port)
{
  _script = strdup(script);
  DOASSERT(_script, "Out of memory");

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

#if TK_MAJOR_VERSION == 4
#  if TK_MINOR_VERSION == 0
  Tk_CreateFileHandler(_serverFd, TK_READABLE | TK_EXCEPTION,
		       _readServer, (ClientData)this);
#  else
  Tcl_CreateFileHandler(Tcl_GetFile((void *)_serverFd, TCL_UNIX_FD),
                        TCL_READABLE, _readServer, 0);
#  endif
#else
  Tcl_CreateFileHandler(_serverFd, TCL_READABLE, _readServer, 0);
#endif
	
  int code = Tcl_EvalFile(_interp, _script);
  if (code != TCL_OK) {
    fprintf(stderr, "Cannot start up Tcl code.\n");
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }
}

TkClient::~TkClient()
{
#if TK_MAJOR_VERSION == 4
#  if TK_MINOR_VERSION == 0
  Tk_DeleteFileHandler(_serverFd);
#  else
  Tcl_DeleteFileHandler(Tcl_GetFile((void *)_serverFd, TCL_UNIX_FD));
#  endif
#else
  Tcl_DeleteFileHandler(_serverFd);
#endif

  delete _script;
}

void TkClient::MainLoop()
{
  Tk_MainLoop();
#ifdef DEBUG
  printf("Returned from Tk loop\n");
#endif
}
