/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999-2000
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
  Revision 1.3  2000/03/14 17:05:05  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.2  1999/08/30 19:34:24  wenger
  Unified X and Y axis drawing code; found and fixed bug 505 (changing axis
  date format didn't force redraw).

  Revision 1.1  1999/08/18 20:46:04  wenger
  First step for axis drawing improvement: moved code to new DevAxis
  class with unchanged functionality.

 */

#include <stdio.h>

#include "DevAxis.h"
#include "WindowRep.h"
#include "View.h"
#include "DevError.h"
#include "Util.h"
#include "Session.h"

//#define DEBUG

//-----------------------------------------------------------------------------
// Constructor.
DevAxis::DevAxis(View *view, AxisType type, Boolean inUse,
    int withTicksWidth, int noTicksWidth, int significantDigits,
    int labelWidth)
{
#if defined(DEBUG)
  printf("DevAxis::DevAxis(%s, %d, %d, %d, %d, %d, %d)\n",
      view->GetName(), type, inUse, withTicksWidth, noTicksWidth,
          significantDigits, labelWidth);
#endif

  _view = view;
  _type = type;
  _inUse = inUse;
  _withTicksWidth = withTicksWidth;
  _noTicksWidth = noTicksWidth;
  _width = _withTicksWidth; // default is ticks enabled
  _labelWidth = labelWidth;

  _attrType = FloatAttr;
  _dateFormat = CopyString(GetDefaultDateFormat());

  char buf[128];
  sprintf(buf, "%%.%dg", significantDigits);
  _floatFormat = CopyString(buf);

  switch (_type) {
  case AxisX:
	_tickLength = 3;
	_tickdrawX = 0;
	_tickdrawY = -_tickLength;
	_dateOrientation = 0.0;
	_dateAlignment1 = WindowRep::AlignWest;
	_dateAlignment2 = WindowRep::AlignEast;
    break;

  case AxisY:
    _tickLength = 4;
	_tickdrawX = -_tickLength;
	_tickdrawY = 0;
	_dateOrientation = 90.0;

	// This shouldn't really be center, but non-center alignments don't seem
	// to work right for non-zero orientations.  RKW 1999-08-30.
	_dateAlignment1 = WindowRep::AlignCenter;
	_dateAlignment2 = WindowRep::AlignCenter;
    break;

  case AxisZ:
    break;

  default:
    reportErrNosys("Illegal axis type");
    break;
  }
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
    FreeString(_dateFormat);
    _dateFormat = CopyString(format);
  }
}

