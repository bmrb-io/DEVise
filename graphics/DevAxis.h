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
  Declaration of DevAxis class (used to draw axes in views).
 */

/*
  $Id$

  $Log$
  Revision 1.2  1999/08/30 19:34:24  wenger
  Unified X and Y axis drawing code; found and fixed bug 505 (changing axis
  date format didn't force redraw).

  Revision 1.1  1999/08/18 20:46:04  wenger
  First step for axis drawing improvement: moved code to new DevAxis
  class with unchanged functionality.

 */

#ifndef _DevAxis_h_
#define _DevAxis_h_


#include <sys/types.h>

#include "DeviseTypes.h"
#include "DevFont.h"
#include "AttrList.h"
#include "WindowRep.h"

class View;

class DevAxis {
public:
  enum AxisType { AxisInvalid = 0, AxisX, AxisY, AxisZ };

  DevAxis(View *view, AxisType type, Boolean inUse, int withTicksWidth,
      int noTicksWidth, int significantDigits, int labelWidth);
  ~DevAxis();

  void DrawAxis(WindowRep *win, int x, int y, int w, int h);

  Boolean IsInUse() { return _inUse; }
  void SetInUse(Boolean inUse) { _inUse = inUse; }

  int Width() { return _width; }
  Boolean TicksEnabled() { return _width > _noTicksWidth; }
  void EnableTicks(Boolean ticksOn) {
    _width = ticksOn ? _withTicksWidth : _noTicksWidth; }

  void GetFont(int &family, float &pointSize, Boolean &bold, Boolean &italic);
  void SetFont(int family, float pointSize, Boolean bold, Boolean italic);

  AttrType GetAttrType() { return _attrType; }
  void SetAttrType(AttrType type) { _attrType = type; }

  const char *GetDateFormat() { return _dateFormat; }
  void SetDateFormat(const char *format);

  const char *GetFloatFormat() { return _floatFormat; }
  void SetFloatFormat(const char *format);

protected:
  struct AxisInfo {
    Boolean _drawAxis;
    Boolean _drawTicks;

    int _x0, _y0, _x1, _y1, _x2, _y2;
    Coord _axisLength; // number of pixels along the axis
    Coord _axisAcross; // number of pixels across the axis
    Coord _lowValue, _highValue; // filter values

    Coord _numTicks;

    Coord _tickX, _tickY;
    Coord _labelX, _labelY;
    Coord *_tickPixVar;
    Coord *_labelPixVar;

    Coord _varLoc0, _varLoc1, _varLoc2;

    int _labelHorSize, _labelVerSize;

    WindowRep::SymbolAlignment _bottomOrLeftAlign;
    WindowRep::SymbolAlignment _topOrRightAlign;

    Coord _dateWidth;
    Coord _dateHeight;
  };

  void SetInfo(AxisInfo &info);
  void CheckIntersect(int x, int y, int w, int h, AxisInfo &info);
  void DrawLine(WindowRep *win, AxisInfo &info);
  void DrawDateTicks(WindowRep *win, AxisInfo &info);
  void DrawFloatTicks(WindowRep *win, AxisInfo &info);
  static void OptimizeTickMarks(Coord low, Coord high, int numTicks,
      Coord &start, int &num, Coord &inc);

private:
  AxisType _type;
  Boolean _inUse;          // TRUE if this axis is in use
  int _labelWidth;         // height (for Y axis) or width (X axis)

  int _width;              // width (for Y axis) or height (X axis)
			   // to draw label, in terms of # of pixels
  int _noTicksWidth;
  int _withTicksWidth;

  View *_view;

  DevFont _font;

  AttrType _attrType;      // type of axis attribute
  char *_dateFormat;       // format for labels if date type
  char *_floatFormat;       // format for labels if float type

  int _tickLength;
  int _tickdrawX, _tickdrawY; // values to draw the ticks

  Coord _dateOrientation;
  WindowRep::SymbolAlignment _dateAlignment1;
  WindowRep::SymbolAlignment _dateAlignment2;
};

#endif // _DevAxis_h_

/*============================================================================*/
