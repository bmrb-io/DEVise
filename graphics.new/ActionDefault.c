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
  Revision 1.17  1996/06/16 01:33:24  jussi
  Added handling of case where xlow == xhigh or ylow == yhigh
  in zooming functions.

  Revision 1.16  1996/06/15 16:11:05  jussi
  Added '5'-key functionality to 3D views.

  Revision 1.15  1996/05/31 19:05:19  jussi
  Removed alternate keyboard mapping for X zoom out.

  Revision 1.14  1996/05/07 16:36:32  jussi
  Changed type of view parameter from View * to ViewGraph *.
  Added handling of keypad number 5 (autoscale view).

  Revision 1.13  1996/04/23 21:58:23  jussi
  A scatter plot now uses both Y and X coordinates when executing
  a TData query.

  Revision 1.12  1996/04/23 19:50:43  jussi
  Enabled queries of scatter plot data. Limited text window dump
  size to 500 lines.

  Revision 1.11  1996/04/22 21:38:04  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.10  1996/04/20 19:56:34  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.9  1996/04/15 15:06:47  jussi
  Interface to ViewGraph's mapping iterator has changed.

  Revision 1.8  1996/01/19 19:52:19  jussi
  Made attribute output appear on separate lines.

  Revision 1.7  1995/12/29 22:43:57  jussi
  Small fixes.

  Revision 1.6  1995/12/28 20:48:04  jussi
  Cleaned up the code a bit.

  Revision 1.5  1995/12/14 21:15:25  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.4  1995/12/14 17:30:03  jussi
  Made small fixes to get rid of g++ -Wall warnings.

  Revision 1.3  1995/09/12 16:05:13  jussi
  Added y scrolling up and down.

  Revision 1.2  1995/09/05 22:13:59  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <time.h>

#include "ActionDefault.h"
#include "TDataMap.h"
#include "Selection.h"
#include "ViewGraph.h"
#include "RecInterp.h"
#include "QueryProc.h"
#include "TData.h"

ActionDefault::ActionDefault(char *name, Coord leftEdge,
			     Boolean useLeftFlag, Coord rightEdge,
			     Boolean useRightFlag):
     Action(name)
{
  left = leftEdge;
  right = rightEdge;
  useLeft = useLeftFlag;
  useRight = useRightFlag;
}

