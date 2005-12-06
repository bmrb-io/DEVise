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
  Revision 1.3.26.1  2005/09/28 22:29:34  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.3  1998/02/12 17:15:38  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.2.2.1  1998/01/28 22:43:16  taodb
  Added support for group communicatoin

  Revision 1.2  1998/01/07 19:28:13  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

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
  DeviseClient(char *name, char *hostname, int port, Boolean createWindow,
  	char* initStr);
  virtual ~DeviseClient();
  virtual int RequestRelinquish() { return Client::RequestRelinquish();}
  virtual int NotifyGrabbed() { return Client::NotifyGrabbed();}

  virtual void MainLoop();

  virtual int EvalCmd(char *cmd) { return Tcl_Eval(_interp, cmd); }
  	// process server command
  virtual void ControlCmd(int argc, const char * const *argv);

  virtual Tcl_Interp *Interp() { return _interp; }
  virtual int ServerFd() { return _serverFd; }
  virtual Boolean SyncDone() { return _syncDone; }

protected:
  Tk_Window _mainWindow;
  Boolean _syncDone;
};

#endif /* _DeviseClient_h_ */

/*============================================================================*/
