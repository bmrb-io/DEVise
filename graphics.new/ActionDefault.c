/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.46  1999/08/23 21:22:54  wenger
  Added special provisions for drill-down on views with fixed text symbols,
  because bounding boxes don't work.

  Revision 1.45  1999/08/11 16:51:12  wenger
  Point query for drill-down now makes use of symbol bounding boxes.

  Revision 1.44  1999/07/19 22:19:03  wenger
  Drill-down now shows as much as it can if buffer fills up.

  Revision 1.43  1999/06/23 19:45:10  wenger
  Increased the number of records drill-down can send to the screen; if there
  are too many records, the message sent to the JavaScreen is now "Too much
  data to show" instead of "see text window"; generally cleaned up the drill-
  down code.

  Revision 1.42  1999/06/21 17:21:22  wenger
  Drill-down now works on all views in a pile.

  Revision 1.41  1999/06/04 16:32:29  wenger
  Fixed bug 495 (problem with cursors in piled views) and bug 496 (problem
  with key presses in piled views in the JavaScreen); made other pile-
  related improvements (basically, I removed a bunch of pile-related code
  from the XWindowRep class, and implemented that functionality in the
  PileStack class).

  Revision 1.40  1999/05/28 16:32:42  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.39  1999/05/26 19:50:48  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.38  1999/04/22 19:29:50  wenger
  Separated the configuration of explicit (user-requested) and implicit
  home actions (no GUI for configuring the implicit home); changed the
  Condor user script accordingly; modified JavaScreen support so that this
  all works for the JS.

  Revision 1.37  1998/12/15 14:55:08  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.36  1998/11/09 20:33:21  wenger
  Fixed bug 433 (drill-down problem); improved debug output in various
  related modules.

  Revision 1.35  1998/08/24 14:57:40  wenger
  Added misc. debug output.

  Revision 1.34  1998/08/17 17:11:50  wenger
  Devised now responds to KeyAction commands from JavaScreen.

  Revision 1.33  1998/08/11 13:43:21  wenger
  Server responds to KeyAction commands from JavaScreen (still needs event
  coordinates); did some cleanup of the ActionDefault class.

  Revision 1.32  1998/04/28 18:02:50  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.31  1998/03/05 08:10:52  zhenhai
  Added ability to view 3D graphs from six directions. Use -gl option to run,
  and click key x,y,z and X,Y,Z to select the direction you are viewing.
  Use arrow keys to pan left right up and down.

  Revision 1.30  1997/11/24 23:14:47  weaver
  Changes for the new ColorManager.

  Revision 1.29  1997/09/05 22:36:13  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.28  1997/05/30 15:41:17  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.27  1997/02/26 16:31:21  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.26.4.2  1997/02/14 23:29:12  wenger
  Fixed off-by-one-hour error in YyDdd_HhMmComposite composite parser;
  fixed another bug in point queries.

  Revision 1.26.4.1  1997/02/14 19:45:19  wenger
  Fixed bug 158; bumped version up to 1.3.2.

  Revision 1.26  1996/11/26 09:33:37  beyer
  Added debugging statement

  Revision 1.25  1996/11/02 00:21:36  flisakow
  Modified the "drilling down" to data to be more generous.  If the pointer
  is not positioned over the center of the record, a second scan is
  performed over the data using the max symbol size to determine the
  range of the scan.

  Revision 1.24  1996/09/13 16:44:55  wenger
  PixelSize in GData now used as multiplier to the size when symbols are
  larger than one screen pixel (makes the '+' and '-' keys more useful).

  Revision 1.23  1996/08/07 20:11:43  wenger
  Fixed various key event-related bugs (see Bugs file); changed
  direction of camera movement for 3D to be more in agreement
  with how panning works for 2D views.

  Revision 1.22  1996/08/04 21:12:57  beyer
  Added support for devise keys

  Revision 1.21  1996/07/21 02:17:35  jussi
  Added testing of xyZoom flag to determine zoom mode (XY or X/Y).

  Revision 1.20  1996/06/27 15:46:11  jussi
  Moved key '5' functionality to ViewGraph::UpdateAutoScale().

  Revision 1.19  1996/06/24 19:43:52  jussi
  Querying records of a 3D view now produces a popup
  window that tells the user that 3D queries are not
  yet supported.

  Revision 1.18  1996/06/21 19:17:10  jussi
  Added correct values for theta, phi, and rho when '5' is pressed
  in radial coordinate mode.

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
#include <ctype.h>

