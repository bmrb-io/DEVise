/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.6  1998/08/11 13:43:23  wenger
  Server responds to KeyAction commands from JavaScreen (still needs event
  coordinates); did some cleanup of the ActionDefault class.

  Revision 1.5  1996/08/04 21:12:57  beyer
  Added support for devise keys

  Revision 1.4  1996/05/07 16:36:33  jussi
  Changed type of view parameter from View * to ViewGraph *.
  Added handling of keypad number 5 (autoscale view).

  Revision 1.3  1995/12/28 20:47:21  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:02  jussi
  Added CVS header.
*/

#ifndef ActionDefault_h
#define ActionDefault_h

#include "Action.h"
#include "Util.h"

class QueryProc;
class RecInterp;
class TData;

/* An action acts on events. Currently we support only
   symbol selected event */

class ActionDefault: public Action {
public:
  ActionDefault(char *name, Coord leftEdge = 0.0, 
		Boolean useLeftFlag = false,
		Coord rightEdge = 0.0, Boolean useRightFlag = false);

  void KeySelected(ViewGraph *view, int c, Coord x, Coord y);

  Boolean PopUp(ViewGraph *view, Coord x, Coord y, Coord xHigh,
		Coord yHigh, int button, char **& msgs, int & numMsgs);

private:
  Boolean PrintRecords(ViewGraph *view, Coord x, Coord y, Coord xHigh,
		       Coord yHigh, char *&errorMsg);

  // Returns true iff too much data to display.
  Boolean GetRecords(QueryProc *qp, RecInterp *recInterp, TData *tdata,
      char *&errorMsg, int &numRecs);
};

#endif
