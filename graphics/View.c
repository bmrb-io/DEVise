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
  Revision 1.22  1996/02/02 21:53:45  jussi
  Removed SetFgColor() right after SetXorMode() which changed the
  effect of the xor function.

  Revision 1.21  1996/02/01 20:28:52  jussi
  Axis areas were cleared even if axes were not displayed. Fixed
  this.

  Revision 1.20  1996/01/30 21:11:21  jussi
  Replaced references to specific colors with references to
  BackgrounColor and ForegroundColor.

  Revision 1.19  1996/01/16 17:07:29  jussi
  Redefined computation of cursor area.

  Revision 1.18  1995/12/29 22:41:01  jussi
  Symbols are turned on and connectors off before a pixel map
  is saved.

  Revision 1.17  1995/12/29 18:33:13  jussi
  Forgot to comment out the #define DEBUG statement in last check-in.

  Revision 1.16  1995/12/29 18:27:52  jussi
  Added new cursor mechanism; cursors are drawn as inverted rectangles
  which are easier to see than the old cursor lines.

  Revision 1.15  1995/12/14 21:11:05  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.14  1995/12/14 17:18:24  jussi
  More small fixes to get rid of g++ -Wall warnings.

  Revision 1.13  1995/12/14 17:14:56  jussi
  Small fixes.

  Revision 1.12  1995/12/06 21:18:12  jussi
  Minor improvements: X high label no longer gets overdrawn by the highlight
  rectangle when label is at top; erasing data area erases top pixel line
  as well; PushClip() and AbsoluteText() interface changed slightly, so
  now the caller has to decrement width and height by one if passing pixel
  coordinates.

  Revision 1.11  1995/12/05 22:05:00  jussi
  A constant integer value is saved at the head of a pixmap file
  to make sure that a machine which tries to load the pixmap has
  the same endianness.

  Revision 1.10  1995/12/05 17:02:00  jussi
  Moved _stats to ViewGraph (superclass) so that statistics can be
  turned on and displayed without having to redisplay the data itself.

  Revision 1.9  1995/12/04 18:06:05  jussi
  Added GetLabelParams and replaced ToggleStatDisplay with SetStatDisplay.

  Revision 1.8  1995/12/02 21:23:26  jussi
  Corrected code which was based on the assumption that a view
  always has origin 0,0 which is no longer true with margins
  surrounding the view. Fixed numerous other bugs.

  Revision 1.7  1995/11/28  05:08:24  ravim
  Support for statistics.

  Revision 1.6  1995/11/25 19:55:37  jussi
  Added debugging statement.

  Revision 1.5  1995/11/24 21:30:39  jussi
  Added copyright notice and cleaned up code. Added debugging
  statements.

  Revision 1.4  1995/09/26 23:05:12  jussi
  Made X low label left-justified.

  Revision 1.3  1995/09/14 19:19:51  jussi
  Corrected a couple of perror messages.

  Revision 1.2  1995/09/05 21:13:15  jussi
  Added/updated CVS header.
*/

#include <time.h>
#include <assert.h>

#include "Util.h"
#include "View.h"
#include "Geom.h"
#include "Action.h"
#include "WindowRep.h"
#include "FilterQueue.h"
#include "Control.h"
#include "TimeStamp.h"
#include "Parse.h"
#include "Init.h"
#ifdef JPEG
#include "Jpeg.h"
#endif

//#define DEBUG

/* width/height of sensitive area for cursor */
static const int VIEW_CURSOR_SENSE = 10;

/* 256K bytes before saving pixel data*/
static const int VIEW_BYTES_BEFORE_SAVE = 256 * 1024;

static const Coord DELTA_X = .000000000001;
static const Coord DELTA_Y = .000000000001;

/* id for the next view created.
   view == NULL return id = 0.*/

int View::_nextId = 0;
ViewList *View::_viewList = NULL;   /* list of all views */

int View::_nextPos = 0;
ViewCallbackList *View::_viewCallbackList;

View::View(char *name, Action *action, VisualFilter &initFilter, 
	   Color fg, Color bg, AxisLabel *xAxis, AxisLabel *yAxis,
	   int weight, Boolean boundary) :
	ViewWin(name, fg, bg, weight, boundary)
{
  _modeRefresh = false;
  _hasTimestamp = false;
  Init(name,action, initFilter, xAxis, yAxis);
  _filterQueue = new FilterQueue();
  ReportViewCreated();
  
  int flushed = _filterQueue->Enqueue(initFilter);
  ReportFilterChanged(initFilter, flushed);
  _highlight = false; 

  _xAxisAttrType = FloatAttr;
  _yAxisAttrType = FloatAttr;

  _pixmap = NULL;
  _pixmapIO = new PixmapIO();
  _compress = new SimpleCompress();

  _cursorsOn = false;
}

void View::Init(char *name,Action *action, VisualFilter &filter,
		AxisLabel *xAxisLabel, AxisLabel *yAxisLabel)
{
  _jump = _zoomIn = _zoomOut = _scrollLeft = _scrollRight = _unknown = 0;
	
  ControlPanel::Instance()->InsertCallback(this);

  _hasXMin = false;

  _hasLastFilter = false;
  _updateTransform = true;
  _refresh = true;

  _xAxisLabel = xAxisLabel;
  _yAxisLabel = yAxisLabel;
  
  _id = ++_nextId;
  
  if (_viewList == NULL)
    _viewList = new ViewList;
  _viewList->Insert(this);
  
  _action = action;
  if (_action == NULL)
    _action = new Action("");
  
  _displaySymbol = true;

  /* init default filter */
  _filterChanged = true;
  if (!_hasTimestamp) {
    _hasTimestamp =true;
    _timeStamp = TimeStamp::NextTimeStamp();
  }
  _filter = filter;
  
  _hasExposure = false; /* no exposure rectangle yet */
  
  _querySent = false;	/* TRUE if query has been sent */
  
  xAxis.inUse = false;
  xAxis.color = GetFgColor();
  xAxis.width = 15;
  xAxis.decimalPlaces = 1;
  xAxis.numTicks = 2;
  xAxis.useNumTicks = true;
  xAxis.fieldWidth = 10;
  
  yAxis.inUse = false;
  yAxis.width = 50;
  yAxis.decimalPlaces = 1;
  yAxis.numTicks = 2;
  yAxis.color = GetFgColor();
  yAxis.useNumTicks = true;
  yAxis.fieldWidth = 10;
  
  _label.occupyTop = false;
  _label.extent = 12;
  _label.name = 0;
  
  _axisDisplay = true;
  
  _cursors = new DeviseCursorList;
  
  Dispatcher::Current()->Register(this);
}

View::~View()
{
  delete _label.name;

  Unmap();
  DeleteFromParent();
  ControlPanel::Instance()->DeleteCallback(this);
  ReportViewDestroyed();
  Dispatcher::Current()->Unregister(this);
}

/***************************************************************
Find view by name. Return NULL if not found.
****************************************************************/

View *View::FindViewByName(char *name)
{
  if (_viewList == NULL) {
    fprintf(stderr,"View::FindViewByName: _viewList empty\n");
    return NULL;
  }
  
  int index;
  for(index = _viewList->InitIterator(); _viewList->More(index); ) {
    View *view = _viewList->Next(index);
    if (strcmp(view->GetName(), name)== 0) {
      _viewList->DoneIterator(index);
      return view;
    }
  }
  _viewList->DoneIterator(index);
  
  fprintf(stderr, "View::FindViewByName: %s not found\n", name);
  return NULL;
}

/***************************************************************
Find view by id
****************************************************************/

View *View::FindViewById(int id)
{
  if (id == 0)
    return NULL;
  
  if (_viewList == NULL) {
    fprintf(stderr,"View::FindViewById: _viewList empty\n");
    Exit::DoExit(1);
  }
  
  for(int index = _viewList->InitIterator(); _viewList->More(index); ) {
    View *view = _viewList->Next(index);
    if (view->GetId() == id) {
      _viewList->DoneIterator(index);
      return view;
    }
  }
  
  fprintf(stderr,"View::FindViewById: %d not found\n", id);
  return NULL;
}

int View::FindViewId(View *view)
{
  if (view == NULL)
    return 0;
  else
    return view->GetId();
}