void ActionDefault::KeySelected(ViewGraph *view, char key, Coord x, Coord y)
{
  ControlPanel::Instance()->SelectView(view);
  VisualFilter filter;

  Boolean isScatterPlot = view->IsScatterPlot();
  isScatterPlot |= (view->GetNumDimensions() != 2);

  if (key == '+') {
    /* increase pixel size */
    Boolean changed = false;
    int index = view->InitMappingIterator();
    while(view->MoreMapping(index)) {
      TDataMap *map = view->NextMapping(index)->map;
      if (map->GetPixelWidth() < 30) {
	changed = true;
	map->SetPixelWidth(map->GetPixelWidth() + 1);
      }
    }
    view->DoneMappingIterator(index);
    if (changed)
      view->Refresh();
  }

  else if (key == '-') {
    /* decrease pixel size */
    Boolean changed = false;
    int index = view->InitMappingIterator();
    while(view->MoreMapping(index)) {
      TDataMap *map = view->NextMapping(index)->map;
      if (map->GetPixelWidth() > 1) { 
	changed = true;
	map->SetPixelWidth(map->GetPixelWidth() - 1);
      }
    }
    view->DoneMappingIterator(index);
    if (changed)
      view->Refresh();
  }

  else if (key == '5') {
    /* show all data records in view i.e. set filter to use the
       actual min/max X values and the actual min/max Y values;
       for 3D graphs, move camera to (0,0,Z) where Z is twice
       the min Z value */
    int index = view->InitMappingIterator();
    if (view->MoreMapping(index)) {
      TDataMap *map = view->NextMapping(index)->map;
      AttrInfo *xAttr = map->MapGAttr2TAttr("x");
      AttrInfo *yAttr = map->MapGAttr2TAttr("y");
      AttrInfo *zAttr = map->MapGAttr2TAttr("z");
      if (view->GetNumDimensions() == 2) {
	view->GetVisualFilter(filter);
	if (xAttr) {
	  if (xAttr->hasHiVal)
	    filter.xHigh = AttrList::GetVal(&xAttr->hiVal, xAttr->type);
	  if (xAttr->hasLoVal)
	    filter.xLow = AttrList::GetVal(&xAttr->loVal, xAttr->type);
	  if (filter.xHigh == filter.xLow) {
	    filter.xHigh += 1.0;
	    filter.xLow -= 1.0;
	  }
	}
	if (yAttr) {
	  if (yAttr->hasHiVal)
	    filter.yHigh = AttrList::GetVal(&yAttr->hiVal, yAttr->type);
	  if (yAttr->hasLoVal)
	    filter.yLow = AttrList::GetVal(&yAttr->loVal, yAttr->type);
	  if (filter.yHigh == filter.yLow) {
	    filter.yHigh += 1.0;
	    filter.yLow -= 1.0;
	  }
	}
	if (!isScatterPlot)
	  filter.yLow = 0;
	view->SetVisualFilter(filter);
      } else {
	Camera c = view->GetCamera();
	c.fx = 0;
	c.fy = 0;
	c.fz = 0;
	double minZ = -4.0;
	if (zAttr && zAttr->hasLoVal)
	  minZ = 2 * AttrList::GetVal(&zAttr->loVal, zAttr->type);
	if (minZ > -4.0)
	  minZ = -4.0;
	if (!c.spherical_coord) {
	  c.x_ = 0;
	  c.y_ = 0;
	  c.z_ = minZ;
	} else {
	  c._theta = 0;
	  c._phi = M_PI_2;
	  c._rho = fabs(minZ);
	}
	view->SetCamera(c);
      }
      view->DoneMappingIterator(index);
    }
  }

  else if (!isScatterPlot && (key == '<' || key == ',' || key == '4')) {
    /* scroll data left */
    view->GetVisualFilter(filter);
    Coord width = filter.xHigh - filter.xLow;
    Coord halfWidth = (filter.xHigh - filter.xLow) / 2.0;
    filter.xLow += halfWidth;
    filter.xHigh = filter.xLow + width;
    if (useRight && filter.xHigh > right) {
      filter.xHigh = right;
      filter.xLow = filter.xHigh - width;
    }
    view->SetVisualFilter(filter);
  }

  else if (!isScatterPlot && key == '9') {
    /* zoom out */
    view->GetVisualFilter(filter);
    Coord halfWidth = (filter.xHigh - filter.xLow) / 2.0;
    if (halfWidth == 0.0)
      halfWidth = 1.0;
    filter.xLow -= halfWidth;
    filter.xHigh += halfWidth;
    Coord xMin;
    if (view->GetXMin(xMin) && filter.xLow < xMin)
      filter.xLow = xMin;
    /*
       if (useLeft && filter.xLow < left)
       filter.xLow = left;
    */
    if (useRight && filter.xHigh > right)
      filter.xHigh = right;
    view->SetVisualFilter(filter);
  }

  else
    Action::KeySelected(view, key, x, y);
}

static const int MAX_MSGS = 50;
static const int MSG_BUF_SIZE = 4096;
static char msgBuf[MSG_BUF_SIZE];
static int msgIndex;
static char *msgPtr[MAX_MSGS];
static int numMsgs;

static Boolean CheckParams()
{
  if (numMsgs >= MAX_MSGS || msgIndex >= MSG_BUF_SIZE)
    return false;
  return true;
}

static Boolean PutMessage(char *msg)
{
  if (!CheckParams())
    return false;
  
  int len = strlen(msg) + 1;
  if (msgIndex + len > MSG_BUF_SIZE)
    return false;
  sprintf(&msgBuf[msgIndex], "%s", msg);
  msgPtr[numMsgs++] = &msgBuf[msgIndex];
  msgIndex += len;
  return true;
}

static Boolean InitPutMessage(double x, AttrType xAttr, double y,
			      AttrType yAttr)
{
  msgIndex = 0;
  numMsgs = 0;
  char buf[128];
  
  if (xAttr == DateAttr) {
    time_t clock = (time_t)x;
    sprintf(buf,"x: %s", DateString(clock));
  } else {
    sprintf(buf,"x: %.2f", x);
  }
  if (!PutMessage(buf))
    return false;
  
  if (yAttr == DateAttr) {
    time_t clock = (time_t)y;
    sprintf(buf,"y: %s", DateString(clock));
  }
  else {
    sprintf(buf,"y: %.2f", y);
  }
  return PutMessage(buf);
}

static void EndPutMessage(int &numMessages, char **&msgs)
{
  numMessages = numMsgs;
  msgs = msgPtr;
}

