/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.12  1998/01/07 19:29:55  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.11.16.1  1998/01/07 16:00:19  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.11  1996/08/29 22:21:41  guangshu
  Added functions Open Data Channel and getFd and several members.

  Revision 1.10  1996/07/13 17:28:00  jussi
  Removed unnecessary virtual function prototypes.

  Revision 1.9  1996/06/27 16:39:50  jussi
  With the new dispatching scheme in place, the server uses blocking-mode
  I/O with interrupts generated from the select() call in the dispatcher.
  The switching between server-listen and server-connected mode was
  streamlined.

  Revision 1.8  1996/05/20 18:42:30  jussi
  Merged with ClientServer library code.

  Revision 1.7  1996/05/15 16:41:17  jussi
  Moved all networking code from ServerAPI.c to ClientAPI.c.
  Improved support for bracketed or non-bracketed arguments
  passed in commands.

  Revision 1.6  1996/05/13 18:13:39  jussi
  Changed type of "flag" argument to ReturnVal(). Merged control
  channel with regular socket pair. Emptied ViewCreated and
  ViewDestroyed and moved them to the header file.

  Revision 1.5  1996/05/11 20:52:57  jussi
  Removed tcl.h and tk.h header file inclusion.

  Revision 1.4  1996/05/11 19:09:44  jussi
  Added replica management.

  Revision 1.3  1996/05/11 17:26:57  jussi
  Reorganized code in order to interface with ParseAPI properly.

  Revision 1.2  1996/05/11 01:53:34  jussi
  Condensed the code by removing some unnecessary Tcl/Tk parts
  which are not used by the server. Changed the client-server
  protocol somewhat.

  Revision 1.1  1996/05/09 18:14:39  kmurli
  Modified Group.C and GroupDir.C to include an oiverloaded functions for
  get_items, subitems to take in a char * instead of Tcp_interp *. This
  is for use in the ServerAPI.c
*/

#ifndef ServerAPI_h
#define ServerAPI_h

#include "Control.h"
#include "ViewCallback.h"
#include "GroupDir.h"
#include "ClientAPI.h"
#include "DeviseServer.h"

class View;
class MapInterpClassInfo;

extern GroupDir *gdir;

class ServerAPI: public ControlPanel, private ViewCallback
{
public:
  ServerAPI();
  virtual ~ServerAPI();

  virtual void SelectView(View *view);

  /* Get/set busy status. */
  virtual void SetBusy();
  virtual void SetIdle();
  virtual Boolean IsBusy();

  /* Start/restart session */
  virtual void StartSession() {}
  virtual void DestroySessionData();
  virtual void RestartSession();

  /* Perform sync operation  */
  virtual void SyncNotify();

  /* Abort program */
  virtual void DoAbort(char *reason);

  /* Get GroupDir info */
  virtual GroupDir *GetGroupDir() { return gdir; }

  /* Get MapInterpClassInfo info */
  virtual MapInterpClassInfo *GetInterpProto() { return _interpProto; }

  virtual void OpenDataChannel(int port);
  virtual int getFd() { return _dataFd; }

  virtual int NumClients() { return _server->NumClients(); }

protected:
  virtual void SubclassInsertDisplay(DeviseDisplay *disp,
				     Coord x, Coord y,
				     Coord w, Coord h) {}
  virtual void SubclassRegisterView(View *view) {}
  virtual void SubclassUnregisterView(View *view) {}
  virtual void SubclassDoInit() {}
  virtual void SubclassDoViewPerspectiveChanged(View *view,
						Coord x, Coord y,
						Coord w, Coord h) {}

private:
  virtual void FilterChanged(View *view, VisualFilter &filter, int flushed);

  int _busy;
  static MapInterpClassInfo *_interpProto;

  virtual int ReturnVal(u_short flag, char *result) {
    return _server->ReturnVal(flag, result);
  }
  virtual int ReturnVal(int argc, char **argv) {
    return _server->ReturnVal(argc, argv);
  }
  virtual int SendControl(u_short flag, char *result) {
    return _server->SendControl(flag, result);
  }
  virtual int SendControl(int argc, char **argv) {
    return _server->SendControl(API_CTL, argc, argv);
  }

  int _dataFd;				// socket for transfering large data, eg.gif

  DeviseServer *_server;
};

#endif
