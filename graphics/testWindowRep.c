/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "Dispatcher.h"
#include "Display.h"
#include "WindowRep.h"

class MyWinCallback: public WindowRepCallback{
public:
    /* draw in the exposed area */
	virtual void HandleExpose(WindowRep * w, int x, int y,
					unsigned width, unsigned height){
		printf("Expose 0x%x,%d,%d,%d,%d\n",w,x,y,x+width-1,y+height-1);
		w->ClearTransformStack();
		w->Line(0.0,0.0,10.0, 10.0, 1.0);
	};

	/* Handle button press event */
	virtual void HandlePress(WindowRep * w, int xlow,
		int ylow, unsigned xhigh, unsigned yhigh, int button){
		printf("Press 0x%x,%d,%d,%d,%d,%d\n",w,xlow, ylow, xhigh, yhigh,button);
	}
			

	/* Handle resize */
	virtual void HandleResize(WindowRep * w, int xlow,
				int ylow, unsigned width, unsigned  height){
		printf("Resize 0x%x,%d,%d,%d,%d\n",w,xlow,ylow,xlow+width-1,
			ylow+height-1);
	};

	virtual void HandleKey(WindowRep * w,char key, int x, int y){
		printf("key 0x%x,%c,%d,%d\n",w,key,x,y);
	};

	/* handle pop-up */
	virtual Boolean HandlePopUp(WindowRep *w, int x, int y, 
		int button, char **&msgs, int &numMsgs){
		printf("PopUp 0x%x,%d,%d\n",w,x,y);
		msgs = msgBuf;
		msgBuf[0] = "hello";
		msgBuf[1] = "universe";
		numMsgs = 2;
		return true;
	};
private:
char *msgBuf[2];
};

main(){
	WindowRepCallback *callBack = new MyWinCallback();
	WindowRep *win = DeviseDisplay::DefaultDisplay()->CreateWindowRep(
		"test",0.0, 0.0, .5, .5);
	win->RegisterCallback(callBack);
	WindowRep *subwin = DeviseDisplay::DefaultDisplay()->CreateWindowRep(
		"sub",0.0, 0.0, .5, .5, WhiteColor, BlackColor, win);
	subwin->RegisterCallback(callBack);
	Dispatcher::RunNoReturn();
}