#include "ActionDefault.h"
#include "TDataMap.h"
#include "ViewGraph.h"
#include "RecInterp.h"
#include "QueryProc.h"
#include "TData.h"
#include "DeviseKey.h"
#include "DepMgr.h"
#include "PileStack.h"
#include "Session.h"

//#define DEBUG

ActionDefault::ActionDefault(char *name, Coord leftEdge,
			     Boolean useLeftFlag, Coord rightEdge,
			     Boolean useRightFlag):
     Action(name)
{
}

void ActionDefault::KeySelected(ViewGraph *view, int key, Coord x, Coord y)
{
#if defined(DEBUG)
  if (view->GetName() != NULL)
  {
    printf("ActionDefault::KeySelected(%s, 0x%x, %g, %g)\n",
         view->GetName(), (int)key, x, y);
  }
  else
  {
    printf("ActionDefault::KeySelected(0x%x, %g, %g)\n", (int)key, x, y);
  }
#endif
    const int minPixelWidth = 1;
    const int maxPixelWidth = 30;

    view->SelectView();
    VisualFilter filter;

    switch(key) {
      case '+': {
	  /* increase pixel size */
	  Boolean changed = false;
	  int index = view->InitMappingIterator();
	  while(view->MoreMapping(index)) {
	      TDataMap *map = view->NextMapping(index)->map;
	      if (map->GetPixelWidth() < maxPixelWidth) {
		  changed = true;
		  map->SetPixelWidth(map->GetPixelWidth() + 1);
	      }
	  }
	  view->DoneMappingIterator(index);
	  if (changed) {
	    DepMgr::Current()->RegisterEvent(view->dispatcherCallback,
	      DepMgr::EventViewSymbolSizeCh);
	    view->Refresh();
	  }
	  break;
      }
      case '-': {
	  /* decrease pixel size */
	  Boolean changed = false;
	  int index = view->InitMappingIterator();
	  while(view->MoreMapping(index)) {
	      TDataMap *map = view->NextMapping(index)->map;
	      if (map->GetPixelWidth() > minPixelWidth) { 
		  changed = true;
		  map->SetPixelWidth(map->GetPixelWidth() - 1);
	      }
	  }
	  view->DoneMappingIterator(index);
	  if (changed) {
	    DepMgr::Current()->RegisterEvent(view->dispatcherCallback,
	      DepMgr::EventViewSymbolSizeCh);
	    view->Refresh();
	  }
	  break;
      }
      case '5':
      case DeviseKey::KP_5:
      case DeviseKey::BEGIN:
      case DeviseKey::KP_BEGIN: {
	  view->GoHome(true);
	  break;
      }
      
      default: {
	  Action::KeySelected(view, key, x, y);
      }
    } // end switch
}

static const int MAX_MSGS = 128;
static const int MSG_BUF_SIZE = 4096;
static char msgBuf[MSG_BUF_SIZE]; // this does need to be static.  RKW 1998-12-14.
static int msgIndex;
static char *msgPtr[MAX_MSGS];
static int numMsgs;

static Boolean CheckParams()
{
    if (numMsgs >= MAX_MSGS || msgIndex >= MSG_BUF_SIZE) {
      return false;
    } else {
      return true;
    }
}

