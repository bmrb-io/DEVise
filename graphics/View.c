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
#include "Util.h"
#include "View.h"
#include "Geom.h"
#include "Action.h"
#include "WindowRep.h"
#include "FilterQueue.h"
#include "Control.h"
#include "TimeStamp.h"
#include "Parse.h"
#ifdef JPEG
#include "Jpeg.h"
#endif

//#define DEBUG

int View::_nextPos = 0;

ViewCallbackList *View::_viewCallbackList;

const Coord DELTA_X = .000000000001;
const Coord DELTA_Y = .000000000001;

/* id for the next view created.
view == NULL return id = 0.*/
int View::_nextId = 0;
ViewList *View::_viewList = NULL;   /* list of all views */

/* subwindow */
View::View(char *name, Action *action, 
	   VisualFilter &initFilter, 
	   Color fg, Color bg,
	   AxisLabel *xAxis, AxisLabel *yAxis ,
	   int weight, Boolean boundary) :
	ViewWin(name,fg, bg,weight, boundary)
{
  _modeRefresh = false;
  _hasTimestamp = false;
  Init(name,action, initFilter, xAxis, yAxis);
  _filterQueue = new FilterQueue();
  ReportViewCreated();
  
  int flushed = _filterQueue->Enqueue(initFilter);
  ReportFilterAdded(initFilter, flushed);
  _highlight = false; 

  _xAxisAttrType = FloatAttr;
  _yAxisAttrType = FloatAttr;

  _pixmap = NULL;

  _pixmapIO = new PixmapIO();
  _compress = new SimpleCompress();
}

/* Init */

void View::Init(char *name,Action *action, VisualFilter &filter,
		AxisLabel *xAxisLabel, AxisLabel *yAxisLabel)
{
  _jump, _zoomIn, _zoomOut, _scrollLeft, _scrollRight = _unknown = 0;
	
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
  if (!_hasTimestamp){
    _hasTimestamp =true;
    _timeStamp = TimeStamp::NextTimeStamp();
  }
  _filter = filter;
  
  _hasExposure = false; /* no exposure rectangle yet */
  
  _querySent = false;	/* TRUE if query has been sent */
  
  xAxis.inUse = false;
  xAxis.color = BlackColor;
  xAxis.width = 20;
  xAxis.decimalPlaces = 1;
  xAxis.numTicks = 2;
  xAxis.useNumTicks = true;
  xAxis.fieldWidth = 10;
  
  yAxis.inUse = false;
  yAxis.width = 60;
  yAxis.decimalPlaces = 1;
  yAxis.numTicks = 2;
  yAxis.color = BlackColor;
  yAxis.useNumTicks = true;
  yAxis.fieldWidth = 10;
  
  _label.name = name;
  _label.occupyTop = false;
  _label.extent = 12;
  
  _axisDisplay = true;
  
  _cursors = new DeviseCursorList;
  
  Dispatcher::Current()->Register(this);
}

View::~View()
{
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
  for (index=_viewList->InitIterator(); _viewList->More(index); ){
    View *view = _viewList->Next(index);
    if (strcmp(view->GetName(),name)== 0){
      _viewList->DoneIterator(index);
      return view;
    }
  }
  _viewList->DoneIterator(index);
  
  fprintf(stderr,"View::FindViewByName: %s not found\n", name);
  return NULL; /* keep compiler happy */
}

/***************************************************************
Find view by id
****************************************************************/

View *View::FindViewById(int id)
{
  if (id == 0)
    return NULL;
  
  if (_viewList == NULL){
    fprintf(stderr,"View::FindViewById: _viewList empty\n");
    Exit::DoExit(1);
  }
  
  int index;
  for (index=_viewList->InitIterator(); _viewList->More(index); ){
    View *view = _viewList->Next(index);
    if (view->GetId() == id){
      _viewList->DoneIterator(index);
      return view;
    }
  }
  
  fprintf(stderr,"View::FindViewById: not found\n");
  Exit::DoExit(1);
	return NULL; /* keep compiler happy */
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
  if (_querySent){
    DerivedAbortQuery();
    ReportQueryDone(0);
  }
}

void View::SetVisualFilter(VisualFilter &filter)
{
#if 0
  printf("%s SetVisualFilter %f %f %f %f %f, %f, %f, %f\n", GetName(),
	 filter.xLow, filter.yLow,
	 filter.xHigh, filter.yHigh,_filter.xLow, _filter.yLow,
	 _filter.xHigh, _filter.yHigh);
#endif

#if 0
  if (strcmp("ams.grid8639.TOTAL_INCIDENT_RADTN.SortedX1",GetName())== 0){
    printf("ams.grid8639.TOTAL_INCIDENT_RADTN.SortedX1 movex %f %f\n",
	   filter.xLow, filter.xHigh);
  }
#endif

#if 0
  if (strcmp("ams.grid8639.TOTAL_INCIDENT_RADTN.SortedX1",GetName())== 0){
    printf("SoretedX1: movex %f %f, old %f %f\n",
	   filter.xLow, filter.xHigh, _filter.xLow, _filter.xHigh);
    if (filter.xLow > _filter.xHigh || filter.xHigh < _filter.xLow)
      printf("jump");
    else if (filter.xLow < _filter.xLow && filter.xHigh > _filter.xHigh)
      printf("zoomOut");
    else if (filter.xLow > _filter.xLow && filter.xHigh < _filter.xHigh)
      printf("zoomIn");
    else if (filter.xLow < _filter.xLow && filter.xHigh < _filter.xHigh)
      printf("scrollLeft");
    else if (filter.xLow > _filter.xLow && filter.xHigh > _filter.xHigh)
      printf("scrollRight");
    else printf("unknown");
    printf("\n");
  }
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
  else _unknown++;
  
  if (_filter.xLow != filter.xLow || _filter.xHigh != filter.xHigh
      || _filter.yLow != filter.yLow || _filter.yHigh != filter.yHigh){
    /* really update the filter */
    /*
       printf("filter changed\n");
       */
    _filterChanged = true;
    if (!_hasTimestamp){
      _timeStamp = TimeStamp::NextTimeStamp();
      _hasTimestamp = true;
    }
    _updateTransform = true;
    _filter = filter;
  }

  int flushed = _filterQueue->Enqueue(filter);
  ReportFilterAdded(filter, flushed);
}

