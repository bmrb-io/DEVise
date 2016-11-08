/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.4.28.1  2005/09/06 21:20:18  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.4  1997/11/24 23:14:52  weaver
  Changes for the new ColorManager.

  Revision 1.3.10.1  1997/05/21 20:40:27  weaver
  Changes for new ColorManager

  Revision 1.3  1995/12/29 22:42:46  jussi
  Added copyright message and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:58  jussi
  Added CVS header.
*/

#ifndef LineConnector_h
#define LineConnector_h

#include "ConnectorShape.h"
#include "Connector.h"
#include "WindowRep.h"

#include "Color.h"

/*************************************************************
A Discrete line connector that plots the following:
                 | (x2,y2)
                 |
(x1,y1)----------+
***************************************************************/

class DiscLineConnector: public ConnectorShape {
public:
  virtual ~DiscLineConnector(){}

  /* draw connection between two symbols */
  virtual void DrawConnection(WindowRep *win, Connector *con) {
    win->SetForeground(con->color);
    win->SetPattern(con->pattern);
    Coord width = 2.0;
    if (con->numShapeAttrs > 0)
      width = con->shapeAttrs[0];
    win->Line(con->x1, con->y1, con->x2, con->y1, width);
    win->Line(con->x2, con->y1, con->x2, con->y2, width);
  }
};

/******************************************************************
A discrete line connector that plots the following:
	   +------- (x2,y2)
           |
    (x1,y1)|
*******************************************************************/

class DiscLineConnector2: public ConnectorShape {
public:
  virtual ~DiscLineConnector2() {}

  /* draw connection between two symbols */
  virtual void DrawConnection(WindowRep *win, Connector *con) {
    win->SetForeground(con->color);
    win->SetPattern(con->pattern);
    Coord width = 2.0;
    if (con->numShapeAttrs > 0)
      width = con->shapeAttrs[0];
    win->Line(con->x1, con->y1, con->x1, con->y2, width);
    win->Line(con->x1, con->y2, con->x2, con->y2, width);
  }
};

/*****************************************************************
Continuous connector
******************************************************************/

class ContLineConnector : public ConnectorShape {
public:
  virtual ~ContLineConnector() {}

  virtual void DrawConnection(WindowRep *win,Connector *con) {
    win->SetForeground(con->color);
    win->SetPattern(con->pattern);
    Coord width = 1.0;
    if (con->numShapeAttrs > 0)
      width = con->shapeAttrs[0];
    win->Line(con->x1, con->y1, con->x2, con->y2, width);
  }
};

/*****************************************************************
bar connector
******************************************************************/

class BarConnector : public ConnectorShape {
public:
  virtual ~BarConnector() {}

  virtual void DrawConnection(WindowRep *win,Connector *con) {
    win->SetForeground(con->color);
    win->SetPattern(con->pattern);
    win->FillRect(con->x1, 0.0, con->x2 - con->x1, con->y1);
  }
};

//******************************************************************************
#endif
