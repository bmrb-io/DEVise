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
  Implementation of ViewLens class.
 */

/*
  $Id$

  $Log$
  Revision 1.10  1997/11/24 23:15:24  weaver
  Changes for the new ColorManager.

  Revision 1.9  1997/09/05 22:36:32  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.8  1997/08/28 18:21:16  wenger
  Moved duplicate code from ViewScatter, TDataViewX, and ViewLens classes
  up into ViewGraph (parent class).

  Revision 1.7  1997/08/20 22:11:15  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.6.8.2  1997/08/15 23:06:35  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.6.8.1  1997/08/07 16:56:46  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.6.6.1  1997/05/21 20:40:53  weaver
  Changes for new ColorManager

  Revision 1.6  1997/03/20 22:26:03  guangshu
  Changed function QueryDone.

  Revision 1.5  1997/02/14 16:47:49  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.4  1997/02/03 19:45:38  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.3  1996/12/03 15:20:03  wenger
  Conditionaled out some debug code.

  Revision 1.2  1996/12/02 16:54:32  wenger
  Fixed compile warning; added standard headers to some files;
  conditionaled out debug code.

 */

//******************************************************************************

#include <time.h>

#include "ViewLens.h"
#include "WindowRep.h"
#include "XWindowRep.h"
#include "TimeStamp.h"
#include "TDataMap.h"
#include "Shape.h"
#include "Util.h"
#include "RecordLink.h"
#include "DrawTimer.h"
#include "Scheduler.h"

//#define DEBUG

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewLens::ViewLens(char* name, VisualFilter& initFilter, QueryProc* qp,
				   PColorID fgid, PColorID bgid,
				   AxisLabel* xAxis, AxisLabel* yAxis,
				   Action* action, ViewLensMode mode, ViewLensDim dimension)
	: ViewGraph(name, initFilter, qp, xAxis, yAxis, fgid, bgid, action)
{
#ifdef DEBUG
	printf("View::ViewLens(%s, this = %p)\n", name, this);
#endif

	viewCallback = new ViewLens_ViewCallback(this);

  _mode = mode;
  _dimension = dimension;
  _lensList = NULL;	// list of views in lens
  _lensLink = NULL;
  View::InsertViewCallback(viewCallback);
  /************TEMP HACK **************/
  _viewLensUpdate = false; // to avoid refresh of the view here 
  XAxisDisplayOnOff(true); 
  YAxisDisplayOnOff(true);
  _viewLensUpdate = true;
  /************TEMP HACK **************/
}

ViewLens::~ViewLens(void) 
{
  View::DeleteViewCallback(viewCallback);
  int index = InitViewLensIterator();
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    _lensList->DeleteCurrent(index);
  }
  DoneViewLensIterator(index);
  delete  _lensList;

	delete viewCallback;
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

void	ViewLens::SetBackground(PColorID bgid)
{
// Why isn't the primary background color changed as well?
//	ViewGraph::SetBackground(bgid);
	GetWindowRep()->SetBackground(bgid);
}

//******************************************************************************

void ViewLens::ReplaceView(ViewInfo *vinfo, ViewGraph *v)       
{
  if (vinfo == NULL) {
     if ( v != NULL ) {
       InsertView(v);
     }
  } else {
     vinfo->view = v;
     vinfo->refreshStatus = false;
     vinfo->toRefresh = false;
  }     
}
void ViewLens::InsertView(ViewGraph *v) 
{
  if (!_lensList) {
    _lensList = new ViewInfoList;
  }
  DOASSERT(_lensList, "Invalid new lenslist");
#ifdef DEBUG
  printf("Inserting view %s in lenslist", v->GetName());
#endif
//  v->SetAltRep(GetWindowRep());
  ViewInfo *vinfo =  new ViewInfo;
  vinfo->view = v;
  vinfo->refreshStatus = false;
  vinfo->toRefresh = false;
  _lensList->Append(vinfo);
}