void View::SubClassMapped()
{
  _updateTransform = true;
  _refresh = true;
}
	
void View::SubClassUnmapped()
{
  if (_querySent) {
    DerivedAbortQuery();
    ReportQueryDone(0);
  }
}

void View::SetVisualFilter(VisualFilter &filter)
{
#ifdef DEBUG
  printf("%s SetVisualFilter %f %f %f %f %f, %f, %f, %f\n", GetName(),
	 filter.xLow, filter.yLow,
	 filter.xHigh, filter.yHigh,_filter.xLow, _filter.yLow,
	 _filter.xHigh, _filter.yHigh);
#endif

  if (filter.xLow > _filter.xHigh || filter.xHigh < _filter.xLow)
    _jump++;
  else if (filter.xLow < _filter.xLow && filter.xHigh > _filter.xHigh)
    _zoomOut++;
  else if (filter.xLow > _filter.xLow && filter.xHigh < _filter.xHigh)
    _zoomIn++;
  else if (filter.xLow < _filter.xLow && filter.xHigh < _filter.xHigh)
    _scrollLeft++;
  else if (filter.xLow > _filter.xLow && filter.xHigh > _filter.xHigh)
    _scrollRight++;
  else
    _unknown++;
  
  ReportFilterAboutToChange();

  /* ignore new filter if same as current one */
  if (_filter.xLow != filter.xLow || _filter.xHigh != filter.xHigh
      || _filter.yLow != filter.yLow || _filter.yHigh != filter.yHigh) {
#ifdef DEBUG
    printf("filter changed\n");
#endif
    _filterChanged = true;
    if (!_hasTimestamp) {
      _timeStamp = TimeStamp::NextTimeStamp();
      _hasTimestamp = true;
    }
    _updateTransform = true;
    _filter = filter;
  }

  int flushed = _filterQueue->Enqueue(filter);
  ReportFilterChanged(filter, flushed);
}

void View::GetVisualFilter(VisualFilter &filter)
{
  filter = _filter;
}

VisualFilter *View::GetVisualFilter()
{
  return &_filter;
}

void View::Mark(int index, Boolean marked)
{
  _filterQueue->Mark(index, marked);
}

void View::HandleExpose(WindowRep *w, int x, int y, unsigned width, 
			unsigned height)
{
#ifdef DEBUG
  printf("View::HandleExpose %d,%d,%u,%u\n", x, y, width, height);
#endif

  if (!_hasExposure) {

    _hasExposure = true;
    _exposureRect.xLow = x;
    _exposureRect.yLow = y;
    _exposureRect.xHigh = x + width - 1;
    _exposureRect.yHigh = y + height - 1;

  } else {

    /* merge with existing exposed region */
    int minX1 = _exposureRect.xLow;
    int minY1 = _exposureRect.yLow;
    int maxX1 = _exposureRect.xHigh;
    int maxY1 = _exposureRect.yHigh;
    
    int minX2 = x;
    int minY2 = y;
    int maxX2 = x + width - 1;
    int maxY2 = y + height - 1;
    
    _exposureRect.xLow = MinMax::min(minX1, minX2);
    _exposureRect.yLow = MinMax::min(minY1, minY2);
    _exposureRect.xHigh = MinMax::max(maxX1, maxX2);
    _exposureRect.yHigh = MinMax::max(maxY1, maxY2);
  }
}

/* Handle button press event */

void View::HandlePress(WindowRep *w, int xlow, int ylow,
		       int xhigh, int yhigh, int button)
{
  ControlPanel::Instance()->SelectView(this);
	
  if (xlow == xhigh && ylow == yhigh) {
    if (CheckCursorOp(w, xlow, ylow, button))
      /* was a cursor event */
      return;
  }
  
  if (_action != NULL) {
    /* transform from screen to world coordinates */
    Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
    FindWorld(xlow, ylow, xhigh, yhigh,
	      worldXLow, worldYLow, worldXHigh, worldYHigh);
    
    _action->AreaSelected(this, worldXLow, worldYLow, worldXHigh, 
			  worldYHigh, button);
  }
}

/* handle key event */

void View::HandleKey(WindowRep *w, char key, int x, int y)
{
  ControlPanel::Instance()->SelectView(this);

  if (_action != NULL) {
    /* xform from screen to world coord */
    Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
    FindWorld(x, y, x, y,
	      worldXLow, worldYLow, worldXHigh, worldYHigh);
    
    _action->KeySelected(this, key, worldXLow, worldYLow);
  }
}

Boolean View::CheckCursorOp(WindowRep *win, int x, int y, int button)
{
  int cursorX, cursorY, cursorW, cursorH;
  Coord worldXLow, worldYLow, worldXHigh, worldYHigh;

  GetXCursorArea(cursorX, cursorY, cursorW, cursorH);
  int cursorMaxX = cursorX + cursorW - 1;
  int cursorMaxY = cursorY + cursorH - 1;

  if (x >= cursorX && x <= cursorMaxX &&
      y >= cursorY && y <= cursorMaxY) {
    /* event for cursor area */
    if (_cursors->Size() > 0) {
      /* change location of the X coord */
      FindWorld(x, y, x + 1, y - 1,
		worldXLow, worldYLow, worldXHigh, worldYHigh);
      DeviseCursor *cursor = _cursors->GetFirst();
      cursor->MoveSourceX((worldXHigh + worldXLow) / 2.0);
    }
    return true;
  }

  return false;
}

Boolean View::HandlePopUp(WindowRep *win, int x, int y, int button,
			  char **&msgs, int &numMsgs)
{
#ifdef DEBUG
  printf("View::HandlePopUp at %d,%d, action = 0x%p\n", x, y, _action);
#endif

  ControlPanel::Instance()->SelectView(this);

  static char *buf[10];
  Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
  
  if (CheckCursorOp(win, x, y, button)) {
    /* it was cursor event */
    return false;
  }

  int labelX, labelY, labelW, labelH;
  GetLabelArea(labelX, labelY, labelW, labelH);

  if (x >= labelX && x <= labelX + labelW - 1
      && y >= labelY && y <= labelY + labelH - 1) {
    buf[0] = GetName();
    msgs = buf;
    numMsgs = 1;
    return true;
  }
  
  if (_action != NULL) {
    /* transform from screen x/y into world x/y */
    FindWorld(x, y, x + 1, y - 1,
	      worldXLow,worldYLow, worldXHigh, worldYHigh);
    return _action->PopUp(this, worldXLow, worldYLow,
			  worldXHigh, worldYHigh, button,
			  msgs, numMsgs);
  }

  return false;
}

/* get area for displaying label */

void View::GetLabelArea(int &x, int &y, int &width, int &height)
{
  unsigned int w, h;
  Geometry(x, y, w, h);

  if (_label.occupyTop) {
    width = w;
    height = _label.extent;
  } else {
    width = _label.extent;
    height = h;
  }
  
  if (width < 0)
    width = 1;
  if (height < 0)
    height = 1;
}

/********************************************************************
get area for displaying X axis. Assumes that X axis is in use.
area = x,y,width, height, and startX, as follows:

          +--+--------+
          |  |        |
          |  |        |
          |  |        |
     (x,y)+--+--------+
          |  ^        | <- height
          |  startX   |
          +-----------+
                ^
                |
	    	 width
************************************************************************/

void View::GetXAxisArea(int &x, int &y, int &width, int &height,
			int &startX)
{
  unsigned int windW, winH;
  Geometry(x, y, windW, winH);

  y += winH - xAxis.width;
  width = windW;
  height = xAxis.width;
  
  if (!_label.occupyTop) {
    x += _label.extent;
    width -= _label.extent;
  }

  startX = x;
  if (yAxis.inUse)
    startX += yAxis.width;
  else {
    // no space space is really necessary, but add just a little bit
    // of space to separate label from the window border better
    startX += 2;
  }

  if (width < 0)
    width = 1;
  if (height < 0)
    height = 1;
}

/********************************************************************
get area for displaying Y axis. Assumes that X axis is in use.
area = x,y,width, height, and startX, as follows:

          width
     (x,y)+--+--------+
          |  |        |
    height|  |        |
          |  |        |
          +--+--------+
          |           | 
          |           |
          +-----------+
********************************************************************/