static Boolean PutMessage(char *msg)
{
#if defined(DEBUG)
    printf("PutMessage(%s)\n", msg);
#endif

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

static void
PutTooMuchMsg()
{
  char buf[128];
  sprintf(buf, "%s%s", "(more data than buffer can hold)",
      Session::GetIsJsSession() ? "" : " (see text window)");
  int length = strlen(buf) + 1;

  while(numMsgs >= MAX_MSGS || msgIndex + length >= MSG_BUF_SIZE) {
    numMsgs--;
    msgIndex = msgPtr[numMsgs] - msgBuf;
  }

  PutMessage(buf);
}

static Boolean InitPutMessage(double x, AttrType xAttr,
			      double y, AttrType yAttr)
{
#if defined(DEBUG)
    printf("InitPutMessage(%g, %g)\n", x, y);
#endif

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
    } else {
        sprintf(buf,"y: %.2f", y);
    }

    return PutMessage(buf);
}

static void EndPutMessage(int &numMessages, char **&msgs)
{
#if defined(DEBUG)
    printf("EndPutMessage()\n");
#endif

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
#if defined(DEBUG)
    char *viewName = view->GetName();
    if (viewName == NULL) viewName = "NULL";
    printf("ActionDefault::PopUp(%s, %g, %g, %g, %g, %d)\n", viewName, x, y,
      xHigh, yHigh, button);
#endif

    view->SelectView();
    
    AttrType xAttr = view->GetXAxisAttrType();
    AttrType yAttr = view->GetYAxisAttrType();

    InitPutMessage((x + xHigh) / 2.0, xAttr, (y + yHigh) / 2.0, yAttr);
    
    if (view->GetNumDimensions() != 2) {
        PutMessage("");
        PutMessage("Record query not supported yet");
        PutMessage("for this type of view.");
        EndPutMessage(numMsgs, msgs);
        return true;
    }

    char *errorMsg = "";
    Boolean printedRecords = false;
    if (view->IsInPileMode()) {
        PileStack *ps = view->GetParentPileStack();
	if (ps) {
	    int index = ps->InitIterator();
	    while (ps->More(index)) {
	        ViewGraph *tmpView = (ViewGraph *)ps->Next(index);
		if (PrintRecords(tmpView, x, y, xHigh, yHigh, errorMsg)) {
		    printedRecords = true;
		}
	    }
	    ps->DoneIterator(index);
	}
    } else if (PrintRecords(view, x, y, xHigh, yHigh, errorMsg)) {
        printedRecords = true;
    }

    if (!printedRecords) {
        PutTooMuchMsg();
    }

    EndPutMessage(numMsgs, msgs);

    return true;
}

