/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2003
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DrillDown class (2D drill-down).
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.3  2003/11/26 17:02:12  wenger
  Fixed bug 896 (drill-down fails on views with count mappings).

  Revision 1.1.2.2  2003/11/21 23:05:11  wenger
  Drill-down now works properly on views that are GAttr link followers
  (fixed bug 893).

  Revision 1.1.2.1  2003/11/21 18:45:07  wenger
  Totally restructured 2D drill-down code into new DrillDown class;
  increased the tolerance for not being *exactly* on a symbol with
  the mouse.

 */

#include <stdio.h>

#include "DrillDown.h"
#include "Session.h"
#include "ViewData.h"
#include "QueryProc.h"
#include "TData.h"
#include "RecInterp.h"
#include "WindowRep.h"
#include "DevError.h"

static const int DEBUG = 0;

//-----------------------------------------------------------------------------

DrillDown *DrillDown::_instance = NULL;

//-----------------------------------------------------------------------------
DrillDown *
DrillDown::GetInstance()
{
    if (DEBUG >= 1) {
        printf("DrillDown::GetInstance()\n");
    }

    if (_instance == NULL) {
        _instance = new DrillDown();
    }

    return _instance;
}

//-----------------------------------------------------------------------------
DevStatus
DrillDown::GetData(ViewData *view, Coord drillX, Coord drillY,
  Coord pixelX, Coord pixelY, int &numMsgs, char **&msgs)
{
    if (DEBUG >= 1) {
        printf("DrillDown::GetData(%s, %g, %g, %g, %g)\n", view->GetName(),
	  drillX, drillY, pixelX, pixelY);
    }

    DevStatus result(StatusOk);

    AttrType xAttr = view->GetXAxisAttrType();
    AttrType yAttr = view->GetYAxisAttrType();

    if (!InitPutMessage(view, drillX, xAttr, drillY, yAttr)) {
        result += StatusFailed;
    }

    if (view->GetNumDimensions() != 2) {
        ProcessMessage("");
	ProcessMessage("Record query not supported yet");
	ProcessMessage("for this type of view.");
    } else {
	//
	// Okay, here's the heart of the search.  We start out looking for
	// symbols that cover the exact point where the drill-down happened;
	// if we don't find any TData records we try again with some tolerance
	// around the drill-down location.  It might make sense to expand
	// the tolerance more slowly, and allow it to get larger, but I'm
	// worried about the cost of the queries, because each TData query
	// potentially touches *every* TData record and also converts it
	// to GData (unless we can do a binary search).  Therefore, for now,
	// I'm setting up the tolerances to just do one more try if we
	// don't get a hit on the first try.  wenger 2003-11-21.
	//
        Boolean gotData = false;
        int pixelTol = 0; // Tolerance around click location
        while (!gotData && pixelTol <= PIXEL_TOL_MAX) {
	    result += ProcessView(view, drillX, drillY, pixelX, pixelY,
	      pixelTol, gotData);
            pixelTol += PIXEL_TOL_INC;
        }

	if (!_spaceLeft) {
	    PutTooMuchMsg();
	}
    }

    EndPutMessage(numMsgs, msgs);

    return result;
}

/*===========================================================================*/

//-----------------------------------------------------------------------------
DrillDown::DrillDown()
{
    if (DEBUG >= 1) {
        printf("DrillDown::DrillDown()\n");
    }

    _recInterp = new RecInterp();
}

//-----------------------------------------------------------------------------
DrillDown::~DrillDown()
{
    if (DEBUG >= 1) {
        printf("DrillDown::~DrillDown()\n");
    }

    delete _recInterp;
}

//-----------------------------------------------------------------------------
DevStatus
DrillDown::ProcessView(ViewData *view, Coord drillX, Coord drillY,
  Coord pixelX, Coord pixelY, int pixelTol, Boolean &gotData)
{
    if (DEBUG >= 1) {
        printf("DrillDown::ProcessView(%s, %g, %g, %g, %g, %d)\n",
	  view->GetName(), drillX, drillY, pixelX, pixelY, pixelTol);
    }

    DevStatus result(StatusOk);
    gotData = false;

    if (view->IsInPileMode()) {
        PileStack *ps = view->GetParentPileStack();
        if (ps) {
            int index = ps->InitIterator();
            while (ps->More(index)) {
                ViewData *tmpView = (ViewData *)ps->Next(index);
		Boolean tmpGotData;
		result += RunQuery(tmpView, drillX, drillY, pixelX, pixelY,
		  pixelTol, tmpGotData);
	        if (tmpGotData) gotData = true;
            }
            ps->DoneIterator(index);
        }
    } else {
        result += RunQuery(view, drillX, drillY, pixelX, pixelY, pixelTol,
	  gotData);
    }

    return result;
}

