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
  Revision 1.2  1995/09/05 22:14:02  jussi
  Added CVS header.
*/

#ifndef ActionDefault_h
#define ActionDefault_h

#include "Action.h"
#include "Util.h"

/* An action acts on events. Currently we support only
   symbol selected event */

class ActionDefault: public Action {
public:
  ActionDefault(char *name, Coord leftEdge = 0.0, 
		Boolean useLeftFlag = false,
		Coord rightEdge = 0.0, Boolean useRightFlag = false);

  void KeySelected(View *, char c, Coord x, Coord y);

  Boolean PopUp(View *view, Coord x, Coord y, Coord xHigh,
		Coord yHigh, int button, char **& msgs, int & numMsgs);

private:
  Boolean PrintRecords(View *view, Coord x, Coord y, Coord xHigh, Coord yHigh,
		       char *&errorMsg);

  Coord left, right;
  Boolean useLeft, useRight;

  char buf1[80];
  char buf2[80];
  char *msgBuf[2];
};
#endif