void View::GetYAxisArea(int &x, int &y, int &width, int &height)
{
  unsigned int winW, winH;
  Geometry(x, y, winW, winH);

  if (_label.occupyTop) {
    y += _label.extent;
    height = winH - _label.extent;
    width = yAxis.width;
  } else {
    x += _label.extent;
    width = yAxis.width;
    height = winH;
  }
  
  if (xAxis.inUse)
    height -= xAxis.width;
  
  if (width <= 0 )
    width = 1;
  if (height <= 0)
    height = 1;
}

void View::GetDataArea(int &x, int &y, int &width,int &height)
{
  unsigned int winWidth, winHeight;
  Geometry(x, y, winWidth, winHeight);
  
#ifdef DEBUG
  printf("GetDataArea: full area at %d,%d, size %d, %d\n",
	 x, y, winWidth, winHeight);
#endif

  if (_label.occupyTop) {
    /* _label occupies top of view */
    y +=  _label.extent; 
    /* 
       subtract 2 from left and right so that data doesn't draw
       over the highlight border
    */
    x += 2;
    width = winWidth - 2 - 2;
    height = winHeight - _label.extent;
  } else {
    /* _label occupies left of view */
    x += _label.extent;
    width = winWidth - _label.extent;
    height = winHeight;
  }
  
  if (_axisDisplay) {
    /* need to display axes */
    if (xAxis.inUse)
      height -= xAxis.width;
    else if (_label.occupyTop)
      height -= 2;

    if (yAxis.inUse) {
      x += yAxis.width;
      width -= yAxis.width;
    }
  }
  
  if (width <= 0) width = 1;
  if (height <= 0) height = 1;
  
#ifdef DEBUG
  printf("GetDataArea %s %d %d %d %d\n", GetName(), x, y, width, height);
#endif
}

void View::DrawAxesLabel(WindowRep *win, int x, int y, int w, int h)
{
#ifdef DEBUG
  printf("DrawAxesLabel %s %d %d %d %d\n", GetName(), x, y, w, h);
#endif

  int winX, winY; 
  unsigned int winW, winH;
  Geometry(winX, winY, winW, winH);
  
  /* Make it possible to update the label areas */
  win->PushTop();
  win->MakeIdentity();
  win->PushClip(winX, winY, winW - 1, winH - 1);
  win->PopTransform();
  
  DrawLabel();

  /* Clear highlight area */
  win->SetFgColor(GetBgColor());
  DrawHighlight();

  if (_axisDisplay) {
    int axisX, axisY, axisWidth, axisHeight, startX;
    if (xAxis.inUse) {
      GetXAxisArea(axisX, axisY, axisWidth, axisHeight, startX);
      win->SetFgColor(GetBgColor());
      win->FillRect(axisX, axisY, axisWidth - 1, axisHeight - 1);
      DrawXAxis(win, x, y, w, h);
    }
    if (yAxis.inUse) {
      GetYAxisArea(axisX, axisY, axisWidth, axisHeight);
      win->SetFgColor(GetBgColor());
      win->FillRect(axisX, axisY, axisWidth - 1, axisHeight - 1);
      DrawYAxis(win, x, y, w, h);
    }
  }

  win->PopClip();
}

void View::DrawLabel()
{
  WindowRep *win = GetWindowRep();
  
  if (_label.occupyTop) {
    /* draw label */
    int labelX, labelY, labelWidth, labelHeight;
    GetLabelArea(labelX, labelY, labelWidth, labelHeight);
    win->SetFgColor(GetBgColor());
    win->FillRect(labelX, labelY, labelWidth - 1, labelHeight - 1);
    win->SetFgColor(GetFgColor());
    win->AbsoluteText(_label.name, labelX, labelY, labelWidth - 1,
		      labelHeight - 1, WindowRep::AlignCenter, true);
  } else {
    /* draw square with cross mark in top-left corner of view */
    int x, y, w, h;
    GetLabelArea(x, y, w, h);
    win->SetFgColor(GetFgColor());
    win->AbsoluteLine(x, y, x + _label.extent - 1, y, 1);
    win->AbsoluteLine(x + _label.extent-1, y,
		      x + _label.extent-1, y + _label.extent-1, 1);
    win->AbsoluteLine(x + _label.extent-1, y + _label.extent-1,
		      x, y + _label.extent-1, 1);
    win->AbsoluteLine(x, y + _label.extent-1, x, y, 1);
    win->AbsoluteLine(x, y, x + _label.extent-1, y + _label.extent-1, 1);
    win->AbsoluteLine(x + _label.extent-1, y, x, y + _label.extent-1, 1);
  }
}

void View::DrawXAxis(WindowRep *win, int x, int y, int w, int h)
{
#ifdef DEBUG
  printf("DrawXAxis %s %d %d %d %d\n", GetName(), x, y, w, h);
#endif
  
  int axisX, axisY, axisWidth, axisHeight, startX;
  GetXAxisArea(axisX, axisY, axisWidth, axisHeight, startX);
  int axisMaxX = axisX + axisWidth - 1;
  int axisMaxY = axisY + axisHeight - 1;
  char *label;
  char buf[30];
  int maxX = x + w - 1;
  int maxY = y + h - 1;
  if (!Geom::RectRectIntersect(x,y,maxX,maxY,axisX,axisY,axisMaxX, axisMaxY)) {
#ifdef DEBUG
    printf("do not intersect\n");
#endif
    return;
  }
  
  win->PushTop();
  win->MakeIdentity();
  win->SetFgColor(xAxis.color);
  
  Coord startWorldX = _filter.xLow;
  Coord endWorldX = _filter.xHigh;
  int numMajorTicks = xAxis.numTicks;
  Coord tickIncrement = (endWorldX - startWorldX) / numMajorTicks;
  
  // keep compiler happy until tickIncrement is used for real
  tickIncrement = tickIncrement;

  if (xAxis.numTicks == 2) {
    /* draw the text */
    Coord drawWidth = axisWidth - (startX - axisX);

    if (_xAxisLabel != NULL)
      label = _xAxisLabel->GenerateLabel( _filter.xLow);
    else {
      if (_xAxisAttrType == DateAttr) {
	time_t tm = (time_t)_filter.xLow;
	label = DateString(tm);
      } else {
	sprintf(buf, "%*.*f", xAxis.fieldWidth, xAxis.decimalPlaces,
		_filter.xLow);
	label = buf;
      }
    }
    
    win->AbsoluteText(label, startX, axisY, drawWidth / 2 - 1, axisHeight - 1,
		      WindowRep::AlignWest, true);
    
    if (_xAxisLabel != NULL)
      label = _xAxisLabel->GenerateLabel(_filter.xHigh);
    else {
      if (_xAxisAttrType == DateAttr) {
	time_t tm = (time_t)_filter.xHigh;
	label = DateString(tm);
      } else {
	sprintf(buf, "%*.*f", xAxis.fieldWidth, xAxis.decimalPlaces,
		_filter.xHigh);
	label = buf;
      }
    }

    win->AbsoluteText(label, startX + drawWidth / 2, axisY, drawWidth / 2 - 1,
		      axisHeight - 1, WindowRep::AlignEast, true);
  }
  
  /* draw a line from startX to the end of the view */

  win->SetPattern(Pattern0);
  win->Line(startX - 1, axisY, axisMaxX, axisY, 1.0);
  
  win->PopTransform();
}

