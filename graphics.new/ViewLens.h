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
  Revision 1.4.8.1  1997/08/07 16:56:46  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.4  1997/03/20 22:26:30  guangshu
  Changed function QueryDone.

  Revision 1.3  1997/02/03 19:45:39  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.2  1996/12/02 16:54:32  wenger
  Fixed compile warning; added standard headers to some files;
  conditionaled out debug code.

 */

#ifndef ViewLens_h
#define ViewLens_h

#include "QueryProc.h"
#include "ViewGraph.h"
#include "TDataViewX.h"
#include "GDataBin.h"
#include "WindowRep.h"
#include "DList.h"
#include "ViewCallback.h"
#include "Color.h"


// pile can be in x, y or z dimensions
enum ViewLensDim { _X_ , _Y_,  _Z_ } ;
enum ViewLensMode {transparent, opaque } ;

struct ViewInfo{
  ViewGraph *view;
  bool toRefresh;		// set when callback by ReportFilterChanged
  bool refreshStatus;		// set when callback by ReportViewRecomputed
}; 

DefinePtrDList(ViewInfoList, ViewInfo *)

static const int MAX_PIXMAP_BUF_SIZE = 256 * 1024;
static char _pixmapbuf[MAX_PIXMAP_BUF_SIZE];

class ViewLens 
: public ViewGraph,
  public QueryCallback,  // for query processing
  public ViewCallback  // for informing that all child views are done
{
  public :
    
    /* create a View Lens with a given list of views */
    ViewLens(char *name, VisualFilter &initFilter, 
             QueryProc *qp,
	     GlobalColor foreground = ForegroundColor, 
	     GlobalColor background = BackgroundColor,
	     AxisLabel *xAxis = NULL, 
	     AxisLabel *yAxis = NULL, 
	     Action *action = NULL,
	     ViewLensMode mode = transparent,
	     ViewLensDim = _Z_);
  
  /* destroy the viewlens */
  ~ViewLens();

  /* Get mode */
  char *GetMode();
  /* Set mode */
  void SetMode(char *modestr);
  void SetBgColor(GlobalColor color)
  {
    GetWindowRep()->SetBgColor(color);
  }
  void SetLink(VisualLink *link) { _lensLink = link; }
  VisualLink *GetLink() { return _lensLink; } 
  /* iterate through all views */
  /* if lenslist is NULL, the functions do stuff so that the caller gets
   * sane return values and can proceed without realising the problem. 
   */
  int iterator_cnt;
  int InitViewLensIterator() { 
    if (_lensList) { 
     printf("--------------------------------------- InitIterator(%d)\n",iterator_cnt++);
     return _lensList->InitIterator(); 
    }
    else 
     return -1;
  }
  Boolean MoreViewsInLens(int index) { 
     if ((index < 0) || !_lensList ) 
        return false;
     else    
        return _lensList->More(index); 
  } 
  ViewInfo *NextViewInfoInLens(int index){ 
     if ((index < 0)|| !_lensList ) 
        return NULL;
     else 
        return _lensList->Next(index); 
  }
  ViewGraph *NextViewInLens(int index){ 
     if ((index < 0)|| !_lensList ) 
        return NULL;
     else 
        return _lensList->Next(index)->view; 
  }
  void DoneViewLensIterator(int index) { 
     if ((index < 0)|| !_lensList ) 
        return;
     else  {
        printf("--------------------------------------- DoneIterator(%d)\n",--iterator_cnt);
        _lensList->DoneIterator(index); 
     }
  }
  void InsertView(ViewGraph *v);
  void ReplaceView(ViewInfo *vinfo, ViewGraph *v);
  void DeleteView(ViewGraph *v);
  int  DeleteViewListTail(int index);
  void Refresh();
  void DrawStack();
  void DrawTransparentStack();
  void DrawOpaqueStack();
  DevisePixmap *
    OverlayPixmap(DevisePixmap *basepixmap,
			    WindowRep *basewin, 
			    DevisePixmap *newpixmap,
			    WindowRep *newwin);
			    
protected:
  virtual void DerivedStartQuery(VisualFilter &filter, int timestamp) ;
  virtual void DerivedAbortQuery();
  
  virtual void Run();
  virtual void FilterChanged(View *view, VisualFilter &filter, 
				       int flushed); 
  virtual void FilterAboutToChange(View *view);
  virtual void ViewDestroyed(View *view);
  virtual void ViewCreated(View *view);
  virtual void ViewRecomputed(View *view);
  /* from querycallback */ 
  /* Query data ready to be returned. Do initialization here.*/
  virtual void QueryInit(void *userData);

  virtual void ReturnGData(TDataMap *mapping, RecId id,
			   void *gdata, int numGData,
			   int &recordsProcessed);
  virtual void *GetObj() { return this; }
  
  /* Done with query */
  virtual void QueryDone(int bytes, void *userData, TDataMap *map=NULL);
  virtual RecordLinkList *GetRecordLinkList() { 
      if ( _curView ) {
        return _curView->SlaveLinkList();
      } else {
        return NULL;
      }
  }
private:
  ViewInfoList *_lensList;
  Boolean _viewLensUpdate;
  VisualLink *_lensLink;  /* the visual link to all views in the lens */
  ViewLensMode _mode;
  ViewLensDim _dimension;
  VisualFilter _queryFilter;
  int          _timestamp;
  QueryProc    *_queryProc;
  TDataMap     *_map;
  int          _index;
  ViewGraph    *_curView;
  void         *_recs[WINDOWREP_BATCH_SIZE]; /* max # of pointers */
};
#endif


  


