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
  Revision 1.4  1995/09/14 20:49:06  jussi
  Added missing semicolon.

  Revision 1.3  1995/09/14 20:32:35  jussi
  Added missing return statement to SetFormat.

  Revision 1.2  1995/09/05 21:13:17  jussi
  Added/update CVS header.
*/

#ifndef View_h
#define View_h
#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "DList.h"
#include "VisualArg.h"
#include "ViewWin.h"
#include "Control.h"
#include "ViewCallback.h"
#include "Color.h"
#include "AttrList.h"
#include "Cursor.h"

class View;
DefinePtrDList(ViewList, View *)

/* rectangle to store screen coordinates */
struct ViewRect {
	int xLow, yLow, xHigh, yHigh;
};

/* Use to generate the label for an axis */
class AxisLabel {
public:
	AxisLabel(char *name) {_name = name; }
	char *GetName() { return _name; }
	virtual char *GenerateLabel(Coord val)= 0;
	virtual char *SetFormat(char *format){return "";};
	virtual char *GetFormat(){return "";}
private:
	char *_name;
};

/* function that generates an axis label */
class GenAxisLabel{
public:
	virtual AxisLabel *MakeAxisLabel(char *name)=0;
};

struct AxisInfo {
	Boolean inUse;          /* TRUE if this axis is in use */
	int width;            /* width (for Y axis) or height (X axis)
							to draw label,in terms of # of pixels */
	int fieldWidth;         /* # of characters for label */
	int decimalPlaces;      /* # of decimal places */
	int numTicks;           /* # of ticks to use */
	Coord tickIncrement;    /* tick increment, if not using number of ticks */
	Color color;            /* color for the ticks */
	Boolean useNumTicks; /* TRUE if use numTicks to draw axis,
							false if tickIncrement is used to draw axis */
};

struct LabelInfo {
	char *name;	/* name used to draw label */
	Boolean occupyTop; /* TRUE if label occupies top of view.
						false if label occupies left of view */
	int extent;	 /* height if label occupies top of view.
					width if label occupies left of view */
};

