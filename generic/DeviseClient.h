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
  Declaration of DeviseClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.2  1997/12/09 19:03:36  wenger
  deviseb now uses client/server library.

  Revision 1.1.2.1  1997/12/09 16:03:46  wenger
  Devise client now uses client/server library.

 */

#ifndef _DeviseClient_h_
#define _DeviseClient_h_

#include <tk.h>

#include "DeviseTypes.h"
#include "TclClient.h"

class DeviseClient : public TclClient {
public:
  DeviseClient(char *name, char *hostname, int port, Boolean createWindow);
  virtual ~DeviseClient();

  virtual void MainLoop();

  virtual int EvalCmd(char *cmd) { return Tcl_Eval(_interp, cmd); }
  virtual void ControlCmd(int argc, char **argv); // process server command

  virtual Tcl_Interp *Interp() { return _interp; }
  virtual int ServerFd() { return _serverFd; }
  virtual Boolean SyncDone() { return _syncDone; }

protected:
  Tk_Window _mainWindow;
  Boolean _syncDone;
};

#endif /* _DeviseClient_h_ */

/*============================================================================*/
