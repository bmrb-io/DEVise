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
  Declaration of ViewLens class.
 */

/*
  $Id$

  $Log$
 */

#ifndef ViewLens_h
#define ViewLens_h

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
#include "ViewGraph.h"


// pile can be in x, y or z dimensions
#define _X_ 1
#define _Y_ 2
#define _Z_ 3

struct ViewInfo{
  View *view;
  bool toRefresh;		// set when callback by ReportFilterChanged
  bool refreshStatus;		// set when callback by ReportViewRecomputed
}; 

DefinePtrDList(ViewInfoList, ViewInfo *)

static const int MAX_PIXMAP_BUF_SIZE = 256 * 1024;
static char _pixmapbuf[MAX_PIXMAP_BUF_SIZE];

class ViewLens 
: public ViewGraph,
  public ViewCallback  // for informing that all child views are done
{
  public :
    
    /* create a View Lens with a given list of views */
    ViewLens(char *name, VisualFilter &initFilter, 
	     GlobalColor foreground = ForegroundColor, 
	     GlobalColor background = BackgroundColor,
	     AxisLabel *xAxis = NULL, 
	     AxisLabel *yAxis = NULL, 
	     Action *action = NULL,
	     bool transparent = true,
	     int dimension = _Z_);
  
  /* destroy the viewlens */
  ~ViewLens();
  
  
  /* iterate through all views */
  int InitViewLensIterator() { return _lensList->InitIterator(); }
  Boolean MoreViewsInLens(int index) { return _lensList->More(index); }
  ViewInfo *NextViewInfoInLens(int index){ return _lensList->Next(index); }
  View *NextViewInLens(int index){ return _lensList->Next(index)->view; }
  void DoneViewLensIterator(int index) { _lensList->DoneIterator(index); }
  void ViewLens::InsertView(View *v);
  void ViewLens::DeleteView(View *v);
  void Refresh();
  void DrawStack();
  void DrawTransparentStack();
  void DrawOpaqueStack();
  DevisePixmap *
    ViewLens::OverlayPixmap(DevisePixmap *basepixmap,
			    WindowRep *basewin, 
			    DevisePixmap *newpixmap,
			    WindowRep *newwin);
			    
protected:
  virtual void DerivedStartQuery(VisualFilter &filter, int timestamp) ;
  virtual void DerivedAbortQuery();
  
  virtual void Run();
  virtual void ViewLens::FilterChanged(View *view, VisualFilter &filter, 
				       int flushed); 
  virtual void ViewLens::FilterAboutToChange(View *view);
  virtual void ViewDestroyed(View *view);
  virtual void ViewLens::ViewCreated(View *view);
  virtual void ViewLens::ViewRecomputed(View *view);
  
  
private:
  ViewInfoList *_lensList;
//  ViewList *_lensList;
  Boolean _viewLensUpdate;
  Boolean _transparent;
  Boolean _dimension;
};

  

#endif


  


