/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DevAxis class.
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>

#include "DevAxis.h"
#include "WindowRep.h"
#include "View.h"
#include "DevError.h"
#include "Util.h"

//#define DEBUG

//-----------------------------------------------------------------------------
// Constructor.
DevAxis::DevAxis(View *view, AxisType type, Boolean inUse,
    int withTicksWidth, int noTicksWidth, int numTicks, int significantDigits,
    int labelWidth)
{
#if defined(DEBUG)
  printf("DevAxis::DevAxis(%s, %d, %d, %d, %d, %d, %d, %d)\n",
      view->GetName(), type, inUse, withTicksWidth, noTicksWidth, numTicks,
          significantDigits, labelWidth);
#endif

  _view = view;
  _type = type;
  _inUse = inUse;
  _withTicksWidth = withTicksWidth;
  _noTicksWidth = noTicksWidth;
  _width = _withTicksWidth; // default is ticks enabled
  _numTicks = numTicks;
  _significantDigits = significantDigits;
  _labelWidth = labelWidth;

  _attrType = FloatAttr;
  _dateFormat = NULL;
}

//-----------------------------------------------------------------------------
// Destructor.
DevAxis::~DevAxis()
{
#if defined(DEBUG)
  printf("DevAxis::~DevAxis()\n");
#endif
//TEMP -- use ObjectValid stuff here?
}

//-----------------------------------------------------------------------------
// Get the axis label font characteristics.
void
DevAxis::GetFont(int &family, float &pointSize, Boolean &bold,
    Boolean &italic)
{
#if defined(DEBUG)
  printf("DevAxis::GetFont()\n");
#endif

  _font.Get(family, pointSize, bold, italic);
}

//-----------------------------------------------------------------------------
// Set the axis label font characteristics.
void
DevAxis::SetFont(int family, float pointSize, Boolean bold, Boolean italic)
{
#if defined(DEBUG)
  printf("DevAxis::SetFont()\n");
#endif

  _font.Set(family, pointSize, bold, italic);
}

//-----------------------------------------------------------------------------
// Set the date format.
void
DevAxis::SetDateFormat(const char *format)
{
  if (format == NULL) format = "";

#if defined(DEBUG)
  printf("DevAxis::SetDateFormat(%s)\n", format);
#endif

  if (_dateFormat == NULL || strcmp(format, _dateFormat)) {
    delete _dateFormat;
    _dateFormat = CopyString(format);
  }
}

//-----------------------------------------------------------------------------
// Draw this axis.
void
DevAxis::DrawAxis(WindowRep *win, int x, int y, int w, int h)
{
#if defined(DEBUG)
  printf("DevAxis::DrawAxis(%d %d %d %d)\n", x, y, w, h);
#endif

  // Make sure the WindowRep has an identity transform matrix, even though
  // this has *probably* already been done.
  win->PushTop();
  win->MakeIdentity();

  switch (_type) {
  case AxisX:
    DrawXAxis(win, x, y, w, h);
    break;

  case AxisY:
    DrawYAxis(win, x, y, w, h);
    break;

  case AxisZ:
    reportErrNosys("Z axis drawing not implemented");
    break;

  default:
    reportErrNosys("Illegal axis type");
    break;
  }

  win->PopTransform();
}

