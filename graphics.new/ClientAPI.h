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


/* Xaw implementation for control panel */

#ifndef PENTIUM 
	#include"missing.h"
#endif

#ifndef TkControl_h
#define TkControl_h
#include <tcl.h>
#include <tk.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
//#include "Dispatcher.h"
//#include "Control.h"
//#include "ViewCallback.h"


#define DOASSERT(c,r)    { if (!(c)) DoAbort(r);}
//class View;
//class MapInterpClassInfo;
class ClientControl
//: public ControlPanel, public DispatcherCallback, private ViewCallback

{
public:
	ClientControl();

	/* Return name of file current being worked on */
	//virtual char *FileName() { return _fileName; }
	//virtual char *FileAlias() { return _fileAlias; }
	virtual ~ClientControl() { 
		exit(0);
	}
	void Run();
	void CheckRead();
	void StartSession();

	static ClientControl * _currentClient=0  ;
#ifdef TK_WINDOW
	virtual Tcl_Interp *GetInterp() { return _interp; }
	virtual Tk_Window GetMainWindow() { return _mainWindow; }
#endif
	virtual void DoAbort(char *reason);
#if 0
	 virtual void SelectView(View *view);

	/* Return name of window last accessed */
	virtual char *WindowName(){ return _winName; }

	/* Return name of gData last accessed */
	virtual char *GDataName() { return _gdataName; }
					
	/* Return name of view last accessed */
	virtual char *ViewName() { return _viewName; }

	/* Get current mode */
	virtual Mode GetMode();

	/* Return TRUE if restoring a session */
	virtual Boolean Restoring();

	/* Set busy status, should be called in pairs. */
	virtual void SetBusy();
	virtual void SetIdle();

	/* Get current busy status */
	virtual Boolean IsBusy();

	virtual char *SessionName() ;
	virtual void SetSessionName(char *name);

	/* Execute script */
	virtual void ExecuteScript(char *script);

	/* Abort program */

protected:
    /* virtual void SubclassInsertDisplay(DeviseDisplay *disp,Coord x, Coord y,
			Coord w,Coord h);
	virtual void SubclassRegisterView(View *view);
	virtual void SubclassUnregisterView(View *view);
	virtual void SubclassDoInit();
	virtual void SubclassDoViewPerspectiveChanged(View *view,
		Coord x, Coord y, Coord w, Coord h);*/
private:
	/* virtual void FilterAboutToChange(View *view) {}
	virtual void FilterChanged(View *view, VisualFilter &filter,
				   int flushed);
	virtual void ViewCreated(View *view);
	virtual void ViewDestroyed(View *view);*/
#endif
/*------------------------------*/
private:
	/* process tcl command */
	int ControlComd(ClientData clientData, Tcl_Interp *interp,
		int argc, char *argv[]);
	static int ControlCmd(ClientData clientData, Tcl_Interp *interp,
		int argc, char *argv[]);
	int receive(Tcl_Interp *, char *);
	int sendServer(char **,int);
	int sendReceive(Tcl_Interp *,char **,int ,char * );
	int sendReturnVals(Tcl_Interp *,char **,int ,char * );


	int MakeReturnVals(struct Tcl_Interp *,char *);

	/* from DispatcherCallback */
	/* char *DispatchedName() {
		return "ClientControl";
	}*/

	// Opens the connection to the server...
	void openConnection(char*,int);

	Tcl_Interp *_interp;
	Tk_Window _mainWindow;


	char *_fileName;
	char *_fileAlias;
	char *_winName;
	char *_gdataName;
	char *_viewName;
	char *_sessionName;
	char *_argv0;
	int _template; /* true if we are loading file as a template */
	int _restoring;
	//int _busy; /* >0 if system is busy processing queries */
	//static ControlPanel::Mode _mode;
	//static MapInterpClassInfo *_interpProto; /* proto interpreted mapping */

	// Socket for connection to the server..
	int socketFd;
	int controlFd;

	void Exit(int num);
	
};
#endif


