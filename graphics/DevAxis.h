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
  Declaration of DevAxis class (used to draw axes in views).
 */

/*
  $Id$

  $Log$
 */

#ifndef _DevAxis_h_
#define _DevAxis_h_


#include <sys/types.h>

#include "DeviseTypes.h"
#include "DevFont.h"
#include "AttrList.h"

class WindowRep;
class View;

class DevAxis {
public:
  enum AxisType { AxisInvalid = 0, AxisX, AxisY, AxisZ };

  DevAxis(View *view, AxisType type, Boolean inUse, int withTicksWidth,
      int noTicksWidth, int numTicks, int significantDigits, int labelWidth);
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

protected:
  void DrawXAxis(WindowRep *win, int x, int y, int w, int h);
  void DrawYAxis(WindowRep *win, int x, int y, int w, int h);
  static void OptimizeTickMarks(Coord low, Coord high, int numTicks,
      Coord &start, int &num, Coord &inc);

private:
  AxisType _type;
  Boolean _inUse;          // TRUE if this axis is in use
  int _width;              // width (for Y axis) or height (X axis)
			   // to draw label, in terms of # of pixels
  int _numTicks;           // # of ticks to use
  int _significantDigits;  // # of significant digits
  int _labelWidth;         // height (for Y axis) or width (X axis)

  int _noTicksWidth;
  int _withTicksWidth;

  char *_dateFormat;

  View *_view;

  DevFont _font;

  AttrType _attrType;
};

#endif // _DevAxis_h_

/*============================================================================*/
