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

  $Log$*/

#ifndef KGraph_h
#define KGraph_h

#include "Display.h"

class KGraph 
{
public:
  KGraph(DeviseDisplay *dis);
  ~KGraph();

  // Initialize the settings
  void Init();

  // Set the number of axes to display
  void setAxes(int num);

  // Specify points along the axes = number of axes in the graph
  void setPoints(Coord *pts, int num);

  // Display on the graph
  void Display();

private:
  DeviseDisplay *_dis;
  WindowRep *_win;
  int _naxes;
  Coord *_pts;

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
