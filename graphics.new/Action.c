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
  Revision 1.1  1996/05/07 16:35:13  jussi
  Moved files from graphics directory.

  Revision 1.7  1996/01/23 20:47:19  jussi
  If both connectors and symbols are turned off, then one
  of them is turned back on. This will prevent a blank
  view.

  Revision 1.6  1995/12/29 22:43:41  jussi
  Added support for line connectors. Type 'c' to toggle
  connectors on or off, and type 's' to toggle symbols.

  Revision 1.5  1995/12/28 20:48:27  jussi
  Cleaned up the code a bit.

  Revision 1.4  1995/11/25 01:11:08  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.3  1995/09/05 21:12:18  jussi
  Added/updated CVS header.

  Revision 1.2  1995/09/05 20:39:17  jussi
  Added CVS header.
*/

#include "Action.h"
#include "Control.h"
#include "ViewGraph.h"

void Action::AreaSelected(ViewGraph *view, Coord xlow, Coord ylow,
			  Coord xhigh, Coord yhigh, int button)
{
  if (xlow == xhigh || ylow == yhigh) {
    ControlPanel::Instance()->SelectView(view);
    return;
  }

  VisualFilter filter;
  view->GetVisualFilter(filter);
  if (button == 1) {
    filter.xLow = xlow;
    filter.xHigh = xhigh;
    view->SetVisualFilter(filter);
  } else if (button == 3) {
    filter.xLow = xlow;
    filter.xHigh = xhigh;
    filter.yLow = ylow;
    filter.yHigh = yhigh;
    view->SetVisualFilter(filter);
  }
}

void Action::KeySelected(ViewGraph *view, char key, Coord x, Coord y)
{
  VisualFilter filter;
  Boolean symbolsOn = view->DisplaySymbols();
  Boolean connectorsOn = view->DisplayConnectors();

  Boolean zoomInX  = (key == '7');
  Boolean zoomOutX = (key == '9');
  Boolean zoomInY  = (key == '1');
  Boolean zoomOutY = (key == '3');

  if (view->IsScatterPlot()) {
    if (zoomInX || zoomInY)
      zoomInX = zoomInY = true;
    if (zoomOutX || zoomOutY)
      zoomOutX = zoomOutY = true;
  }

  if (key == 'c') {
    (void)view->DisplayConnectors(!connectorsOn);
    if (connectorsOn && !symbolsOn)
      (void)view->DisplaySymbols(true);
  }

  else if (key == 's') {
    (void)view->DisplaySymbols(!symbolsOn);
    if (!connectorsOn && symbolsOn)
      (void)view->DisplayConnectors(true);
  }

  else if (key == '>' || key == '.' || key == '6') {
    /* scroll data right */
    view->GetVisualFilter(filter);
    Coord width = filter.xHigh - filter.xLow;
    Coord halfWidth = (filter.xHigh - filter.xLow) / 2.0;
    filter.xLow -= halfWidth;
    Coord xMin;
    if (view->GetXMin(xMin) && filter.xLow < xMin)
      filter.xLow = xMin;
    filter.xHigh = filter.xLow + width;
    view->SetVisualFilter(filter);
  }

  else if (key == '<' || key == ',' || key == '4') {
    /* scroll data left */
    view->GetVisualFilter(filter);
    Coord width = filter.xHigh - filter.xLow;
    Coord halfWidth = (filter.xHigh - filter.xLow) / 2.0;
    filter.xLow += halfWidth;
    filter.xHigh = filter.xLow + width;
    view->SetVisualFilter(filter);
  }

  else if (key == '8') {
    /* scroll data up */
    view->GetVisualFilter(filter);
    Coord height = filter.yHigh - filter.yLow;
    Coord halfHeight = height / 2.0;
    filter.yLow -= halfHeight;
    filter.yHigh = filter.yLow + height;
    view->SetVisualFilter(filter);
  }

  else if (key == '2') {
    /* scroll data down */
    view->GetVisualFilter(filter);
    Coord height = filter.yHigh - filter.yLow;
    Coord halfHeight = height / 2.0;
    filter.yLow += halfHeight;
    filter.yHigh = filter.yLow + height;
    view->SetVisualFilter(filter);
  }

  if (zoomInX) {
    /* zoom in X */
    view->GetVisualFilter(filter);
    Coord quarterWidth = (filter.xHigh - filter.xLow) / 4.0;
    filter.xLow += quarterWidth;
    filter.xHigh -= quarterWidth;
    view->SetVisualFilter(filter);
  }

  if (zoomOutX) {
    /* zoom out X */
    view->GetVisualFilter(filter);
    Coord halfWidth = (filter.xHigh-filter.xLow) / 2.0;
    filter.xLow -= halfWidth;
    filter.xHigh += halfWidth;
    Coord xMin;
    if (view->GetXMin(xMin) && filter.xLow < xMin)
      filter.xLow = xMin;
    view->SetVisualFilter(filter);
  }

  if (zoomInY) {
    /* zoom in Y */
    view->GetVisualFilter(filter);
    Coord quarterHeight = (filter.yHigh - filter.yLow) / 4.0;
    filter.yLow += quarterHeight;
    filter.yHigh -= quarterHeight;
    view->SetVisualFilter(filter);
  }

  if (zoomOutY) {
    /* zoom out Y */
    view->GetVisualFilter(filter);
    Coord halfHeight = (filter.yHigh - filter.yLow) / 2.0;
    filter.yLow -= halfHeight;
    filter.yHigh += halfHeight;
    view->SetVisualFilter(filter);
  }
}