//-----------------------------------------------------------------------------
DevStatus
DrillDown::RunQuery(ViewData *view, Coord drillX, Coord drillY, Coord pixelX,
  Coord pixelY, int pixelTol, Boolean &gotData)
{
    if (DEBUG >= 1) {
        printf("DrillDown::RunQuery(%s, %g, %g, %g, %g, %d)\n",
	  view->GetName(), drillX, drillY, pixelX, pixelY, pixelTol);
    }

    DevStatus result(StatusOk);
    gotData = false;

    // Set up the visual filter to use for the query.
    VisualFilter filter;
    filter.flag = VISUAL_X | VISUAL_Y;
    filter.xLow = drillX - pixelX * pixelTol;
    filter.xHigh = drillX + pixelX * pixelTol;
    filter.yLow = drillY - pixelY * pixelTol;
    filter.yHigh = drillY + pixelY * pixelTol;

    // If there's a count mapping, change the query filter accordingly.
    Boolean enabled;
    char *countAttr;
    char *putAttr;
    int initialValue;
    view->GetCountMapping(enabled, countAttr, putAttr, initialValue);
    if (enabled) {
        if (!strcmp(putAttr, "X")) {
	    // Don't query on X; note that we can't do a query with the
	    // flag set to VISUAL_Y;
	    filter.xLow = -MAXFLOAT;
	    filter.xHigh = MAXFLOAT;
	} else if (!strcmp(putAttr, "Y")) {
	    // Don't query on Y.
            filter.flag = VISUAL_X;
	}
    }

    if (DEBUG >= 2) {
        printf("  Filter for query: %d x:(%f,%f) y:(%f,%f)\n", filter.flag,
	  filter.xLow, filter.xHigh, filter.yLow, filter.yHigh);
    }

    TDataMap *map = view->GetFirstMap();
    if (!map) {
	reportErrNosys("No mapping found for view!");
        result += StatusFailed;
    } else {
        QueryProc *qp = QueryProc::Instance();
	Boolean approxFlag = false;
        qp->InitTDataQuery(map, filter, view->GetQueryCallback(), approxFlag);

	TData *tdata = map->GetPhysTData();
	ProcessData(view, qp, tdata, map, gotData);

        qp->DoneTDataQuery();
    }

    return result;
}

//-----------------------------------------------------------------------------
DevStatus
DrillDown::ProcessData(ViewData *view, QueryProc *qp, TData *tdata,
  TDataMap *map, Boolean &gotData)
{
    if (DEBUG >= 2) {
        printf("DrillDown::ProcessData()\n");
    }

    DevStatus result(StatusOk);

    AttrList *attrs = tdata->GetAttrList();
    if (!attrs) {
	reportErrNosys("No attribute info!");
        result += StatusFailed;
	return result;
    }
    _recInterp->SetAttrs(attrs);

    RecId startRid;
    char *tdBuf;
    int numRecs;
    while (qp->GetTData(startRid, numRecs, tdBuf)) {
        if (DEBUG >= 3) {
            printf("    Got %d records starting at %ld\n", numRecs, startRid);
	}

	// Check records against any GAttr links that the view is a
	// follower of.
	ViewData::SymbolInfo symInfo[WINDOWREP_BATCH_SIZE];
	GAttrLinkFollower(view, map, startRid, tdBuf, numRecs, symInfo);

	char *tdPtr = tdBuf;
	for (int recNum = 0; recNum < numRecs; ++recNum) {
	    if (symInfo[recNum].inGAttrLink) {
	        gotData = true;
	        _recInterp->SetBuf(tdPtr);
	        ShowRecord(attrs);
	        tdPtr += tdata->RecSize();
	    }
	}
    }

    return result;
}

//-----------------------------------------------------------------------------
void
DrillDown::GAttrLinkFollower(ViewData *view, TDataMap *map, RecId startRid,
  char *tdBuf, int numRecs, ViewData::SymbolInfo symInfo[])
{
    if (DEBUG >= 2) {
        printf("DrillDown::GAttrLinkFollower()\n");
    }

    const int GDATA_BUF_SIZE = 6400 * sizeof(double);
    // Force buffer to be aligned on double.
    double alignBuf[GDATA_BUF_SIZE / sizeof(double)];
    char *gdBuf = (char *)alignBuf;

    map->ConvertToGData(startRid, tdBuf, numRecs, gdBuf, GDATA_BUF_SIZE);
    
    view->GAttrLinkFollower(map, gdBuf, numRecs, map->GDataRecordSize(),
      symInfo);
}

