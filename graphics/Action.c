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
  Revision 1.4  1995/11/25 01:11:08  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.3  1995/09/05 21:12:18  jussi
  Added/updated CVS header.

  Revision 1.2  1995/09/05 20:39:17  jussi
  Added CVS header.
*/

#include "Action.h"
#include "Control.h"
#include "View.h"

void Action::AreaSelected(View *view, Coord xlow, Coord ylow,
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

void Action::KeySelected(View *view, char key, Coord x, Coord y)
{
  VisualFilter filter;

  if (key == 'c') {
#if 0
    view->DisplayConnectors(!v->DisplayConnectors());
#endif
  }

  else if (key == 's') {
#if 0
    view->DisplaySymbols(!view->DisplaySymbols());
#endif
  }

  else if (key == '>' || key == '.' || key == '6') {
    /* scroll data right */
    view->GetVisualFilter(filter);
    Coord width = filter.xHigh - filter.xLow;
    Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
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
    Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
    filter.xLow += halfWidth;
    filter.xHigh = filter.xLow + width;
    view->SetVisualFilter(filter);
  }

  else if (key == 'a' || key == 'A' || key == '7') {
    /* zoom in */
    view->GetVisualFilter(filter);
    Coord quarterWidth = (filter.xHigh-filter.xLow)/4.0;
    filter.xLow += quarterWidth;
    filter.xHigh -= quarterWidth;
    view->SetVisualFilter(filter);
  }

  else if (key == 'z' || key == 'z' || key == '9') {
    /* zoom out */
    view->GetVisualFilter(filter);
    Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
    filter.xLow -= halfWidth;
    filter.xHigh += halfWidth;
    Coord xMin;
    if (view->GetXMin(xMin) && filter.xLow < xMin)
      filter.xLow = xMin;
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

  else if (key == 's' || key == 'S' || key == '1') {
    /* zoom in y */
    view->GetVisualFilter(filter);
    Coord quarterHeight = (filter.yHigh-filter.yLow)/4.0;
    filter.yLow += quarterHeight;
    filter.yHigh -= quarterHeight;
    view->SetVisualFilter(filter);
  }

  else if (key == 'x' || key == 'X' || key == '3') {
    /* zoom out */
    view->GetVisualFilter(filter);
    Coord halfHeight = (filter.yHigh-filter.yLow)/2.0;
    filter.yLow -= halfHeight;
    filter.yHigh += halfHeight;
    view->SetVisualFilter(filter);
  }
}
