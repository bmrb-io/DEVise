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
  Revision 1.5  1996/05/31 20:59:07  jussi
  Added ClearGraph() method and renamed 'rad' to 'diam'.

  Revision 1.4  1995/12/16 00:48:20  ravim
  Handles window events for popup and resize.

  Revision 1.3  1995/12/08 23:44:53  ravim
  Polygon filled.

  Revision 1.2  1995/12/06 05:44:38  ravim
  Initial version.
*/

#ifndef KGraph_h
#define KGraph_h

#include "Geom.h"
#include "WindowRep.h"

class KGraph: protected WindowRepCallback
{
public:
  KGraph();
  virtual ~KGraph();

  // Initialize the settings
  void InitGraph(char *winname, char *statname);

  // Set the number of axes to display
  void SetAxes(int num);

  // Specify points along the axes = number of axes in the graph
  void SetPoints(Coord *pts, int num);

  // Display on the graph
  void DisplayGraph();

protected:
  WindowRep *_win;
  char *_winame;
  char *_statname;
  int _naxes;
  Coord *_pts;
  Point *_xyarr;

  // Coords of center of circle and diameter
  Coord _cx, _cy;
  int _diam;

  void ClearGraph();
  void DrawCircle();
  void DrawAxes();
  void PlotPoints();
  void ShowVal();
  void Rotate(Coord xval, int degree, Coord &retx, Coord &rety);
  Coord Scale(Coord xval, Coord max);

  // Handle window events
  virtual void HandlePress(WindowRep *w, int xlow, int ylow, int xhigh,
			   int yhigh, int button);
  virtual void HandleResize(WindowRep *w, int xlow, int ylow,
                            unsigned width, unsigned height);
  virtual Boolean HandlePopUp(WindowRep *w, int x, int y, int button, 
			      char **&msgs, int &numMsgs);

  char **_msgBuf;
};

#endif
