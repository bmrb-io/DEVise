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
  Implementation of TclClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.2.28.1  2005/09/28 22:29:28  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.2  1998/01/07 19:27:52  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.1  1997/12/09 19:03:25  wenger
  deviseb now uses client/server library.

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "TclClient.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

TclClient::TclClient(char *name, char *hostname, int port) :
	Client(name, hostname, port)
{
  _interp = Tcl_CreateInterp();
  DOASSERT(_interp, "Cannot create Tcl interpreter");
  if (Tcl_Init(_interp) == TCL_ERROR)
    DOASSERT(0, "Cannot initialize Tcl");
}

TclClient::~TclClient()
{
  Tcl_DeleteInterp(_interp);
}

int TclClient::ServerCmd(int argc, const char * const *argv)
{
#ifdef DEBUG	
  printf("Function %s, %d args\n", argv[0], argc);
#endif

  int flag = Client::ServerCmd(argc, argv);

  Tcl_SetResult(_interp, _result, TCL_VOLATILE);
  delete _result;
  _result = NULL;

  if (flag <= 0) {
    return TCL_ERROR;
  }

  return TCL_OK;
}

void TclClient::ReadServer()
{
  Client::ReadServer();
}

void TclClient::ControlCmd(int argc, const char * const *argv)
{
  Client::ControlCmd(argc, argv);
#ifdef DEBUG
  printf("Executing control command: \"%s\"\n", cmd);
#endif
  (void)Tcl_Eval(_interp, _cmd);
  delete _cmd;
  _cmd = NULL;
}