int ViewLens::DeleteViewListTail(int index) 
{
   if (index < 0) {
      return index;
   }
   while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    _lensList->DeleteCurrent(index);
   }
   return index;  
}

void ViewLens::DeleteView(ViewGraph *v) 
{
  DOASSERT(_lensList, "Invalid lenslist");
  if (v == NULL) 
    return;
  bool found = false;
  
  int index = InitViewLensIterator();
  ViewInfo *vi = NULL;
  
  while(MoreViewsInLens(index)) {
    vi = NextViewInfoInLens(index);
    DOASSERT(vi, "Invalid vi");
    
    if (vi->view == v) {
      found = true;
      break;
    }
  }
  DoneViewLensIterator(index);
  if (found) {
#ifdef DEBUG
    printf("Removing view %s from lens %s\n", v->GetName(), GetName());
#endif
    _lensList->Delete(vi);
#ifdef DEBUG
    _lensList->PrintIterators();
#endif
  }

}

char* ViewLens::GetMode() 
{
  if (_mode == transparent) {
     return "transparent";
  }
  if (_mode == opaque) {
     return "opaque";
  }
  return NULL;
}
 void ViewLens::SetMode(char *modestr)
 {
   if (strcmp(modestr, "transparent") == 0) {
      _mode = transparent;
   } else if ( strcmp(modestr, "opaque") == 0) {
      _mode = opaque;
   }
 }

void ViewLens::Refresh()
{
  if (!_viewLensUpdate) {
    return;
  }
  printf("ViewLens %s : refreshing \n", GetName());
  
  int index = InitViewLensIterator();
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    vi->refreshStatus = false;
    vi->toRefresh = false;
  }
  DoneViewLensIterator(index);
    
  View::Refresh();
}

void ViewLens::DerivedStartQuery(VisualFilter &filter,
			       int timestamp)
{
#ifdef DEBUG
  printf("ViewLens:: %s start query \n", GetName());
#endif
  _timestamp = timestamp;
  _index = InitViewLensIterator();
  ViewInfo *vi = NULL;
  ViewGraph *v = NULL;
  if (MoreViewsInLens(_index)) {
    vi = NextViewInfoInLens(_index); 
    v = vi->view;
    _curView = v;
  }
  v->GetVisualFilter(_queryFilter);
  ViewGraph::DerivedStartQuery(_queryFilter, timestamp);
}

void ViewLens::DerivedAbortQuery()
{
#ifdef DEBUG
  printf("ViewLens:: %s abort query \n", GetName());
#endif
  DoneViewLensIterator(_index);
  /* Do stuff here */
  ViewGraph::DerivedAbortQuery();
}



void ViewLens::DrawStack()
{
	WindowRep*	winRep = GetWindowRep();

  unsigned int Width, Height;
  DevisePixmap *basepixmap = 0;
  winRep->SetForeground(winRep->GetBackground());
  winRep->SetPattern(Pattern0);
  winRep->SetLineWidth(0);
  winRep->Dimensions(Width, Height);
  winRep->FillRect(0, 0, Width - 1, Height - 1);
  
  if (_mode == transparent) {
    DrawTransparentStack();
  }
  else if (_mode == opaque) {
    DrawOpaqueStack();
  }
  else {
    printf("mode not supported\n");
  }
}

void ViewLens::DrawTransparentStack()
{
  WindowRep *winRep = GetWindowRep();
  //currently implementing only z stack
  int index = InitViewLensIterator();
  DevisePixmap *basepixmap = 0;
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    ViewGraph*v = vi->view;
    WindowRep *viewWinRep = v->GetWindowRep();
    DevisePixmap *newpixmap = viewWinRep->GetPixmap();      
    if (basepixmap == NULL) {
      basepixmap = newpixmap;
    }
    else { // also do x and y stack
      basepixmap = OverlayPixmap(basepixmap, winRep,
				 newpixmap,viewWinRep);
				 
      viewWinRep->FreePixmap(newpixmap);
    }
    printf("Done pixmap for view %s\n", v->GetName());
  }
  DoneViewLensIterator(index);
  winRep->SetCopyMode();
  winRep->DisplayPixmap(basepixmap);
}