Boolean ActionDefault::PrintRecords(ViewGraph *view, Coord x, Coord y, 
				    Coord xHigh, Coord yHigh, char *&errorMsg)
{
#if defined(DEBUG)
    printf("ActionDefault::PrintRecords <%s> %.2f %.2f %.2f %.2f\n",
           view->GetName(), x, y, xHigh, yHigh);
#endif

    static RecInterp *recInterp = NULL;
    
    if (!recInterp) {
      recInterp = new RecInterp;
    }
    
    /* get mapping */
    TDataMap *map = view->GetFirstMap();
    if (!map) {
        errorMsg = "No mapping found!";
        return false;
    }

    int numRecs;
    QueryProc *qp = QueryProc::Instance();

#if defined(DEBUG)
    printf("ActionDefault::PrintRecords: getting TData\n");
#endif

    int numDimensions;
    VisualFlag *dimensionInfo;
    numDimensions = map->DimensionInfo(dimensionInfo);

    // Note: changed approxFlag to always be false here, because having it
    // be true means that the Y axis attribute will be ignored in the query.
    // RKW 1998-11-09.
    Boolean approxFlag = false;
    
    TData *tdata = map->GetPhysTData();
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
    filter.yLow = y;
    filter.yHigh = yHigh;

    if (!approxFlag) {
        // for scatter plot, we filter using Y coordinates as well;
        // allow user inaccuracy that is five times larger than for
        // sortedX views (where approximation is used anyway), that is,
        // roughly five pixels in each direction
	// Note: changed the factor -- RKW 1999-08-11.
        filter.flag = VISUAL_X | VISUAL_Y;
        float xDiff = 2.0 * fabs(xHigh - x);
        float yDiff = 2.0 * fabs(yHigh - y);
        filter.xLow  = x - xDiff / 2;
        filter.xHigh = x + xDiff / 2;
        filter.yLow  = y - yDiff / 2;
        filter.yHigh = y + yDiff / 2;
    }

#if defined(DEBUG)
    printf("running query: %d x:(%f,%f) y:(%f,%f)\n",
           approxFlag, filter.xLow, filter.xHigh, filter.yLow, filter.yHigh);
#endif

    qp->InitTDataQuery(map, filter, approxFlag);
    Boolean tooMuch = GetRecords(qp, recInterp, tdata, errorMsg, numRecs);
    qp->DoneTDataQuery();

    if (approxFlag || numRecs) {
        return(!tooMuch);
    }

#if defined(DEBUG)
    printf("Didn't get any records on the first try.\n");
#endif

    //
    // Continue only if we have a constant Fixed Text Label symbol type
    // (bounding boxes for that symbol type don't work).
    //
    TDataMap *tdMap = view->GetFirstMap();
    const GDataAttrOffset *gdOffsets = tdMap->GetGDataOffset();
    if (gdOffsets->_shapeOffset >= 0) {
      // symbol type isn't constant
      return(!tooMuch);
    } else if (tdMap->GetShape(NULL) != 16) {
      // symbol type isn't Fixed Text Label
      return(!tooMuch);
    }

//~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

    // Try it again, with a larger visual filter to try to find
    // records.

    // for scatter plot, we filter using Y coordinates as well;
    // allow user inaccuracy that is five times larger than for
    // sortedX views (where approximation is used anyway), that is,
    // roughly five pixels in each direction
    filter.flag = VISUAL_X | VISUAL_Y;

    // Make the visual filter 20% of the view's width and 20% of the
    // view's height (values chosen fairly arbitrarily).
    const VisualFilter *viewVisFilterP = view->GetVisualFilter();
    Coord width = (viewVisFilterP->xHigh - viewVisFilterP->xLow) * 0.2;
    Coord height = (viewVisFilterP->yHigh - viewVisFilterP->yLow) * 0.1;

    filter.xLow  = x - width / 2;
    filter.xHigh = x + width / 2;
    filter.yLow  = y - height / 2;
    filter.yHigh = y + height / 2;

#if defined(DEBUG)
    printf("running query: %d x:(%f,%f) y:(%f,%f)\n",
           approxFlag, filter.xLow, filter.xHigh, filter.yLow, filter.yHigh);
#endif

    qp->InitTDataQuery(map, filter, approxFlag);
    tooMuch = GetRecords(qp, recInterp, tdata, errorMsg, numRecs);
    qp->DoneTDataQuery();

    return(!tooMuch);
}

Boolean
ActionDefault::GetRecords(QueryProc *qp, RecInterp *recInterp, TData *tdata,
    char *&errorMsg, int &numRecs)
{
    numRecs = 0;

    Boolean tooMuch = false;
    RecId startRid;
    char *tdBuf;
    AttrList *attrs = tdata->GetAttrList();

    while (qp->GetTData(startRid, numRecs, tdBuf)) {
        char *ptr = tdBuf;
        for(int i = 0; i < numRecs; i++) {
            recInterp->SetBuf(ptr);
            ptr += tdata->RecSize();
            
            if (!tooMuch) {
              PutMessage("");
            } else if (Session::GetIsJsSession()) {
              puts("");
	    }

            // set up for display inside the window

            for(int j = 0; j < attrs->NumAttrs(); j++) {
                char attrBuf[128];
                recInterp->PrintAttr(attrBuf, j, true);
                if (!tooMuch) {
                    if (!PutMessage(attrBuf)) {
			if (Session::GetIsJsSession()) {
                          errorMsg = "Too much data to show.";
			} else {
                          errorMsg = "See text window.";
                          PrintMsgBuf();
			}
                        tooMuch = true;
                    }
                } else if (!Session::GetIsJsSession()) {
                    puts(attrBuf);
                }
            }
        }
    }

    return tooMuch;
}
