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
  Revision 1.2  1996/12/02 16:54:32  wenger
  Fixed compile warning; added standard headers to some files;
  conditionaled out debug code.

 */

#include <time.h>

#include "Util.h"
#include "ViewLens.h"
#include "Geom.h"
#include "WindowRep.h"
#include "XWindowRep.h"
#include "FilterQueue.h"
#include "Control.h"
#include "TimeStamp.h"
#include "Parse.h"
#include "Map3D.h"
#include "Display.h"

#define DEBUG

ViewLens::ViewLens(char *name, VisualFilter &initFilter, 
		   GlobalColor foreground, GlobalColor background,
		   AxisLabel *xAxis, AxisLabel *yAxis,
		   Action *action, bool transparent, int dimension)
: ViewGraph(name, initFilter, xAxis, yAxis, foreground, background,action)
{
#ifdef DEBUG
  printf("View::ViewLens(%s, this = %p)\n", name, this);
#endif
  _transparent = transparent;
  _dimension = dimension;
  _lensList = NULL;	// list of views in lens
  _viewLensUpdate = false ;	// flag to signal update of the viewlens
  View::InsertViewCallback(this);
}
       
void ViewLens::InsertView(View *v) 
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

void ViewLens::DeleteView(View *v) 
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

ViewLens::~ViewLens() 
{
  View::DeleteViewCallback(this);
  int index = InitViewLensIterator();
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    _lensList->DeleteCurrent(index);
  }
  DoneViewLensIterator(index);
  delete  _lensList;
}

  
void ViewLens::Refresh()
{
  printf("ViewLens %s : refreshing \n", GetName());
  
  int index = InitViewLensIterator();
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    vi->refreshStatus = false;
    vi->toRefresh = false;
  }
  DoneViewLensIterator(index);
  _viewLensUpdate = false;
    
  View::Refresh();
}

/* from View */
void ViewLens::Run() 
{
  Dispatcher::Current()->CancelCallback(_dispatcherID);
  bool done = true;
  int index = InitViewLensIterator();
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
#ifdef DEBUG
//    printf("checking view %s\n", vi->view->GetName());
#endif      
    if (vi->toRefresh && !vi->refreshStatus) {
      done = false;
      break;
#ifdef DEBUG
//    printf("view %s not done\n", vi->view->GetName());
#endif
    }
  }
  DoneViewLensIterator(index);
  if (done) {
#ifdef DEBUG
    printf("ViewLens %s can now proceed\n", GetName());
#endif
    /* Do pixmap stuff here */
    DrawStack();
  } 
  else {
#ifdef DEBUG
//    printf("ViewLens %s cannot  proceed\n", GetName());
    Dispatcher::Current()->RequestCallback(_dispatcherID);
#endif
  }

}

/* from ViewCallback */
void ViewLens::ViewRecomputed(View *view) {
#ifdef DEBUG
  printf("Got information of completion of %s\n", view->GetName());
#endif
  int index = InitViewLensIterator();
  while(MoreViewsInLens(index)) {
    ViewInfo *vi = NextViewInfoInLens(index);
    if (vi->view == view) { 
#ifdef DEBUG
      printf("found view %s\n", vi->view->GetName());
#endif
      vi->refreshStatus = true;
      vi->toRefresh = false;
      break;
    }
  }
  DoneViewLensIterator(index);
  Dispatcher::Current()->RequestCallback(_dispatcherID);
}


/* from ViewCallback */
void ViewLens::ViewCreated(View *view) {
  /* not needed */
}


/* from ViewCallback */
void ViewLens::ViewDestroyed(View *view) {
  /* not needed */
  DeleteView(view);
}


/* from ViewCallback */
void ViewLens::FilterAboutToChange(View *view) 
{
/*
  Boolean found = false;

  int index = InitViewLensIterator();
  ViewInfo *vi;

  while(MoreViewsInLens(index)) {
    vi = NextViewInfoInLens(index); 
    if (view ==  vi->view) {
      found = true;
      break;
    }
  }
  DoneViewLensIterator(index);

  if (found) {
    WindowRep *winRep = GetWindowRep();
    winRep->SetFgColor(winRep->GetBgColor());
    winRep->SetPattern(Pattern0);
    winRep->SetLineWidth(0);
    int height, width;
    winRep->Dimensions(width, height);
    winRep->FillRect(0, 0, width - 1, height - 1);
  }
*/
}

/* from ViewCallback */

void ViewLens::FilterChanged(View *view, VisualFilter &filter, int flushed) 
{
  // check if the view is in our lenslist 
  Boolean found = false;
  int index = InitViewLensIterator();
  ViewInfo *vi = NULL;

  while(MoreViewsInLens(index)) {
    vi = NextViewInfoInLens(index); 
    if (view ==  vi->view) {
      found = true;
      break;
    }
  }
  DoneViewLensIterator(index);
  if (found) {
#ifdef DEBUG
    printf("ViewLens : callback from view %s refreshing\n", view->GetName());
#endif
    vi->toRefresh = true;
    vi->refreshStatus = false;

  }

}

void ViewLens::DerivedStartQuery(VisualFilter &filter,
			       int timestamp)
{
#ifdef DEBUG
  printf("ViewLens:: %s start query \n", GetName());
#endif
  /* DO stuff here */
  ReportQueryDone(0);
}

void ViewLens::DerivedAbortQuery()
{
#ifdef DEBUG
  printf("ViewLens:: %s abort query \n", GetName());
#endif
  /* Do stuff here */
  ReportQueryDone(0);
}



void ViewLens::DrawStack()
{
  WindowRep *winRep = GetWindowRep();
  unsigned int Width, Height;
  DevisePixmap *basepixmap = 0;
  winRep->SetFgColor(winRep->GetBgColor());
  winRep->SetPattern(Pattern0);
  winRep->SetLineWidth(0);
  winRep->Dimensions(Width, Height);
  winRep->FillRect(0, 0, Width - 1, Height - 1);
  
  if (_transparent) {
    DrawTransparentStack();
  }
  else {
    DrawOpaqueStack();
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
    View*v = vi->view;
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
    View*v = vi->view;
    viewWinRep = v->GetWindowRep();
  }
  DoneViewLensIterator(index);
  DevisePixmap *basepixmap = viewWinRep->GetPixmap();      
  winRep->SetCopyMode();
  winRep->DisplayPixmap(basepixmap);
  viewWinRep->FreePixmap(basepixmap);
}


// works only if the pixmaps are of the same size.
DevisePixmap *ViewLens::OverlayPixmap(DevisePixmap *basepixmap,
				      WindowRep *basewin,
				      DevisePixmap *newpixmap,
				      WindowRep *newwin)
{
#ifdef DEBUG
  printf("ViewLens::DisplayPixmap %d bytes\n", basepixmap->compressedBytes);
#endif
  
  unsigned char *basedata = basepixmap->data;
  unsigned char *newdata = newpixmap->data;
  unsigned long fgColor = newwin->GetLocalColor(newwin->GetFgColor());
  unsigned long bgColor = newwin->GetLocalColor(newwin->GetBgColor());
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
    printf("%s\n", basebuf);
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
      if((unsigned char) newbuf[j] != bgColor) {
	tmpbasebuf[j] = newbuf[j];
      }
    }
    char *cBuf 
      = ((XWindowRep *)basewin)->GetCompress()
	->CompressLine(tmpbasebuf, 
		       basepixmap->bytes_per_line,
		       outCount1);
    printf("%s\n", tmpbasebuf);
    delete tmpbasebuf;
    printf("cBuf = %s\n", cBuf);

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