void ViewLens::DrawOpaqueStack()
{
  WindowRep *winRep = GetWindowRep();
  WindowRep *viewWinRep = NULL;
  int index = InitViewLensIterator();
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    ViewGraph*v = vi->view;
    viewWinRep = v->GetWindowRep();
  }
  DoneViewLensIterator(index);
  DevisePixmap *basepixmap = viewWinRep->GetPixmap();      
  winRep->SetCopyMode();
  winRep->DisplayPixmap(basepixmap);
  viewWinRep->FreePixmap(basepixmap);
}


// works only if the pixmaps are of the same size.
// May be trouble for color management...
DevisePixmap *ViewLens::OverlayPixmap(DevisePixmap *basepixmap,
				      WindowRep *basewin,
				      DevisePixmap *newpixmap,
				      WindowRep *newwin)
{
	XColorID	xfgid = AP_GetXColorID(newwin->GetForeground());
	XColorID	xbgid = AP_GetXColorID(newwin->GetBackground());

#ifdef DEBUG
  printf("ViewLens::DisplayPixmap %d bytes\n", basepixmap->compressedBytes);
#endif
  
  unsigned char *basedata = basepixmap->data;
  unsigned char *newdata = newpixmap->data;

    if (abs(basepixmap->height - newpixmap->height) > 5) {
    return NULL;
  }
  int ht = (basepixmap->height > newpixmap->height) ? 
    newpixmap->height : basepixmap->height;

  int index1=0, index2 = 0;
  int outIndex1 = 0;
  
  for(int i = 0; i < ht; i++) {
    int baseCount, newCount;
    int outCount1, outCount2;
    memcpy((char *)&baseCount, (char *)&basedata[index1], sizeof(int));
    memcpy((char *)&newCount,  (char *)&newdata[index2], sizeof(int));
    index1 += sizeof(int);
    index2 += sizeof(int);
    
    char *basebuf 
      = ((XWindowRep *)basewin)->GetCompress()->DecompressLine((char *)&basedata[index1], 
					      baseCount, outCount1);
    //printf("%s\n", basebuf);
    char *tmpbasebuf = (char *) malloc(outCount1);
    memcpy(tmpbasebuf, basebuf, outCount1);
    char *newbuf 
      = ((XWindowRep *)newwin)->GetCompress()->DecompressLine((char *)&newdata[index2],
					     newCount, outCount2);
    index1 += baseCount;
    index2 += newCount;
    DOASSERT(outCount1 == basepixmap->bytes_per_line, "Invalid pixmap format");
    DOASSERT(outCount2 == newpixmap->bytes_per_line, "Invalid pixmap format");
    
    for (int j = 0; j < outCount1; j++) {
      if((unsigned char) newbuf[j] != xbgid) {
	tmpbasebuf[j] = newbuf[j];
      }
    }
    char *cBuf 
      = ((XWindowRep *)basewin)->GetCompress()
	->CompressLine(tmpbasebuf, 
		       basepixmap->bytes_per_line,
		       outCount1);
    //printf("%s\n", tmpbasebuf);
    delete tmpbasebuf;
    //printf("cBuf = %s\n", cBuf);

    if (outCount1 + outIndex1 + sizeof(int) > MAX_PIXMAP_BUF_SIZE ) {
      return NULL;
    }

    memcpy(&_pixmapbuf[outIndex1], (char *)&outCount1, sizeof(int));
    outIndex1 += sizeof(int);
    memcpy(&_pixmapbuf[outIndex1], cBuf, outCount1);
    outIndex1 += outCount1;
  }
  basepixmap->compressedBytes = outIndex1;
  delete basepixmap->data;
  basepixmap->data = (unsigned char *)malloc(outIndex1);
  basepixmap->height = ht;
  memcpy(basepixmap->data, _pixmapbuf, outIndex1);
  return basepixmap;
}

