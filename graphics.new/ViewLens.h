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
  Revision 1.10  1998/03/08 00:01:15  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

  Revision 1.9  1998/02/10 21:13:20  wenger
  Changed signatures of ReturnGData() in QueryCallback and its subclasses
  to pass back lists of records drawn (not implemented yet); moved
  declaration of ViewGraph_QueryCallback from ViewGraph.h to ViewGraph.c.

  Revision 1.8  1997/12/12 05:50:46  weaver
  *** empty log message ***

  Revision 1.7  1997/11/24 23:15:25  weaver
  Changes for the new ColorManager.

  Revision 1.6  1997/08/28 18:21:17  wenger
  Moved duplicate code from ViewScatter, TDataViewX, and ViewLens classes
  up into ViewGraph (parent class).

  Revision 1.5  1997/08/20 22:11:15  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.4.8.1  1997/08/07 16:56:46  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.4.6.1  1997/05/21 20:40:53  weaver
  Changes for new ColorManager

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

//******************************************************************************
//
//******************************************************************************

#ifndef __VIEWLENS_H
#define __VIEWLENS_H

//******************************************************************************
// Libraries
//******************************************************************************

#include "ViewGraph.h"
#include "WindowRep.h"
#include "DList.h"
#include "ViewCallback.h"
#include "DeviseLink.h"

#include "Color.h"

//******************************************************************************

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

//******************************************************************************
// class ViewLens
//******************************************************************************

class ViewLens : public ViewGraph
{
		friend class ViewLens_ViewCallback;

	private:

		ViewInfoList*	_lensList;
		Boolean			_viewLensUpdate;
		DeviseLink*		_lensLink;		// Visual link to all views in the lens
		ViewLensMode	_mode;
		ViewLensDim		_dimension;
		ViewGraph*		_curView;
		void*			_recs[WINDOWREP_BATCH_SIZE];	// Max # pointers

		// Callback Adapters
		ViewLens_ViewCallback*	viewCallback;
		
	public:

		// Constructors and Destructors
//		ViewLens(char* name, VisualFilter& initFilter, QueryProc* qp,
//				 PColorID fgid = nullPColorID,
//				 PColorID bgid = nullPColorID,
//				 AxisLabel* xAxis = NULL, AxisLabel* yAxis = NULL,
//				 Action* action = NULL,
//				 ViewLensMode mode = transparent, ViewLensDim = _Z_);
		ViewLens(char* name, VisualFilter& initFilter, QueryProc* qp,
				 PColorID fgid = GetPColorID(defForeColor),
				 PColorID bgid = GetPColorID(defBackColor),
				 AxisLabel* xAxis = NULL, AxisLabel* yAxis = NULL,
				 Action* action = NULL,
				 ViewLensMode mode = transparent, ViewLensDim = _Z_);
		virtual ~ViewLens(void);

		// Getters and Setters
		virtual void	SetBackground(PColorID bgid);

		char*			GetMode(void);
		void			SetMode(char* modestr);

		DeviseLink*		GetLink(void)				{ return _lensLink; } 
		void			SetLink(DeviseLink* link)	{ _lensLink = link; }

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
  
  


	protected:

		// Callback methods (DispatcherCallback)
		virtual void	Run(void);

		// Callback methods (QueryCallback)
		virtual void	QueryDone(int bytes, void* userData,
								  TDataMap* map = NULL);
		virtual void*	GetObj(void) { return this; }
		virtual MSLinkList*		GetRecordLinkList(void);
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList,
									int &recordsDrawn,
									BooleanArray*& drawnList);

		// Callback methods (ViewCallback)
		virtual void	ViewCreated(View* view) {}
		virtual void	ViewDestroyed(View* view);
		virtual void	ViewRecomputed(View* view);
		virtual void	FilterAboutToChange(View *view);
		virtual void	FilterChanged(View* view, VisualFilter& filter,
									  int flushed);
};

//******************************************************************************
// class ViewLens_ViewCallback
//******************************************************************************

class ViewLens_ViewCallback : public ViewCallback
{
	private:

		ViewLens*	_parent;
		
	public:

		ViewLens_ViewCallback(ViewLens* parent)
			: _parent(parent) {}

		virtual void	ViewCreated(View* view)
		{
			_parent->ViewCreated(view);
		}

		virtual void	ViewDestroyed(View* view)
		{
			_parent->ViewDestroyed(view);
		}

		virtual void	ViewRecomputed(View* view)
		{
			_parent->ViewRecomputed(view);
		}

		virtual void	FilterAboutToChange(View *view)
		{
			_parent->FilterAboutToChange(view);
		}

		virtual void	FilterChanged(View* view, VisualFilter& filter,
									  int flushed)
		{
			_parent->FilterChanged(view, filter, flushed);
		}
};

//******************************************************************************
#endif


  