//-----------------------------------------------------------------------------
DevStatus
DrillDown::ShowRecord(AttrList *attrs)
{
    DevStatus result(StatusOk);

    // Blank space between records.
    ProcessMessage("");

    for (int attrNum = 0; attrNum < attrs->NumAttrs(); ++attrNum) {
        char attrBuf[128];
	_recInterp->PrintAttr(attrBuf, sizeof(attrBuf), attrNum, true);
	ProcessMessage(attrBuf);
    }

    return result;
}

//-----------------------------------------------------------------------------
Boolean
DrillDown::InitPutMessage(ViewData *view, double drillX, AttrType xAttr,
  double drillY, AttrType yAttr)
{
    if (DEBUG >= 1) {
        printf("DrillDown::InitPutMessage(%s, %g, %g)\n", view->GetName(),
          drillX, drillY);
    }

    if (Session::GetIsJsSession()) {
        _maxMsgs = MAX_MSGS;
    } else {
        _maxMsgs = MIN(MAX_MSGS, NON_JS_MAX_MSGS);
    }
    _msgBufIndex = 0;
    _numMsgs = 0;
    _spaceLeft = true;

    char buf[128];
    int formatted;

    if (xAttr == DateAttr) {
        formatted = snprintf(buf, sizeof(buf), "Drill-down x: %s",
	  DateString(drillX, view->GetXAxisDateFormat()));
    } else {
        formatted = snprintf(buf, sizeof(buf), "Drill-down x: %.2f", drillX);
    }
    checkAndTermBuf2(buf, formatted);

    if (!PutMessage(buf)) {
        return false;
    }
    
    if (yAttr == DateAttr) {
        formatted = snprintf(buf, sizeof(buf), "Drill-down y: %s",
	  DateString(drillY, view->GetYAxisDateFormat()));
    } else {
        formatted = snprintf(buf, sizeof(buf), "Drill-down y: %.2f", drillY);
    }
    checkAndTermBuf2(buf, formatted);

    return PutMessage(buf);
}

//-----------------------------------------------------------------------------
void
DrillDown::ProcessMessage(const char *msg)
{
    if (_spaceLeft) {
        if (!PutMessage(msg)) {
	    _spaceLeft = false;

	    if (!Session::GetIsJsSession()) {
	        PrintMsgBuf();
	    }
	}
    } else if (!Session::GetIsJsSession()) {
        puts(msg);
    }
}

//-----------------------------------------------------------------------------
// Returns true iff the message was successfully added to the buffer.
Boolean
DrillDown::PutMessage(const char *msg)
{
    if (DEBUG >= 4) {
        printf("PutMessage(%s)\n", msg);
    }

    int len = strlen(msg) + 1;
    if (!RoomForNewMessage(len)) {
        return false;
    }

    int formatted = snprintf(&_msgBuf[_msgBufIndex], len, "%s", msg);
    checkAndTermBuf(&_msgBuf[_msgBufIndex], len, formatted);
    _msgArray[_numMsgs++] = &_msgBuf[_msgBufIndex];
    _msgBufIndex += len;

    return true;
}

//-----------------------------------------------------------------------------
// Returns true iff we have enough room to add a message to the buffer.
Boolean
DrillDown::RoomForNewMessage(int msgLen)
{
    if (_numMsgs >= _maxMsgs || _msgBufIndex + msgLen > MSG_BUF_SIZE) {
        return false;
    } else {
	return true;
    }
}

//-----------------------------------------------------------------------------
void
DrillDown::PutTooMuchMsg()
{
    char buf[128];
    int formatted = snprintf(buf, sizeof(buf), "%s%s",
      "(more data than buffer can hold)",
      Session::GetIsJsSession() ? "" : " (see text window)");
    checkAndTermBuf2(buf, formatted);
    int length = strlen(buf) + 1;

    while (_numMsgs >= _maxMsgs || _msgBufIndex + length >= MSG_BUF_SIZE) {
        _numMsgs--;
        _msgBufIndex = _msgArray[_numMsgs] - _msgBuf;
    }

    PutMessage(buf);
}

//-----------------------------------------------------------------------------
void
DrillDown::EndPutMessage(int &numMessages, char **&msgs)
{
    if (DEBUG >= 1) {
        printf("EndPutMessage()\n");
    }

    numMessages = _numMsgs;
    msgs = _msgArray;
}

//-----------------------------------------------------------------------------
void
DrillDown::PrintMsgBuf()
{
    for(int i = 0; i < _numMsgs; i++) {
        puts(_msgArray[i]);
    }
}

/*===========================================================================*/
