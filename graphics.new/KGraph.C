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
  Revision 1.12  1996/07/14 04:04:42  jussi
  ViewKGraph is now derived from KGraph instead of declaring
  KGraph as its member variable. This allows ViewKGraph to
  intercept WindowDestroy events and destroy the graph.

  Revision 1.11  1996/07/13 17:28:41  jussi
  Added window flush call.

  Revision 1.10  1996/05/31 20:58:50  jussi
  Fixed problem with calculation of center and size of circle.

  Revision 1.9  1996/02/02 21:52:20  jussi
  Removed SetFgColor() right after SetXorMode() which changed the
  effect of the xor function.

  Revision 1.8  1996/01/30 21:12:56  jussi
  Minor changes in visual appearance.

  Revision 1.7  1996/01/29 23:56:42  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.6  1995/12/28 20:07:34  jussi
  Small fixes to remove compiler warnings.

  Revision 1.5  1995/12/16 00:47:58  ravim
  Handle window events for popup and resize.

  Revision 1.4  1995/12/08 23:45:48  ravim
  Polygon filled. Window name and stat name displayed.

  Revision 1.3  1995/12/07 02:19:00  ravim
  Init() clears the display.

  Revision 1.2  1995/12/06 05:45:08  ravim
  Initial version.
*/

#include <stdio.h>

#include "KGraph.h"
#include "Display.h"

//#define DEBUG

#define MAX_POPUP_LEN 30

KGraph::KGraph()
{
  DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();  

  // Create a new window
  _win = disp->CreateWindowRep("Kiviat Graph", 0, 0, 0.25, 0.25);
  _win->RegisterCallback(this);

  _naxes = 0;
  _pts = NULL;
  _xyarr = NULL;
  _winame = NULL;
  _msgBuf = NULL;
}

KGraph::~KGraph()
{
  /* We don't need to destroy the window because it's already
     being destroyed -- that's why this destructor is being
     called in the first place. We also don't need to remove
     the callback from the window's callback list; the
     window does that for us. */

  delete _pts;
  delete _xyarr;
  if (_msgBuf) {
    for(int i = 0; i < _naxes+3; i++)
      delete _msgBuf[i];
  }
  delete _msgBuf;
}

void KGraph::InitGraph(char *winname, char *statname)
{
  _naxes = 0;

  delete _pts;
  delete _xyarr;
  if (_msgBuf) {
    for(int i = 0; i < _naxes + 3; i++)
      delete _msgBuf[i];
  }
  delete _msgBuf;

  // Copy window name
  _winame = winname;
  _statname = statname;
}

void KGraph::SetAxes(int num)
{
  _naxes = num;
  _pts = new Coord[num];
  _xyarr = new Point[num];
  int i;
  for(i = 0; i < num; i++)
    _pts[i] = 0.0;
  _msgBuf = new (char *)[_naxes + 3];
  for(i = 0; i < _naxes + 3; i++)
    _msgBuf[i] = new char[MAX_POPUP_LEN];
}

void KGraph::SetPoints(Coord *pts, int num)
{
  for(int i = 0; i < num; i++)
    _pts[i] = pts[i];
}

void KGraph::DisplayGraph()
{
  if (!_naxes)
    return;

  // Clear graph
  ClearGraph();
  // Draw the circle
  DrawCircle();
  // Plot points
  PlotPoints();
  // Draw Axes
  DrawAxes();
  // Flush window
  _win->Flush();
}

void KGraph::ClearGraph()
{
  unsigned int w, h;
  _win->Dimensions(w, h);
  _win->SetFgColor(BackgroundColor);
  _win->FillRect(0, 0, w, h);
}

void KGraph::DrawCircle()
{
  unsigned int w, h;
  _win->Dimensions(w, h);

  // Compute size and center of circle
  h = (w > h) ? h : w;
  _cy = h / 2;
  _cx = h / 2;
  _diam = h - 10;

  // Draw Circle
  _win->SetFgColor(BlueColor);
  _win->Arc(_cx, _cy, _diam, _diam, 0, 2 * PI);
}    

void KGraph::DrawAxes()
{
  _win->SetXorMode();

  Coord x, y;
  int theta = 360 / _naxes;

  // Draw axes
  for(int i = 0; i < _naxes; i++) {
    Rotate(_diam / 2, i * theta, x, y);
    _win->Line(_cx, _cy, x, y, 1);
  }

  _win->SetCopyMode();
}

void KGraph::PlotPoints()
{
  _win->SetFgColor(GreenColor);
  int theta = 360 / _naxes;

  // Find max of all the points to be plotted
  Coord max = _pts[0];
  int i;
  for(i = 1; i < _naxes; i++)
    max = (_pts[i] > max) ? _pts[i] : max;

  // Plot all points by drawing lines between them in order
  Coord x1, y1, x2, y2;
  Rotate(Scale(_pts[_naxes - 1], max), (_naxes - 1) * theta, x1, y1);

  for(i = 0; i < _naxes ; i++) {
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
  unsigned int w,h;
  _win->Dimensions(w, h);

  // Show the plotted values by the side
  // xval to start at
  Coord startx = h + 10;
  Coord starty = 10;
  
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
  for(int i = 0; i < _naxes; i++) {
    char buf[15];
    sprintf(buf, "%g", _pts[i]);
    _win->AbsoluteText(buf, startx, starty, 500, 13, WindowRep::AlignWest);
    starty += 17;
  }
}

Coord KGraph::Scale(Coord xval, Coord max)
{
  return (xval * _diam) / (2 * max);
}

void KGraph::Rotate(Coord xval, int degree, Coord &retx, Coord &rety)
{
  retx = _cx + xval * cos(ToRadian(degree));
  rety = _cy + xval * sin(ToRadian(degree));
}

void KGraph::HandlePress(WindowRep *w, int xlow, int ylow, int xhigh, 
			 int yhigh, int button)
{
#ifdef DEBUG
  printf("HandlePress: button %d\n",button);
#endif
}

void KGraph::HandleResize(WindowRep *w, int xlow, int ylow,
                          unsigned width, unsigned height)
{
  DisplayGraph();
}

Boolean KGraph::HandlePopUp(WindowRep *w, int x, int y, int button, 
			  char **&msgs, int &numMsgs)
{
  // Message displayed consists of three generic lines followed by values
  // for each of the axes.

  msgs = _msgBuf;
  sprintf(_msgBuf[0], "%s", _winame);
  sprintf(_msgBuf[1], "%s", _statname);
  sprintf(_msgBuf[2], "Values clockwise from 0");
  
  for(int i = 0; i < _naxes; i++)
    sprintf(_msgBuf[3+i], "%g", _pts[i]);

  numMsgs = _naxes + 3;
  return true;
}