//-----------------------------------------------------------------------------
// Draw X axis.
//TEMP -- combine with Y
void
DevAxis::DrawXAxis(WindowRep *win, int x, int y, int w, int h)
{
#if defined(DEBUG)
  printf("DevAxis::DrawXAxis(%d %d %d %d)\n", x, y, w, h);
#endif
  
  DOASSERT(_view->GetNumDimensions() == 2, "Invalid number of dimensions");

  const int tickLength = 3;

  int axisX, axisY, axisWidth, axisHeight, startX;
  _view->GetXAxisArea(axisX, axisY, axisWidth, axisHeight, startX);
  startX--; // Kludge -- RKW 1999-04-20.
  int axisMaxX = axisX + axisWidth - 1;
  int axisMaxY = axisY + axisHeight - 1;

  {
    Coord maxX = x + w - 1.0;
    Coord maxY = y + h - 1.0;
    if (!Geom::RectRectIntersect(x, y, maxX, maxY, axisX, axisY, axisMaxX,
        axisMaxY)) {
//TEMP -- does this ever happen?
#if defined(DEBUG)
      printf("do not intersect\n");
#endif
      return;
    }
  }

  VisualFilter filter;
  _view->GetVisualFilter(filter);
  
  win->SetForeground(_view->GetForeground());
  win->SetPattern(Pattern0);
  win->SetNormalFont();
  _font.SetWinFont(win);

  /* draw a line from startX to the end of the view */
  win->Line(startX - 1, axisY, axisMaxX, axisY, 1);

  // Don't draw ticks and labels if space is too small.
  if (_width <= tickLength || !TicksEnabled()) return;

  Coord drawWidth = axisWidth - (startX - axisX);
  int numTicks = (int)(drawWidth / _labelWidth);

  /* if axis is date, draw values and return */
  if (_attrType == DateAttr) {

    // Draw the labels (date values).
    const char *label = DateString((time_t)filter.xLow, _dateFormat);
    win->AbsoluteText(label, startX, axisY - tickLength - (axisHeight-1),
        drawWidth / 2 - 1, axisHeight - 1, WindowRep::AlignWest, true);
    label = DateString((time_t)filter.xHigh, _dateFormat);
    win->AbsoluteText(label, startX + drawWidth / 2,
        axisY - tickLength - (axisHeight-1), drawWidth / 2 - 1, axisHeight - 1,
	WindowRep::AlignEast, true);

    // Draw ticks.
    win->Line(startX - 1, axisY, startX - 1, axisY - tickLength, 1);
    win->Line(axisMaxX, axisY, axisMaxX, axisY - tickLength, 1);

    return;
  }

  /*
     since neither custom nor date labels are used, we can draw
     tick marks and label each tick with the corresponding value
  */

  Coord tickMark;
  int nTicks;
  Coord tickInc;
  OptimizeTickMarks(filter.xLow, filter.xHigh, _numTicks,
		    tickMark, nTicks, tickInc);

  /* draw tick marks */
  for(int i = 0; i < nTicks; i++) {
    Coord xPixel = (tickMark - filter.xLow) * (axisMaxX - startX) /
        (filter.xHigh - filter.xLow) + startX;

    // Draw the tick mark itself.
    win->Line(xPixel, axisY, xPixel, axisY - tickLength, 1);

    char buf[32];
    sprintf(buf, "%.*g", _significantDigits, tickMark);

    Coord labelX = xPixel - _labelWidth / 2;

    /* make sure label doesn't go past left or right edge */
    Boolean pastLeft, pastRight;
    if (labelX < startX) {
      pastLeft = true;
    } else {
      pastLeft = false;
    }

    if (labelX + _labelWidth > axisX + axisWidth) {
      pastRight = true;
    } else {
      pastRight = false;
    }

    if (!pastLeft && !pastRight) {
      win->AbsoluteText(buf, labelX, axisY - tickLength - (axisHeight-1),
			_labelWidth,
			axisHeight - 1, WindowRep::AlignCenter, true);
#if defined(DEBUG)
        printf("  Label <%s> at %g, %d\n", buf, labelX, axisY);
#endif
    } else if (pastLeft && !pastRight) {
      labelX = startX;
      win->AbsoluteText(buf, labelX, axisY - tickLength - (axisHeight-1), 
			_labelWidth,
			axisHeight - 1, WindowRep::AlignWest, true);
#if defined(DEBUG)
        printf("  Label <%s> at %g, %d\n", buf, labelX, axisY);
#endif
    } else if (!pastLeft && pastRight) {
      labelX = axisX + axisWidth - _labelWidth;
      win->AbsoluteText(buf, labelX, axisY - tickLength - (axisHeight-1),
			_labelWidth,
			axisHeight - 1, WindowRep::AlignEast, true);
#if defined(DEBUG)
        printf("  Label <%s> at %g, %d\n", buf, labelX, axisY);
#endif
    } else {
      // Don't draw the label.
    }

    tickMark += tickInc;
  }
}

