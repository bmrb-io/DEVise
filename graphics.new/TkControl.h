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
  Revision 1.21  1997/11/19 17:02:12  wenger
  Fixed error in OpenDataChannel().

  Revision 1.20  1997/11/18 23:27:04  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.19  1997/11/12 15:46:41  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
  into the trunk.

  Revision 1.18.12.2  1998/01/07 16:00:24  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.18.12.1  1997/11/11 19:14:01  wenger
  Added getWindowImageAndSize and waitForQueries commands; fixed bug in
  WindowRep::ExportGIF() inheritance.

  Revision 1.18  1997/02/03 19:45:37  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.17  1996/12/12 22:03:17  jussi
  Cleaned up termination code in RestartSession().

  Revision 1.16  1996/08/29 22:23:25  guangshu
  Added functions OpenDataChannel and getFd.

  Revision 1.15  1996/08/04 21:29:53  beyer
  Added Raise() to raise the control panel to the top of the stacking order.

  Revision 1.14  1996/07/13 17:27:50  jussi
  Removed unnecessary virtual function prototypes.

  Revision 1.13  1996/06/24 19:47:25  jussi
  Added a destructor.

  Revision 1.12  1996/05/15 16:45:10  jussi
  Added support for the new server synchronization mechanism.
  Removed ExecuteScript function.

  Revision 1.11  1996/05/13 18:11:01  jussi
  Emptied ViewCreated and ViewDestroyed and moved them to
  the header file.

  Revision 1.10  1996/05/11 20:52:33  jussi
  Replaced strcpy() in ReturnVal() with Tcl_SetResult() in order
  to avoid buffer overruns. Added DoQuit() and PrintStat()
  to RestartSession().

  Revision 1.9  1996/05/11 19:10:14  jussi
  Added null support for replica management.

  Revision 1.8  1996/05/11 17:28:39  jussi
  Reorganized the code somewhat in order to match the ParseAPI
  interface.

  Revision 1.7  1996/05/11 03:14:12  jussi
  Removed some control panel variables like FileName, GDataName,
  WindowName etc.

  Revision 1.6  1996/04/16 19:46:24  jussi
  Added DoAbort() method.

  Revision 1.5  1996/01/27 00:21:36  jussi
  Added ExecuteScript() method.

  Revision 1.4  1995/12/29 18:27:00  jussi
  Added FilterAboutToChange() to facilitate new cursor mechanism.

  Revision 1.3  1995/11/29 15:10:23  jussi
  Added copyright notice and added primitives for Tk window support.

  Revision 1.2  1995/09/05 22:16:11  jussi
  Added CVS header.
*/

#ifndef TkControl_h
#define TkControl_h

#define OPEN_TEST_FILE 1

#include <tcl.h>
#include <tk.h>
#if OPEN_TEST_FILE
#include <fcntl.h>
#endif

#include "Dispatcher.h"
#include "Control.h"
#include "ViewCallback.h"
#include "GroupDir.h"
#include "QueryProc.h"
#include "ParseAPI.h"

class View;
class MapInterpClassInfo;

extern GroupDir *gdir;

class TkControlPanel: public ControlPanel, public DispatcherCallback,
                      private ViewCallback
{
public:
  TkControlPanel();
  virtual ~TkControlPanel();

#ifdef TK_WINDOW
  virtual Tcl_Interp *GetInterp() { return _interp; }
  virtual Tk_Window GetMainWindow() { return _mainWindow; }
#endif

  virtual void SelectView(View *view);

  /* Get/set busy status. */
  virtual void SetBusy();
  virtual void SetIdle();
  virtual Boolean IsBusy();

  /* Start/restart session */
  virtual void StartSession();
  virtual void DestroySessionData();
  virtual void RestartSession() { DoQuit(); }

  /* Perform sync operation  */
  virtual void SyncNotify();

  /* Raise the control panel */
  virtual void Raise();
  
  virtual void NotifyFrontEnd(char *script);

  /* Abort program */
  virtual void DoAbort(char *reason);

  /* Get GroupDir info */
  virtual GroupDir *GetGroupDir() { return gdir; }

  /* Get MapInterpClassInfo info */
  virtual MapInterpClassInfo *GetInterpProto() { return _interpProto; }

#if OPEN_TEST_FILE
  int _fd;
  virtual void OpenDataChannel(int port) { _fd = open("/tmp/devise_tst",
      O_RDWR | O_CREAT | O_TRUNC, 0644); }
  virtual int getFd(){ return _fd; } 
#else
  virtual void OpenDataChannel(int port) { }
  virtual int getFd(){ return -1;} 
#endif

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

  static int DEViseCmd(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[]);

  Tcl_Interp *_interp;
  Tk_Window _mainWindow;
  char *_argv0;

  virtual int ReturnVal(u_short flag, char *result) {
    Tcl_SetResult(_interp, result, TCL_VOLATILE);
    return 1;
  }
  virtual int ReturnVal(int argc, char **argv) {
    Tcl_ResetResult(_interp);
    for(int i = 0; i < argc; i++) 
      Tcl_AppendElement(_interp, argv[i]);
    return 1;
  }

  char *DispatchedName() { return "TkControlPanel"; }
  void Run();
};

#endif
