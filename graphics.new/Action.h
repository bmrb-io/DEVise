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
  Revision 1.1  1996/05/07 16:35:14  jussi
  Moved files from graphics directory.

  Revision 1.4  1995/11/25 01:12:28  jussi
  Added copyright notice and added #include <stdio.h> to get sprintf
  definition.

  Revision 1.3  1995/09/05 21:12:20  jussi
  Added/updated CVS header.

  Revision 1.2  1995/09/05 20:39:18  jussi
  Added CVS header.
*/

#ifndef Action_h
#define Action_h

#include <stdio.h>
#include "DeviseTypes.h"

/* An action acts on events. Currently we support only
   symbol selected event*/

class ViewGraph;

class Action {
public:
  Action(char *name) { 
      _name = name;
      _magnification = 2.0; 
  }

  char *GetName() { return _name; }

  virtual void AreaSelected(ViewGraph *view, Coord xlow, Coord ylow,
			    Coord xhigh, Coord yhigh, int button);
  
  virtual void KeySelected(ViewGraph *view, int c, Coord x, Coord y);
  
  virtual Boolean PopUp(ViewGraph *view, Coord x, Coord y,
			Coord xHigh, Coord yHigh,
			int button, char **&msgs,
			int &numMsgs) {
    sprintf(buf1, "x:%.2f", x);
    sprintf(buf2, "y:%.2f", y);
    msgBuf[0] = buf1;
    msgBuf[1] = buf2;
    msgs = msgBuf;
    numMsgs = 2;
    return true;
  }

  /* change the magnification level for zoom in/out */
  void SetMagnification(double mag) { _magnification = mag; }
  double  Magnification() { return _magnification; }


protected:

  char buf1[50];
  char buf2[50];
  char *msgBuf[2];
  char *_name;

  double _magnification;

};

class GenAction {
public:
  virtual Action *MakeAction(char *name) = 0;
};

#endif
