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

ActionDefault::ActionDefault( char *name, Coord leftEdge,
			     Boolean useLeftFlag, Coord rightEdge,
			     Boolean useRightFlag):
     Action(name)
{
  left = leftEdge;
  right = rightEdge;
  useLeft = useLeftFlag;
  useRight = useRightFlag;
}

void ActionDefault::KeySelected(View *view, char key, Coord x, Coord y)
{
  ControlPanel::Instance()->SelectView(view);
  VisualFilter filter;
  ViewGraph *vg = (ViewGraph *)view;

  if (key == '+') {
    /* increase pixel size */
    Boolean changed = false;
    for(vg->InitMappingIterator(); vg->MoreMapping();) {
      TDataMap *map = vg->NextMapping();
      if (map->GetPixelWidth() < 30) {
	changed = true;
	map->SetPixelWidth(map->GetPixelWidth()+1);
      }
    }
    vg->DoneMappingIterator();
    if (changed) view->Refresh();
  }

  else if (key == '-') {
    /* decrease pixel size */
    Boolean changed = false;
    for(vg->InitMappingIterator(); vg->MoreMapping();) {
      TDataMap *map = vg->NextMapping();
      if (map->GetPixelWidth() >1) { 
	changed = true;
	map->SetPixelWidth(map->GetPixelWidth()-1);
      }
    }
    vg->DoneMappingIterator();
    if (changed) view->Refresh();
  }

  else if (key == '<' || key == ',' || key == '4') {
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

  else if (key == 'z' || key == 'z' || key == '9') {
    /* zoom out */
    view->GetVisualFilter(filter);
    Coord halfWidth = (filter.xHigh - filter.xLow) / 2.0;
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
    printf("%s\n", msgPtr[i]);
}

Boolean ActionDefault::PopUp(View *view, Coord x, Coord y, Coord xHigh,
			     Coord yHigh, int button, char **& msgs,
			     int &numMsgs)
{
  ControlPanel::Instance()->SelectView(view);
  
  AttrType xAttr = view->GetXAxisAttrType();
  AttrType yAttr = view->GetYAxisAttrType();
  
  InitPutMessage((x+xHigh)/2.0, xAttr, (y+yHigh)/2.0, yAttr);
  
  char *errorMsg;
  if (!PrintRecords(view, x, y, xHigh, yHigh, errorMsg)) {
    InitPutMessage((x+xHigh)/2.0, xAttr, (y+yHigh)/2.0, yAttr);
    PutMessage("");
    PutMessage(errorMsg);
  }

  EndPutMessage(numMsgs, msgs);

  return true;
}

Boolean ActionDefault::PrintRecords(View *view, Coord x, Coord y, 
				    Coord xHigh, Coord yHigh, char *&errorMsg)
{
#ifdef DEBUG
  printf("ActionDefault::PrintRecords %s %.2f %.2f %.2f %.2f\n",
	 view->GetName(), x, y, xHigh, yHigh);
#endif

  static RecInterp *recInterp = NULL;
  
  if (recInterp == NULL)
    recInterp = new RecInterp;
  
  VisualFilter filter;
  filter.flag = VISUAL_X;
  filter.xLow = x;
  filter.xHigh = xHigh;
  
  /* get mapping */
  ViewGraph *vg = (ViewGraph *)view;
  TDataMap *map = NULL;
  for (vg->InitMappingIterator(); vg->MoreMapping(); ) {
    map = vg->NextMapping();
    break;
  }
  vg->DoneMappingIterator();
  if (map == NULL) {
    errorMsg = "PrintRecord: no mapping";
    return false;
  }
  
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
  
  if (!approxFlag) {
    errorMsg = "Query of scatter plot to be implemented";
    return false;
  }
  
  TData *tdata = map->GetTData();
  AttrList *attrs = tdata->GetAttrList();
  if (attrs == NULL) {
    errorMsg = "PrintRecords: no attribute info";
    return false;
  }

  recInterp->SetAttrs(attrs);
#if 0
  recInterp->PrintAttrHeading();
#endif

  Boolean tooMuch = false;
  qp->InitTDataQuery(map, filter, approxFlag);

  while (qp->GetTData(startRid, numRecs, buf)) {
    char *ptr = buf;
    for(int i = 0; i< numRecs; i++) {
      recInterp->SetBuf(ptr);
      ptr += tdata->RecSize();
      
      if (!tooMuch) {
	/* set up for display inside the window */
	PutMessage("");
	for(int j = 0; j < attrs->NumAttrs(); j++) {
	  char buf[128];
	  recInterp->PrintAttr(buf, j, true);
	  if (!tooMuch) {
	    if (!PutMessage(buf)) {
	      PrintMsgBuf();
	      errorMsg = "see text window";
	      tooMuch = true;
	    }
	  }
	  else {
	    printf(buf);
	  }
	}
	if (tooMuch)
	  printf("\n");
      }
      else  /* print in text window */
	recInterp->Print(true);
    }
  }
  qp->DoneTDataQuery();
  return !tooMuch;
}