void View::DrawYAxis(WindowRep *win, int x, int y, int w, int h)
{
#ifdef DEBUG
  printf("DrawYAxis %s %d %d %d %d\n", GetName(), x, y, w, h);
#endif
  
  char buf[30];

  int axisX, axisY, axisWidth, axisHeight;
  Coord startY;
  GetYAxisArea(axisX, axisY, axisWidth, axisHeight);
  startY = axisY;
  int axisMaxX = axisX + axisWidth - 1;
  int axisMaxY = axisY + axisHeight - 1;

  Coord maxX = x + w;
  Coord maxY = y + h;
  if (!Geom::RectRectIntersect(x, y, maxX, maxY, axisX, axisY,
			       axisMaxX, axisMaxY)) {
#ifdef DEBUG
    printf("do not intersect\n");
#endif
    return;
  }

  win->PushTop();
  win->MakeIdentity();
  win->SetFgColor(yAxis.color);

  char *label;
  
  if (yAxis.numTicks == 2) {
    /* draw lower Y coord */
    Coord drawHeight = axisHeight - (startY - axisY);
    if (_yAxisLabel != NULL)
      label = _yAxisLabel->GenerateLabel(_filter.yLow);
    else {
      if (_yAxisAttrType == DateAttr) {
	time_t tm = (time_t)_filter.yLow;
	label = DateString(tm);
      } else {
	sprintf(buf,"%*.*f",yAxis.fieldWidth, yAxis.decimalPlaces,
		_filter.yLow);
	label = buf;
      }
    }

    win->AbsoluteText(label, axisX, startY + drawHeight / 2, axisWidth - 1,
		      drawHeight / 2 - 1, WindowRep::AlignSouth, true);
    
    /* draw upper Y coord */
    if (_yAxisLabel != NULL)
      label = _yAxisLabel->GenerateLabel(_filter.yHigh);
    else {
      if (_yAxisAttrType == DateAttr) {
	time_t tm = (time_t)_filter.yHigh;
	DateString(tm);
      } else {
	sprintf(buf,"%*.*f",yAxis.fieldWidth,
		yAxis.decimalPlaces,_filter.yHigh);
	label = buf;
      }
    }

    win->AbsoluteText(label, axisX, startY, axisWidth - 1,
		      drawHeight / 2 - 1, WindowRep::AlignNorth, true);
  }

  /* draw a line from startY to the bottom of the view */

  win->SetPattern(Pattern0);
  win->Line(axisMaxX, startY, axisMaxX, axisMaxY + 1, 1.0);
  
  win->PopTransform();
}

#if 0
void View::AxisDisplayOnOff(Boolean stat)
{
  if (stat != _axisDisplay) {
    _axisDisplay = stat;
    _updateTransform = true;
  }
}
#endif

/* Find world coord given screen coord */

void View::FindWorld(int sx1,int sy1, int sx2, int sy2,
		     Coord &xLow, Coord &yLow, Coord &xHigh, Coord &yHigh)
{
  Transform2D transform;
  CalcTransform(transform);

  Coord x1, x2, y1, y2;
  transform.InverseTransform(sx1, sy1, x1, y1);
  transform.InverseTransform(sx2, sy2, x2, y2);
  xLow = MinMax::min(x1, x2);
  xHigh = MinMax::max(x1, x2);
  yLow = MinMax::min(y1, y2);
  yHigh = MinMax::max(y1, y2);
}

/* Calculate the transformation matrix used to translate from
   world to screen coordinates */

void View::CalcTransform(Transform2D &transform)
{
  transform.MakeIdentity();

  int dataX, dataY, dataWidth, dataHeight;
  GetDataArea(dataX, dataY, dataWidth, dataHeight);

#ifdef DEBUG
  printf("transform data: %d,%d,%d,%d\n", dataX, dataY,
	 dataWidth, dataHeight);
#endif
  
  /* translate to 0, 0 */
  transform.Translate(-_filter.xLow, -_filter.yLow);

  /* scale to size of the screen */
  transform.Scale((Coord)dataWidth/(_filter.xHigh -  _filter.xLow),
		  (Coord)(dataHeight)/(_filter.yHigh - _filter.yLow));

  /* invert the Y coord to fit (0,0) at top-left corner */
  transform.Scale(1.0, -1.0);
  transform.Translate(0.0, dataHeight);
  
  /* Translate to beginning of data area. */
  transform.Translate(dataX, dataY);
}

/* For query processing */

void View::ReportQueryDone(int bytes)
{
#ifdef DEBUG
  printf("View::ReportQueryDone\n");
#endif

  _bytes = bytes;
  _querySent = false;

  _cursorsOn = false;
  (void)DrawCursors();

  GetWindowRep()->PopClip();

  DrawLogo();

  _hasLastFilter = false;
  
  ControlPanel::Instance()->SetIdle();
}

/***********************************************************************
Send a query, if there is one.  Abort existing query if necessary. 
XXX: need to crop exposure against _filter before sending query.
**********************************************************************/

void View::Run()
{
  ControlPanel::Mode mode = ControlPanel::Instance()->GetMode();
#ifdef DEBUGxxx
  if (mode == ControlPanel::LayoutMode)
    printf("layout mode ");
  else
    printf("disp mode ");
  printf("%d %d %d %d\n", _hasExposure, _filterChanged, _refresh,
	 _updateTransform);
#endif
  
  if (mode == ControlPanel::LayoutMode && 
      (_hasExposure || _filterChanged || _refresh || _updateTransform)) { 
    /* keep track that events occurred while in LayoutMode*/
    _modeRefresh = true;
  } else if (mode == ControlPanel::DisplayMode && _modeRefresh) {
    /* need to refresh in display mode because events occurred in
       LayoutMode */
    _refresh = true;
  }
  
  Boolean notMapped = !Mapped();
  Boolean iconified = Iconified();
  
  if (_querySent) {
    if (_hasExposure || _filterChanged || _refresh || _updateTransform ||
	notMapped || iconified ) {
#ifdef DEBUG
      printf("View:: aborting\n");
#endif
      DerivedAbortQuery();
      ReportQueryDone(0);
      _refresh = true;
    } else
      return;
  }
  
  if (notMapped || iconified)
    return;

  if (!_hasExposure && !_filterChanged && !_refresh)
    return;

  WindowRep *winRep = GetWindowRep();
  int scrnX, scrnY, scrnWidth, scrnHeight;
  unsigned int sW, sH;
  Geometry(scrnX, scrnY, sW, sH);

  scrnWidth = sW;
  scrnHeight = sH;
  VisualFilter newFilter;
  
  if (RestorePixmap(_filter, newFilter) == PixmapTotal) {
#ifdef DEBUG
    printf("View::Run: Restored complete pixmap for\n  %s\n", GetName());
#endif

    if (_updateTransform) {
      UpdateTransform(GetWindowRep());
      _updateTransform = false;
    }
    /* Set up transformation to draw the axes */
    winRep->PushTop();
    winRep->MakeIdentity();
    
    /* Draw axes */
    DrawAxesLabel(winRep, scrnX, scrnY, sW, sH);
    
    /* Draw highlight border */
    Boolean oldHighlight = _highlight;
    _highlight = false;
    Highlight(oldHighlight);
    
    /* Pop the transform */
    winRep->PopTransform();
    
    /* Draw cursors */
    _cursorsOn = false;
    (void)DrawCursors();
    DrawLogo();

    _hasExposure = false;
    _filterChanged = false;
    _refresh = false;
    _hasLastFilter = false;
    return;
  }
  
#ifdef DEBUG
  printf("Run: window 0x%p scrollable is %d\n", 
	 winRep, (winRep->Scrollable() ? 1 : 0));
#endif
  
  if (!_updateTransform && !_hasExposure && !_refresh && _filterChanged) {
    /* Do scroll, if we can  */
    UpdateFilterStat stat;
    stat = UpdateFilterWithScroll();
    if (stat == Scrolled) {
      /* did scrolling. Can keep this filter */
      /* update so that _hasFilter is no longer true,
	 and _hasExposure is true. */
      _hasExposure = true;
      _filterChanged = false;
      _hasLastFilter = true;
      _lastFilter = _filter;
    } else if (stat == SameFilter) {
      /* A sequence of scroll/zoom between updates
	 made the latest filter the same as the one being displayed */
      _filterChanged = false;
      _hasLastFilter = true;
      _lastFilter = _filter;
      return;
    }
  }
  
  /* Update the WindowRep's transformation matrix */
  if (_updateTransform) {
    UpdateTransform(winRep);
    _updateTransform = false;
  }
  
  if (_hasExposure) {
    /* limit exposure to the size of the window */
#ifdef DEBUG
    printf("exposure (%d,%d),(%d,%d) ",
	   _exposureRect.xLow, _exposureRect.yLow,
	   _exposureRect.xHigh, _exposureRect.yHigh);
#endif
    
    _exposureRect.xLow = MinMax::max(_exposureRect.xLow, 0);
    _exposureRect.xLow = MinMax::min(_exposureRect.xLow, scrnWidth - 1);
    _exposureRect.xHigh = MinMax::max(_exposureRect.xLow,
				      _exposureRect.xHigh);
    _exposureRect.xHigh = MinMax::min(_exposureRect.xHigh, scrnWidth - 1);
    
    _exposureRect.yLow = MinMax::max(_exposureRect.yLow, 0);
    _exposureRect.yLow = MinMax::min(_exposureRect.yLow, scrnHeight - 1);
    _exposureRect.yHigh = MinMax::max(_exposureRect.yLow,
				      _exposureRect.yHigh);
    _exposureRect.yHigh = MinMax::min(_exposureRect.yHigh, scrnHeight - 1);
    
#ifdef DEBUG
    printf("--> (%d,%d),(%d,%d)\n",
	   _exposureRect.xLow, _exposureRect.yLow,
	   _exposureRect.xHigh, _exposureRect.yHigh);
#endif
  }
  
  if (_filterChanged || _refresh) {
    /* Need to redraw the whole screen */
    _queryFilter = _filter;
    _hasLastFilter = true;
    _lastFilter = _queryFilter;
  } else {
    /* _hasExposure only */
    _queryFilter.flag = VISUAL_LOC; 
    FindWorld(_exposureRect.xLow, _exposureRect.yLow,
	      _exposureRect.xHigh, _exposureRect.yHigh,
	      _queryFilter.xLow, _queryFilter.yLow, 
	      _queryFilter.xHigh, _queryFilter.yHigh);
    
#ifdef DEBUG
    printf("exposure world %f,%f,%f,%f\n",
	   _queryFilter.xLow, _queryFilter.yLow,
	   _queryFilter.xHigh, _queryFilter.yHigh);
#endif
  }
  
  /* Send the new query */
  
  /* Set up transformation to draw the axes */
  winRep->PushTop();
  winRep->MakeIdentity();
  
  /* Draw axes */
  DrawAxesLabel(winRep, scrnX, scrnY, scrnWidth, scrnHeight);
  
  /* Draw highlight border */
  Boolean oldHighlight = _highlight;
  _highlight = false;
  Highlight(oldHighlight);

  /* push clip region using this transform */
  int dataX, dataY, dataW, dataH;
  GetDataArea(dataX, dataY, dataW, dataH);
  winRep->PushClip(dataX, dataY, dataW - 1, dataH - 1);
  
  /* pop the transform */
  winRep->PopTransform();
  
  /* blank out area to be drawn */
  winRep->SetFgColor(GetBgColor());
  winRep->FillRect(_queryFilter.xLow, _queryFilter.yLow,
		   _queryFilter.xHigh - _queryFilter.xLow,
		   _queryFilter.yHigh - _queryFilter.yLow);
  
  _hasExposure = false;
  _filterChanged = false;
  _refresh = false;

  if (mode == ControlPanel::DisplayMode) {
    _querySent = true;
    /* now sending query for events that occurred while in
       layout mode */
    _modeRefresh = false; 
    
    ControlPanel::Instance()->SetBusy();
    if (!_hasTimestamp)
      _timeStamp = TimeStamp::NextTimeStamp();
    _hasTimestamp = false;
    _bytes = 0;
    DerivedStartQuery(_queryFilter, _timeStamp);
  } else {
    winRep->PopClip();
  }

#ifdef DEBUG
  printf("after Run %d %d %d %d\n",
	 _hasExposure, _filterChanged, _refresh, _updateTransform);
#endif
}