//-----------------------------------------------------------------------------
// Draw Y axis.
//TEMP -- combine with X
void
DevAxis::DrawYAxis(WindowRep *win, int x, int y, int w, int h)
{
#if defined(DEBUG)
  printf("DevAxis::DrawYAxis(%d %d %d %d)\n", x, y, w, h);
#endif
  
  DOASSERT(_view->GetNumDimensions() == 2, "Invalid number of dimensions");

  const int tickLength = 4;

  int axisX, axisY, axisWidth, axisHeight;
  _view->GetYAxisArea(axisX, axisY, axisWidth, axisHeight);
  Coord startY = axisY;
  int axisMaxX = axisX + axisWidth - 1;
  int axisMaxY = axisY + axisHeight - 1;

  {
    Coord maxX = x + w - 1.0;
    Coord maxY = y + h - 1.0;
    if (!Geom::RectRectIntersect(x, y, maxX, maxY, axisX, axisY,
			         axisMaxX, axisMaxY)) {
#if defined(DEBUG)
      printf("do not intersect\n");
#endif
      return;
    }
  }

  VisualFilter filter;
  _view->GetVisualFilter(filter);

  win->SetForeground(_view->GetForeground());
  win->SetPattern(Pattern0);
  win->SetNormalFont();
  _font.SetWinFont(win);

  /* draw a line from startY to the bottom of the view */
  win->Line(axisMaxX, startY, axisMaxX, axisMaxY, 1.0);

  // Don't draw ticks and labels if space is too small.
  if (_width <= tickLength || !TicksEnabled()) return;

  Coord drawHeight = axisHeight - (startY - axisY);
  // Factor of 0.6 spaces things out a little better -- RKW 1999-04-15.
  int numTicks = (int)(drawHeight / _labelWidth * 0.6);

  /* if axis is date, draw values and return */
  if (_attrType == DateAttr) {

    // Draw the labels (date values).
    const char *label = DateString((time_t)filter.yLow, _dateFormat);
    win->AbsoluteText(label, axisX, startY, axisWidth - 1,
		      drawHeight / 2 - 1, WindowRep::AlignCenter, true, 90.0);
    label = DateString((time_t)filter.yHigh, _dateFormat);
    win->AbsoluteText(label, axisX, startY + drawHeight / 2, axisWidth - 1,
		      drawHeight / 2 - 1, WindowRep::AlignCenter, true, 90.0);

    // Draw ticks.
    win->Line(axisMaxX, axisY, axisMaxX - tickLength, axisY, 1);
    win->Line(axisMaxX, axisMaxY, axisMaxX - tickLength, axisMaxY, 1);

    return;
  }

  /*
     since neither custom nor date labels are used, we can draw
     tick marks and label each tick with the corresponding value
  */

  Coord tickMark;
  int nTicks;
  Coord tickInc;
  OptimizeTickMarks(filter.yLow, filter.yHigh, numTicks,
		    tickMark, nTicks, tickInc);

  /* draw tick marks */
  Coord oldTop = -100.0;
  for(int i = 0; i < nTicks; i++) {
    // Note: it would be better to use the WindowRep's transform here, but
    // Zhenhai's inversion makes that a pain.  RKW 1999-04-20.
    Coord yPixel = (tickMark - filter.yLow) * (axisMaxY-startY) /
      (filter.yHigh - filter.yLow) + startY;

    // Draw the tick mark itself.
    win->Line(axisMaxX, yPixel, axisMaxX - tickLength, yPixel, 1);

    char buf[32];
    sprintf(buf, "%.*g", _significantDigits, tickMark);

    Coord labelY = yPixel - _labelWidth/2;

    /* make sure label doesn't go past bottom or top edge */
    Boolean pastBottom, pastTop;
    if (labelY < startY) {
      pastBottom = true;
    } else {
      pastBottom = false;
    }

    if (labelY + _labelWidth > axisY + axisHeight) {
      pastTop = true;
    } else {
      pastTop = false;
    }

    if (!pastBottom && !pastTop) {
	  if (labelY > oldTop) {
        win->AbsoluteText(buf, axisX, labelY, axisWidth-1,
			  _labelWidth, WindowRep::AlignCenter, true);
#if defined(DEBUG)
        printf("  Label <%s> at %d, %g\n", buf, axisX, labelY);
#endif
	    oldTop = labelY + _labelWidth;
	  }
    } else if (pastBottom && !pastTop) {
      labelY = startY;
	  if (labelY > oldTop) {
        win->AbsoluteText(buf, axisX, labelY, axisWidth-1,
			  _labelWidth, WindowRep::AlignSouth, true);
#if defined(DEBUG)
        printf("  Label <%s> at %d, %g\n", buf, axisX, labelY);
#endif
	    oldTop = labelY + _labelWidth;
	  }
    } else if (!pastBottom && pastTop) {
      labelY = axisMaxY - _labelWidth;
	  if (labelY > oldTop) {
        win->AbsoluteText(buf, axisX, labelY, axisWidth-1,
			  _labelWidth, WindowRep::AlignNorth, true);
#if defined(DEBUG)
        printf("  Label <%s> at %d, %g\n", buf, axisX, labelY);
#endif
	    oldTop = labelY + _labelWidth;
	  }
    } else {
      // Don't draw the label.
    }

    tickMark += tickInc;
  }
}