//******************************************************************************
// Callback Methods (DispatcherCallback)
//******************************************************************************

void	ViewLens::Run(void) 
{
	bool	done = true;
	int		index = InitViewLensIterator();

	while(MoreViewsInLens(index))
	{
		ViewInfo*	vi = NextViewInfoInLens(index);
#ifdef DEBUG
//		printf("checking view %s\n", vi->view->GetName());
#endif      

		if (vi->toRefresh && !vi->refreshStatus)
		{
			done = false;
			break;
#ifdef DEBUG
//			printf("view %s not done\n", vi->view->GetName());
#endif
		}
	}

	DoneViewLensIterator(index);

	if (done)
	{
#ifdef DEBUG
		printf("ViewLens %s can now proceed\n", GetName());
#endif
		// Do pixmap stuff here
//		DrawStack();
//		_timestamp = TimeStamp::NextTimeStamp();
//		DerivedStartQuery(_queryFilter, _timestamp);

		View::Run();
	} 
	else
	{
#ifdef DEBUG
//		printf("ViewLens %s cannot  proceed\n", GetName());
		Scheduler::Current()->RequestCallback(_dispatcherID);
#endif
	}
}

//******************************************************************************
// Callback Methods (QueryCallback)
//******************************************************************************

void	ViewLens::QueryDone(int bytes, void* userData, TDataMap* map = NULL)
{
#ifdef DEBUG
	printf("ViewLens::Query done, index = %d, bytes = %d\n", _index, bytes);
#endif

	ViewInfo*	vi = NULL;
	ViewGraph*	v = NULL;

	_pstorage.Clear();

	if ( _index < 0)
		return;

	if(MoreViewsInLens(_index))
	{
		vi = NextViewInfoInLens(_index); 
		v = (ViewGraph*)vi->view;
		_curView = v;
	}
	else
	{
		printf("All views done \n");

		DoneViewLensIterator(_index);
		ReportQueryDone(bytes, false);
		_map = 0;
		_index = -1;
		_curView = NULL;

		return;
  }

	v->GetVisualFilter(_queryFilter);
	ViewGraph::QueryDone(bytes, userData, map);
}

RecordLinkList*		ViewLens::GetRecordLinkList()
{
	return (_curView ? _curView->SlaveLinkList() : (RecordLinkList*)NULL);
}