void View::HandleResize(WindowRep *w, int xlow, int ylow,
			unsigned width, unsigned height)
{
#ifdef DEBUG
  printf("View::HandleResize(%d,%d,%d,%d)\n", xlow, ylow, width, height);
#endif
  
  /* base class should handle this, too */
  ViewWin::HandleResize(w, xlow, ylow, width, height);
  
  _updateTransform = true; /* need to update the transformation */
  
  /* XXX: abort current transaction if any */
}

void View::UpdateTransform(WindowRep *winRep)
{
#ifdef DEBUG
  printf("View::UpdateTransform\n");
#endif

  winRep->ClearTransformStack();
  Transform2D transform;
  CalcTransform(transform);
  winRep->PostMultiply(&transform);
}

/* Get label parameters */

void View::GetLabelParam(Boolean &occupyTop, int &extent, char *&name)
{
  occupyTop = _label.occupyTop;
  extent = _label.extent;
  name = _label.name;
}

/* Set label parameters */

void View::SetLabelParam(Boolean occupyTop, int extent, char *name)
{
  delete _label.name;

  if (name) {
    _label.name = new char [strlen(name) + 1];
    strcpy(_label.name, name);
  } else
    _label.name = 0;

  _label.occupyTop = occupyTop;
  _label.extent = extent;

  // when title is supposed to appear on the left side of the view,
  // it doesn't actually appear at all; therefore when there's no
  // title we force it to go to the left

  if (!_label.name)
    occupyTop = false;

  _updateTransform = true;
  _refresh = true;
}

void View::XAxisDisplayOnOff(Boolean stat)
{
  if (stat != xAxis.inUse) {
    xAxis.inUse = stat;
    _updateTransform = true;
    _refresh = true;
  }
}

void View::YAxisDisplayOnOff(Boolean stat)
{
  if (stat != yAxis.inUse) {
    yAxis.inUse = stat;
    _updateTransform  = true;
    _refresh = true;
  }
}

/* Find real window coords */

inline void FindRealCoord(WindowRep *winRep, Coord xlow, Coord ylow, 
			  Coord xhigh, Coord yhigh, int &txlow, int &tylow,
			  int &txhigh, int &tyhigh)
{
  Coord xl,yl, xh, yh;
  winRep->Transform(xlow, ylow, xl, yl);
  winRep->Transform(xhigh, yhigh, xh, yh);
  txlow = (int)MinMax::min(xl, xh);
  txhigh = (int)MinMax::max(xl, xh);
  tylow = (int)MinMax::min(yl, yh);
  tyhigh = (int)MinMax::max(yl,yh);
}

/* Update filter by scrolling.
Algorithm:
	If no exposure and can scroll, scroll the window. 
	Change _filterChanged into _hasExposure
Return:
	0: can not scroll.
	1: scroll was performed, and update the _exposureRect.
	2: can scroll, and no exposure left.
	true if filter was scrolled.
	false if not.
*/

