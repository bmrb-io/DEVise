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
  Revision 1.2  1995/12/06 05:44:38  ravim
  Initial version.
*/

#ifndef KGraph_h
#define KGraph_h

#include "Geom.h"
#include "Display.h"

class KGraph 
{
public:
  KGraph(DeviseDisplay *dis);
  ~KGraph();

  // Initialize the settings
  void Init(char *winname, char *statname);

  // Set the number of axes to display
  void setAxes(int num);

  // Specify points along the axes = number of axes in the graph
  void setPoints(Coord *pts, int num);

  // Display on the graph
  void Display();

private:
  DeviseDisplay *_dis;
  WindowRep *_win;
  char *_winame;
  char *_statname;
  int _naxes;
  Coord *_pts;
  Point *_xyarr;

  // Coords of center of circle and radius
  Coord cx, cy;
  int rad;

  void DrawCircle();
  void DrawAxes();
  void PlotPoints();
  void ShowVal();
  void Rotate(Coord xval, int degree, Coord &retx, Coord &rety);
  Coord Scale(Coord xval, Coord max);

};

#endif
