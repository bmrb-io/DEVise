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
  Revision 1.5  1996/08/04 21:07:47  beyer
  changed key handling

  Revision 1.4  1996/03/29 18:14:08  wenger
  Got testWindowRep to compile and run, added drawing in
  windows; fixed a few more compile warnings, etc.

  Revision 1.3  1995/12/14 21:14:41  jussi
  Replaced 0x%x with 0x%p. Added copyright notice.

  Revision 1.2  1995/09/05 21:13:53  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include "Dispatcher.h"
#include "Display.h"
#include "WindowRep.h"
#include "Init.h"
#include "Control.h"

class MyWinCallback: public WindowRepCallback{
public:
    /* draw in the exposed area */
	virtual void HandleExpose(WindowRep * w, int x, int y,
					unsigned width, unsigned height){
		printf("Expose 0x%p,%d,%d,%d,%d\n",w,x,y,x+width-1,y+height-1);
		w->ClearTransformStack();
		w->Line(0.0,0.0,10.0, 10.0, 1.0);
	};

	/* Handle button press event */
	virtual void HandlePress(WindowRep * w, int xlow,
		int ylow, unsigned xhigh, unsigned yhigh, int button){
		printf("Press 0x%p,%d,%d,%d,%d,%d\n",w,xlow, ylow, xhigh, yhigh,button);
	}
			

	/* Handle resize */
	virtual void HandleResize(WindowRep * w, int xlow,
				int ylow, unsigned width, unsigned  height){
		printf("Resize 0x%p,%d,%d,%d,%d\n",w,xlow,ylow,xlow+width-1,
			ylow+height-1);
	};

	virtual void HandleKey(WindowRep * w, int key, int x, int y){
		printf("key 0x%p,0x%x,%d,%d\n",w,key,x,y);
	};

	/* handle pop-up */
	virtual Boolean HandlePopUp(WindowRep *w, int x, int y, 
		int button, char **&msgs, int &numMsgs){
		printf("PopUp 0x%p,%d,%d\n",w,x,y);
		msgs = msgBuf;
		msgBuf[0] = "hello";
		msgBuf[1] = "universe";
		numMsgs = 2;

		w->SetXorMode();
		w->FillRect(0.0, 0.0, (double) x, (double) y);
		w->SetCopyMode();

		return true;
	};
private:
char *msgBuf[2];
};

main(int argc, char **argv){
	Init::DoInit(argc,argv);
	ControlPanel::_controlPanel = GetNewControl();
	WindowRepCallback *callBack = new MyWinCallback();
	WindowRep *win = DeviseDisplay::DefaultDisplay()->CreateWindowRep(
		"test",0.0, 0.0, .5, .5);
	win->RegisterCallback(callBack);
	WindowRep *subwin = DeviseDisplay::DefaultDisplay()->CreateWindowRep(
		"sub",0.0, 0.0, .5, .5, WhiteColor, BlackColor, win);
	subwin->RegisterCallback(callBack);
	Dispatcher::RunNoReturn();
}

/* The following are dummies just to catch the references. */

int RTreeFile;

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize)
{
}

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize)
{
}

class ControlPanelDum : public ControlPanel {
public:
  virtual void SelectView(View *view) {}
  virtual void SetBusy() {}
  virtual void SetIdle() {}
  virtual Boolean IsBusy() { return false; }
  virtual int ReturnVal(u_short flag, char *result) { return 0; }
  virtual int ReturnVal(int argc, char **argv) { return 0; }
  virtual GroupDir *GetGroupDir() { return NULL; }
  virtual MapInterpClassInfo *GetInterpProto() { return NULL; }
  virtual int AddReplica(char *hostName, int port) { return 0; }
  virtual int RemoveReplica(char *hostName, int port) { return 0; }
  virtual void OpenDataChannel(int port) {}
  virtual int getFd() { return 0; }

protected:
  virtual void SubclassInsertDisplay(DeviseDisplay *disp,
                                     Coord x, Coord y,
                                     Coord w, Coord h) {}
  virtual void SubclassDoInit() {}
};

ControlPanel *
GetNewControl()
{
  return new ControlPanelDum;
}