View::UpdateFilterStat View::UpdateFilterWithScroll()
{
#ifdef DEBUG
  printf("UpdateFilterWithScroll: _refresh %d, _filterChanged %d, _hasExposure %d\n",
	 _refresh, _filterChanged, _hasExposure);
#endif
  
  WindowRep *winRep = GetWindowRep();
  Coord xlow, ylow, xhigh, yhigh;
  xlow = _filter.xLow;
  ylow = _filter.yLow;
  xhigh = _filter.xHigh;
  yhigh = _filter.yHigh;

  if ( /* make sure we can scroll */
      winRep->Scrollable() 
      /* and have current and last filter available */
      && _hasLastFilter && _filterChanged
      /* and we are not refreshing the window by redraw */
      && !_refresh
      /* And did not zoom */
      &&fabs((_filter.xHigh - _filter.xLow)-
	     (_lastFilter.xHigh-_lastFilter.xLow)) <= DELTA_X
      &&fabs((_filter.yHigh - _filter.yLow)-
	     (_lastFilter.yHigh-_lastFilter.yLow)) <= DELTA_Y
      /* and the two filters overlap */
      && Geom::RectRectClip(xlow, ylow, xhigh, yhigh,_lastFilter.xLow,
			    _lastFilter.yLow, _lastFilter.xHigh, _lastFilter.yHigh)
      
      /* and scrollling to the right or scroll left */
      &&(	(_lastFilter.yLow == _filter.yLow &&
		 _lastFilter.yHigh == _filter.yHigh )
	 ||	(_lastFilter.xLow == _filter.xLow &&
		 _lastFilter.xHigh == _filter.xHigh ))) {
    
    /* Find the real window coord of the overlap region */
    int fxl, fyl, fxh, fyh; /* transformed filter coord */
    int lxl, lyl, lxh, lyh; /* xformed last filter */
    int oxl, oyl, oxh, oyh; /* xformed overlap */
    FindRealCoord(winRep, _filter.xLow, _filter.yLow, 
		  _filter.xHigh, _filter.yHigh, fxl, fyl, fxh, fyh);
    FindRealCoord(winRep, _lastFilter.xLow, _lastFilter.yLow, 
		  _lastFilter.xHigh, _lastFilter.yHigh, lxl, lyl, lxh, lyh);
    FindRealCoord(winRep, xlow, ylow, xhigh, yhigh, oxl, oyl, oxh, oyh);
    int overlapWidth = oxh-oxl+1;
    int overlapHeight = oyh-oyl+1;
    
    int dataX, dataY, dataW, dataH;
    GetDataArea(dataX, dataY, dataW, dataH);

#ifdef DEBUG
    printf("data: x:%d, y:%d, w:%d, h:%d\n", dataX, dataY, dataW, dataH);
#endif

    winRep->ScrollAbsolute(oxl, oyl, overlapWidth, overlapHeight,
			oxl-fxl+lxl, oyl-fyl+lyl);
    
    /* Find exposed region for redraw */
    if (_filter.xLow > _lastFilter.xLow) {
      /* scrolling data left */
      _exposureRect.xLow = lxh-overlapWidth;
      _exposureRect.yLow = lyl;
      _exposureRect.xHigh = lxh;
      _exposureRect.yHigh = lyh;
    }
    else if (_filter.xLow < _lastFilter.xLow) {
      /* scrolling data right */
      _exposureRect.xLow = lxl;
      _exposureRect.yLow = lyl;
      _exposureRect.xHigh = lxl+overlapWidth;
      _exposureRect.yHigh = lyh;
    }
    else if (_filter.yLow > _lastFilter.yLow) {
      /* scroll data down */
      _exposureRect.xLow = lxl;
      _exposureRect.yLow = lyl;
      _exposureRect.xHigh = lxh;
      _exposureRect.yHigh = lyl+overlapHeight;
    }
    else if (_filter.yLow < _lastFilter.yLow) {
      /* scroll up */
      _exposureRect.xLow = lxl;
      _exposureRect.yLow = lyl+overlapHeight-1;
      _exposureRect.xHigh = lxh;
      _exposureRect.yHigh = lyh;
    }
    else {
      /* new filter same as old filter. */
      return SameFilter;
    }

#ifdef DEBUG
    printf("after scroll exposure (%d,%d),(%d,%d)\n",
	   _exposureRect.xLow, _exposureRect.yLow,
	   _exposureRect.xHigh, _exposureRect.yHigh);
#endif
    
    return Scrolled;
  }
  return NotScrolled;
}

void View::Refresh()
{
  _refresh = true;
}

void View::ReportViewCreated()
{
  if (_viewCallbackList == NULL)
    return;
  
  int index;
  for(index = _viewCallbackList->InitIterator(); 
      _viewCallbackList->More(index);) {
    ViewCallback *callBack = _viewCallbackList->Next(index);
    callBack->ViewCreated(this);
  }
  _viewCallbackList->DoneIterator(index);
}

void View::ReportViewDestroyed()
{
  if (_viewCallbackList == NULL)
    return;
  
  int index;
  for(index = _viewCallbackList->InitIterator(); 
      _viewCallbackList->More(index);) {
    ViewCallback *callBack = _viewCallbackList->Next(index);
    callBack->ViewDestroyed(this);
  }
  _viewCallbackList->DoneIterator(index);
}

void View::ReportFilterAboutToChange()
{
  if (_viewCallbackList == NULL)
    return;
  
  int index;
  for(index = _viewCallbackList->InitIterator(); 
      _viewCallbackList->More(index);) {
    ViewCallback *callBack = _viewCallbackList->Next(index);
    callBack->FilterAboutToChange(this);
  }
  _viewCallbackList->DoneIterator(index);
}

void View::ReportFilterChanged(VisualFilter &filter, int flushed)
{
  if (_viewCallbackList == NULL)
    return;
  
  int index;
  for(index = _viewCallbackList->InitIterator(); 
      _viewCallbackList->More(index);) {
    ViewCallback *callBack = _viewCallbackList->Next(index);
    callBack->FilterChanged(this, filter, flushed);
  }
  _viewCallbackList->DoneIterator(index);
}

void View::InsertViewCallback(ViewCallback *callBack)
{
  if (_viewCallbackList == NULL)
    _viewCallbackList = new ViewCallbackList;
  
  _viewCallbackList->Append(callBack);
}

void View::DeleteViewCallback(ViewCallback *callBack)
{
  if (_viewCallbackList == NULL) {
    fprintf(stderr,"View::DeleteViewCallback: empty list\n");
    Exit::DoExit(2);
  }
  _viewCallbackList->Delete(callBack);
}

void View::Iconify(Boolean iconified)
{
  if (_querySent && iconified) {
    DerivedAbortQuery();
    ReportQueryDone(0);
    _refresh = true;
  }
}

void View::ModeChange(ControlPanel::Mode mode)
{
#ifdef DEBUG
  printf("Change mode %d,%d,%d,%d\n",
	 _hasExposure,_filterChanged,_refresh,_updateTransform);
#endif

  if (mode == ControlPanel::LayoutMode && _querySent) {
#ifdef DEBUG
    printf("abort query from mode change\n");
#endif
    DerivedAbortQuery();
    ReportQueryDone(0);
    _refresh = true;
    _modeRefresh = true;
  }
}

void View::Highlight(Boolean flag)
{
#ifdef DEBUG
  printf("Highlight view %s %d\n", GetName(), flag);
#endif

  if (_highlight == flag)
    return;

  _highlight = flag;

  WindowRep *winRep = GetWindowRep();
  winRep->SetXorMode();
  DrawHighlight();
  winRep->SetCopyMode();
}

void View::DrawHighlight()
{
  if (!Mapped())
    return;

  WindowRep *winRep = GetWindowRep();

  int x,y;
  unsigned int w,h;
  
  if (_label.occupyTop) {
    Geometry(x, y, w, h);
    winRep->AbsoluteLine(x, y, w - 1, x, 3);
    winRep->AbsoluteLine(x + w - 1, y, x + w - 1, y + h - 1, 3);
    winRep->AbsoluteLine(x + w - 1, y + h - 1, x, y + h - 1, 3);
    winRep->AbsoluteLine(x, y + h - 1, x, y, 3);
  } else {
    int labelW, labelH;
    GetLabelArea(x, y, labelW, labelH);
    winRep->AbsoluteLine(x + _label.extent / 2, y + _label.extent, 
			 x + _label.extent / 2, labelH, _label.extent);
  }
}

void View::DrawLogo()
{
#ifdef DEBUG
  printf("DrawLogo\n");
#endif

  if (!Init::DisplayLogo())
    return;

  // logo gets displayed in small font (default: 5x7)
  char *logo = "DEVise v0.9";
  const int logoWidth = 5 * strlen(logo) + 2 - 1;
  const int logoHeight = 7 + 2 - 1;

  int x, y;
  unsigned int w, h;
  Geometry(x, y, w, h);

  w--;

  WindowRep *win = GetWindowRep();
  win->PushTop();
  win->MakeIdentity();

  win->SetSmallFont();
  win->SetFgColor(GetFgColor());
  win->FillRect(x + w - logoWidth, y, logoWidth, logoHeight);
  win->SetFgColor(GetBgColor());
  win->FillRect(x + w - logoWidth + 1, y + 1, logoWidth - 2, logoHeight - 2);
  win->SetFgColor(GetFgColor());
  win->AbsoluteText(logo, x + w - logoWidth + 1, y + 1, logoWidth - 2,
		    logoHeight - 2, WindowRep::AlignWest, true);
  win->SetNormalFont();

  win->PopTransform();
}

void View::SetXAxisAttrType(AttrType type)
{ 
  _xAxisAttrType = type;
}

void View::SetYAxisAttrType(AttrType type)
{
  _yAxisAttrType = type;
}

char *View::DispatchedName()
{
  return "View";
}

void View::SetAction(Action *action)
{ 
#ifdef DEBUG
  printf("SetAction 0x%p\n",action);
#endif
  
  _action = action; 
}

void View::AppendCursor(DeviseCursor *cursor)
{
  _cursors->Append(cursor);
}

void View::DeleteCursor(DeviseCursor *cursor)
{
  _cursors->Delete(cursor);
}

