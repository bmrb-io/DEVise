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

/* Xaw implementation for control panel */

#ifndef TkControl_h
#define TkControl_h

#include <tcl.h>
#include <tk.h>
#include "Dispatcher.h"
#include "Control.h"
#include "ViewCallback.h"

class View;
class MapInterpClassInfo;

class TkControlPanel: public ControlPanel, public DispatcherCallback,
                      private ViewCallback{
public:
	TkControlPanel();

#ifdef TK_WINDOW
	virtual Tcl_Interp *GetInterp() { return _interp; }
	virtual Tk_Window GetMainWindow() { return _mainWindow; }
#endif

	virtual void SelectView(View *view);

	/* Get current mode */
	virtual Mode GetMode();

	/* Set busy status, should be called in pairs. */
	virtual void SetBusy();
	virtual void SetIdle();

	/* Get current busy status */
	virtual Boolean IsBusy();

	virtual void StartSession();

	/* Execute script */
	virtual void ExecuteScript(char *script);

	/* Abort program */
	virtual void DoAbort(char *reason);

protected:
	virtual void SubclassInsertDisplay(DeviseDisplay *disp,
					   Coord x, Coord y,
					   Coord w,Coord h);
	virtual void SubclassRegisterView(View *view);
	virtual void SubclassUnregisterView(View *view);
	virtual void SubclassDoInit();
	virtual void SubclassDoViewPerspectiveChanged(View *view,
						      Coord x, Coord y,
						      Coord w, Coord h);
	
	virtual void DestroyClientData();

private:
	virtual void FilterAboutToChange(View *view) {}
	virtual void FilterChanged(View *view, VisualFilter &filter,
				   int flushed);
	virtual void ViewCreated(View *view);
	virtual void ViewDestroyed(View *view);

	/* process tcl command */
	static int ControlCmd(ClientData clientData, Tcl_Interp *interp,
		int argc, char *argv[]);

	/* from DispatcherCallback */
	char *DispatchedName() {
		return "TkControlPanel";
	}
	void Run();

	Tcl_Interp *_interp;
	Tk_Window _mainWindow;

	char *_argv0;
	int _busy; /* >0 if system is busy processing queries */
	static ControlPanel::Mode _mode;
	static MapInterpClassInfo *_interpProto; /* proto interpreted mapping */
};

#endif