void	ViewLens::ReturnGData(TDataMap* mapping, RecId recId,
							  void* gdata, int numGData, int& recordsProcessed,
							  Boolean needDrawnList, int &recordsDrawn,
							  BooleanArray*& drawnList)
{
#if defined(DEBUG)
	printf("ViewLens::ReturnGData()\n");
//	printf("ViewLens %d recs buf start 0x%p\n", numGData, gdata);
#endif

	drawnList = NULL;
	recordsProcessed = numGData;

	// Cancel the draw timeout for now, so we don't have to deal with
	// the possibility that some symbols won't be drawn.  This should
	// be changed to allow the timeout to happen once we have a chance
	// to test this code.  RKW Aug. 15, 1997.
	DrawTimer::Cancel();

	Coord		maxWidth, maxHeight, maxDepth;
	WindowRep*	win = GetWindowRep();

	mapping->UpdateMaxSymSize(gdata, numGData);
	mapping->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

	GDataAttrOffset*	offset = mapping->GetGDataOffset();
	int					gRecSize = mapping->GDataRecordSize();
	char*				ptr = (char*)gdata;
	int					recIndex = 0;
	int					firstRec = 0;

	for(int i=0; i<numGData; i++)
	{
		// Extract X, Y, shape, and color information from gdata record
		Coord		x = ShapeGetX(ptr, mapping, offset);
		Coord		y = ShapeGetY(ptr, mapping, offset);
		ShapeID		shape = GetShape(ptr, mapping, offset);

		win->SetForeground(mapping->GetPColorID(ptr));

		Boolean		complexShape = mapping->IsComplexShape(shape);

		complexShape |= (GetNumDimensions() == 3);

		// Contiguous ranges which match the filter's X and Y range
		// are stored in the record link
		if (!complexShape && (x + maxWidth / 2 < _queryFilter.xLow || 
							  x - maxWidth / 2 > _queryFilter.xHigh || 
							  y + maxHeight / 2 < _queryFilter.yLow || 
							  y - maxHeight / 2 > _queryFilter.yHigh))
		{
			if (i > firstRec)
				WriteMasterLink(recId + firstRec, i - firstRec);
			// Next contiguous batch of record ids starts at i+1
			firstRec = i + 1;
			ptr += gRecSize;
			continue;
		}

		// Draw data only if window is not iconified
		if (!Iconified())
		{
			_recs[recIndex++] = ptr;
			if (recIndex == WINDOWREP_BATCH_SIZE)
			{
				mapping->DrawGDataArray(this, win, _recs, recIndex, 
										recordsProcessed);
				recIndex = 0;
			}
		}

		ptr += gRecSize;
	}

	if (numGData > firstRec)
		WriteMasterLink(recId + firstRec, numGData - firstRec);

	if (!Iconified() && recIndex > 0)
		mapping->DrawGDataArray(this, win, _recs, recIndex, recordsProcessed);
}

//******************************************************************************
// Callback Methods (ViewCallback)
//******************************************************************************

void	ViewLens::ViewDestroyed(View* view)
{
	DeleteView((ViewGraph*)view);
}

void	ViewLens::ViewRecomputed(View* view)
{
#ifdef DEBUG
	printf("Got information of completion of %s\n", view->GetName());
#endif

	int		index = InitViewLensIterator();
	bool	found = false;

	while(MoreViewsInLens(index))
	{
		ViewInfo*	vi = NextViewInfoInLens(index);

		if (vi->view == view)
		{ 
#ifdef DEBUG
			printf("found view %s\n", vi->view->GetName());
#endif

			found = true;
			vi->refreshStatus = true;
			vi->toRefresh = false;
		}
	}

	DoneViewLensIterator(index);

	if (found)
	{
		View::Refresh();
//		Scheduler::Current()->RequestCallback(_dispatcherID);
	}
}

void	ViewLens::FilterAboutToChange(View* view) 
{
/*
	Boolean		found = false;
	int			index = InitViewLensIterator();
	ViewInfo*	vi;

	while(MoreViewsInLens(index))
	{
		vi = NextViewInfoInLens(index); 

		if (view ==  vi->view)
		{
			found = true;
			break;
		}
	}

	DoneViewLensIterator(index);

	if (found)
	{
		WindowRep*	winRep = GetWindowRep();
		int 		height, width;

		winRep->SetForeground(winRep->GetBackground());
		winRep->SetPattern(Pattern0);
		winRep->SetLineWidth(0);

		winRep->Dimensions(width, height);
		winRep->FillRect(0, 0, width - 1, height - 1);
	}
*/
}

void	ViewLens::FilterChanged(View* view, VisualFilter& filter, int flushed) 
{
	// check if the view is in our lenslist 
	Boolean		found = false;
	int			index = InitViewLensIterator();
	ViewInfo*	vi = NULL;

	while(MoreViewsInLens(index))
	{
		vi = NextViewInfoInLens(index); 

		if (view ==  vi->view)
		{
			found = true;
			break;
		}
	}

	DoneViewLensIterator(index);

	if (found)
	{
#ifdef DEBUG
		printf("ViewLens_ViewCallback : callback from view %s refreshing\n",
			   view->GetName());
#endif
		vi->toRefresh = true;
		vi->refreshStatus = false;
	}
}

//******************************************************************************
