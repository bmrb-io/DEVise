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
  Revision 1.4  1995/12/08 23:45:48  ravim
  Polygon filled. Window name and stat name displayed.

  Revision 1.3  1995/12/07 02:19:00  ravim
  Init() clears the display.

  Revision 1.2  1995/12/06 05:45:08  ravim
  Initial version.
*/

#include <stdio.h>
#include "Transform.h"
#include "WindowRep.h"
#include "KGraph.h"

#define MAX_POPUP_LEN 30

KGraph::KGraph(DeviseDisplay *dis)
{
  _dis = dis;

  // Create a new window
  _win = _dis->CreateWindowRep("Kiviat Graph", 0, 0, 0.25, 0.25,
			       BlackColor, WhiteColor);
  _win->RegisterCallback(this);

  _naxes = 0;
  _pts = NULL;
  _xyarr = NULL;
  _winame = NULL;
  msgBuf = NULL;
}

KGraph::~KGraph()
{
  delete [] _pts;
  delete [] _xyarr;
  if (msgBuf)
    for (int i = 0; i < _naxes+3; i++)
      delete msgBuf[i];
  delete [] msgBuf;
}

void KGraph::Init(char *winname, char *statname)
{
  _naxes = 0;
  delete [] _pts;
  delete [] _xyarr;
  if (msgBuf)
    for (int i = 0; i < _naxes+3; i++)
      delete msgBuf[i];
  delete [] msgBuf;

  // Clear up any existing display on the windowrep - is this the right way??
  // We want to reuse the window for displaying a different graph.
  int x0, y0;
  unsigned int w,h;
  _win->Origin(x0, y0);
  _win->Dimensions(w, h);
  _win->SetFgColor(WhiteColor);
  _win->FillRect(x0, y0, w, h);

  // Copy window name
  _winame = winname;
  _statname = statname;
}

void KGraph::setAxes(int num)
{
  _naxes = num;
  _pts = new Coord[num];
  _xyarr = new Point[num];
  for (int i = 0; i < num; i++)
    _pts[i] = 0.0;
  msgBuf = new (char *)[_naxes+3];
  for (i = 0; i < _naxes+3; i++)
    msgBuf[i] = new char[MAX_POPUP_LEN];
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
  // Plot points
  PlotPoints();
  // Show values - show only on button press now
//  ShowVal();
  // Draw Axes
  DrawAxes();
}


void KGraph::DrawCircle()
{
  _win->SetFgColor(BlueColor);

  int x0, y0;
  unsigned int w,h;
  _win->Origin(x0, y0);
  _win->Dimensions(w, h);

  h = (w > h)? h : w;
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
    _win->Line(cx, cy, x, y, 1);
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
    // store this point in a x-y array - need to fill polygon later
    _xyarr[i].x = x2;
    _xyarr[i].y = y2;

    _win->Line(x1, y1, x2, y2, 2);
    x1 = x2;
    y1 = y2;
  }

  // Finally have to fill the interior region with some color.
  _win->FillPoly(_xyarr, _naxes);
}

void KGraph::ShowVal()
{
  int x0, y0;
  unsigned int w,h;
  _win->Origin(x0, y0);
  _win->Dimensions(w, h);

  // Show the plotted values by the side
  // xval to start at
  Coord startx = x0 + h + 10;
  Coord starty = y0 + 10;
  
  // First a generic message
  
  _win->SetFgColor(RedColor);
  _win->AbsoluteText(_winame, startx, starty, 500, 13,
		     WindowRep::AlignWest);
  starty += 17;
  _win->AbsoluteText(_statname, startx, starty, 500, 13,
		     WindowRep::AlignWest);
  starty += 17;

  _win->SetFgColor(BlueColor);
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

void KGraph::HandlePress(WindowRep *w, int xlow, int ylow, int xhigh, 
			 int yhigh, int button)
{
  printf("HandlePress : button %d\n",button);
  

}

Boolean KGraph::HandlePopUp(WindowRep *w, int x, int y, int button, 
			  char **&msgs, int &numMsgs)
{
  // Message displayed consists of three generic lines followed by values
  // for each of the axes.
  msgs = msgBuf;
  sprintf(msgBuf[0], "%s", _winame);
  sprintf(msgBuf[1], "%s", _statname);
  sprintf(msgBuf[2], "Values clockwise from 0");
  
  for (int i = 0; i < _naxes; i++)
    sprintf(msgBuf[3+i], "%4.2f", _pts[i]);

  numMsgs = _naxes+3;
  return true;
}

void KGraph::HandleResize(WindowRep *w, int xlow, int ylow, unsigned width,
			  unsigned height)
{
  Display();

}
