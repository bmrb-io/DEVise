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
  Revision 1.2  1995/12/06 05:45:08  ravim
  Initial version.
*/

#include <stdio.h>
#include "Transform.h"
#include "WindowRep.h"
#include "KGraph.h"

KGraph::KGraph(DeviseDisplay *dis)
{
  _dis = dis;

  // Create a new window
  _win = _dis->CreateWindowRep("KGraph", 0, 0, 0.40, 0.25,
			       BlackColor, WhiteColor);

  _naxes = 0;
  _pts = NULL;
}

KGraph::~KGraph()
{
  delete [] _pts;

}

void KGraph::Init()
{
  _naxes = 0;
  delete [] _pts;

  // Clear up any existing display on the windowrep - is this the right way??
  // We want to reuse the window for displaying a different graph.
  int x0, y0;
  unsigned int w,h;
  _win->Origin(x0, y0);
  _win->Dimensions(w, h);
  _win->SetFgColor(WhiteColor);
  _win->FillRect(x0, y0, w, h);

}

void KGraph::setAxes(int num)
{
  _naxes = num;
  _pts = new Coord[num];
  for (int i = 0; i < num; i++)
    _pts[i] = 0.0;
}

void KGraph::setPoints(Coord *pts, int num)
{
  for (int i = 0; i < num; i++)
    _pts[i] = pts[i];
}

void KGraph::Display()
{
  if (!_naxes) return;

  // Draw the circle
  DrawCircle();
  // Draw Axes
  DrawAxes();
  // Plot points
  PlotPoints();
  // Show values
  ShowVal();
}


void KGraph::DrawCircle()
{
  _win->SetFgColor(BlueColor);

  int x0, y0;
  unsigned int w,h;
  _win->Origin(x0, y0);
  _win->Dimensions(w, h);

  // Compute center of circle
  cy = y0 + h/2;
  cx = x0 + h/2;

  rad = h - 10;
  // Draw Circle
  _win->Arc(cx, -cy+9, rad, rad, 0, 2*PI);

}    

void KGraph::DrawAxes()
{
  _win->SetFgColor(RedColor);

  Coord x, y;
  int theta = 360/_naxes;

  // Draw axes
  for (int i = 0; i < _naxes; i++)
  {
    Rotate(rad/2, i*theta, x, y);
    _win->Line(cx, cy, x, y, 2);
  }

  // Generate origin label
  Rotate(rad/2, 0, x, y);
  _win->AbsoluteText("O ", x+4, y-6 , 50, 10, WindowRep::AlignWest);

}

void KGraph::PlotPoints()
{
  _win->SetFgColor(GreenColor);
  int theta = 360/_naxes;

  // Find max of all the points to be plotted
  Coord max = _pts[0];
  for (int i = 1; i < _naxes; i++)
    max = (_pts[i] > max)? _pts[i] : max;

  // Plot all points by drawing lines between them in order
  Coord x1, y1, x2, y2;
  Rotate(Scale(_pts[_naxes-1], max), (_naxes-1)*theta, x1, y1);

  for (i = 0; i < _naxes ; i++)
  {
    Rotate(Scale(_pts[i], max), i*theta, x2, y2);
    _win->Line(x1, y1, x2, y2, 2);
    x1 = x2;
    y1 = y2;
  }
}

void KGraph::ShowVal()
{
  int x0, y0;
  unsigned int w,h;
  _win->Origin(x0, y0);
  _win->Dimensions(w, h);
  _win->SetFgColor(BlueColor);

  // Show the plotted values by the side
  // xval to start at
  Coord startx = x0 + h + 10;
  Coord starty = y0 + 10;
  
  // First a generic message
  _win->AbsoluteText("Values clockwise from O", startx, starty, 500, 13,
		     WindowRep::AlignWest);

  starty += 17;
  // For each axes display value
  for (int i = 0; i < _naxes; i++)
  {
    char buf[15];
    sprintf(buf, "%4.2f", _pts[i]);
    _win->AbsoluteText(buf, startx, starty, 500, 13, WindowRep::AlignWest);
    starty += 17;
  }
}

Coord KGraph::Scale(Coord xval, Coord max)
{
  return (xval*rad)/(2*max);
}

void KGraph::Rotate(Coord xval, int degree, Coord &retx, Coord &rety)
{
  retx = cx + xval * cos(ToRadian(degree));
  rety = cy + xval * sin(ToRadian(degree));
}
