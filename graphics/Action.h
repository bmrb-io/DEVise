/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:39:18  jussi
  Added CVS header.
*/

#ifndef Action_h
#define Action_h
#include "DeviseTypes.h"

/* An action acts on events. Currently we support only
symol selected event*/

class View;

class Action {
public:
	Action(char *name) { _name = name; }
	char *GetName() { return _name; }

	virtual void AreaSelected(View *, Coord xlow, Coord ylow,
		Coord xhigh, Coord yhigh, int button);
	
	/* key pressed */
	virtual void KeySelected(View *, char c, Coord x, Coord y);

	virtual Boolean PopUp(View *, Coord x, Coord y,
		Coord /*xHigh */, Coord /*yHigh*/, 
		int /* button*/, char **& msgs, 
		int & numMsgs) {
		sprintf(buf1,"x:%.2f",x);
		sprintf(buf2,"y:%.2f",y);
		msgBuf[0] = buf1;
		msgBuf[1] = buf2;
		msgs = msgBuf;
		numMsgs = 2;
		return true;
	 }
private:
	char buf1[50];
	char buf2[50];
	char *msgBuf[2];
	char *_name;
};

class GenAction {
public:
	virtual Action *MakeAction(char *name) = 0;
};

#endif