class Action;
class FilterQueue;
/* height of area used to draw cursor */
const int VIEW_CURSOR_HEIGHT= 3;
const int VIEW_CURSOR_WIDTH= 3;
/* 512K bytes before saving pixel data*/
const int VIEW_BYTES_BEFORE_SAVE = 524288; 
class View: public ViewWin, private DispatcherCallback,
	private ControlPanelCallback {
public:
	View(char *name, Action *action, VisualFilter &initFilter,
		Color foreground= BlackColor, Color background = BlackColor,
		AxisLabel *xAxis=NULL, AxisLabel *yAxis = NULL,
		int weight=1, Boolean boundary = true);
	~View();

	char *GetName(){ return ViewWin::GetName();}
	void Geometry(int &x, int &y, unsigned int &w, unsigned int &h){
		ViewWin::Geometry(x,y,w,h); }
	WindowRep *GetWindowRep(){ return ViewWin::GetWindowRep(); }

	/* Highlight a view of depending on flag.*/
	void Highlight(Boolean flag);

	int GetId() { return _id; }

	/* setting/getting visual filter */
	void SetVisualFilter(VisualFilter &filter);
	void GetVisualFilter(VisualFilter &filter);
	VisualFilter *GetVisualFilter();

	/* history queue */
	FilterQueue *GetHistory() { return _filterQueue; }

	/* For history initialization only */
	void ClearHistory();
	void InsertHistory(VisualFilter &filter);


	/* Mark/unmark nth visualfilter in history */
	void Mark(int index, Boolean true_false);

	static View *FindViewByName(char *name);
	static View *FindViewById(int viewId);
	static int FindViewId(View *view);
	/* get all views */
	static int InitViewIterator() { return _viewList->InitIterator(); }
	static Boolean MoreView(int index) { return _viewList->More(index); }
	static View *NextView(int index){ return _viewList->Next(index); }
	static void DoneViewIterator(int index) { _viewList->DoneIterator(index); }


   /* Set axes callback */
   void SetXAxisAttrType(AttrType type);
   AttrType GetXAxisAttrType() { return _xAxisAttrType;}
   void SetYAxisAttrType(AttrType type);
   AttrType GetYAxisAttrType() { return _yAxisAttrType; }
   void SetXAxisLabel(AxisLabel *callback){ _xAxisLabel = callback;}
   AxisLabel *GetXAxisLabel() { return _xAxisLabel; }
   void SetYAxisLabel(AxisLabel *callback){ _yAxisLabel = callback;}
   AxisLabel *GetYAxisLabel() { return _yAxisLabel; }

   /* set/clear min info about a view */
   void SetXMin(Boolean hasXMin, Coord xMin=0.0) {
		_hasXMin = hasXMin;
		_xMin = xMin;
   }
   Boolean GetXMin(Coord &xMin){
		xMin = _xMin;
		return _hasXMin;
   }

   void SetAction(Action *action);
   Action *GetAction() { return _action; }

   /*
   void AxisDisplayOnOff(Boolean stat);
   */
   void XAxisDisplayOnOff(Boolean stat);
   void YAxisDisplayOnOff(Boolean stat);
   void AxisDisplay(Boolean &xOnOff, Boolean &yOnOff) {
		xOnOff = xAxis.inUse;
		yOnOff = yAxis.inUse;
   }

   /* change label area */
   void SetLabelParam(Boolean occpyTop, int extent, char *name=NULL);

	void Refresh() ;

	/* Insert callback */
	static void InsertViewCallback(ViewCallback *callBack);
	static void DeleteViewCallback(ViewCallback *callBack);

	/* Insert class callback */

	void Iconify(Boolean iconified);

	/* Cursor manipulations */
	void AppendCursor(DeviseCursor *cursor);
	void DeleteCursor(DeviseCursor *cursor);
	void DrawCursors();

	/******** Pixmap manipulations *********/

	/* Put current pixmap into buffer. bytes == # of bytes
	used to create the pixmap. */
	void CachePixmap(int bytes);

	/* Look into pixmap buffer for pixmap that we can use for drawing
	filter.  Restore the pixmap, and return a new visual filter. */
	enum PixmapStat { PixmapTotal , PixmapPartial, PixmapNone };
	PixmapStat RestorePixmap(VisualFilter filter, VisualFilter &newFilter);

	/* Append pixmaps in pixmap buffer into file.*/
	void SavePixmaps(FILE *file);

	/* Restore pixmaps from an open file into pixmap buffer*/
	void LoadPixmaps(FILE  *file);

	/* Invalidate pixmaps because of changes in mapping.
	This needs to be developed into a more complicated interface
	in order to detect whether mapping has been changed back */
	void InvalidatePixmaps();

	/*Print view statistics */
	virtual void PrintStat();

protected:
    /* called by base class when it has been mapped/unmapped */
	virtual void SubClassMapped(); /* called just after mapping */
	virtual void SubClassUnmapped(); /* called just before unmapping */


	/* Base class decides when to start/abort a query. Derived
	class must implement the following: */
	virtual void DerivedStartQuery(VisualFilter &filter, int timestamp)=0;
	virtual void DerivedAbortQuery()=0;

	/* When a query is done, derived class must inform the base class */
	void ReportQueryDone(int bytes);

	/* Report size of data display area. */
	void GetDataArea(int &x, int &y, int &width,int &height);

private:
	/* Draw highlight */
	void DrawHighlight();

	/* from ControlPanelCallback */
	void ModeChange(ControlPanel::Mode mode);

	void ReportViewCreated();
	void ReportViewDestroyed();
	void ReportFilterAdded(VisualFilter &filter, int flushed);

	/* from DispatcherCallback */
	/* when it's our turn to run: Send a query, if there is one. 
	Aborting existing if necessary */
	virtual char *DispatchedName();
	virtual void Run() ;

	/* Update the visual filter with scrolling. Return
	the stat as followed:
		NotScrolled: can't scroll new filter with respect to old.
		Scrolled: was able to scroll, and _exposureRect was updated.
		SameFilter: new and old filter are the same. _exposureRect
			was not changed.*/
	enum UpdateFilterStat { NotScrolled, Scrolled, SameFilter };
	UpdateFilterStat UpdateFilterWithScroll();

	/* Get area for cursor */
	void GetXCursorArea(int &x, int &y, int &w, int &h);
	void GetYCursorArea(int &x, int &y, int &w, int &h);

	/* Get area for displaying label */
	void GetLabelArea(int &x, int &y, int &w, int &h);
	void View::GetXAxisArea(int &x, int &y, int &width, int &height,
		int &startX);
	void GetYAxisArea(int &x, int &y, int &width, int &height);

	/* Drawing axes and label*/
	void DrawAxesLabel(WindowRep *win,int x, int y, int w, int h);
	void DrawXAxis(WindowRep *win,int x, int y, int w, int h);
	void DrawYAxis(WindowRep *win,int x, int y, int w, int h);
	void DrawLabel(WindowRep *win, int x,int y, int w, int h);
	void DoDrawCursors(int x, int y, int w, int h);

	/* Calculate the transformation for this view */
	void CalcTransform(Transform2D &transform);

	/* Find World coord given screen coord */
	void FindWorld(int sx1, int sy1, int sx2, int sy2,
		Coord &xLow, Coord &yLow, Coord &xHigh, Coord &yHigh);

	/* from WindowRepCallback */
	/* draw in the exposed area */
	virtual void HandleExpose(WindowRep * w, int x, int y,
			unsigned width, unsigned height);

	/* Handle button press event */
	virtual void HandlePress(WindowRep * w, int xlow,
		int ylow, int xhigh, int yhigh, int button);

	/* handle key event */
	virtual void HandleKey(WindowRep *w ,char key, int x, int y);

	virtual void HandleResize(WindowRep * w, int xlow,
				int ylow, unsigned width, unsigned height);


	/* handle pop-up */
	virtual Boolean HandlePopUp(WindowRep *, int x, int y, int button,
		char **&msgs, int &numMsgs);

	/* Initialize */
	void Init(char *name, Action *action, VisualFilter &filter,
		AxisLabel *xAxis, AxisLabel *yAxis);

	/* check Cursor Op. Return cursor operated on */
	Boolean CheckCursorOp(WindowRep *win, int x, int y, int button);
Boolean  _displaySymbol; /* true if to be displayed */
AxisInfo xAxis, yAxis;   /* X and y axis info */
Boolean _axisDisplay;   /* TRUE if axes should be displayed */

Action *_action;

AxisLabel *_xAxisLabel;
AxisLabel *_yAxisLabel;

/* TRUE if _filter has changed since last time query was sent */
Boolean _filterChanged; 
VisualFilter _filter; /* new state of visual filter */

Boolean _hasExposure; /* TRUE if _exporsureRect has valid data */
ViewRect _exposureRect; /* Rect that has been exposed and needs to be redrawn*/

Boolean _querySent;	 /* TRUE if query has been snet */
VisualFilter _queryFilter; /* filter used for query */
ViewRect _queryRect; /* screen coordinates used for query */

int _id;    /* id of this view */
static int _nextId; /* id of next view */
static ViewList *_viewList; /* list of all views */

LabelInfo _label;	/* info about label */
Boolean _updateTransform; /* TRUE if we need to update transform for the 
		window */

void UpdateTransform(WindowRep *winRep); /* update transformation */


Boolean _hasLastFilter; /* TRUE if we have last filter used for query */
VisualFilter _lastFilter;

Boolean _refresh;	/* TRUE if we are refreshing */

FilterQueue *_filterQueue; /* stores history of filter changes */

static ViewCallbackList *_viewCallbackList;

Boolean _hasTimestamp;
int _timeStamp;

/* true if events occurs in layout mode that requires refresh when
in display mode. This variable is sent when anything happens during
LayoutMode that requires a refresh. It is resent only when a query
is sent. */
Boolean _modeRefresh;
Boolean _highlight;
AttrType _xAxisAttrType;
AttrType _yAxisAttrType;

Boolean _hasXMin;
Coord _xMin;
DeviseCursorList *_cursors;
DevisePixmap *_pixmap;
int _bytes; /* # of data bytes used to draw the current view */
static int _nextPos; /* next position in file to read from input file */

PixmapIO *_pixmapIO; /* IO for read/write pixmap */
Compression *_compress; /* compression class */

/* count # of times something happens */
int _jump, _zoomIn, _zoomOut, _scrollLeft, _scrollRight, _unknown;
};


#endif
