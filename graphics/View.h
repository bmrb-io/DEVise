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
  Revision 1.38  1996/08/04 21:03:36  beyer
  Added view-locks and changed key handling.

  Revision 1.37  1996/08/03 15:35:09  jussi
  Solid3D flag now has three values: wireframe, solid with frame,
  and solid without frame.

  Revision 1.36  1996/07/25 14:22:59  jussi
  Added aborted parameter to ReportQueryDone().

  Revision 1.35  1996/07/23 19:34:06  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.34  1996/07/23 17:16:30  jussi
  Added support for piled views.

  Revision 1.33  1996/07/21 02:22:05  jussi
  Added _xyZoom variable and associated methods.

  Revision 1.32  1996/07/20 18:47:20  jussi
  Added solid3D flag.

  Revision 1.31  1996/07/13 17:26:55  jussi
  Added ViewRecomputed() callback interface.

  Revision 1.30  1996/07/12 23:42:08  jussi
  Integrated Init() method with the constructor.

  Revision 1.29  1996/07/08 20:31:01  jussi
  Added PixmapEnabled() method.

  Revision 1.28  1996/07/02 22:44:16  jussi
  Removed unnecessary code.

  Revision 1.27  1996/06/27 19:04:50  jussi
  The user can now switch between 2D and 3D display, the
  data is refreshed and displayed accordingly.

  Revision 1.26  1996/06/27 15:43:58  jussi
  Added method AbortAndReexecuteQuery which allows the QueryProc
  to ask the view to re-issue queries when TData has changed.

  Revision 1.25  1996/06/21 19:32:07  jussi
  Moved all 3D-related code to Map3D.C and Map3D.h.

  Revision 1.24  1996/06/15 14:09:46  jussi
  Added yuc\'s 3D methods.

  Revision 1.23  1996/06/15 13:46:49  jussi
  X and Y axes now use the view foreground color as their color.
  The axis.color field was removed.

  Revision 1.22  1996/06/13 00:14:33  jussi
  Added support for XY cursors. All types of cursors can now
  be moved by clicking on their new location in the data area.
  Previously only the X label area was sensitive for cursor
  movement.

  Revision 1.21  1996/05/31 15:30:29  jussi
  Added support for tick marks in axes. The spacing and location
  of tick marks is set automatically to something reasonable,
  even if the axis range is not.

  Revision 1.20  1996/05/07 16:32:14  jussi
  Moved Action member variable to ViewGraph. Move implementation of
  HandleKey, HandlePress and HandlePopup to ViewGraph as well.

  Revision 1.19  1996/04/20 19:52:21  kmurli
  Changed Viex.c to use a pipe mechanism to call itself if it needs to be
  done again. The view now is not called contiously by the Dispatcher,instead
  only of there is some data in the pipe.
  The pipe mechanism is implemented transparently through static functions
  in the Dispatcher.c (InsertMarker,CreateMarker,CloseMarker,FlushMarker)

  Revision 1.18  1996/04/10 02:43:54  jussi
  Added small check to SetXMin() to accommodate multiple mappings
  per view.

  Revision 1.17  1996/04/09 22:51:58  jussi
  Added SetOverrideColor() and GetOverrideColor().

  Revision 1.16  1996/03/06 19:35:26  jussi
  Added GetNumDimensions() and SetNumDimensions().

  Revision 1.15  1996/02/06 19:32:28  jussi
  Moved logo drawing to ViewWin.c.

  Revision 1.14  1996/02/05 23:57:00  jussi
  Added DEVise logo display.

  Revision 1.13  1996/01/30 00:02:41  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.12  1996/01/16 17:06:47  jussi
  Minor updates.

  Revision 1.11  1995/12/29 22:42:12  jussi
  Added support for line connectors.

  Revision 1.10  1995/12/29 18:28:26  jussi
  Added new cursor mechanism; cursors are drawn as inverted rectangles
  which are easier to see than the old cursor lines.

  Revision 1.9  1995/12/05 17:02:13  jussi
  Moved _stats to ViewGraph (superclass) so that statistics can be
  turned on and displayed without having to redisplay the data itself.

  Revision 1.8  1995/12/04 18:06:36  jussi
  Added GetLabelParams and replaced ToggleStatDisplay with SetStatDisplay.

  Revision 1.7  1995/12/02 21:24:45  jussi
  Changed "boundary" default to false. Views no longer have a
  boundary because Tk or other type of margin controls will
  surround each view.

  Revision 1.6  1995/11/28 05:10:23  ravim
  Support for statistics.

  Revision 1.5  1995/11/24 21:32:04  jussi
  Added copyright notice and cleaned up the code. Made GetDataArea
  a protected method so that TDataViewX and ViewScatter can access
  the size of the data area.

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
  virtual char *GenerateLabel(Coord val) = 0;
  virtual char *SetFormat(char *format) { return ""; }
  virtual char *GetFormat() { return ""; }

 private:
  char *_name;
};

