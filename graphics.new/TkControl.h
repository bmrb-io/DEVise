/* TkControl.h: Xaw implementation for control panel  */
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

	/* Return name of file current being worked on */
	virtual char *FileName() { return _fileName; }
	virtual char *FileAlias() { return _fileAlias; }


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


	virtual void StartSession();
	virtual char *SessionName() ;
	virtual void SetSessionName(char *name);



protected:
    virtual void SubclassInsertDisplay(DeviseDisplay *disp,Coord x, Coord y,
			Coord w,Coord h);
	virtual void SubclassRegisterView(View *view);
	virtual void SubclassUnregisterView(View *view);
	virtual void SubclassDoInit();
	virtual void SubclassDoViewPerspectiveChanged(View *view,
		Coord x, Coord y, Coord w, Coord h);
	
private:
	/* from ViewCallback */
	/* Called by View when its visual filter has changed.
	flushed == index of 1st element in the history that has been flushed.*/
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

	char *_fileName;
	char *_fileAlias;
	char *_winName;
	char *_gdataName;
	char *_viewName;
	char *_sessionName;
	char *_argv0;
	int _template; /* true if we are loading file as a template */
	int _restoring;
	int _busy; /* >0 if system is busy processing queries */
	static ControlPanel::Mode _mode;
	static MapInterpClassInfo *_interpProto; /* proto interpreted mapping */
};
#endif
