/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001-2003
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DrillDown3D class.

  Note: we might eventually have to change this to take piles into account;
  however, because of the way 3D piles are implemented in the JS, I don't
  think we have to do that right now.  RKW 2001-05-18.
 */

/*
  $Id$

  $Log$
  Revision 1.4.10.2  2003/11/21 23:05:11  wenger
  Drill-down now works properly on views that are GAttr link followers
  (fixed bug 893).

  Revision 1.4.10.1  2003/04/24 17:31:16  wenger
  Fixed bug in drill-down code; axis date formats now also apply to
  drill-down coordinates; we now check for buffer overruns in
  DateString().

  Revision 1.4  2001/05/27 18:51:16  wenger
  Improved buffer checking with snprintfs.

  Revision 1.3  2001/05/24 18:42:02  wenger
  Fixed bug 674 (drill-down doesn't work correctly on record link follower
  views).

  Revision 1.2  2001/05/18 21:14:58  wenger
  Fixed bug 671 (potential GData buffer overflow).

  Revision 1.1  2001/05/18 19:25:35  wenger
  Implemented the DEVise end of 3D drill-down; changed DEVise version to
  1.7.3.

 */


#include <stdio.h>

#include "DrillDown3D.h"
#include "ViewData.h"
#include "TDataMap.h"
#include "TData.h"
#include "QueryProc.h"
#include "AttrList.h"
#include "RecInterp.h"

#define DEBUG 0

//-----------------------------------------------------------------------------
// Constructor.
DrillDown3D::DrillDown3D()
{
#if (DEBUG >= 1)
    printf("DrillDown3D::DrillDown3D()\n");
#endif

    _status = StatusOk;

    _recordCount = 0;

    _gdataBuf = new char[GDATA_BUF_SIZE]; // must be aligned for double
    _recInterp = new RecInterp;
}

//-----------------------------------------------------------------------------
// Destructor.
DrillDown3D::~DrillDown3D()
{
#if (DEBUG >= 1)
    printf("DrillDown3D::~DrillDown3D()\n");
#endif

    delete [] _gdataBuf;
    delete _recInterp;
}

//-----------------------------------------------------------------------------
// Run the appropriate query and generate the TData information.
DevStatus
DrillDown3D::RunQuery(ViewData *view, int count, Point3D coords[])
{
#if (DEBUG >= 1)
    printf("DrillDown3D::GetRecords(%s, ", view->GetName());
    for (int index = 0; index < count; index++) {
        printf("(%g, %g, %g) ", coords[index].x_, coords[index].y_,
	  coords[index].z_);
    }
    printf(")\n");
#endif

    if (view->GetDrillDownDisabled()) {
        printf("Drill down disabled in view <%s>\n", view->GetName());
	_status += StatusCancel;
    }

    if (count > 0) {
        _count = count;
        _coords = coords;

        GetMapping(view);
	_callback = view->GetQueryCallback();
        SetVisualFilter();
        ExecuteQuery(view);
    }

#if (DEBUG >= 1)
    printf("  Found %d records\n", _recordCount);
#endif

    return _status;
}

//-----------------------------------------------------------------------------
// Get the results that we generated with RunQuery().
void
DrillDown3D::GetResults(int &count, const char *const *& messages)
{
#if (DEBUG >= 1)
    printf("DrillDown3D::GetResults()\n");
#endif

    if (_status.IsComplete()) {
        count = _results.GetCount();
        messages = _results.GetArgs();
    } else {
        count = 0;
        messages = NULL;
    }
}

//-----------------------------------------------------------------------------
// Get the TDataMap and related info for the view.
void
DrillDown3D::GetMapping(ViewData *view)
{
#if (DEBUG >= 2)
    printf("DrillDown3D::GetMapping()\n");
#endif

    if (_status.IsComplete()) {
        _tdMap = view->GetFirstMap();
        if (!_tdMap) {
	    reportErrNosys("No mapping found!");
	    _status += StatusFailed;
        } else {
            TData *tdata = _tdMap->GetPhysTData();
	    _attrs = tdata->GetAttrList();
	    if (!_attrs) {
	        reportErrNosys("No attribute info!");
		_status += StatusFailed;
	    } else {
	        _recInterp->SetAttrs(_attrs);
	    }
	}
    }
}