/* function that generates an axis label */
class GenAxisLabel {
 public:
  virtual AxisLabel *MakeAxisLabel(char *name) = 0;
};

struct AxisInfo {
  Boolean inUse;          /* TRUE if this axis is in use */
  int width;              /* width (for Y axis) or height (X axis)
			     to draw label, in terms of # of pixels */
  int numTicks;           /* # of ticks to use */
  int significantDigits;  /* # of significant digits */
  int labelWidth;         /* heigh (for Y axis) or width (X axis)
			     of tick labels */
};

struct LabelInfo {
  char *name;	          /* name used to draw label */
  Boolean occupyTop;      /* TRUE if label occupies top of view.
			     false if label occupies left of view */
  int extent;	          /* height if label occupies top of view.
			     width if label occupies left of view */
};

class FilterQueue;

class View
: public ViewWin,
  private DispatcherCallback,
  private ControlPanelCallback
{
  public:

	enum VIEW_LOCK {
	    LOWER_LEFT  = 0x0001,
	    UPPER_LEFT  = 0x0002,
	    LOWER_RIGHT = 0x0004,
	    UPPER_RIGHT = 0x0008,
	    XWIDTH      = 0x0010,
	    YWIDTH      = 0x0020,

	    LEFT_SIDE   = LOWER_LEFT|UPPER_LEFT,
	    RIGHT_SIDE  = LOWER_RIGHT|UPPER_RIGHT,
	    TOP_SIDE    = UPPER_LEFT|UPPER_RIGHT,
	    BOTTOM_SIDE = LOWER_LEFT|LOWER_RIGHT,
	    ALL_LOCKS   = 0xffff
	};


	View(char *name, VisualFilter &initFilter,
		Color foreground = ForegroundColor,
	        Color background = BackgroundColor,
		AxisLabel *xAxis = NULL, AxisLabel *yAxis = NULL,
		int weight = 1, Boolean boundary = false);
	virtual ~View();

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

	/* iterate through all views */
	static int InitViewIterator() { return _viewList->InitIterator(); }
	static Boolean MoreView(int index) { return _viewList->More(index); }
	static View *NextView(int index){ return _viewList->Next(index); }
	static void DoneViewIterator(int index) { _viewList->DoneIterator(index); }

	/* Set axes callback */
	void SetXAxisAttrType(AttrType type);
	AttrType GetXAxisAttrType() { return _xAxisAttrType; }
	void SetYAxisAttrType(AttrType type);
	AttrType GetYAxisAttrType() { return _yAxisAttrType; }
	void SetXAxisLabel(AxisLabel *callback){ _xAxisLabel = callback; }
	AxisLabel *GetXAxisLabel() { return _xAxisLabel; }
	void SetYAxisLabel(AxisLabel *callback){ _yAxisLabel = callback; }
	AxisLabel *GetYAxisLabel() { return _yAxisLabel; }

	/* set/clear min info about a view */
	void SetXMin(Boolean hasXMin, Coord xMin = 0.0) {
	  _hasXMin = hasXMin;
	  if (_hasXMin && xMin < _xMin) {
	      _xMin = xMin;
	  }
	}
	Boolean GetXMin(Coord &xMin) {
	  xMin = _xMin;
	  return _hasXMin;
	}

	void XAxisDisplayOnOff(Boolean stat);
	void YAxisDisplayOnOff(Boolean stat);
	void AxisDisplay(Boolean &xOnOff, Boolean &yOnOff) {
	  xOnOff = xAxis.inUse;
	  yOnOff = yAxis.inUse;
	}

	/* view locks - lock corners or widths */
	void SetViewLocks(int locks) { _view_locks |= locks; }
	void ClearViewLocks(int locks) { _view_locks &= ~locks; }
	void ToggleViewLocks(int locks) { _view_locks ^= locks; }
	int ViewLocks() { return _view_locks; }
	bool IsViewLocked(int locks) { return (_view_locks & locks) != 0; }

	/* get label parameters */
	void GetLabelParam(Boolean &occupyTop, int &extent, char *&name);
	
	/* set label parameters */
	void SetLabelParam(Boolean occupyTop, int extent, char *name = 0);

	/* abort and re-execute query */
	void AbortAndReexecuteQuery();

	void Refresh();

	/* Insert callback */
	static void InsertViewCallback(ViewCallback *callBack);
	static void DeleteViewCallback(ViewCallback *callBack);

	/* Iconify view */
	void Iconify(Boolean iconified);

	/* Cursor manipulations */
	void AppendCursor(DeviseCursor *cursor);
	void DeleteCursor(DeviseCursor *cursor);
	Boolean DrawCursors();
	Boolean HideCursors();

	/* Get/set dimensionality */
	int GetNumDimensions() { return _numDimensions; }
	void SetNumDimensions(int d);

	/* Get/set wireframe/solid/solid+frame 3D objects */
	int GetSolid3D() { return _solid3D; }
	void SetSolid3D(int solid);

	/* Get/set XY zoom or X/Y zoom */
	Boolean IsXYZoom() { return _xyZoom; }
	void SetXYZoom(Boolean xyZoom) { _xyZoom = xyZoom; }

	/* Get/set data value display flag */
	Boolean GetDisplayDataValues() { return _dispDataValues; }
	void SetDisplayDataValues(Boolean disp);

	/* Get/set pile mode */
	Boolean IsInPileMode() { return _pileMode; }
	void SetPileMode(Boolean mode);

	/* Get/set override color */
	Color GetOverrideColor(Boolean &active) {
	  active = _hasOverrideColor;
	  return _overrideColor;
	}
	void SetOverrideColor(Color color, Boolean active);

	/******** Pixmap manipulations *********/

	/* Put current pixmap into buffer. bytes == # of bytes
	used to create the pixmap. */
	void CachePixmap(int bytes);

	/* Look into pixmap buffer for pixmap that we can use for drawing
	filter.  Restore the pixmap, and return a new visual filter. */
	enum PixmapStat { PixmapTotal , PixmapPartial, PixmapNone };
	PixmapStat RestorePixmap(VisualFilter filter, VisualFilter &newFilter);
        virtual Boolean PixmapEnabled() { return true; }

	/* Append pixmaps in pixmap buffer into file.*/
	void SavePixmaps(FILE *file);

	/* Restore pixmaps from an open file into pixmap buffer*/
	void LoadPixmaps(FILE *file);

	/* Invalidate pixmaps because of changes in mapping.
	This needs to be developed into a more complicated interface
	in order to detect whether mapping has been changed back */
	void InvalidatePixmaps();

	/* Print view statistics */
	virtual void PrintStat();

	/* Toggle symbol display */
	virtual Boolean DisplaySymbols() { return false; }
	virtual Boolean DisplaySymbols(Boolean state) { return state; }

	/* Toggle connector display */
	virtual Boolean DisplayConnectors() { return false; }
	virtual Boolean DisplayConnectors(Boolean state) { return state; }

	/* Report size of data display area. */
	void GetDataArea(int &x, int &y, int &width,int &height);

	/* 3D functions */
        void Draw3DAxis();
	Camera GetCamera() { return _camera; }
	void SetCamera(Camera new_camera);
	void SetViewDir(int H, int V);
	void CompRhoPhiTheta();

	virtual void SetFgBgColor(Color fg, Color bg) {
	  ViewWin::SetFgBgColor(fg, bg);
	  Refresh();
	}

protected:
	/* called by base class when it has been mapped/unmapped */
	virtual void SubClassMapped();   /* called just after mapping */
	virtual void SubClassUnmapped(); /* called just before unmapping */

	/* Base class decides when to start/abort a query. Derived
	   class must implement the following methods. */
	virtual void DerivedStartQuery(VisualFilter &filter,
				       int timestamp) = 0;
	virtual void DerivedAbortQuery() = 0;

	/* When a query is done, derived class must inform the base class */
	void ReportQueryDone(int bytes, Boolean aborted = false);

	/* Report size of data display area. */
	/* void GetDataArea(int &x, int &y, int &width,int &height); */

	/* Find World coord given screen coord */
	void FindWorld(int sx1, int sy1, int sx2, int sy2,
		       Coord &xLow, Coord &yLow, Coord &xHigh, Coord &yHigh);


	/* check Cursor Op. Return cursor operated on */
	Boolean CheckCursorOp(WindowRep *win, int x, int y, int button);

	/* Get area for displaying label */
	void GetLabelArea(int &x, int &y, int &w, int &h);
	void GetXAxisArea(int &x, int &y, int &width, int &height,
			  int &startX);
	void GetYAxisArea(int &x, int &y, int &width, int &height);

	/* Optimize spacing and location of tick marks */
	void OptimizeTickMarks(Coord low, Coord high, int numTicks,
			       Coord &start, int &num, Coord &inc);

protected:
	void DrawHighlight();

	/* from ControlPanelCallback */
	void ModeChange(ControlPanel::Mode mode);

	void ReportViewCreated();
	void ReportFilterAboutToChange();
	void ReportFilterChanged(VisualFilter &filter, int flushed);
	void ReportViewRecomputed();
	void ReportViewDestroyed();

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

	/* Drawing axes and label*/
	void DrawAxesLabel(WindowRep *win, int x, int y, int w, int h);
	void DrawXAxis(WindowRep *win, int x, int y, int w, int h);
	void DrawYAxis(WindowRep *win, int x, int y, int w, int h);
	void DrawLabel();
	void DoDrawCursors();

	/* Calculate the transformation for this view */
	void CalcTransform(Transform2D &transform);
	void CalcTransform(Transform3D &transform);

	/* Find World coord given screen coord */
	/* void FindWorld(int sx1, int sy1, int sx2, int sy2,
		Coord &xLow, Coord &yLow, Coord &xHigh, Coord &yHigh); */

	/* from WindowRepCallback */
	/* draw in the exposed area */
	virtual void HandleExpose(WindowRep * w, int x, int y,
				  unsigned width, unsigned height);

	/* Handle button press event */
	virtual void HandlePress(WindowRep * w, int xlow,
				 int ylow, int xhigh, int yhigh,
				 int button) = 0;

	/* handle key event */
	virtual void HandleKey(WindowRep *w ,int key, int x, int y) = 0;

	virtual void HandleResize(WindowRep * w, int xlow,
				  int ylow, unsigned width,
				  unsigned height);

	/* handle pop-up */
	virtual Boolean HandlePopUp(WindowRep *, int x, int y, int button,
				    char **&msgs, int &numMsgs) = 0;

	Boolean  _displaySymbol; /* true if to be displayed */
	AxisInfo xAxis, yAxis;   /* X and y axis info */

	AxisLabel *_xAxisLabel;
	AxisLabel *_yAxisLabel;

	DispatcherID _dispatcherID;

	/* TRUE if _filter has changed since last time query was sent */
	Boolean _filterChanged; 
	VisualFilter _filter;      /* new state of visual filter */
	
	Boolean _hasExposure;      /* TRUE if _exporsureRect has valid data */
	ViewRect _exposureRect;    /* Rect that has been exposed
				      and needs to be redrawn */
	
	Boolean _querySent;	    /* TRUE if query has been sent */
	VisualFilter _queryFilter;  /* filter used for query */
	ViewRect _queryRect;        /* screen coordinates used for query */

	int _id;                    /* id of this view */
	static int _nextId;         /* id of next view */
	static ViewList *_viewList; /* list of all views */

	LabelInfo _label;	  /* info about label */
	Boolean _updateTransform; /* TRUE if we need to update transform
				     for the window */

	void UpdateTransform(WindowRep *winRep); /* update transformation */

	Boolean _hasLastFilter; /* TRUE if last filter was used for query */
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

	int _view_locks;	// bits from VIEW_LOCK

	DeviseCursorList *_cursors;
	DevisePixmap *_pixmap;
	int _bytes;          /* # data bytes used to draw the current view */
	static int _nextPos; /* next position in file to read from
				input file */
	
	PixmapIO *_pixmapIO;    /* IO for read/write pixmap */
	Compression *_compress; /* compression class */

	Boolean _cursorsOn;             /* true if cursors displayed */

	int _numDimensions;             /* number of dimensions */
        Boolean _solid3D;               /* true if solid 3D objects */

        Boolean _xyZoom;                /* true if XY zooms */
        Boolean _dispDataValues;        /* true if data values display
                                           in graph */
        Boolean _pileMode;              /* true if view is in pile mode */
        Boolean _pileViewHold;          /* true if bottom view must hold */

	/* count # of times something happens */
	int _jump, _zoomIn, _zoomOut, _scrollLeft, _scrollRight, _unknown;

	Boolean _hasOverrideColor;      /* override color defined */
	Color _overrideColor;           /* color that overrides color
					   defined in mapping */

	/* 3D data structures */
	Camera _camera;
};

#endif
