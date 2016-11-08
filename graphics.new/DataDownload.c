/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2012
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DataDownload class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  2012/04/30 22:21:19  wenger
  Merged js_data_save_br_0 thru js_data_save_br_1 to trunk.

  Revision 1.1.2.6  2012/04/30 20:40:04  wenger
  (Hopefully final) cleanup.

  Revision 1.1.2.5  2012/04/30 19:08:42  wenger
  Fixed toolbar icons for data download; added mouse cursor icon for
  data download, and set the mouse cursor when we go into data download
  mode; added data download info to the JavaScreen help page; updated
  JS & DEVise version histories; put view names back into the download
  files because I think piles are too confusing otherwise.

  Revision 1.1.2.4  2012/04/27 15:36:24  wenger
  We now escape any commas in the actual data strings for data download.

  Revision 1.1.2.3  2012/04/26 22:57:39  wenger
  Created the DrillDown::AdjustFilterForCountMapping() method, and
  DataDownload uses DrillDown::GAttrLinkFollower() to remove
  duplicate code; removed some debug/test code.

  Revision 1.1.2.2  2012/04/24 20:46:52  wenger
  Actual data saving now working except for some things like escaping
  commas; the code still needs some cleanup; JS GUI and help still need work.

  Revision 1.1.2.1  2012/04/24 20:09:58  wenger
  Saving of actual data values now mostly working (print format
  needs some fixes).

 */

#include <stdio.h>

#include "DataDownload.h"
#include "QueryProc.h"
#include "TData.h"
#include "TDataMap.h"
#include "RecInterp.h"
#include "DevError.h"
#include "DrillDown.h"

static const int DEBUG = 0;

//-----------------------------------------------------------------------------

DataDownload *DataDownload::_instance = NULL;

//-----------------------------------------------------------------------------
DataDownload *
DataDownload::GetInstance()
{
    if (DEBUG >= 1) {
        printf("DataDownload::GetInstance()\n");
    }

    if (_instance == NULL) {
        _instance = new DataDownload();
    }

    _instance->Clear();

    return _instance;
}

//-----------------------------------------------------------------------------
DevStatus
DataDownload::SaveData(ViewData *view, Boolean useVisualFilter, FILE *fp)
{
    if (DEBUG >= 1) {
	printf("DataDownload::SaveData(%s)\n", view->GetName());
    }
    DevStatus result(StatusOk);

    _fp = fp;

    // At first we thought we'd take out the view name, but now I think
    // piles are too confusing without the view names.  We want to
    // add some further descriptive information here in phase 2.
    // wenger 2012-04-30
    fprintf(_fp, "# Data from view %s\n", view->GetName());

    fprintf(_fp, "# Data/view description: (not yet implemented)\n");

    _map = view->GetFirstMap();
    _tdata = _map->GetPhysTData();
    _attrs = _tdata->GetAttrList();
    if (!_attrs) {
	reportErrNosys("No attribute info!");
        result += StatusFailed;
	return result;
    }
    _recInterp->SetAttrs(_attrs);

    fprintf(_fp, "# Data source %s\n", _tdata->GetName());

    fprintf(_fp, "# Fields: ");
    const char *separator = "";
    _attrs->InitIterator();
    while (_attrs->More()) {
	AttrInfo *info = _attrs->Next();
    	fprintf(_fp, "%s%s", separator, info->name);
	separator = ",";
    }
    _attrs->DoneIterator();
    fprintf(_fp, "\n");

    if (DrillDown::EmptyTData(view)) {
    	fprintf(_fp, "# Empty data source\n");
        return result;
    }

    result += RunQuery(view, useVisualFilter);

    return result;
}

/*===========================================================================*/

//-----------------------------------------------------------------------------
DataDownload::DataDownload()
{
    if (DEBUG >= 1) {
        printf("DataDownload::DataDownload()\n");
    }

    _recInterp = new RecInterp();
    Clear();
}

//-----------------------------------------------------------------------------
DataDownload::~DataDownload()
{
    if (DEBUG >= 1) {
        printf("DataDownload::~DataDownload()\n");
    }

    delete _recInterp;
    _recInterp = NULL;
    Clear();
}

//-----------------------------------------------------------------------------
void
DataDownload::Clear()
{
    if (DEBUG >= 1) {
        printf("DataDownload::Clear()\n");
    }

    _fp = NULL;
    _map = NULL;
    _tdata = NULL;
    _attrs = NULL;
}

//-----------------------------------------------------------------------------
DevStatus
DataDownload::RunQuery(ViewData *view, Boolean useVisualFilter)
{
    if (DEBUG >= 1) {
        printf("DataDownload::RunQuery(%s)\n", view->GetName());
    }

    DevStatus result(StatusOk);

    // Set up the visual filter to use for the query.
    VisualFilter filter;
    if (useVisualFilter) {
    	view->GetVisualFilter(filter);

        // If there's a count mapping, change the query filter accordingly.
        DrillDown::AdjustFilterForCountMapping(view, filter);
    } else {
    	filter.flag = 0;
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
	ProcessData(view, qp);

        qp->DoneTDataQuery();
    }

    return result;
}

//-----------------------------------------------------------------------------
DevStatus
DataDownload::ProcessData(ViewData *view, QueryProc *qp)
{
    if (DEBUG >= 2) {
        printf("DataDownload::ProcessData(%s)\n", view->GetName());
    }

    DevStatus result(StatusOk);

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
	DrillDown::GAttrLinkFollower(view, _map, startRid, tdBuf, numRecs, symInfo);

	char *tdPtr = tdBuf;
	for (int recNum = 0; recNum < numRecs; ++recNum) {
	    if (symInfo[recNum].inGAttrLink) {
	        _recInterp->SetBuf(tdPtr);
	        ShowRecord();
	        tdPtr += _tdata->RecSize();
	    }
	}
    }

    return result;
}

//-----------------------------------------------------------------------------
DevStatus
DataDownload::ShowRecord()
{
    DevStatus result(StatusOk);

    const char *separator = "";
    char attrBuf[1024];
    for (int attrNum = 0; attrNum < _attrs->NumAttrs(); ++attrNum) {
	_recInterp->PrintAttr(attrBuf, sizeof(attrBuf), attrNum, false);
	// We're writing a CSV file, so escape any commas in actual
	// data strings.
	if (!EscapeChars(attrBuf, sizeof(attrBuf), ",").IsComplete()) {
	  reportErrNosys("Warning: buffer overflow; record is truncated");
	}
	fprintf(_fp, "%s%s", separator, attrBuf);
	separator = ",";
    }
    fprintf(_fp, "\n");

    return result;
}

/*===========================================================================*/