void View::GetVisualFilter(VisualFilter &filter)
{
  filter = _filter;
}

VisualFilter *View::GetVisualFilter()
{
  return &_filter;
}

void View::Mark(int index, Boolean true_false)
{
  _filterQueue->Mark(index, true_false);
}

void View::HandleExpose(WindowRep * w, int x, int y, unsigned width, 
			unsigned height)
{
#ifdef DEBUG
  printf("View::HandleExpose(%d,%d,%d,%d)\n",x,y,width,height);
#endif

  if (!_hasExposure) {
    _hasExposure = true;
    _exposureRect.xLow= x;
    _exposureRect.yLow = y;
    _exposureRect.xHigh = x+width-1;
    _exposureRect.yHigh = y+height-1;

  } else {

    /* merge with existing exposed region */
    int minX1 = _exposureRect.xLow;
    int minY1 = _exposureRect.yLow;
    int maxX1 = _exposureRect.xHigh;
    int maxY1 = _exposureRect.yHigh;
    
    int minX2 = x;
    int minY2 = y;
    int maxX2 = x+width-1;
    int maxY2 = y+height-1;
    
    _exposureRect.xLow = MinMax::min(minX1,minX2);
    _exposureRect.yLow = MinMax::min(minY1,minY2);
    _exposureRect.xHigh = MinMax::max(maxX1,maxX2);
    _exposureRect.yHigh = MinMax::max(maxY1,maxY2);
  }
}

/* Handle button press event */

void View::HandlePress(WindowRep * w, int xlow,
		       int ylow, int xhigh, int yhigh, int button)
{
  ControlPanel::Instance()->SelectView(this);
	
  if (xlow == xhigh && ylow == yhigh){
    if (CheckCursorOp(w,xlow,ylow, button))
      /* was a cursor event */
      return;
  }
  
  if (_action != NULL){
    /* transform from screen to world coordinates */
    Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
    FindWorld(xlow,ylow,xhigh,yhigh,
	      worldXLow,worldYLow, worldXHigh, worldYHigh);
    
    _action->AreaSelected(this, worldXLow,worldYLow, worldXHigh, 
			  worldYHigh, button);
  }
}

/* handle key event */

void View::HandleKey(WindowRep *w ,char key, int x, int y)
{
  ControlPanel::Instance()->SelectView(this);
  if (_action != NULL){
    /* xform from screen to world coord */
    Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
    FindWorld(x,y,x,y,
	      worldXLow,worldYLow, worldXHigh, worldYHigh);
    
    _action->KeySelected(this,key,worldXLow, worldYLow);
  }
}

Boolean View::CheckCursorOp(WindowRep *win, int x, int y, int button)
{
  int cursorX, cursorY, cursorW, cursorH;
  Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
  GetXCursorArea(cursorX, cursorY, cursorW, cursorH);
  int cursorMaxX = cursorX+cursorW-1;
  int cursorMaxY = cursorY+cursorH-1;
  if (x >= cursorX && x <= cursorMaxX &&
      y >= cursorY && y <= cursorMaxY) {
    /* event for cursor area */
    if (_cursors->Size() > 0){
      /* change location of the X coord */
      FindWorld(x,y,x+1,y-1,
		worldXLow,worldYLow, worldXHigh, worldYHigh);
      DeviseCursor *cursor = _cursors->GetFirst();
      cursor->MoveSourceX((worldXHigh+worldXLow)/2.0);
    }
    return true;
  }
  else
    return false;
}

/* handle pop-up */

Boolean View::HandlePopUp(WindowRep *win, int x, int y, int button,
			  char **&msgs, int &numMsgs)
{
#ifdef DEBUG
  printf("PopUp, action = 0x%x\n",_action);
#endif

  ControlPanel::Instance()->SelectView(this);
  static char *buf[10];
  Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
  
  if (CheckCursorOp(win,x,y,button)){
    /* it was cursor event */
    return false;
  }

  if (x >= 0 && x <= _label.extent && y >= 0 && y <= _label.extent){
    buf[0] = GetName();
    msgs = buf;
    numMsgs = 1;
    return true;
  }
  
  if (_action != NULL) {
    /* transform from screen x/y into world x/y */
    FindWorld(x,y,x+1,y-1,
	      worldXLow,worldYLow, worldXHigh, worldYHigh);
    return _action->PopUp(this, worldXLow, worldYLow,
			  worldXHigh, worldYHigh, button,
			  msgs, numMsgs);
  }
  else
    return false;
}

/* get area for displaying label */