/* print contents of message buffer */

static void PrintMsgBuf()
{
  for(int i = 0; i < numMsgs; i++)
    puts(msgPtr[i]);
}

Boolean ActionDefault::PopUp(ViewGraph *view, Coord x, Coord y, Coord xHigh,
			     Coord yHigh, int button, char **& msgs,
			     int &numMsgs)
{
  ControlPanel::Instance()->SelectView(view);
  
  AttrType xAttr = view->GetXAxisAttrType();
  AttrType yAttr = view->GetYAxisAttrType();
  
  InitPutMessage((x + xHigh) / 2.0, xAttr, (y + yHigh) / 2.0, yAttr);
  
  char *errorMsg;
  if (!PrintRecords(view, x, y, xHigh, yHigh, errorMsg)) {
    InitPutMessage((x + xHigh) / 2.0, xAttr, (y + yHigh) / 2.0, yAttr);
    PutMessage("");
    PutMessage(errorMsg);
  }

  EndPutMessage(numMsgs, msgs);

  return true;
}

Boolean ActionDefault::PrintRecords(ViewGraph *view, Coord x, Coord y, 
				    Coord xHigh, Coord yHigh, char *&errorMsg)
{
#ifdef DEBUG
  printf("ActionDefault::PrintRecords %s %.2f %.2f %.2f %.2f\n",
	 view->GetName(), x, y, xHigh, yHigh);
#endif

  static RecInterp *recInterp = NULL;
  
  if (!recInterp)
    recInterp = new RecInterp;
  
  /* get mapping */
  int index = view->InitMappingIterator();
  if (!view->MoreMapping(index)) {
    errorMsg = "No mapping found!";
    view->DoneMappingIterator(index);
    return false;
  }

  TDataMap *map = view->NextMapping(index)->map;
  view->DoneMappingIterator(index);
  
  RecId startRid;
  int numRecs;
  char *buf;
  QueryProc *qp = QueryProc::Instance();

#ifdef DEBUG
  printf("ActionDefault::PrintRecords: getting TData\n");
#endif

  int numDimensions;
  VisualFlag *dimensionInfo;
  numDimensions = map->DimensionInfo(dimensionInfo);
  Boolean approxFlag = (numDimensions == 1 && dimensionInfo[0] == VISUAL_X);
  
  TData *tdata = map->GetTData();
  AttrList *attrs = tdata->GetAttrList();
  if (!attrs) {
    errorMsg = "No attribute info!";
    return false;
  }

  recInterp->SetAttrs(attrs);
#if 0
  recInterp->PrintAttrHeading();
#endif

  VisualFilter filter;
  filter.flag = VISUAL_X;
  filter.xLow = x;
  filter.xHigh = xHigh;

  if (!approxFlag) {
    // for scatter plot, we filter using Y coordinates as well;
    // allow user inaccuracy that is five times larger than for
    // sortedX views (where approximation is used anyway), that is,
    // roughly five pixels in each direction
    filter.flag = VISUAL_X | VISUAL_Y;
    float xDiff = 10 * fabs(xHigh - x);
    float yDiff = 10 * fabs(yHigh - y);
    filter.xLow  = x - xDiff / 2;
    filter.xHigh = x + xDiff / 2;
    filter.yLow  = y - yDiff / 2;
    filter.yHigh = y + yDiff / 2;
  }

  Boolean tooMuch = false;
  qp->InitTDataQuery(map, filter, approxFlag);

  int linesDisplayed = 0;

  while (qp->GetTData(startRid, numRecs, buf)) {
    char *ptr = buf;
    for(int i = 0; i< numRecs; i++) {
      recInterp->SetBuf(ptr);
      ptr += tdata->RecSize();
      
      if (tooMuch)
	puts("");
      else
	PutMessage("");

      /* set up for display inside the window */

      for(int j = 0; j < attrs->NumAttrs(); j++) {
	char buf[128];
	recInterp->PrintAttr(buf, j, true);
	if (!tooMuch) {
	  if (!PutMessage(buf)) {
	    PrintMsgBuf();
	    errorMsg = "see text window";
	    tooMuch = true;
	  }
	} else {
	  puts(buf);
	}
	++linesDisplayed;
      }

      if (linesDisplayed > 500) {
	printf("\nToo many records to display\n");
	break;
      }
    }
    
    if (linesDisplayed > 500)
      break;
  }

  qp->DoneTDataQuery();

  return !tooMuch;
}