//-----------------------------------------------------------------------------
// Set the floating-point format.
void
DevAxis::SetFloatFormat(const char *format)
{
  if (format == NULL) format = "";

#if defined(DEBUG)
  printf("DevAxis::SetFloatFormat(%s)\n", format);
#endif

  if (_floatFormat == NULL || strcmp(format, _floatFormat)) {
    FreeString(_floatFormat);
    _floatFormat = CopyString(format);
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

  DOASSERT(_view->GetNumDimensions() == 2, "Invalid number of dimensions");

  // Make sure the WindowRep has an identity transform matrix, even though
  // this has *probably* already been done.
  win->PushTop();
  win->MakeIdentity();

  AxisInfo info;
  SetInfo(info);

  if (info._drawAxis) {
    CheckIntersect(x, y, w, h, info);
  }

  if (info._drawAxis) {
	DrawLine(win, info);
  }

  if (info._drawTicks) {
    if (_attrType == DateAttr) {
	  DrawDateTicks(win, info);
    }
  }

  if (info._drawTicks) {
	DrawFloatTicks(win, info);
  }

  win->PopTransform();
}

//-----------------------------------------------------------------------------
void
DevAxis::SetInfo(AxisInfo &info)
{
  info._drawAxis = false;
  info._drawTicks = false;

  int axisX, axisY, axisWidth, axisHeight, startX;

  VisualFilter filter;
  _view->GetVisualFilter(filter);

  switch (_type) {
  case AxisX:
	info._drawAxis = true;

    _view->GetXAxisArea(axisX, axisY, axisWidth, axisHeight, startX);
    startX--; // Kludge -- RKW 1999-04-20.
	info._x0 = axisX;
	info._y0 = axisY;
	info._x1 = startX - 1;
	info._y1 = axisY;
	info._x2 = axisX + axisWidth - 1;
	info._y2 = axisY;
	info._axisLength = axisWidth - (startX - axisX);
	info._axisAcross = axisHeight;

	info._lowValue = filter.xLow;
	info._highValue = filter.xHigh;

    info._tickPixVar = &info._tickX;
    info._labelPixVar = &info._labelX;

    info._tickY = info._y1;
    info._labelY = info._y0 - _tickLength - (info._axisAcross-1);

    info._varLoc0 = info._x0;
    info._varLoc1 = info._x1;
    info._varLoc2 = info._x2;
  
    info._labelHorSize = _labelWidth;
    info._labelVerSize = (int)info._axisAcross - 1;

    info._bottomOrLeftAlign = WindowRep::AlignWest;
    info._topOrRightAlign = WindowRep::AlignEast;

	info._dateWidth = info._axisLength / 2 - 1;
	info._dateHeight = info._axisAcross - 1;
    break;

  case AxisY:
	info._drawAxis = true;

    _view->GetYAxisArea(axisX, axisY, axisWidth, axisHeight);
	info._x0 = axisX;
	info._y0 = axisY;
	info._x1 = axisX + axisWidth - 1;
	info._y1 = axisY;
	info._x2 = info._x1;
	info._y2 = axisY + axisHeight - 1;
	info._axisLength = axisHeight;
	info._axisAcross = axisWidth;

	info._lowValue = filter.yLow;
	info._highValue = filter.yHigh;

    info._tickPixVar = &info._tickY;
	info._labelPixVar = &info._labelY;

    info._tickX = info._x1;
	info._labelX = info._x0;

    info._varLoc0 = info._y0;
    info._varLoc1 = info._y1;
    info._varLoc2 = info._y2;

	info._labelHorSize = (int)info._axisAcross;
	info._labelVerSize = _labelWidth;

    info._bottomOrLeftAlign = WindowRep::AlignSouth;
    info._topOrRightAlign = WindowRep::AlignNorth;

	info._dateWidth = info._axisAcross - 1;
	info._dateHeight = info._axisLength / 2 - 1;
    break;

  case AxisZ:
    reportErrNosys("Z axis drawing not implemented");
    break;

  default:
    reportErrNosys("Illegal axis type");
    break;
  }
}

//-----------------------------------------------------------------------------
void
DevAxis::CheckIntersect(int x, int y, int w, int h, AxisInfo &info)
{
  Coord maxX = x + w - 1.0;
  Coord maxY = y + h - 1.0;
  if (!Geom::RectRectIntersect(x, y, maxX, maxY, info._x0, info._y0, info._x2,
      info._y2)) {
#if defined(DEBUG)
    printf("do not intersect\n");
#endif
    info._drawAxis = false;
  }
}

//-----------------------------------------------------------------------------
void
DevAxis::DrawLine(WindowRep *win, AxisInfo &info)
{
  win->SetForeground(_view->GetForeground());
  win->SetPattern(Pattern0);
  win->SetNormalFont();
  _font.SetWinFont(win);

  // Draw the line for the axis.
  win->Line(info._x1, info._y1, info._x2, info._y2, 1.0);

  // Don't draw ticks and labels if space is too small.
  if (_width > _tickLength && TicksEnabled()) info._drawTicks = true;
}

//-----------------------------------------------------------------------------
void
DevAxis::DrawDateTicks(WindowRep *win, AxisInfo &info)
{
  if (!Session::GetIsJsSession() || 1/*TEMP*/) {

    // Draw the labels (date values).

    *info._labelPixVar = info._varLoc1;
    const char *label = DateString((time_t)info._lowValue, _dateFormat);
    win->AbsoluteText(label, info._labelX, info._labelY, info._dateWidth,
	    info._dateHeight, _dateAlignment1, true, _dateOrientation);

    *info._labelPixVar = info._varLoc1 + (info._axisLength / 2);
    label = DateString((time_t)info._highValue, _dateFormat);
    win->AbsoluteText(label, info._labelX, info._labelY, info._dateWidth,
	   info._dateHeight, _dateAlignment2, true, _dateOrientation);
  }

  // Draw ticks.
  win->Line(info._x1, info._y1, info._x1 + _tickdrawX,
      info._y1 + _tickdrawY, 1);
  win->Line(info._x2, info._y2, info._x2 + _tickdrawX,
      info._y2 + _tickdrawY, 1);

  info._drawTicks = false;
}

//-----------------------------------------------------------------------------
void
DevAxis::DrawFloatTicks(WindowRep *win, AxisInfo &info)
{
  info._numTicks = info._axisLength / _labelWidth;

  // Factor of 0.6 spaces things out a little better for Y axis --
  // RKW 1999-04-15.
  if (_type == AxisY) {
    info._numTicks *= 0.6;
  }

  // Figure out how to space the tick marks.
  Coord tickMark;
  int nTicks;
  Coord tickInc;
  OptimizeTickMarks(info._lowValue, info._highValue, (int)info._numTicks,
      tickMark, nTicks, tickInc);

  /* draw tick marks */
  for(int i = 0; i < nTicks; i++) {

    // Note: it would be better to use the WindowRep's transform here, but
    // Zhenhai's inversion makes that a pain.  RKW 1999-04-20.
    *info._tickPixVar = (tickMark - info._lowValue) * (info._varLoc2 - info._varLoc1) /
        (info._highValue - info._lowValue) + info._varLoc1;

    // Draw the tick mark itself.
    win->Line(info._tickX, info._tickY, info._tickX + _tickdrawX, info._tickY + _tickdrawY, 1);

    char buf[32];
    sprintf(buf, _floatFormat, tickMark);

	*info._labelPixVar = *info._tickPixVar - _labelWidth / 2;

    /* make sure label doesn't go past left or right edge */
    Boolean pastLeftOrBottom, pastRightOrTop;
	if (*info._labelPixVar < info._varLoc0) {
      pastLeftOrBottom = true;
    } else {
      pastLeftOrBottom = false;
    }

	if (*info._labelPixVar + _labelWidth > info._varLoc2) {
      pastRightOrTop = true;
    } else {
      pastRightOrTop = false;
    }

	if (!Session::GetIsJsSession() || 1/*TEMP*/) {
	  WindowRep::SymbolAlignment align;
	  Boolean drawLabel = true;
      if (!pastLeftOrBottom && !pastRightOrTop) {
	    // We're fine where we are.
		align = WindowRep::AlignCenter;
      } else if (pastLeftOrBottom && !pastRightOrTop) {
		*info._labelPixVar = info._varLoc1;
		align = info._bottomOrLeftAlign;
      } else if (!pastLeftOrBottom && pastRightOrTop) {
		*info._labelPixVar = info._varLoc2 - _labelWidth;
		align = info._topOrRightAlign;
      } else {
        // Don't draw the label.
		drawLabel = false;
      }

	  if (drawLabel) {
        win->AbsoluteText(buf, info._labelX, info._labelY, info._labelHorSize, info._labelVerSize,
			  align, true);
#if defined(DEBUG)
          printf("  Label <%s> at %g, %g\n", buf, info._labelX, info._labelY);
#endif
	  }
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
