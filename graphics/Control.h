/*
  $Id$

  $Log$
  Revision 1.4  1996/04/16 19:45:11  jussi
  Added DoAbort() method.

  Revision 1.3  1996/01/27 00:21:45  jussi
  Added ExecuteScript() method.

  Revision 1.2  1995/09/05 21:12:32  jussi
  Added/update CVS header.
*/

#ifndef Control_h
#define Control_h
#include "DeviseTypes.h"
#include "VisualArg.h"
#include "DList.h"

class Dispatcher;

/* list of views for the control panel, arranged according
to dispatcher */
struct CPViewList;
class View;

DefinePtrDList(CPViewListList,CPViewList *);

class ClassInfo;
class ClassDir;

class ControlPanelCallback;


DefinePtrDList(ControlPanelCallbackList ,ControlPanelCallback *);

class DeviseDisplay;
class ControlPanel  {
public:
	enum Mode { DisplayMode , LayoutMode};

	void InsertCallback(ControlPanelCallback *callback);
	void DeleteCallback(ControlPanelCallback *callback);

	/* Register class with control panel.
	transient == true if it's a transient class to be removed
	when closing a session.*/
	static void RegisterClass(ClassInfo *cInfo, Boolean transient = false);

	/* Make view the current view */
	virtual void SelectView(View *view)= 0;

	/* Find pointer to instance with given name */
	static void *FindInstance(char *name);

	/* Return name of file */
	virtual char *FileName() = 0;
	virtual char *FileAlias() = 0;


	/* Return name of window last accessed */
	virtual char *WindowName()= 0;

	/* Return name of gData last accessed */
	virtual char *GDataName()= 0;

	/* Return name of view last accessed */
	virtual char *ViewName() = 0;

	/*Return TRUE if currently restoring a session*/
	virtual Boolean Restoring()=0;

	/* Get current mode */
	virtual Mode GetMode()=0;

	/* Set busy status, should be called in pairs. */
	virtual void SetBusy()=0;
	virtual void SetIdle() = 0;

	/* Get current busy status */
	virtual Boolean IsBusy() = 0;

	/* Start current session */
	virtual void StartSession() = 0;
	virtual char *SessionName() =0;
	virtual void SetSessionName(char *name) = 0;

	/* Execute script */
	virtual void ExecuteScript(char *script) = 0;

	/* Instantiate control panel into display */
	static void InsertDisplay(DeviseDisplay *disp,Coord x=0.0, Coord y=0.4, 
		Coord w=0.15, Coord h=0.59){
		Instance()->SubclassInsertDisplay(disp,x,y,w,h);
	};

	/* Init control panel, before dispatcher starts running.
	Create control panel if not already created.
	Update contronl panel state to reflect current dispatcher */
	static void Init(){
		Instance()->SubclassDoInit();
	};

	/* return the one and only instance of control panel */
	static ControlPanel *Instance();

	/* report mode change */
	void ReportModeChange(Mode mode);

	/* quit */
	virtual void DoQuit();

	/* abort */
	virtual void DoAbort(char *reason);

protected:

	/* Get ClassDir info */
	static ClassDir *GetClassDir() ;

	friend class Dispatcher;

	/* dervied classes must implement these: */
	virtual void SubclassInsertDisplay(DeviseDisplay *disp,Coord x, Coord y, 
		Coord w,Coord h)= 0;
	virtual void SubclassDoInit()=0;


	/* helper functions for derived classes */



	/* return */
	void DoReturn();

	/* Change context and reset current view */
	void DoContext();

	/* do go/stop */
	void DoGo(Boolean state);

	/* do single step */
	void DoStep();

	/* make current view display/not display symbols according to state */
	void DoDisplaySymbols(Boolean state);

	void DoDisplayConnectors(Boolean stat);

	/* display/not display axes for current view. */
	void DoDisplayCurrentAxes(Boolean stat);

	/* display/not display axes for all views in the dispatcher.*/
	void DoDisplayAxes(Boolean stat);

	/* change one of the visual filters. 
	on == TRUE means turn on the filter, otherwise, turn off the filter.
	flag is one of: VISUAL_COLOR or VISUAL_PATTERN */
	void ChangeIntVisualFilter(Boolean on, VisualFlag flag,int minVal,
		int maxVal);

	/* change one of the visual filters. 
	on == TRUE means turn on the filter, otherwise, turn off the filter.
	flag is one of: VISUAL_SIZE or VISUAL_ORIENTATION.
	Note: input orientations are in degrees.
	*/
	void ChangeFloatVisualFilter(Boolean on, VisualFlag flag,double minVal,
		double  maxVal);

	/* scrol current view by the amount proportional to its
	current width. 0<= abs(amount) <= 1, amount >0 means scroll right,
	otherwise, scroll left */
	void DoScrollX(double amount);
	void DoScrollY(double amount);

	/* zoom  current width by the given amount.
	0 <= amount */
	void DoZoomXY(double amount);
	void DoZoomX(double amount);
	void DoZoomY(double amount);

	/* do initialize before dispatche starts running. This is used
	to get control panel's notion of current dispatcher in synch
	with the dispatchers' notion of current dispatcher.
	Return control panel switched internally to a new current dispatcher */
	Boolean DoInit();

	ControlPanel();
	static ControlPanel *_controlPanel; /* one and only control panel */

private:
	/* update current view to reflect current dispatcher */
	void UpdateNewDispatcher();



	static ClassDir *_classDir;

	ControlPanelCallbackList *_callbacks;
};

class ControlPanelCallback {
public:
	virtual void ModeChange(ControlPanel::Mode mode)=0;
};

#endif