void View::GetXCursorArea(int &x, int &y, int &w, int &h)
{
  int startX;
  if (xAxis.inUse) {
    GetXAxisArea(x, y, w, h, startX);
    x = startX;
  } else {
    /* X axis not in use */
    GetDataArea(x, y, w, h);
    y += h - VIEW_CURSOR_SENSE;
    h = VIEW_CURSOR_SENSE;
  }

#ifdef DEBUG
  printf("GetXCursorArea  %s %d %d %d %d\n", GetName(), x, y, w, h);
#endif
}

void View::GetYCursorArea(int &x, int &y, int &w, int &h)
{
  if (yAxis.inUse) {
    GetYAxisArea(x, y, w, h);
  } else {
    GetDataArea(x, y, w, h);
    w = VIEW_CURSOR_SENSE;
  }

#ifdef DEBUG
  printf("GetYCursorArea  %s %d %d %d %d\n", GetName(), x, y, w, h);
#endif
}

Boolean View::DrawCursors()
{
#ifdef DEBUG
  printf("Drawcursors for %s\n", GetName());
#endif

  if (!Mapped()) {
#ifdef DEBUG
    printf("not mapped\n");
#endif
    return false;
  }

  if (!_cursorsOn) {
    DoDrawCursors();
    _cursorsOn = true;
    return false;
  }

  return true;
}

Boolean View::HideCursors()
{
#ifdef DEBUG
  printf("Hidecursors for %s\n", GetName());
#endif

  if (!Mapped()) {
#ifdef DEBUG
    printf("not mapped\n");
#endif
    return false;
  }

  if (_cursorsOn) {
    DoDrawCursors();
    _cursorsOn = false;
    return true;
  }

  return false;
}

void View::DoDrawCursors()
{
#ifdef DEBUG
  printf("DoDrawCursors\n");
#endif

  WindowRep *winRep = GetWindowRep();
  winRep->SetXorMode();
  
  int index;
  for(index = _cursors->InitIterator(); _cursors->More(index);) {
    Coord xLow, yLow, xHigh, yHigh;
    DeviseCursor *cursor = _cursors->Next(index);
    VisualFilter *filter;
    Color color;
    cursor->GetVisualFilter(filter, color);
#if 0
    // cursor drawn in xor mode, so foreground color won't matter
    // (and should not be set here)
    winRep->SetFgColor(color);
#endif

    if (filter->flag & VISUAL_X) {
#ifdef DEBUG
      printf("DoDrawCursors: Drawing X cursor in\n  %s\n", GetName());
#endif
      if(!(filter->xHigh < _filter.xLow || filter->xLow > _filter.xHigh)) {
	xLow = MinMax::max(_filter.xLow, filter->xLow);
	xHigh = MinMax::min(_filter.xHigh, filter->xHigh);
	winRep->FillRect(xLow, _filter.yLow, xHigh - xLow,
			 _filter.yHigh - _filter.yLow);
      }
    }

    if (filter->flag & VISUAL_Y) {
#ifdef DEBUG
      printf("DoDrawCursors: Drawing Y cursor in\n  %s\n", GetName());
#endif
      if(!(filter->yHigh < _filter.yLow || filter->yLow > _filter.yHigh)) {
	yLow = MinMax::max(_filter.yLow, filter->yLow);
	yHigh = MinMax::min(_filter.yHigh, filter->yHigh);
	winRep->FillRect(_filter.xLow, yLow,
			 _filter.xHigh - _filter.xLow, yHigh - yLow);
      }
    }
  }

  winRep->SetCopyMode();
  _cursors->DoneIterator(index);
}

void View::ClearHistory()
{
  _filterQueue->Clear();
}

void View::InsertHistory(VisualFilter &filter)
{
  (void)_filterQueue->Enqueue(filter, filter.marked);
}

/* Put current pixmap into buffer. bytes == # of bytes
   used to create the pixmap. */

void View::CachePixmap(int bytes)
{
}

/* Look into pixmap buffer for pixmap that we can use for drawing
   filter. Restore the pixmap, and return a new visual filter. */

View::PixmapStat View::RestorePixmap(VisualFilter filter,
				     VisualFilter &newFilter)
{
  int x, y;
  unsigned int width, height;
  Geometry(x, y, width, height);

#ifdef DEBUG
  if (_pixmap != NULL) {
    printf("RestorePixmap %s\n", GetName());
    printf("cur filter: %f,%f,%f,%f\n", filter.xLow, filter.xHigh,
	   filter.yLow, filter.yHigh);
    printf("pix filter: %f,%f,%f,%f\n",
	   _pixmap->filter.xLow, _pixmap->filter.xHigh,
	   _pixmap->filter.yLow, _pixmap->filter.yHigh);
    printf("cur width %d, height %d, pix %d, %d\n", width, height,
	   _pixmap->width, _pixmap->height);
  }
#endif

  if (Mapped() && _pixmap != NULL && 
      filter.xLow ==  _pixmap->filter.xLow &&
      filter.yLow == _pixmap->filter.yLow &&
      filter.xHigh == _pixmap->filter.xHigh &&
      filter.yHigh == _pixmap->filter.yHigh &&
      _pixmap->width == (int)width && _pixmap->height == (int)height) {
    GetWindowRep()->DisplayPixmap(_pixmap);
    return PixmapTotal;
  }

  return PixmapNone;
}

void View::InvalidatePixmaps()
{
  if (!_pixmap)
    return;

  free(_pixmap->data);
  delete _pixmap;
  _pixmap = NULL;
}

