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

#include <tcl.h>
#include <tk.h>

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
  virtual void RestartSession() {
    DestroySessionData();
    QueryProc::Instance()->PrintStat();
    DoQuit();
  }

  /* Perform sync operation  */
  virtual void SyncNotify();

  /* Abort program */
  virtual void DoAbort(char *reason);

  /* Get GroupDir info */
  virtual GroupDir *GetGroupDir() { return gdir; }

  /* Get MapInterpClassInfo info */
  virtual MapInterpClassInfo *GetInterpProto() { return _interpProto; }

  /* Add replica server */
  virtual int AddReplica(char *hostName, int port) { return 1; }

  /* Remove replica server */
  virtual int RemoveReplica(char *hostName, int port) { return 1; }

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
  virtual void FilterAboutToChange(View *view) {}
  virtual void FilterChanged(View *view, VisualFilter &filter, int flushed);
  virtual void ViewCreated(View *view) {}
  virtual void ViewDestroyed(View *view) {}

  char *DispatchedName() { return "TkControlPanel"; }
  void Run();

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
};

#endif
