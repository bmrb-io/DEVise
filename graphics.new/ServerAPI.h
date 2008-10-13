/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2008
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
  Revision 1.23  2005/12/06 20:04:12  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.22.14.1  2005/09/28 17:14:50  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.22  2000/02/16 18:51:43  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.21  2000/01/13 23:07:11  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.20  1999/10/18 15:36:43  wenger
  Window destroy events are handled better (DEVise doesn't crash); messages
  such as window destroy notifications are now passed to the client in
  client/server form.  (Parsing a string into arguments was moved from the
  Session class to the ArgList class.)

  Revision 1.19  1999/10/04 19:37:10  wenger
  Mouse location is displayed in "regular" DEVise.

  Revision 1.18  1998/09/28 20:05:54  wenger
  Fixed bug 383 (unnecessary creation of QueryProc); moved all
  DestroySessionData() code from subclasses of ControlPanel into base class,
  because it was all the same; found and fixed bug 398 (caused by a change
  in the propagation of view selections).

  Revision 1.17  1998/09/22 17:24:00  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.16  1998/05/02 09:00:51  taodb
  Added support for JAVA Screen and command logging

  Revision 1.15  1998/02/26 20:49:04  taodb
  Replaced ParseAPI() with Command Object Interface

  Revision 1.14  1998/02/12 17:17:08  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.13  1998/02/02 18:26:16  wenger
  Strings file can now be loaded manually; name of strings file is now
  stored in session file; added 'serverExit' command and kill_devised
  script to cleanly kill devised; fixed bug 249; more info is now
  printed for unrecognized commands.

  Revision 1.12.2.1  1998/01/28 22:44:01  taodb
  Added support for group communicatoin

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

  virtual void ShowMouseLocation(const char *dataX, const char *dataY);

  /* Get/set busy status. */
  virtual void SetBusy();
  virtual void SetIdle();
  virtual Boolean IsBusy();

  /* Start/restart session */
  virtual void StartSession() {}
  virtual void RestartSession();

  /* Perform sync operation  */
  virtual void SyncNotify();

  /* Abort program */
  virtual void DoAbort(const char *reason);

  /* Get GroupDir info */
  virtual GroupDir *GetGroupDir() { return gdir; }

  /* Get MapInterpClassInfo info */
  virtual MapInterpClassInfo *GetInterpProto() { return _interpProto; }

  virtual void OpenDataChannel(int port);
  virtual int getFd() { return _dataFd; }

  virtual int NumClients() { return _server->NumClients(); }

  virtual void Raise();

  virtual void NotifyFrontEnd(const char *script);


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
  virtual void FilterChanged(View *view, const VisualFilter &filter,
      int flushed);

  int _busy;
  static MapInterpClassInfo *_interpProto;

  virtual int ReturnVal(u_short flag, const char *result) {
	_valueReturned = true;
    return _server->ReturnVal(flag, result);
  }
  virtual int ReturnVal(int argc, const char * const *argv) {
	_valueReturned = true;
    return _server->ReturnVal(argc, argv);
  }

  virtual int ReturnVal(int flag, int argc, const char * const *argv,
      bool addBrace){
	_valueReturned = true;
    return _server->ReturnVal(flag, argc, argv, addBrace);
  } 
  virtual int SendControl(u_short flag, char *result, bool grp_enable) {
	if (grp_enable)
	{
		_server->ServerClientCmd(flag, 1,&result);
	}
    return _server->SendControl(flag, result);
  }

  virtual int SendControl(int argc, const char * const *argv, bool grp_enable) {
	if (grp_enable)
	{
		_server->ServerClientCmd(API_CTL, argc, argv);
	}
    return _server->SendControl(API_CTL, argc, argv);
  }

  int _dataFd;				// socket for transfering large data, eg.gif

  public:
  DeviseServer *_server;
};

#endif