void View::SavePixmaps(FILE *file)
{
#ifdef DEBUG
  printf("SavePixmap at bytes %ld\n", ftell(file));
  printf("View SavePixmaps bytes %d\n", _bytes);
#endif

  // Store a known 4-byte integer at head of file to distinguish
  // the byte order (little/big endian) used on this machine; at
  // load time, check the value of the integer.

  unsigned long int magic = 0xdeadbeef;
  if (fwrite(&magic, sizeof magic, 1, file) != 1) {
    perror("View::SavePixmaps");
    Exit::DoExit(1);
  }
  
  /* Disable cursors if currently displayed */
  Boolean cursorState = HideCursors();
  Boolean dispSymbol = DisplaySymbols(true);
  Boolean dispConnector = DisplayConnectors(false);

#ifndef JPEG

  //
  // Save pixmap in run-length-encoded format
  //

  int saved = 0;
  DevisePixmap *pixmap = 0;
  if (Iconified()|| !Mapped() ||_refresh || _bytes < VIEW_BYTES_BEFORE_SAVE) {
    if (_pixmap != NULL) {
      /* save old pixmap */
      saved = 1;
      pixmap = _pixmap;
    } else
      saved = 0;
  } else {
    /* save current pixmap */
    saved = 1;
    WindowRep *winRep = GetWindowRep();
    pixmap = winRep->GetPixmap();
    if (!pixmap) {
      saved = 0;
    }
  }
  
  if (fwrite(&saved, sizeof(saved), 1, file) != 1) {
    perror("View::SavePixmaps");
    Exit::DoExit(1);
  }

  if (!saved) {
    /* Return cursors to original state */
    if (cursorState)
      (void)DrawCursors();
    (void)DisplaySymbols(dispSymbol);
    (void)DisplayConnectors(dispConnector);
    return;
  }
  
  assert(pixmap);

#ifdef DEBUG
  printf("Saving Pixmap for %s, data %d bytes\n", GetName(), _bytes);
#endif

  pixmap->dataBytes = _bytes; /* # of data bytes used to create pixmap*/
  pixmap->filter = _filter;
  if (GetXAxisAttrType() == DateAttr) {
    /* Convert to date and back to be consistent with control panel's
       date format. Otherwise, we might get round-off errors */
    (void)ParseFloatDate(DateString(pixmap->filter.xLow),
			 pixmap->filter.xLow);
    (void)ParseFloatDate(DateString(pixmap->filter.xHigh),
			 pixmap->filter.xHigh);
  }
  if (GetYAxisAttrType() == DateAttr) {
    (void)ParseFloatDate(DateString(pixmap->filter.yLow),
			 pixmap->filter.yLow);
    (void)ParseFloatDate(DateString(pixmap->filter.yHigh),
			 pixmap->filter.yHigh);
  }
  
#ifdef DEBUG
  printf("SavePixmap filter %f,%f,%f,%f\n",
	 pixmap->filter.xLow, pixmap->filter.xHigh,
	 pixmap->filter.yLow, pixmap->filter.yHigh);
#endif
		
  if (fwrite(pixmap, sizeof(*pixmap), 1, file) != 1) {
    perror("View::SavePixmaps 1");
    Exit::DoExit(1);
  }
  if (fwrite(pixmap->data, pixmap->compressedBytes, 1, file) != 1) {
    perror("View::SavePixmaps 2");
    Exit::DoExit(1);
  }

#else

  //
  // Save pixmap in JPEG format
  //

  int num[2] = {0, 0};
  int pos = ftell(file);
  if (!Mapped() || _bytes < 0 * VIEW_BYTES_BEFORE_SAVE || _refresh) {
    num[1] = pos + 2 * sizeof(int);
    if (fwrite(num, sizeof(num), 1, file) != 1) {
      perror("View::SavePixmaps");
      Exit::DoExit(1);
    }
    /* Return cursors to original state */
    if (cursorState)
      (void)DrawCursors();
    (void)DisplaySymbols(dispSymbol);
    (void)DisplayConnectors(dispConnector);
    return;
  }
  
  /* Save window pixmap */
  num[0] = 1;

#if 0
  if (fwrite(&num, sizeof(num), 1, file) != 1) {
    perror("View::SavePixmaps num");
    Exit::DoExit(1);
  }
#endif

  WindowRep *winRep = GetWindowRep();
  DevisePixmap *pixmap = winRep->GetPixmap();
  pixmap->dataBytes = _bytes;
  pixmap->filter = _filter;
  
#ifdef DEBUG
  printf("pixMap dataBytes %d, imageBytes %d, width %d, height %d, bpl %d, pad %d\n",
	 pixmap->dataBytes, pixmap->imageBytes, pixmap->width,
	 pixmap->height, pixmap->bytes_per_line, pixmap->padding);
#endif
  
#if 0
  if (fwrite(pixmap, sizeof(*pixmap), 1, file) != 1 ) {
    perror("View::SavePixmaps: pixmap");
    Exit::DoExit(1);
  }
#endif
  
#ifdef DEBUG
  printf("before compress, at %d\n", ftell(file));
#endif

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo,file);
  cinfo.image_width = pixmap->width;
  cinfo.image_height = pixmap->height;
  cinfo.input_components = pixmap->bytes_per_line/pixmap->width;
  cinfo.in_color_space = JCS_GRAYSCALE;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 100, TRUE);
  jpeg_start_compress(&cinfo, TRUE);
  
  unsigned char *data = (unsigned char *)pixmap->data;
  for(int i = 0; i < pixmap->height; i++) {
    row_pointer[0] = &data[i * pixmap->bytes_per_line];
    (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  
  int posNow = ftell(file);
  fseek(file, pos + sizeof(int), 0);
  fwrite(&posNow, sizeof(int), 1, file);
  fseek(file, posNow, 0);
  
#ifdef DEBUG
  printf("after compress, at %d\n", ftell(file));
#endif
#endif

  /* Return cursors to original state */
  if (cursorState)
    (void)DrawCursors();
  (void)DisplaySymbols(dispSymbol);
  (void)DisplayConnectors(dispConnector);
}

/* Restore pixmaps from an open file into pixmap buffer*/

void View::LoadPixmaps(FILE  *file)
{
  // The first 4-byte integer contains a known value. Check the value
  // to make sure we're reading data stored in the same endian order
  // as this machine is using.

  unsigned long int check;
  if (fread(&check, sizeof check, 1, file) != 1) {
    perror("View::LoadPixmaps");
    Exit::DoExit(1);
  }

  if (check != 0xdeadbeef) {
    printf("Note: Pixel image for %s not in compatible format.\n",
	   GetName());
    _pixmap = NULL;
    return;
  }
  
#ifndef JPEG

  //
  // Load image stored in run-length-encoded format
  //

  int saved;
  if (fread(&saved, sizeof(saved), 1, file) != 1 ) {
    perror("View::LoadPixmaps num");
    Exit::DoExit(1);
  }
  if (!saved) {
#ifdef DEBUG
    printf("View::LoadPixmaps: %s not saved\n", GetName());
#endif
    _pixmap = NULL;
    return;
  }

#ifdef DEBUG
  printf("Loading Pixmap for %s\n", GetName());
#endif

  DevisePixmap *pixmap = new DevisePixmap();
  if (fread(pixmap, sizeof(*pixmap), 1, file) != 1 ) {
    perror("View::LoadPixmaps 1");
    Exit::DoExit(1);
  }

#ifdef DEBUG
  printf("LoadPixmap filter %f,%f,%f,%f\n",
	 pixmap->filter.xLow, pixmap->filter.xHigh,
	 pixmap->filter.yLow, pixmap->filter.yHigh);
#endif
		
  if (!(pixmap->data = (unsigned char *)malloc(pixmap->compressedBytes))) {
    fprintf(stderr,"View::LoadPixmaps out of memory\n");
    Exit::DoExit(1);
  }
  
  if (fread(pixmap->data, pixmap->compressedBytes, 1, file) != 1 ) {
    perror("View::LoadPixmaps 2");
    Exit::DoExit(1);
  }
  _pixmap = pixmap;
  
#else

  //
  // Load image stored in JPEG format
  //

  fseek(file, _nextPos, 0);
  printf("Load pixmap at bytes %d\n", ftell(file));

  int num[2];
  if (fread(num, sizeof(num), 1, file) != 1) {
    perror("View::LoadPixmaps num");
    Exit::DoExit(1);
  }
  _nextPos = num[1];
  
  if (num[0] <= 0 )
    return;
  
  if (num[0] > 1) {
    fprintf(stderr, "View::LoadPixmaps: %d pixmaps > 1\n", num);
    Exit::DoExit(2);
  }
  
  _pixmap = new DevisePixmap();
  if (fread(_pixmap, sizeof(*_pixmap), 1, file) != 1) {
    perror("View::LoadPixmap");
    Exit::DoExit(1);
  }

#ifdef DEBUG
  printf("pixMap dataBytes %d, imageBytes %d, width %d, height %d, bpl %d pad %d\n",
	 _pixmap->dataBytes, _pixmap->imageBytes, _pixmap->width,
	 _pixmap->height, _pixmap->bytes_per_line, _pixmap->padding);
#endif
  
#ifdef DEBUG
  printf("before decompress, at %d\n", ftell(file));
#endif

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  
  jpeg_stdio_src(&cinfo,file);
  (void)jpeg_read_header(&cinfo,TRUE);
  jpeg_start_decompress(&cinfo);

#ifdef DEBUG
  printf("allocating %d bytes\n", _pixmap->imageBytes);
#endif

  unsigned char *data = (unsigned char *)malloc(_pixmap->imageBytes);
  if (!data) {
    fprintf(stderr,"View::LoadPixmap: no memory\n");
    Exit::DoExit(1);
  }

  _pixmap->data = data;
  unsigned char *buffer[1];
  for(int i = 0; i < _pixmap->height; i++) {
    buffer[0] = &data[i * _pixmap->bytes_per_line];
#ifdef DEBUG
    printf("reading line %d\n", i);
#endif
    jpeg_read_scanlines(&cinfo,buffer,1);
  }

  (void)jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

#ifdef DEBUG
  printf("after decompress, at %d\n", ftell(file));
#endif
#endif
}

/* Print view statistics */

void View::PrintStat()
{
  int total = _jump + _zoomIn + _zoomOut + _scrollLeft + _scrollRight
              + _unknown;
  printf("View %s: total operationss %d\n", GetName(), total);

  if (total <= 0)
    return;

  printf("  Jump %d, %.2f%%\n", _jump, 100.0 * _jump / total);
  printf("  ZoomIn %d, %.2f%%\n", _zoomIn, 100.0 * _zoomIn / total);
  printf("  ZoomOut %d, %.2f%%\n", _zoomOut, 100.0 * _zoomOut / total);
  printf("  ScrollLeft %d, %.2f%%\n", _scrollLeft,
	 100.0 * _scrollLeft / total);
  printf("  ScrollRight %d, %.2f%%\n", _scrollRight,
	 100.0 * _scrollRight / total);
  printf("  Unknown %d, %.2f%%\n", _unknown, 100.0 * _unknown / total);
}
