/*
  $Id$

  $Log$*/

#ifndef ActionDefault_h
#define ActionDefault_h
#include "Action.h"
#include "Util.h"

/* An action acts on events. Currently we support only
symol selected event*/

class ActionDefault: public Action {
public:
	ActionDefault(char *name, Coord leftEdge = 0.0, 
		Boolean useLeftFlag = false,
		Coord rightEdge = 0.0, Boolean useRightFlag = false);

	virtual void AreaSelected(View *, Coord /*xlow*/, Coord /*ylow*/,
		Coord /*xhigh*/, Coord /*yhigh*/, int /*button*/);

	/* key pressed */
	virtual void KeySelected(View *, char c, Coord x, Coord y);

	virtual Boolean PopUp(View *view, Coord x, Coord y, Coord xHigh,
			Coord yHigh,
			int /* button*/, char **& msgs, int & numMsgs);

private:
	Boolean PrintRecords(View *view, Coord x, Coord y, Coord xHigh, Coord yHigh,
		char *&errorMsg);

Coord left, right;
Boolean useLeft, useRight;
private:
char buf1[80];
char buf2[80];
char *msgBuf[2];
};
#endif