//-----------------------------------------------------------------------------
// Optimize spacing and location of tick marks.
void
DevAxis::OptimizeTickMarks(Coord low, Coord high, int numTicks,
    Coord &start, int &num, Coord &inc)
{
#if defined(DEBUG)
  printf("DevAxis::OptimizeTickMarks(%g, %g, %d)\n", low, high, numTicks);
#endif

  numTicks = MAX(numTicks, 2);
  Coord tickIncrement = (high - low) / (numTicks - 1);

  /* adjust tick increment so that we get nice tick values */
  double exponent = log10(tickIncrement);

  /* negative exponent must round down, not up; floor does this, truncating
   * to integer does not! */
  Coord tickpow = floor(exponent);
  double mantissa = tickIncrement / pow(10.0, tickpow);

  if (mantissa < 1.5) {
    mantissa = 1.0;
  } else if (mantissa < 2.25) {
    mantissa = 2.0;
  } else if (exponent >= 1 && mantissa < 2.75) {
    mantissa = 2.5;
#if 0 // I don't like this one.  RKW 1999-04-20.
  } else if (mantissa < 3.5) {
    mantissa = 3.0;
#endif
  } else if (mantissa < 4.5) {
    mantissa = 4.0;
  } else if (mantissa < 7.5) {
    mantissa = 5.0;
  } else {
    mantissa = 10.0;
  }

  inc = mantissa * pow(10.0, tickpow);
#if defined(DEBUG)
  printf("Old tickIncrement %g, new %g\n", tickIncrement, inc);
#endif

  // Adjust the first tick mark so that tick marks end
  // up on nice coordinate values.
  Coord tmpMantissa = low / pow(10.0, tickpow) / mantissa;
  tmpMantissa = ceil(tmpMantissa);
  start = tmpMantissa * pow(10.0, tickpow) * mantissa;
  if (start == -0.0) start = 0.0;

  num = (int)((high - start) / inc) + 1;

#if defined(DEBUG)
  printf("%d ticks from %g to %g @ %g\n", num, start, high, inc);
#endif

  // Make sure we get at least two tick marks.
  if (num < 2) {
    OptimizeTickMarks(low, high, numTicks + 1, start, num, inc);
  }
}

/*============================================================================*/