void View::GetLabelArea(int &x, int &y,int &width, int &height)
{
  unsigned int w,h;
  Geometry(x,y,w,h);
  x=0;
  y=0;
  if (_label.occupyTop) {
    width = w;
    height = _label.extent;
  }
  else {
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
  Geometry(x,y,windW,winH);
  x= 0;
  y = winH - xAxis.width ;
  width = windW ;
  height = xAxis.width ;
  
  if (yAxis.inUse)
    startX = x+ yAxis.width;
  else startX = x + 3*VIEW_CURSOR_WIDTH;
  
  if (!_label.occupyTop){
    /* make from for label */
    startX += _label.extent;
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
  Geometry(x,y,winW, winH);
  if (_label.occupyTop){
    x=0;
    y= _label.extent;
    height = winH - _label.extent;
    width = yAxis.width;
  }
  else {
    x = _label.extent;
    y = 0;
    width = yAxis.width;
    height = winH ;
  }
  
  if (xAxis.inUse)
    height -= xAxis.width;
  else height -= 3*VIEW_CURSOR_WIDTH;
  
  if (width <= 0 )
    width = 1;
  if (height <= 0)
    height = 1;
}

void View::GetDataArea(int &x, int &y, int &width,int &height)
{
  unsigned int winWidth, winHeight;
  Geometry(x,y,winWidth, winHeight);
  
  if (_label.occupyTop){
    /* _label occupies top of view */
    x = 0;
    y =  _label.extent; 
    width = winWidth;
    height = winHeight - _label.extent;
  } else {
    /* _label occupies left of view */
    x = _label.extent;
    y = 0;
    width = winWidth - _label.extent;
    height = winHeight;
  }
  
  if (_axisDisplay){
    /* need to display axes */
    if (xAxis.inUse)
      height -= xAxis.width;
    else {
      /* leave room for cursor */
      height -= 3*VIEW_CURSOR_HEIGHT;
    }
    
    if (yAxis.inUse) {
      x += yAxis.width;
      width -= yAxis.width;
    } else {
      /* room for cursor */
      x += VIEW_CURSOR_WIDTH*3;
      width -= 3*VIEW_CURSOR_WIDTH;
    }
  }
  
  if (width <= 0) width = 1;
  if (height <=0) height = 1;
  
#ifdef DEBUG
  printf("GetDataArea %s %d %d %d %d\n", GetName(), x, y, width, height);
#endif
}

void View::DrawAxesLabel(WindowRep *win,int x, int y, int w, int h)
{
#ifdef DEBUG
  printf("DrawAxesLabel %s %d %d %d %d\n", GetName(),x, y, w, h);
#endif

  int winX, winY; 
  unsigned int winW, winH;
  Geometry(winX, winY, winW, winH);
  
  /* Make it possible to update the label areas */
  win->PushTop();
  win->MakeIdentity();
  win->PushClip(0,0, winW, winH);
  win->PopTransform();
  
  DoDrawCursors(x,y,w,h);
  DrawLabel(win, x, y, w, h);
  if (_axisDisplay){
    if (xAxis.inUse)
      DrawXAxis(win, x, y, w, h);
    if (yAxis.inUse)
      DrawYAxis(win, x, y, w, h);
  }
  win->PopClip();
}

void View::DrawLabel(WindowRep *win, int x, int y, int w, int h)
{
  /* draw label */

  if (_label.occupyTop){
    int labelX,labelY,labelWidth,labelHeight;
    GetLabelArea(labelX,labelY,labelWidth,labelHeight);
    win->SetFgColor(win->GetBgColor());
    win->FillRect(labelX,labelY,labelWidth,labelHeight);
    win->SetFgColor(BlackColor);
    win->AbsoluteText(_label.name,labelX, labelY, labelWidth, labelHeight,
		      WindowRep::AlignWest,true);
  } else {
    int x, y, w, h;
    GetLabelArea(x, y, w, h);
    win->SetFgColor(HighlightColor);
    win->AbsoluteLine(x,y,x+_label.extent-1,y,1);
    win->AbsoluteLine(x+_label.extent-1,y,
		      x+_label.extent-1,y+_label.extent-1, 1);
    win->AbsoluteLine(x+_label.extent-1,y+_label.extent-1,
		      x, y+_label.extent-1,1);
    win->AbsoluteLine(x,y+_label.extent-1,x,y,1);
    win->AbsoluteLine(x,y,x+_label.extent-1,y+_label.extent-1,1);
    win->AbsoluteLine(x+_label.extent-1,y,x,y+_label.extent-1,1);
  }
}

void View::DrawXAxis(WindowRep *win,int x,int y,int w,int h)
{
#ifdef DEBUG
  printf("DrawXAxis %s %d %d %d %d\n",GetName(), x,y,w,h);
#endif
  
  int axisX, axisY, axisWidth, axisHeight, startX;
  GetXAxisArea(axisX,axisY,axisWidth,axisHeight, startX);
  int axisMaxX = axisX+axisWidth-1;
  int axisMaxY = axisY+axisHeight-1;
  char *label;
  char buf[30];
  int maxX = x+w-1;
  int maxY = y+h-1;
  if (!Geom::RectRectIntersect(x,y,maxX,maxY,axisX,axisY,axisMaxX, axisMaxY)) {
#ifdef DEBUG
    printf("do not intersect\n");
#endif
    return;
  }
  
  win->PushTop();
  win->MakeIdentity();
  
  Coord startWorldX = _filter.xLow;
  Coord endWorldX = _filter.xHigh;
  int numMajorTicks = xAxis.numTicks;
  Coord tickIncrement = (endWorldX-startWorldX)/numMajorTicks;
  
  if (xAxis.numTicks == 2) {
    /* draw the text */
    Coord drawWidth = axisWidth - (startX - axisX);

    if (_xAxisLabel != NULL)
      label= _xAxisLabel->GenerateLabel( _filter.xLow);
    else {
      if (_xAxisAttrType == DateAttr){
	time_t tm = (time_t)_filter.xLow;
	label = DateString(tm);
      }
      else {
	sprintf(buf, "%-*.*f", xAxis.fieldWidth,
		xAxis.decimalPlaces, _filter.xLow);
	label= buf;
      }
    }
    
    /* Don't need this because cursor routine already
       filled in the background.
       win->SetFgColor(win->GetBgColor());
       win->FillRect(startX,axisY,drawWidth/2,axisHeight);
    */

    win->SetFgColor(xAxis.color);
    win->AbsoluteText(label,startX, axisY, drawWidth/2, axisHeight,
		      WindowRep::AlignWest,false);
    
    if (_xAxisLabel != NULL)
      label= _xAxisLabel->GenerateLabel(_filter.xHigh);
    else {
      if (_xAxisAttrType == DateAttr){
	time_t tm = (time_t)_filter.xHigh;
	label = DateString(tm);
      }
      else {
	sprintf(buf,"%*.*f",xAxis.fieldWidth,xAxis.decimalPlaces,
		_filter.xHigh);
	label= buf;
      }
    }
    /* Don't need this because cursor routine already
       filled in the background.
       win->SetFgColor(win->GetBgColor());
       win->FillRect(startX+drawWidth/2,axisY,drawWidth/2,axisHeight);
    */

    win->SetFgColor(xAxis.color);
    win->AbsoluteText(label,startX+drawWidth/2, axisY, drawWidth/2,
		      axisHeight, WindowRep::AlignEast,false);
  }
  
  /* draw a line from startX to the end of the view */

  win->SetFgColor(xAxis.color);
  win->SetPattern(Pattern0);
  win->Line(startX,axisY,axisMaxX, axisY, 2.0);
  
  win->PopTransform();
}

void View::DrawYAxis(WindowRep *win,int x,int y,int w,int h)
{
  char buf[30];

  int axisX, axisY, axisWidth, axisHeight;
  Coord startY;
  GetYAxisArea(axisX,axisY, axisWidth, axisHeight);
  startY = axisY;
  int axisMaxX = axisX + axisWidth-1;
  int axisMaxY = axisY + axisHeight-1;

  Coord maxX = x+w; Coord maxY = y+h;
  if (!Geom::RectRectIntersect(x,y,maxX,maxY,
			       axisX,axisY, axisMaxX, axisMaxY))
    return;

  win->PushTop();
  win->MakeIdentity();

   char *label;
  
  if (yAxis.numTicks == 2){
    /* draw lower Y coord */
    Coord drawHeight = axisHeight - (startY - axisY);
    if (_yAxisLabel != NULL)
      label = _yAxisLabel->GenerateLabel(_filter.yLow);
    else {
      if (_yAxisAttrType == DateAttr){
	time_t tm = (time_t)_filter.yLow;
	label = DateString(tm);
      }
      else {
	sprintf(buf,"%*.*f",yAxis.fieldWidth, yAxis.decimalPlaces,
		_filter.yLow);
	label = buf;
      }
    }

    /*
       win->SetFgColor(win->GetBgColor());
       win->FillRect( axisX, startY+drawHeight/2, axisWidth,
       drawHeight/2);
    */

    win->SetFgColor(yAxis.color);
    win->AbsoluteText(label, axisX, startY+drawHeight/2, axisWidth,
		      drawHeight/2, WindowRep::AlignSouth, true);
    
    /* draw upper Y coord */
    if (_yAxisLabel != NULL)
      label = _yAxisLabel->GenerateLabel(_filter.yHigh);
    else {
      if (_yAxisAttrType == DateAttr){
	time_t tm = (time_t)_filter.yHigh;
	DateString(tm);
      }
      else {
	sprintf(buf,"%*.*f",yAxis.fieldWidth,
		yAxis.decimalPlaces,_filter.yHigh);
	label = buf;
      }
    }
    /*
       win->SetFgColor(win->GetBgColor());
       win->FillRect(axisX, startY, axisWidth,
       drawHeight/2);
    */

    win->SetFgColor(yAxis.color);
    win->AbsoluteText(label, axisX, startY, axisWidth,
		      drawHeight/2, WindowRep::AlignNorth, true);
  }

  /* draw a line from startY to the bottom of the view */

  win->SetFgColor(yAxis.color);
  win->SetPattern(Pattern0);
  win->Line(axisMaxX, startY, axisMaxX, axisMaxY, 2.0);
  
  win->PopTransform();
}

#if 0
void View::AxisDisplayOnOff(Boolean stat)
{
  if (stat != _axisDisplay){
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
  transform.InverseTransform(sx1,sy1,x1,y1);
  transform.InverseTransform(sx2,sy2,x2,y2);
  xLow = MinMax::min(x1,x2);
  xHigh = MinMax::max(x1,x2);
  yLow = MinMax::min(y1,y2);
  yHigh = MinMax::max(y1,y2);
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
  /*
     transform.Scale((Coord)dataWidth/(_filter.xHigh- _filter.xLow+1),
     (Coord)(dataHeight)/(_filter.yHigh-_filter.yLow+1));
  */
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
  GetWindowRep()->PopClip();
  _hasLastFilter = false;
  
#if 0
  int x, y, w, h;
  DrawAxesLabel(GetWindowRep(),x,y,w,h);
#endif

  ControlPanel::Instance()->SetIdle();
}

/***********************************************************************
Send a query, if there is one.  Abort existing query if necessary. 
XXX: need to crop exposure against _filter before sending query.
**********************************************************************/

void View::Run()
{
	ControlPanel::Mode mode = ControlPanel::Instance()->GetMode();
	/*
	if (mode == ControlPanel::LayoutMode)
		printf("layout mode ");
	else printf("disp mode ");
	printf("%d %d %d %d\n", _hasExposure,_filterChanged,_refresh,_updateTransform);
	*/
	if (mode ==  ControlPanel::LayoutMode && 
		(_hasExposure || _filterChanged || _refresh || _updateTransform)){ 
		/* keep track that events occurred while in LayoutMode*/
		_modeRefresh = true;
	}
	else if (mode == ControlPanel::DisplayMode && _modeRefresh){
		/* need to refresh in display mode because events occurred in
		LayoutMode */
		_refresh = true;
	}

	Boolean notMapped = !Mapped();
	Boolean iconified = Iconified();
	if (_querySent){
		if (_hasExposure || _filterChanged || _refresh || _updateTransform ||
			notMapped || iconified ) {
			/*
			printf("View:: aborting\n");
			*/
			DerivedAbortQuery();
			ReportQueryDone(0);
			_refresh = true;
		}
		else return;
	}

	if ( notMapped || iconified)
		/* don't do anything */
		return;


	WindowRep *winRep = GetWindowRep();
	int scrnX, scrnY, scrnWidth, scrnHeight;
	unsigned  int sW, sH;
	Geometry(scrnX, scrnY, sW, sH);
	scrnWidth = sW;
	scrnHeight = sH;
	Boolean scrolled = false;
	if (_hasExposure || _filterChanged || _refresh){
		VisualFilter newFilter;
		if (RestorePixmap(_filter,newFilter) == PixmapTotal) {
			if (_updateTransform){
				UpdateTransform(GetWindowRep());
				_updateTransform = false;
			}
			/* Set up transformation to draw the axes */
			winRep->PushTop();
			winRep->MakeIdentity();

			/* Draw axes */
			DrawAxesLabel(winRep, 0, 0, sW, sH);

			/* Draw highlight border */
			DrawHighlight();

			/* pop the transform */
			winRep->PopTransform();

			_hasExposure = false;
			_filterChanged = false;
			_refresh = false;
			_hasLastFilter = false;
			return;
		}

		/*
		printf("Run: window 0x%x scrollable is %d\n", 
			winRep, (winRep->Scrollable()?1:0));
		*/
		if (!_updateTransform && !_hasExposure && !_refresh && _filterChanged){
			/* Do scroll, if we can  */
			UpdateFilterStat stat;
			stat = UpdateFilterWithScroll();
			if (stat == Scrolled ){
				/* did scrolling. Can keep this filter */
				/* update so that _hasFilter is no longer true,
				and _hasExposure is true. */
				_hasExposure = true;
				_filterChanged = false;

				_hasLastFilter = true;
				_lastFilter = _filter;
			}
			else if (stat == SameFilter) {
				/* A sequence of scroll/zoom between updates
				made the latest filter the same as the one being displayed */
				_filterChanged = false;
				_hasLastFilter = true;
				_lastFilter = _filter;
				return;
			}
		}

		/* Update the WindowRep's transformation matrix */
		if (_updateTransform){
			UpdateTransform(winRep);
			_updateTransform = false;
		}

		if (_hasExposure){
			/* limit exposure to the size of the window */
			/*
			printf("exposure (%d,%d),(%d,%d) ",
				_exposureRect.xLow,_exposureRect.yLow,
				_exposureRect.xHigh,_exposureRect.yHigh);
			*/
			
			_exposureRect.xLow=MinMax::max(_exposureRect.xLow,0);
			_exposureRect.xLow= MinMax::min(_exposureRect.xLow,scrnWidth-1);
			_exposureRect.xHigh=MinMax::max(_exposureRect.xLow,
							_exposureRect.xHigh);
			_exposureRect.xHigh= MinMax::min(_exposureRect.xHigh,scrnWidth-1);

			_exposureRect.yLow=MinMax::max(_exposureRect.yLow,0);
			_exposureRect.yLow= MinMax::min(_exposureRect.yLow,scrnHeight-1);
			_exposureRect.yHigh=MinMax::max(_exposureRect.yLow,
							_exposureRect.yHigh);
			_exposureRect.yHigh= MinMax::min(_exposureRect.yHigh,scrnHeight-1);

			/*
			printf("-->(%d,%d),(%d,%d)\n",
				_exposureRect.xLow,_exposureRect.yLow,
				_exposureRect.xHigh,_exposureRect.yHigh);
			*/
			
		}
		
		if (_filterChanged || _refresh){
			/* Need to redraw the whole screen */
			_queryFilter = _filter;

			_hasLastFilter = true;
			_lastFilter = _queryFilter;
		} else {
			/* _hasExposure only */
			_queryFilter.flag = VISUAL_LOC; 
			Coord xLow, yLow, xHigh, yHigh;
			FindWorld(_exposureRect.xLow, _exposureRect.yLow,
				_exposureRect.xHigh, _exposureRect.yHigh,
				_queryFilter.xLow, _queryFilter.yLow, 
				_queryFilter.xHigh, _queryFilter.yHigh);

			/*
			printf("exposure world %f,%f,%f,%f\n",
				_queryFilter.xLow,_queryFilter.yLow,
				_queryFilter.xHigh, _queryFilter.yHigh);
			*/
		}


		/* Send the new query */


		/* Set up transformation to draw the axes */
		winRep->PushTop();
		winRep->MakeIdentity();

		/* Draw axes */
		DrawAxesLabel(winRep, 0, 0, scrnWidth, scrnHeight);

		/* Draw highlight border */
		DrawHighlight();

		/* push clip region using this transform */
		int dataX, dataY, dataW, dataH;
		GetDataArea(dataX, dataY, dataW, dataH);
		winRep->PushClip(dataX, dataY, dataW, dataH);

		/* pop the transform */
		winRep->PopTransform();

		/*
		Coord tx, ty;
		winRep->Transform(100.0,100.0, tx,ty);
		printf("scrn w/h=%d,%d after postmultiply 100,100--> %f,%f\n",
			scrnWidth, scrnHeight, tx,ty);
		*/

		/* blank out area to be drawn */
		winRep->SetFgColor(winRep->GetBgColor());
		winRep->FillRect(_queryFilter.xLow,_queryFilter.yLow,
			_queryFilter.xHigh-_queryFilter.xLow+1,
			_queryFilter.yHigh-_queryFilter.yLow+1);

		_hasExposure = false;
		_filterChanged = false;
		_refresh = false;
		if (mode == ControlPanel::DisplayMode){
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
		}
		else {
			winRep->PopClip();
		}
	}
	/*
	printf("after Run %d %d %d %d\n",
			_hasExposure,_filterChanged,_refresh,_updateTransform);
	*/
}

void View::HandleResize(WindowRep * w, int xlow,
			int ylow, unsigned width, unsigned height)
{
#ifdef DEBUG
  printf("View::HandleResize(%d,%d,%d,%d)\n",xlow,ylow,width,height);
#endif
  
  /* base class should handle this, too */
  ViewWin::HandleResize(w,xlow,ylow,width,height);
  
  _updateTransform = true; /* need to update the transformation */
  
  /* new exposure */
  /*
     _hasExposure = true;
     _exposureRect.xLow = 0;
     _exposureRect.yLow =0;
     _exposureRect.xHigh = width-1;
     _exposureRect.yHigh = height-1;
     */
  
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

/* Set label parameters */

void View::SetLabelParam(Boolean occupyTop, int extent, char *name)
{
  if (name != NULL)
    _label.name = name;
  _label.occupyTop = occupyTop;
  _label.extent = extent;
  _updateTransform = true;
}

void View::XAxisDisplayOnOff(Boolean stat)
{
  if (stat != xAxis.inUse){
    xAxis.inUse = stat;
    _updateTransform = true;
    _refresh = true;
  }
}
void View::YAxisDisplayOnOff(Boolean stat)
{
  if (stat != yAxis.inUse){
    yAxis.inUse = stat;
    _updateTransform  = true;
    _refresh = true;
  }
}

/* Find real window coords */

inline FindRealCoord(WindowRep *winRep, Coord xlow, Coord ylow, 
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
		 _lastFilter.xHigh == _filter.xHigh ))){
    
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
    Coord exposeXLow, exposeYLow, exposeWidth, exposeHeight;
    if (_filter.xLow > _lastFilter.xLow){
      /* scrolling data left */
      _exposureRect.xLow = lxh-overlapWidth;
      _exposureRect.yLow = lyl;
      _exposureRect.xHigh = lxh;
      _exposureRect.yHigh = lyh;
    }
    else if (_filter.xLow < _lastFilter.xLow){
      /* scrolling data right */
      _exposureRect.xLow = lxl;
      _exposureRect.yLow = lyl;
      _exposureRect.xHigh = lxl+overlapWidth;
      _exposureRect.yHigh = lyh;
    }
    else if (_filter.yLow > _lastFilter.yLow){
      /* scroll data down */
      _exposureRect.xLow = lxl;
      _exposureRect.yLow = lyl;
      _exposureRect.xHigh = lxh;
      _exposureRect.yHigh = lyl+overlapHeight;
    }
    else if (_filter.yLow < _lastFilter.yLow){
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
  for (index = _viewCallbackList->InitIterator(); 
       _viewCallbackList->More(index); ) {
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
  for (index = _viewCallbackList->InitIterator(); 
       _viewCallbackList->More(index); ) {
    ViewCallback *callBack = _viewCallbackList->Next(index);
    callBack->ViewDestroyed(this);
  }
  _viewCallbackList->DoneIterator(index);
}

void View::ReportFilterAdded(VisualFilter &filter, int flushed)
{
  if (_viewCallbackList == NULL)
    return;
  
  int index;
  for (index = _viewCallbackList->InitIterator(); 
       _viewCallbackList->More(index); ) {
    ViewCallback *callBack = _viewCallbackList->Next(index);
    callBack->FilterChanged(this, filter, flushed);
  }
  _viewCallbackList->DoneIterator(index);
}

/* Insert callback */

void View::InsertViewCallback(ViewCallback *callBack)
{
  if (_viewCallbackList == NULL)
    _viewCallbackList = new ViewCallbackList;
  
  _viewCallbackList->Append(callBack);
}

void View::DeleteViewCallback(ViewCallback *callBack)
{
  if (_viewCallbackList == NULL){
    fprintf(stderr,"View::DeleteViewCallback: empty list\n");
    Exit::DoExit(2);
  }
  _viewCallbackList->Delete(callBack);
}

void View::Iconify(Boolean iconified)
{
  if (_querySent && iconified){
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

  if (mode == ControlPanel::LayoutMode && _querySent){
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
  printf("Highlight view %s %d\n",GetName(),flag);
#endif

  if (_highlight != flag){
    _highlight = flag;
    DrawHighlight();
  }
}

void View::DrawHighlight()
{
  if (!Mapped())
    return;
  WindowRep *winRep = GetWindowRep();
  if (_highlight){
    winRep->SetFgColor(HighlightColor);
  }
  else winRep->SetFgColor(ViewWin::GetBgColor());
  int x,y;
  unsigned int w,h;
  
  if (_label.occupyTop){
    Geometry(x,y,w,h);
    winRep->AbsoluteLine(0,0,w-1,0,3);
    winRep->AbsoluteLine(w-1,0,w-1,h-1,3);
    winRep->AbsoluteLine(w-1,h-1,0,h-1,3);
    winRep->AbsoluteLine(0,h-1,0,0,3);
  } else {
    int labelW, labelH;
    GetLabelArea(x, y,labelW, labelH);
    winRep->AbsoluteLine(x+_label.extent/2,y+_label.extent, 
			 x+_label.extent/2,labelH,_label.extent);
  }
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
  printf("SetAction 0x%x\n",action);
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
  if (xAxis.inUse){
    GetXAxisArea(x,y,w,h,startX);
  }
  else {
    /* X axis not in use */
    GetDataArea(x,y,w,h);
    y = y+h;
    h = VIEW_CURSOR_HEIGHT*3;
  }
}

void View::GetYCursorArea(int &x, int &y, int &w, int &h)
{
  if (yAxis.inUse){
    GetYAxisArea(x,y,w,h);
  } else {
    GetDataArea(x,y,w,h);
    x -= VIEW_CURSOR_WIDTH*3;
    w = VIEW_CURSOR_WIDTH*3;
  }

#ifdef DEBUG
  printf("GetYCursorArea  %s %d %d %d %d\n",GetName(), x, y, w, h);
#endif
}

void View::DrawCursors()
{
#ifdef DEBUG
  printf("Drawcursors for %s\n", GetName());
#endif

  if (!Mapped()){
#ifdef DEBUG
    printf("not mapped\n");
#endif
    return;
  }
  int x,y;
  unsigned int w,h;
  Geometry(x,y,w,h);
  DrawAxesLabel(GetWindowRep(),0,0,w,h);
}

void View::DoDrawCursors(int drawX, int drawY, int drawW,int drawH)
{
  WindowRep *winRep = GetWindowRep();
  
  int index;
  int x,y,w,h;
  GetXCursorArea(x,y,w,h);

#ifdef DEBUG
  printf("DoDrawCursors XCursorArea (%d,%d), %d %d\n", x,y,w,h);
#endif

  int yCursorX, yCursorY, yCursorW, yCursorH;
  GetYCursorArea(yCursorX,yCursorY,yCursorW,yCursorH);

#ifdef DEBUG
  printf("YCursorArea (%d,%d), %d %d\n", yCursorX,yCursorY,yCursorW,yCursorH);
#endif

  Boolean doXCursor = false;
  if (Geom::RectRectIntersect(drawX, drawY, drawX+drawW-1,drawY+drawH-1,
			      x,y,x+w-1,y+h-1))
    doXCursor = true;
  
  Boolean doYCursor = false;
  if (Geom::RectRectIntersect(drawX, drawY, drawX+drawW-1,drawY+drawH-1,
			      yCursorX,yCursorY,yCursorX+yCursorW-1,
			      yCursorY+yCursorH-1))
    doYCursor = true;
  
  if (!doXCursor && !doYCursor)
    return;
  
  Point points[4];
  
  if (doXCursor){
    /* Clear X rectangle */
    points[0].x = x; points[0].y = y;
    points[1].x = x+w; points[1].y = y;
    points[2].x = x+w; points[2].y = y+h;
    points[3].x = x; points[3].y = y+h;
    winRep->SetFgColor(GetBgColor());
    winRep->FillPixelPoly(points,4);
  }
  
  if ( doYCursor){
    points[0].x = yCursorX; points[0].y = yCursorY;
    points[1].x = yCursorX+yCursorW; points[1].y = yCursorY;
    points[2].x = yCursorX+yCursorW; points[2].y = yCursorY+yCursorH;
    points[3].x = yCursorX; points[3].y = yCursorY+yCursorH;
    winRep->SetFgColor(GetBgColor());
    winRep->FillPixelPoly(points,4);
  }
  
  Transform2D transform;
  Coord xl, yl, xh, yh;
  Coord xLow, yLow, xHigh, yHigh;
  CalcTransform(transform);
  int xCursorYCoord = y +h - VIEW_CURSOR_HEIGHT; /* y Coord of X cursor */
  int yCursorXCoord = yCursorX + VIEW_CURSOR_WIDTH ; /* x Coord of y cursor*/
  for (index = _cursors->InitIterator(); _cursors->More(index); ){
    DeviseCursor *cursor = _cursors->Next(index);
    VisualFilter *filter;
    Color color;
    cursor->GetVisualFilter(filter, color);
    winRep->SetFgColor(color);

    /* calculate where to draw the line */
    if (doXCursor && filter->flag & VISUAL_X ){
      if(!(filter->xHigh < _filter.xLow || filter->xLow > _filter.xHigh)){
	xLow = MinMax::max(_filter.xLow, filter->xLow);
	xHigh = MinMax::min(_filter.xHigh, filter->xHigh);
	
	transform.Transform(xLow,_filter.yLow, xl, yl);
	transform.Transform(xHigh,_filter.yLow, xh, yh);
	if (xh <= xl) xh = xl+1;
	winRep->AbsoluteLine((int)xl,xCursorYCoord, (int)xh,
			     xCursorYCoord, VIEW_CURSOR_HEIGHT);
	winRep->AbsoluteLine((int)xl,xCursorYCoord, (int)xl,
			     xCursorYCoord-4, VIEW_CURSOR_HEIGHT);
	winRep->AbsoluteLine((int)xh,xCursorYCoord, (int)xh,
			     xCursorYCoord-4, VIEW_CURSOR_HEIGHT);
	
      }
    }

    if (doYCursor && filter->flag & VISUAL_Y){
      if(!(filter->yHigh < _filter.yLow || filter->yLow > _filter.yHigh)){
	yLow = MinMax::max(_filter.yLow, filter->yLow);
	yHigh = MinMax::min(_filter.yHigh, filter->yHigh);
	
	transform.Transform(_filter.xLow,yLow, xl, yl);
	transform.Transform(_filter.xLow,yHigh, xh, yh);

#ifdef DEBUG
	printf("Draw y cursor %d %d %d %d\n",
	       yCursorXCoord, (int)yl, yCursorXCoord, (int)yh);
#endif

	winRep->AbsoluteLine(yCursorXCoord, (int)yl, 
			     yCursorXCoord, (int)yh, VIEW_CURSOR_WIDTH);
	winRep->AbsoluteLine(yCursorXCoord, (int)yl, 
			     yCursorXCoord+5, (int)yl, VIEW_CURSOR_WIDTH);
	winRep->AbsoluteLine(yCursorXCoord, (int)yh, 
			     yCursorXCoord+5, (int)yh, VIEW_CURSOR_WIDTH);
      }
    }
  }
  _cursors->DoneIterator(index);

#if 0
  winRep->Flush();
#endif
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
   filter.  Restore the pixmap, and return a new visual filter. */

View::PixmapStat View::RestorePixmap(VisualFilter filter,
				     VisualFilter &newFilter)
{
  int x, y;
  unsigned int width,height;
  Geometry(x,y,width,height);

#if 0
  if (_pixmap != NULL) {
    printf("RestorePixmap %s\n",GetName());
    printf("cur filter: %f,%f %f,%f\n",filter.xLow, filter.xHigh, filter.yLow,
	   filter.yHigh);
    printf("pix filter: %f,%f %f,%f\n",_pixmap->filter.xLow, _pixmap->filter.xHigh,
	   _pixmap->filter.yLow, _pixmap->filter.yHigh);
  }
#endif

  if (Mapped() && _pixmap != NULL && 
      filter.xLow ==  _pixmap->filter.xLow &&
      filter.yLow == _pixmap->filter.yLow &&
      filter.xHigh == _pixmap->filter.xHigh &&
      filter.yHigh == _pixmap->filter.yHigh &&
      _pixmap->width == width && _pixmap->height == height){
    GetWindowRep()->DisplayPixmap(_pixmap);
    return PixmapTotal;
  }
  else {
    return PixmapNone;
  }
}

void View::InvalidatePixmaps()
{
  if (_pixmap != NULL){
    free(_pixmap->data);
    delete _pixmap;
    _pixmap = NULL;
  }
}

static void PrintLine(unsigned char *line, int len)
{
  for (int i=0; i < len ; i++){
    printf("%x ", line[i]);
  }
  printf("\n");
}

void View::SavePixmaps(FILE *file)
{
#ifdef DEBUG
  printf("SavePixmap at bytes %d\n", ftell(file));
  printf("View SavePixmaps bytes %d\n",_bytes);
#endif

#ifndef JPEG
  int saved = 0;
  DevisePixmap *pixmap;
  if (Iconified()|| !Mapped() ||_refresh ||_bytes < VIEW_BYTES_BEFORE_SAVE){
    if (_pixmap != NULL){
      /* save old pixmap */
      saved = 1;
      pixmap = _pixmap;
    }
    else saved = 0;
  }
  else {
    /* save current pixmap */
    saved = 1;
    WindowRep *winRep = GetWindowRep();
    pixmap = winRep->GetPixmap();
    if (pixmap == NULL) {
      saved = 0;
    }
  }
  
  if (fwrite(&saved,sizeof(saved),1,file) != 1){
    perror("View::SavePixmaps");
    Exit::DoExit(1);
  }
  if (!saved)
    return;
  
#ifdef DEBUG
  printf("Saving Pixmap for %s, data %d bytes\n", GetName(), _bytes);
#endif

  pixmap->dataBytes = _bytes; /* # of data bytes used to create pixmap*/
  pixmap->filter = _filter;
  if (GetXAxisAttrType() == DateAttr){
    /* Convert to date and back to be consistent with control panel's
       date format. Otherwise, we might get round-off errors */
    (void)ParseFloatDate(DateString(pixmap->filter.xLow),
			 pixmap->filter.xLow);
    (void)ParseFloatDate(DateString(pixmap->filter.xHigh),
			 pixmap->filter.xHigh);
  }
  if (GetYAxisAttrType() == DateAttr){
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
		
  if (fwrite(pixmap,sizeof(*pixmap),1,file) != 1){
    perror("View::SavePixmaps 1");
    Exit::DoExit(1);
  }
  if (fwrite(pixmap->data,pixmap->compressedBytes,1,file) != 1){
    perror("View::SavePixmaps 2");
    Exit::DoExit(1);
  }

#else

  int num[2] = {0,0};
  int pos = ftell(file);
  if (!Mapped() || /*_bytes < VIEW_BYTES_BEFORE_SAVE ||*/ _refresh){
    num[1] = pos+2*sizeof(int);
    if (fwrite(num, sizeof(num), 1, file) != 1){
      perror("View::SavePixmaps");
      Exit::DoExit(1);
    }
    return;
  }
  
  /* Save window pixmap */
  num[0] = 1;
  /*
     if (fwrite(&num, sizeof(num), 1, file) != 1){
     perror("View::SavePixmaps num");
     Exit::DoExit(1);
     }
     */
  WindowRep *winRep = GetWindowRep();
  DevisePixmap *pixmap = winRep->GetPixmap();
  pixmap->dataBytes = _bytes;
  pixmap->filter = _filter;
  
  printf("pixMap dataBytes %d, imageBytes %d, width %d, height %d, bpl %d, pad %d\n",
	 pixmap->dataBytes, pixmap->imageBytes, pixmap->width,
	 pixmap->height, pixmap->bytes_per_line, pixmap->padding);
  fwrite(pixmap->data,sizeof(char),pixmap->imageBytes,file);
  return;
  
#if 0
  if (fwrite(pixmap,sizeof(*pixmap),1,file) != 1 ) {
    perror("View::SavePixmaps: pixmap");
    Exit::DoExit(1);
  }
#endif
  
  printf("before compress, at %d\n", ftell(file));

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
  int i;
  for (i=0; i < pixmap->height; i++){
    row_pointer[0] = &data[i*pixmap->bytes_per_line];
    (void)jpeg_write_scanlines(&cinfo,row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  
  int posNow = ftell(file);
  fseek(file, pos+sizeof(int), 0);
  fwrite(&posNow, sizeof(int),1, file);
  fseek(file,posNow,0);
  
  printf("after compress, at %d\n", ftell(file));
#endif

}

/* Restore pixmaps from an open file into pixmap buffer*/
void View::LoadPixmaps(FILE  *file)
{
#ifndef JPEG
  int saved;
  if (fread(&saved,sizeof(saved),1,file) != 1 ){
    perror("View::LoadPixmaps num");
    Exit::DoExit(1);
  }
  if (!saved){
#ifdef DEBUG
    printf("View::LoadPixmaps: not saved\n");
#endif
    _pixmap = NULL;
    return;
  }

#ifdef DEBUG
  printf("Loading Pixmap for %s\n", GetName());
#endif

  DevisePixmap *pixmap = new DevisePixmap();
  if (fread(pixmap,sizeof(*pixmap),1,file) != 1 ){
    perror("View::LoadPixmaps 1");
    Exit::DoExit(1);
  }
  if ((pixmap->data = (unsigned char *)malloc(pixmap->compressedBytes)) 
      == NULL){
    fprintf(stderr,"View::LoadPixmaps out of memory\n");
    Exit::DoExit(1);
  }
  
  if (fread(pixmap->data,pixmap->compressedBytes,1,file) != 1 ){
    perror("View::LoadPixmaps 2");
    Exit::DoExit(1);
  }
  _pixmap = pixmap;
  
#else

  fseek(file, _nextPos, 0);
  printf("Load pixmap at bytes %d\n", ftell(file));

  int num[2];
  if (fread(num, sizeof(num), 1, file) != 1){
    perror("View::LoadPixmaps num");
    Exit::DoExit(1);
  }
  _nextPos = num[1];
  
  if (num[0] <= 0 )
    return;
  
  if (num[0] > 1){
    fprintf(stderr,"View::LoadPixmaps: %d pixmaps>1\n",num);
    Exit::DoExit(2);
  }
  
  _pixmap = new DevisePixmap();
  if (fread(_pixmap,sizeof(*_pixmap),1,file) != 1){
    perror("View::LoadPixmap");
    Exit::DoExit(1);
  }
  printf("pixMap dataBytes %d, imageBytes %d, width %d, height %d, bpl %d pad %d\n",
	 _pixmap->dataBytes, _pixmap->imageBytes, _pixmap->width,
	 _pixmap->height, _pixmap->bytes_per_line, _pixmap->padding);
  
  printf("before decompress, at %d\n", ftell(file));
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  
  jpeg_stdio_src(&cinfo,file);
  (void)jpeg_read_header(&cinfo,TRUE);
  jpeg_start_decompress(&cinfo);
  printf("allocating %d bytes\n", _pixmap->imageBytes);
  unsigned char *data = (unsigned char *)
    malloc(_pixmap->imageBytes);
  if (data == NULL){
    fprintf(stderr,"View::LoadPixmap: no memory\n");
    Exit::DoExit(1);
  }
  _pixmap->data = data;
  
  unsigned char *buffer[1];
  int i;
  for (i=0; i < _pixmap->height; i++){
    buffer[0] = &data[i*_pixmap->bytes_per_line];
    /*
       printf("reading line %d\n", i);
       */
    jpeg_read_scanlines(&cinfo,buffer,1);
  }
  (void)jpeg_finish_decompress(&cinfo);
  
  jpeg_destroy_decompress(&cinfo);

  printf("after decompress, at %d\n", ftell(file));
#endif
}

/* Print view statistics */
void View::PrintStat()
{
  int total =  _jump+ _zoomIn+ _zoomOut+ _scrollLeft+ _scrollRight+ _unknown;
  printf("View %s: total ops %d\n", GetName(),total);
  if (total > 0){
    printf("jump %d, %.2f%%\n",_jump, 
	   (float)_jump/(float)total*100.0);
    printf("zoomIn %d, %.2f%%\n",_zoomIn, 
	   (float)_zoomIn/(float)total*100.0);
    printf("zoomOut %d, %.2f%%\n",_zoomOut,
	   (float)_zoomOut/(float)total*100.0);
    printf("ScrollLeft %d, %.2f%%\n",_scrollLeft, 
	   (float)_scrollLeft/(float)total*100.0);
    printf("scrollRight %d, %.2f%%\n",_scrollRight, 
	   (float)_scrollRight/(float)total*100.0);
    printf("unknown %d, %.2f%%\n",_unknown, 
	   (float)_unknown/(float)total*100.0);
  }
}