//-----------------------------------------------------------------------------
// Set the visual filter for the query according to the GData values we
// need to find.
void
DrillDown3D::SetVisualFilter()
{
#if (DEBUG >= 2)
    printf("DrillDown3D::SetVisualFilter()\n");
#endif

    if (_status.IsComplete()) {

	// Set the visual filter (X and Y only) to include all of the
	// coordinates.
	_filter.flag = VISUAL_X | VISUAL_Y;
	_filter.xLow = _coords[0].x_;
	_filter.xHigh = _coords[0].x_;
	_filter.yLow = _coords[0].y_;
	_filter.yHigh = _coords[0].y_;

	for (int index = 1; index < _count; index++) {
	    _filter.xLow = MIN(_filter.xLow, _coords[index].x_);
	    _filter.xHigh = MAX(_filter.xHigh, _coords[index].x_);
	    _filter.yLow = MIN(_filter.yLow, _coords[index].y_);
	    _filter.yHigh = MAX(_filter.yHigh, _coords[index].y_);
	    
	}

	// Give ourselves a little margin.
        Coord xMargin = (_filter.xHigh - _filter.xLow) * 0.001;
        Coord yMargin = (_filter.yHigh - _filter.yLow) * 0.001;
	_filter.xLow -= xMargin;
	_filter.xHigh += xMargin;
	_filter.yLow -= yMargin;
	_filter.yHigh += yMargin;
    }
}

//-----------------------------------------------------------------------------
// Execute the actual query and get the results.
void
DrillDown3D::ExecuteQuery(ViewData *view)
{
#if (DEBUG >= 2)
    printf("DrillDown3D::ExecuteQuery()\n");
#endif

    if (_status.IsComplete()) {
        QueryProc *qp = QueryProc::Instance();
	qp->InitTDataQuery(_tdMap, _filter, _callback, false);

	RecId startRid;
	int numRecs;
	char *tdBuf;
	
	while (qp->GetTData(startRid, numRecs, tdBuf)) {
#if (DEBUG >= 3)
	    printf("Got %d TData records\n", numRecs);
#endif
	    //TEMP -- we should probably deal here with having to call
	    // ConvertToGData() several times because of the buffer being
	    // too small.
	    _tdMap->ConvertToGData(startRid, tdBuf, numRecs,
	      (void *)_gdataBuf, GDATA_BUF_SIZE);

	    // Check records against any GAttr links that the view is a
	    // follower of.
	    ViewData::SymbolInfo symInfo[WINDOWREP_BATCH_SIZE];
	    view->GAttrLinkFollower(_tdMap, _gdataBuf, numRecs,
	      _tdMap->GDataRecordSize(), symInfo);

	    char *tdPtr = tdBuf;
	    char *gdPtr = _gdataBuf;
	    for (int index = 0; index < numRecs; index++) {
	        ProcessRecord(tdPtr, gdPtr);
		tdPtr += _tdMap->TDataRecordSize();
		gdPtr += _tdMap->GDataRecordSize();
	    }
        }
    }
}

//-----------------------------------------------------------------------------
// Determine whether a record we got from the query matches one of the
// points we're looking for.
void
DrillDown3D::ProcessRecord(char *tData, char *gData)
{
#if (DEBUG >= 3)
    printf("DrillDown3D::ProcessRecord()\n");
#endif

    Coord gdX = _tdMap->GetX(gData);
    Coord gdY = _tdMap->GetY(gData);
    Coord gdZ = _tdMap->GetZ(gData);

    for (int index = 0; index < _count; index++) {
        if (dequal(gdX, _coords[index].x_) && dequal(gdY, _coords[index].y_) &&
	  dequal(gdZ, _coords[index].z_)) {
	    _recordCount++;
	    PrintTData(gdX, gdY, gdZ, tData);
	}
    }
}

//-----------------------------------------------------------------------------
// Print the info about the given record to the results.
void
DrillDown3D::PrintTData(Coord gdX, Coord gdY, Coord gdZ, char *tData)
{
#if (DEBUG >= 3)
    printf("DrillDown3D::PrintTData()\n");
#endif

    _recInterp->SetBuf(tData);

    if (_results.GetCount() > 0) {
	// Record divider.
        _results.AddArg("");
    }

    const int bufSize = 128;
    char attrBuf[bufSize];
    int formatted;

    formatted = snprintf(attrBuf, bufSize, "x: %g", gdX);
    checkAndTermBuf(attrBuf, bufSize, formatted);
    _results.AddArg(attrBuf);

    formatted = snprintf(attrBuf, bufSize, "y: %g", gdY);
    checkAndTermBuf(attrBuf, bufSize, formatted);
    _results.AddArg(attrBuf);

    formatted = snprintf(attrBuf, bufSize, "z: %g", gdZ);
    checkAndTermBuf(attrBuf, bufSize, formatted);
    _results.AddArg(attrBuf);

    for (int index = 0; index < _attrs->NumAttrs(); index++) {
	_recInterp->PrintAttr(attrBuf, bufSize, index, true);
	_results.AddArg(attrBuf);
    }

}

/*===========================================================================*/
