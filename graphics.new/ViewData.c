/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.34.2.1  2013/06/13 21:03:01  wenger
  Changes to get DEVise to compile/link on CentOS6 (with comments for
  a bunch of unfixed warnings); minor mods to get this version to also
  compile on RHEL5...

  Revision 1.34  2013/01/28 20:37:43  wenger
  Changed a bunch more sprintfs to snprintfs.

  Revision 1.33  2009/05/13 22:41:30  wenger
  Merged x86_64_centos5_br_0 thru x86_64_centos5_br_1/dist_1_9_1x2 to
  the trunk.

  Revision 1.32.10.2  2009/05/06 20:19:19  wenger
  Got rid of extra debug output, cleaned up a few things.

  Revision 1.32.10.1  2009/05/01 22:26:41  wenger
  Debug code (and a few actual changes) trying to get DEVise to work
  on the x86_64/Centos 5 machines at BMRB (currently, opening
  histogram2.ds causes a core dump).

  Revision 1.32  2005/12/06 20:04:15  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.31  2003/01/13 19:25:28  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.30  2002/05/01 21:30:13  wenger
  Merged V1_7b0_br thru V1_7b0_br_1 to trunk.

  Revision 1.29.4.9  2005/09/06 21:20:18  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.29.4.8  2003/11/21 23:05:12  wenger
  Drill-down now works properly on views that are GAttr link followers
  (fixed bug 893).

  Revision 1.29.4.7  2003/09/08 20:53:22  wenger
  Fixed a bug in that the intersection of multiple GAttr links was
  not being computed correctly.

  Revision 1.29.4.6  2003/08/01 22:04:27  wenger
  Fixed bug 871 (home doesn't work right on GAttribute link followers).

  Revision 1.29.4.5  2003/06/16 16:42:06  wenger
  Fixed bug 879 (a problem with the home code) -- made pretty significant
  improvements to how we deal with the data ranges.

  Revision 1.29.4.4  2003/02/04 19:41:16  wenger
  Added union capability for multiple GData attribute links (will help
  with restraint visualizations for BMRB).

  Revision 1.29.4.3  2002/09/10 17:13:21  wenger
  Fixed bug 821 (GAttr links fail when follower has "complex" symbols)
  (this involved splitting up ViewData::ReturnGData() into smaller
  methods to make it easier to deal with); fixed bug 214 (record links
  fail if leader view has "complex" symbols); added debug output of
  total records processed and drawn to view data.

  Revision 1.29.4.2  2002/09/05 19:14:03  wenger
  Implemented GData attribute value links (but not GUI for creating
  them).

  Revision 1.29.4.1  2002/04/30 17:48:11  wenger
  More improvements to 'home' functionality, especially on QPFull_X
  queries.

  Revision 1.29  2001/12/28 18:34:40  wenger
  Fixed bugs 727 and 730 (problems with line graphs in DEVise).

  Revision 1.28  2001/07/19 17:19:49  wenger
  Fixed bug in home on linked/piled views.

  Revision 1.27  2001/07/13 22:17:14  wenger
  Automatic view home now takes symbol bounding boxes into account (fixes
  bugs 547 and 557).

  Revision 1.26  2001/04/03 19:57:40  wenger
  Cleaned up code dealing with GData attributes in preparation for
  "external process" implementation.

  Revision 1.25  2000/03/14 21:51:48  wenger
  Added more invalid object checking; had to take some memory checking
  out of client-side stuff for linking reasons.

  Revision 1.24  1999/07/21 18:51:14  wenger
  Moved alignment and data font information from view into mapping.

  Revision 1.23  1999/05/28 16:32:46  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.22  1999/05/26 19:50:55  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.21  1999/05/21 14:52:50  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.20  1999/05/20 15:17:56  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.19  1999/05/12 21:01:57  wenger
  Views containing view symbols can now be piled.

  Revision 1.18  1999/04/05 16:16:02  wenger
  Record- and set-link follower views with auto filter update enabled have
  'home' done on them after they are updated by a record link or set link.

  Revision 1.17  1999/03/12 18:46:05  wenger
  Implemented duplicate symbol elimination.

  Revision 1.16  1999/03/03 18:22:04  wenger
  Fixed bugs 426 and 432 (problems with '5' (home) key); fixed bugs 466
  and 467 (query errors with sorted attributes); minor improvements to
  view symbols.

  Revision 1.15  1999/02/11 19:54:59  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.14.2.1  1998/12/29 17:25:13  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.14  1998/12/01 20:04:32  wenger
  More reductions of memory usage in DEVise -- basically eliminated the
  histogram capability (this really saves a lot, since there are big
  structures in every ViewGraph for this); made creation of TDatas more
  efficient by bypassing command code.

  Revision 1.13  1998/11/09 20:33:26  wenger
  Fixed bug 433 (drill-down problem); improved debug output in various
  related modules.

  Revision 1.12  1998/10/29 21:46:08  wenger
  Added "proof-of-concept" code for filter links; added warning when
  more than one mapping is inserted into a view; updated bug and to-do
  lists.

  Revision 1.11  1998/10/20 19:46:17  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.10  1998/07/10 21:20:08  wenger
  Minor cleanups and improvements.

  Revision 1.9  1998/05/06 22:05:01  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.8  1998/04/30 14:24:22  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.7  1998/04/29 17:53:57  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.6  1998/04/10 18:29:32  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.5  1998/02/26 22:59:56  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.4  1998/02/20 06:17:14  beyer
  resurected histograms

  Revision 1.3  1998/02/13 15:51:36  wenger
  Changed ViewData to be based on old ViewScatter class instead of
  TDataViewX; ViewData now returns a list of the records drawn to
  the query processor; removed unused GDataBinX class.

*/

//******************************************************************************

#include "ViewData.h"
#include "TDataMap.h"
#include "Shape.h"
#include "Util.h"
#include "RecordLink.h"
#include "BooleanArray.h"
#include "CountMapping.h"
#include "DerivedTable.h"
#include "DupElim.h"
#include "MappingInterp.h"
#include "GAttrLink.h"

//#define DEBUG 0
//#define TEST_FILTER_LINK // TEMP -- remove later. RKW 1998-10-29.

#if defined(TEST_FILTER_LINK) // TEMP -- remove later. RKW 1998-10-29.
inline Coord GetShapeAttr(int shapeAttrNum, char *ptr, TDataMap *map,
  const GDataAttrOffset *offset)
{
  if (offset->_shapeAttrOffset[shapeAttrNum] < 0) {
    const ShapeAttr *attrs = map->GetDefaultShapeAttrs();
    return attrs[shapeAttrNum];
  }
  return GetAttr(ptr, _shapeAttrOffset[shapeAttrNum], Coord, offset);
}
#endif

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewData::ViewData(char* name, VisualFilter& initFilter, QueryProc* qp,
						 PColorID fgid, PColorID bgid, 
						 AxisLabel* xAxis, AxisLabel* yAxis, Action* action)
	:	ViewGraph(name, initFilter, qp, xAxis, yAxis, fgid, bgid, action)
{
#if (DEBUG >= 1)
	printf("ViewData::ViewData(0x%p, %s)\n", this, name);
#endif

    _objectValid.Set();
	_passTwoGData = NULL;
	_gAttrLinkMode = LinkIntersection;
}

//******************************************************************************
ViewData::~ViewData()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
	printf("ViewData::~ViewData(0x%p, %s)\n", this, _name);
#endif

  // We have to do this here, even though it's done in the ViewGraph
  // destructor, so that we unlink TAttrLinks before the ViewData stuff
  // is destroyed.
  UnlinkMasterSlave();

  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *table = _derivedTables.Next(index);
	delete table;
	_derivedTables.DeleteCurrent(index);
  }
  _derivedTables.DoneIterator(index);
}

//******************************************************************************
// Callback Methods (QueryCallback)
//******************************************************************************

void
ViewData::QueryInit(void* userData)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("ViewData(%s)::QueryInit()\n", GetName());
#endif

  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *table = _derivedTables.Next(index);
    if (!table->Initialize().IsComplete()) {
	  char errBuf[1024];
         int formatted = snprintf(errBuf, sizeof(errBuf),
	   "Error initializing table %s", table->GetName());
	 checkAndTermBuf2(errBuf, formatted);
	  reportErrNosys(errBuf);
    }
  }
  _derivedTables.DoneIterator(index);

  InvalidateDataRanges();

  _dataRangeFirst = true;
  _twoPass = false;

  _totalRecordsProcessed = 0;
  _totalRecordsDrawn = 0;

  ViewGraph::QueryInit(userData);
}

//******************************************************************************
void
ViewData::QueryDone(int bytes, void* userData, Boolean allDataReturned,
  TDataMap* map)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("ViewData(%s)::QueryDone(%d, %d)\n", GetName(), bytes,
      allDataReturned);
  printf("%d records processed\n", _totalRecordsProcessed);
  printf("%d records drawn\n", _totalRecordsDrawn);
#endif

  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *table = _derivedTables.Next(index);
    if (!table->Done().IsComplete()) {
	  char errBuf[1024];
         int formatted = snprintf(errBuf, sizeof(errBuf),
	   "Error terminating table %s", table->GetName());
	 checkAndTermBuf2(errBuf, formatted);
    }
  }
  _derivedTables.DoneIterator(index);

  if (_twoPass) {
    DrawPassTwo(map);
  }

  if (allDataReturned) {
    _dataRangesValid = true;
#if (DEBUG >= 1)
    printf("View <%s>: X: %g, %g; Y: %g, %g\n", GetName(), _dataXMin,
      _dataXMax, _dataYMin, _dataYMax);
#endif
  }

  ViewGraph::QueryDone(bytes, userData, allDataReturned, map);
}

//******************************************************************************
void	ViewData::ReturnGData(TDataMap* mapping, RecId recId,
								 void* gdata, int numGData,
								 int& recordsProcessed,
								 Boolean needDrawnList, int& recordsDrawn,
								 BooleanArray*& drawnList)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
	printf("ViewData(%s)::ReturnGData(%d, 0x%p, %d)\n", GetName(), (int)recId,
	  gdata, numGData);
    printf("Visual filter: (%g, %g), (%g, %g)\n", _queryFilter.xLow,
	  _queryFilter.yHigh, _queryFilter.xHigh, _queryFilter.yLow);
#endif

	DOASSERT(numGData <= WINDOWREP_BATCH_SIZE,
	  "Too many records in one batch");

	// Note: the whole business for making Lines and LineShades work won't
	// work right if the shape is variable. RKW 2001-12-28.
    ShapeID shape = mapping->GetShape((char *)gdata);
	if ((shape == SHAPE_LINE) || (shape == SHAPE_LINE_SHADE)) {
	  if (!_twoPass) {
	    _twoPass = true;
		InitPassTwo(mapping);
	  }
	}

	// If window is iconified, say that we "processed" all records, even if
	// they didn't actually get drawn.
	recordsProcessed = numGData;

	drawnList = NULL;
	recordsDrawn = -1;

	// Check whether we have a mapping iterator open.
	if(_index < 0) {
		_totalRecordsProcessed += recordsProcessed;
		return;
	}
  
#if (DEBUG >= 1) || 0
	printf("ViewData %d recs buf start 0x%p\n", numGData, gdata);
#endif

	int					gRecSize = mapping->GDataRecordSize();

	//
	// This array stores info about the GData records, so we can keep
	// track of various things we calculate, and don't have to repeatedly
	// pull values out of the actual GData.
	//
	SymbolInfo symArray[WINDOWREP_BATCH_SIZE];

	//
    // Get info from GData records, figure out whether symbols are within
	// visual filter, etc.
	//
	Boolean abort;
	GetGDataValues(mapping, gdata, numGData, gRecSize, symArray, abort);
	if (abort) {
	    return;
	}

	//
	// Test records against GAttr links we're a follower of.
	//
	GAttrLinkFollower(mapping, gdata, numGData, gRecSize, symArray);

	//
	// Update the data ranges based on the bounding boxes of symbols that
	// passed link filters.
	//
    UpdateDataRanges(numGData, symArray);

	//
	// Insert records into GAttr links we're a leader of.
	//
	GAttrLinkLeader(mapping, gdata, numGData, gRecSize, symArray);

	//
	// Insert records into record links we're a leader of.
	//
	RecordLinkLeader(recId, numGData, symArray);

	//
	// If _homeAfterQueryDone is true, all we're trying to do is get all
	// of the data values to correctly update the visual filter; therefore
	// we don't want to waste time actually drawing or doing statistics
	// on the data.
	//
	if (_homeAfterQueryDone) {
#if (DEBUG >= 1)
        printf("View %s returning without drawing because _homeAfterQueryDone "
		  "is set\n", GetName());
#endif
		_totalRecordsProcessed += recordsProcessed;
	    return;
    }

#if (DEBUG >= 2)
    printf("View %s data range: (%g, %g), (%g, %g)\n", GetName(), _dataXMin,
	    _dataXMax, _dataYMin, _dataYMax);
#endif

	//
	// Actually draw the symbols corresponding to the GData records that
	// have made it through the visual filter at GAttr link filters (or
	// send GData to the JS, or whatever).
	//
	DrawGData(mapping, recId, gdata, numGData, gRecSize, symArray,
	  recordsProcessed, needDrawnList, recordsDrawn, drawnList);

	//
	// Do statistics only on the data corresponding to the symbols that
	// actually got drawn (note: if the window is iconified, we do the
	// statistics on all records).
	//
	CalculateStats(mapping, gdata, gRecSize, symArray, recordsProcessed);

	_totalRecordsProcessed += recordsProcessed;
}

//******************************************************************************
Boolean
ViewData::HasDerivedTable()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
	printf("ViewData::HasDerivedTable()\n");
#endif

	Boolean result = _derivedTables.Size() > 0;

	return result;
}

//******************************************************************************
void
ViewData::InsertValues(TData *tdata, int recCount, void **tdataRecs)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
	printf("ViewData::InsertValues(%d)\n", recCount);
#endif

	int index = _derivedTables.InitIterator();
	while (_derivedTables.More(index)) {
	  DerivedTable *table = _derivedTables.Next(index);
	  if (!table->InsertValues(tdata, recCount, tdataRecs).IsComplete()) {
		char errBuf[1024];
        int formatted = snprintf(errBuf, sizeof(errBuf),
		"Error inserting values into table %s",
		    table->GetName());
		checkAndTermBuf2(errBuf, formatted);
		reportErrNosys(errBuf);
	  }
	}
	_derivedTables.DoneIterator(index);
}

//******************************************************************************
char *
ViewData::CreateDerivedTable(const char *namePrefix,
     const char *masterAttrName)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
	printf("ViewData::CreateDerivedTable(%s)\n", masterAttrName);
#endif

  //TEMP: we should check here to avoid creating two tables with the same
  //attribute.
  TDataMap *tdMap = GetFirstMap();
  if (tdMap == NULL) return NULL;
  TData *tdata = tdMap->GetPhysTData();
  if (tdata == NULL) return NULL;

  int namelen = strlen(namePrefix) + strlen(masterAttrName) + 2;
  char *name = new char[namelen];
  int formatted = snprintf(name, namelen, "%s:%s", namePrefix,
    masterAttrName);
  checkAndTermBuf(name, namelen, formatted);
  DevStatus result;
  DerivedTable *table = new DerivedTable(name, tdata, masterAttrName, result);
  if (!result.IsComplete()) {
    delete table;
    table = NULL;
	delete [] name;
	name = NULL;
  } else {
	_derivedTables.Insert(table);
  }

  return name;
}

//******************************************************************************
void
ViewData::DestroyDerivedTable(char *tableName)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("ViewData::DestroyDerivedTable(%s)\n", tableName);
#endif

  Boolean found = false;
  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *table = _derivedTables.Next(index);
	if (!strcmp(tableName, table->GetName())) {
	  delete table;
	  _derivedTables.DeleteCurrent(index);
	  found = true;
	  break;
	}
  }
  _derivedTables.DoneIterator(index);

  if (!found) {
	char errBuf[1024];
	int formatted = snprintf(errBuf, sizeof(errBuf),
	  "Table %s not found in table list", tableName);
	checkAndTermBuf2(errBuf, formatted);
	reportErrNosys(errBuf);
  }
}

//******************************************************************************
DerivedTable *
ViewData::GetDerivedTable(char *tableName)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("ViewData::GetDerivedTable(%s)\n", tableName);
#endif

  DerivedTable *table = NULL;
  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *tmpTable = _derivedTables.Next(index);
	if (!strcmp(tableName, tmpTable->GetName())) {
	  table = tmpTable;
	  break;
	}
  }
  _derivedTables.DoneIterator(index);

  return table;
}

//******************************************************************************
void
ViewData::GetGDataValues(TDataMap *mapping, void *gdata, int numGData,
  int gRecSize, SymbolInfo symArray[], Boolean &abort)
{
    abort = false;

#if defined(TEST_FILTER_LINK) // TEMP -- remove later. RKW 1998-10-29.
    VisualFilter linkFilter;
	if (!strcmp(GetName(), "FilterSlaveView")) {
	  View *masterView = FindViewByName("FilterMasterView");
	  if (masterView == NULL) {
	    fprintf(stderr, "Can't find master view\n");
		abort = true;
	    return;
      }
	  masterView->GetVisualFilter(linkFilter);
	}

	const GDataAttrOffset*	offset = mapping->GetGDataOffset();
#endif

    // Get info from GData records, figure out whether symbols are within
	// visual filter.
	char *dataP = (char *) gdata;
	for (int recNum = 0; recNum < numGData; recNum++) {
      if (_countMapping) {
	    DevStatus result = _countMapping->ProcessRecord(dataP);
	  }

	  SymbolInfo &symbol = symArray[recNum];

	  Coord x = symbol.x = mapping->GetX(dataP);
	  Coord y = symbol.y = mapping->GetY(dataP);

	  //
	  // Get the bounding box for this symbol.
	  //
	  mapping->GetBoundingBox(dataP, symbol.bBULx, symbol.bBULy,
	      symbol.bBLRx, symbol.bBLRy);

	  // Adjust the bounding box for "pixel size" (+/- key).
      symbol.bBULx *= mapping->GetPixelWidth();
      symbol.bBULy *= mapping->GetPixelWidth();
      symbol.bBLRx *= mapping->GetPixelWidth();
      symbol.bBLRy *= mapping->GetPixelWidth();

	  // Make the bounding box absolute instead of relative to symbol
	  // position.
      symbol.bBULx += x;
      symbol.bBULy += y;
	  symbol.bBLRx += x;
	  symbol.bBLRy += y;

#if (DEBUG >= 3)
      printf("  Record bounding box: UL: %g, %g; LR: %g, %g\n",
	    symbol.bBULx, symbol.bBULy, symbol.bBLRx, symbol.bBLRy);
#endif

	  ShapeID shape = symbol.shape = mapping->GetShape(dataP);
      symbol.isComplex = mapping->IsComplexShape(shape) ||
		  (GetNumDimensions() == 3);

	  //
	  // Note: here we're trying to find out whether _any part_ of the symbol
	  // is within the visual filter.  This sometimes fails, both by including
	  // symbols it shouldn't and by excluding symbols it shouldn't.
	  // RKW Feb. 12, 1998.
      // Use of maxWidth and maxHeight here is probably goofed up if symbols
	  // are rotated.  RKW Aug. 8, 1997.
	  //
	  symbol.inFilter = InVisualFilter2(symbol.bBULx, symbol.bBULy,
	      symbol.bBLRx, symbol.bBLRy);

#if defined(TEST_FILTER_LINK) // TEMP -- remove later. RKW 1998-10-29.
	  if (!strcmp(GetName(), "FilterSlaveView")) {
	    Coord linkX = GetShapeAttr(5, dataP, mapping, offset);
	    Coord linkY = GetShapeAttr(6, dataP, mapping, offset);
        symbol.inFilter &= InVisualFilter(linkFilter, linkX, linkY,
		  0.0, 0.0);
      }
#endif

	  dataP += gRecSize;
	}
}

//******************************************************************************
void
ViewData::GAttrLinkFollower(TDataMap *mapping, void *gdata, int numGData,
  int gRecSize, SymbolInfo symArray[])
{
	Boolean firstLink = true;
	int index = _slaveLink.InitIterator();
	while (_slaveLink.More(index)) {
	  MasterSlaveLink *msLink = _slaveLink.Next(index);
	  if (msLink->GetFlag() == VISUAL_GATTR) {
#if (DEBUG >= 2)
	    printf("View %s testing records against link %s\n", GetName(),
		    msLink->GetName());
#endif 

		GAttrLink *gaLink = (GAttrLink *)msLink;
	    char *gDataRec = (char *) gdata;

	    for (int recNum = 0; recNum < numGData; recNum++) {
		  // Note: we used to not test records against the GAttribute link(s)
		  // if we already knew we weren't going to draw them; however, this
		  // goofs up the data ranges for home, so now we always test against
		  // the GAttribute link(s).  wenger 2003-08-01.
		  if (firstLink) {
		    symArray[recNum].inGAttrLink =
		        gaLink->TestRecord(mapping, (char *)gDataRec);
		  } else {
		    if (_gAttrLinkMode == LinkIntersection) {
		      symArray[recNum].inGAttrLink = symArray[recNum].inGAttrLink &&
		          gaLink->TestRecord(mapping, (char *)gDataRec);
		    } else if (_gAttrLinkMode == LinkUnion) {
		      symArray[recNum].inGAttrLink = symArray[recNum].inGAttrLink ||
		          gaLink->TestRecord(mapping, (char *)gDataRec);
		    } else {
		      DOASSERT(false, "Illegal link mode");
		    }
		  }

	      gDataRec += gRecSize;
		}
	  }

	  firstLink = false;
	}
	_slaveLink.DoneIterator(index);
}

//******************************************************************************
void
ViewData::UpdateDataRanges(int numGData, SymbolInfo symArray[])
{
  for (int recNum = 0; recNum < numGData; recNum++) {
	SymbolInfo &symbol = symArray[recNum];
    if (symbol.inGAttrLink) {
#if (DEBUG >= 2)
      printf("  Updating data ranges with (%g, %g), (%g, %g)\n", symbol.bBULx,
	      symbol.bBULy, symbol.bBLRx, symbol.bBLRy);
#endif
      if (_dataRangeFirst) {
	    _dataXMin = symbol.bBULx;
		_dataXMax = symbol.bBLRx;
		_dataYMin = symbol.bBLRy;
		_dataYMax = symbol.bBULy;
	    _dataRangeFirst = false;
	  } else {
	    _dataXMin = MIN(_dataXMin, symbol.bBULx);
		_dataXMax = MAX(_dataXMax, symbol.bBLRx);
		_dataYMin = MIN(_dataYMin, symbol.bBLRy);
		_dataYMax = MAX(_dataYMax, symbol.bBULy);
	  }
	}
  }
}

//******************************************************************************
void
ViewData::GAttrLinkLeader(TDataMap *mapping, void *gdata, int numGData,
  int gRecSize, SymbolInfo symArray[])
{
	int index = _masterLink.InitIterator();
	while (_masterLink.More(index)) {
	  MasterSlaveLink *msLink = _masterLink.Next(index);
	  if (msLink->GetFlag() == VISUAL_GATTR) {
#if (DEBUG >= 2)
	    printf("View %s inserting records into link %s\n", GetName(),
		    msLink->GetName());
#endif

		GAttrLink *gaLink = (GAttrLink *)msLink;
	    char *gDataRec = (char *) gdata;

	    for (int recNum = 0; recNum < numGData; recNum++) {
		  if (symArray[recNum].Passes()) {
		    gaLink->InsertRecord(mapping, (char *)gDataRec);
		  }
	      gDataRec += gRecSize;
		}
	  }
	}
	_masterLink.DoneIterator(index);
}

//******************************************************************************
void
ViewData::RecordLinkLeader(RecId recId, int numGData, SymbolInfo symArray[])
{
	int			firstRec = 0;

	//
	// Go thru the GData records, and insert the ones that *don't* pass
	// the visual filter and GAttr links into records links we're a master
	// of.
	//
	for (int i = 0; i < numGData; i++)
	{
		if (!symArray[i].Passes()) {

			// Here we have a record outside the visual filter.  If the
			// previous record was _inside_ the visual filter, write
			// the previous contiguous batch of "inside the filter"
			// records to the link.
			if (!MoreMapping(_index))
			{
				if (i > firstRec) {
					WriteMasterLink(recId + firstRec, i - firstRec);
				}

				// Next contiguous batch of record ids starts at i+1
				firstRec = i + 1;
			}
		}
	}

	if (!MoreMapping(_index) && (numGData > firstRec)) {
		WriteMasterLink(recId + firstRec, numGData - firstRec);
    }
}

//******************************************************************************
void
ViewData::DrawGData(TDataMap *mapping, RecId recId, void *gdata, int numGData,
  int gRecSize, SymbolInfo symArray[], int &recordsProcessed,
  Boolean needDrawnList, int& recordsDrawn, BooleanArray*& drawnList)
{
	char*		ptr = (char*)gdata;
	Boolean		timedOut = false;
	Boolean		recsFiltered = false;
	Boolean		timeoutAllowed = (!_dupElim) && (!_countMapping);
	int			recIndex = 0;
	int			reverseIndex[WINDOWREP_BATCH_SIZE + 1];
	WindowRep*	win = GetWindowRep();
	void*		recs[WINDOWREP_BATCH_SIZE];

	reverseIndex[0] = 0;

	//
	// Draw symbols
	//
	for (int i=0; i<numGData && !timedOut; i++)
	{
		// I don't know why the heck we draw a "complex" symbol here even if
		// it falls outside the visual filter; this also goofs up record links
		// if master view has complex symbols. RKW Aug. 8, 1997.
		if (symArray[i].ShouldDraw()) 
		{
			// reverseIndex is set up this way because if you have some records
			// outside the visual filter, you want to count all such records as
			// processed if they're before the last record actually drawn.
			reverseIndex[recIndex + 1] = i + 1;

			// Draw data only if window is not iconified
			if (!Iconified())
			{
				if (!_dupElim || _dupElim->DrawSymbol(ptr)) {
				    recs[recIndex++] = ptr;
				}

				if (recIndex == WINDOWREP_BATCH_SIZE)
				{
					int		tmpRecs;

					mapping->DrawGDataArray(this, win, recs, recIndex,
					  tmpRecs, timeoutAllowed, _twoPass);
					_totalRecordsDrawn += tmpRecs;
                    if (_twoPass) {
					  _passTwoGData->AddRecords(recIndex, recs);
					}

					// Note: if first records are outside filter, might
					// have _processed_ some records even if didn't draw any.
					recordsProcessed = reverseIndex[tmpRecs];

#if (DEBUG >= 2)
					printf("  tmpRecs = %d, recordsProcessed = %d\n", tmpRecs,
						   recordsProcessed);
#endif
					if (tmpRecs < recIndex)
						timedOut = true; // Draw timed out.

					recIndex = 0;
				}
			}
		}

		ptr += gRecSize;
	}

	if (!Iconified() && recIndex > 0 && !timedOut)
	{
		int		tmpRecs;

		mapping->DrawGDataArray(this, win, recs, recIndex, tmpRecs,
		  timeoutAllowed, _twoPass);
		_totalRecordsDrawn += tmpRecs;
        if (_twoPass) {
		  _passTwoGData->AddRecords(recIndex, recs);
		}
		recordsProcessed = reverseIndex[tmpRecs];

#if (DEBUG >= 2)
		printf("  tmpRecs = %d, recordsProcessed = %d\n", tmpRecs,
			   recordsProcessed);
#endif
	}


	//
	// Update the list of records drawn.
	//
	if (needDrawnList) {
	  if (!recsFiltered && recordsProcessed == numGData) {
		recordsDrawn = numGData;
	  } else {
        drawnList = new BooleanArray(numGData);
        drawnList->Clear();

	    int tmpRecsDrawn = 0;
	    for (int recNum = 0; recNum < recordsProcessed; recNum++)
	    {
	      if (symArray[recNum].ShouldDraw()) {
		    tmpRecsDrawn++;
		    drawnList->Set(recNum, true);
	      }
	    }
	    recordsDrawn = tmpRecsDrawn;
	  }
	}
}

//******************************************************************************
void
ViewData::CalculateStats(TDataMap *mapping, void *gdata, int gRecSize,
  SymbolInfo symArray[], int recordsProcessed)
{
	char *ptr = (char*)gdata;

	for (int recNum=0; recNum<recordsProcessed; recNum++)
	{
		// Extract X, Y, shape, and color information from gdata record
		Coord		x = symArray[recNum].x;
		Coord		y = symArray[recNum].y;
		ShapeID		shape = symArray[recNum].shape;

		// Note size of 0.0, 0.0 here, so it's not the same as the
		// previous call to InVisualFilter().
		// Do we really want the set of records we do stats on to not be
		// the same as the set we draw?
#if 0 // Do stats if bounding box is in the visual filter, not just if X, Y
      // is in the visual filter, so stats agree with what is drawn.
	  // RKW 1999-05-28.
		if (symArray[recNum].Passes() &&
			InVisualFilter(_queryFilter, x, y, (Coord)0.0, (Coord)0.0))
#else
		if (symArray[recNum].Passes())
#endif
		{
			PColorID	pcid = mapping->GetPColorID(ptr);

#if !VIEW_MIN_STATS
			// Palette size variability warning...
			if ((pcid != nullPColorID) && (pcid < gMaxNumColors))
				_stats[pcid].Sample(x, y);
#endif

			_allStats.Sample(x, y);
			_allStats.Histogram(y);

			if(_glistX.Size() <= MAX_GSTAT)
			{
				double			X = x;
				BasicStats*		bsx;

				if(_gstatX.Lookup(x, bsx))
				{
					DOASSERT(bsx, "GData Stat look error");
					bsx->Sample(x,y);
				}
				else
				{
					bsx = new BasicStats();
					DOASSERT(bsx, "Out of memory");
					bsx->Init(0);
					_glistX.InsertOrderly(x, 1);
					bsx->Sample(x, y);
					_gstatX.Insert(X, bsx);
					_blist.Insert(bsx);
				}
			}
			else
			{
				_gstatInMem = false;	// Submit the query to DTE
			}

			if(_glistY.Size() <= MAX_GSTAT)
			{
				double			Y =  y;
				BasicStats*		bsy = NULL;

				if(_gstatY.Lookup(y, bsy))
				{
					DOASSERT(bsy, "GData Stat look error");
					bsy->Sample(y,x);
				}
				else
				{
					bsy = new BasicStats();
					DOASSERT(bsy, "Out of memory");
					bsy->Init(0);
					_glistY.InsertOrderly(y, 1);
					bsy->Sample(y,x);
					_gstatY.Insert(y, bsy);
					_blist.Insert(bsy);
				}
			}
			else
			{
				// mark gstatBuf false cleanup the gstat list, the group
				// by query will be submitted to DTE when requested
				_gstatInMem = false;
			}
		}

		ptr += gRecSize;
	}
}

//******************************************************************************
void
ViewData::InitPassTwo(TDataMap *map)
{
#if (DEBUG >= 1)
  printf("ViewData::InitPassTwo()\n");
#endif

  _passTwoGData = new GDataStore(map);
}

//******************************************************************************
void
ViewData::DrawPassTwo(TDataMap *map)
{
#if (DEBUG >= 1)
  printf("ViewData::DrawPassTwo()\n");
#endif

  int recCount = _passTwoGData->GetRecCount();
  void **gdata = _passTwoGData->GetGData();


  int tmpRecs;
  map->DrawGDataArray(this, GetWindowRep(), gdata, recCount, tmpRecs,
      false, false);
  DOASSERT(tmpRecs == recCount, "Not all records drawn/processed");

  _passTwoGData->ReleaseGData();

  delete _passTwoGData;
  _passTwoGData = NULL;
}

//******************************************************************************
GDataStore::GDataStore(TDataMap *map)
{
#if (DEBUG >= 2)
  printf("GDataStore::GDataStore()\n");
#endif

  _map = map;
  _recordSize = map->GDataRecordSize();

  _recordCount = 0;
  _gdataList._next = NULL;
  _gdataList._gdataChunk = NULL;
  _gdataList._recCount = 0;
  _lastNode = &_gdataList;
  _gdata = NULL;
}

//******************************************************************************
GDataStore::~GDataStore()
{
#if (DEBUG >= 2)
  printf("GDataStore::~GDataStore()\n");
#endif

  _recordCount = 0;

  GDataNode *node = _gdataList._next;
  while (node != NULL) {
    GDataNode *oldNode = node;
    node = node->_next;
    delete [] oldNode->_gdataChunk;
    delete oldNode;
  }

  ReleaseGData();
}

//******************************************************************************
void
GDataStore::AddRecords(int recordCount, void **gdata)
{
#if (DEBUG >= 2)
  printf("GDataStore::AddRecords(%d)\n", recordCount);
#endif

  _recordCount += recordCount;

  GDataNode *node = new GDataNode();
  node->_next = NULL;
  node->_gdataChunk = new char[recordCount * _recordSize];
  memcpy(node->_gdataChunk, *gdata, recordCount * _recordSize);
  node->_recCount = recordCount;

  _lastNode->_next = node;
  _lastNode = node;
}

//******************************************************************************
void **
GDataStore::GetGData()
{
#if (DEBUG >= 2)
  printf("GDataStore::GetGData()\n");
  printf("  returning %d records\n", _recordCount);
#endif

  //
  // Set up an array of GDataSort objects so we can sort the GData records
  // (really the pointers to them) by X value.
  //
  GDataSort *recs = new GDataSort[_recordCount];
  int indTot = 0;

  GDataNode *node = _gdataList._next;
  while (node != NULL) {
	for (int indChunk = 0; indChunk < node->_recCount; indChunk++) {
	  char *gdataRec = node->_gdataChunk + indChunk * _recordSize;
	  recs[indTot]._x = _map->GetX(gdataRec);
	  recs[indTot]._gdataRec = gdataRec;
	  indTot++;
	}
    node = node->_next;
  }
  DOASSERT(indTot == _recordCount, "Incorrect number of records");

  //
  // Okay, now go ahead and do the actual sort.
  //
  qsort(recs, _recordCount, sizeof(GDataSort), Compare);

  //
  // Now make a new array of pointers to the GData records (without the
  // X values) in sorted order.
  //
  _gdata = new void *[_recordCount];
  for (int index = 0; index < _recordCount; index++) {
    _gdata[index] = recs[index]._gdataRec;
  }

  delete [] recs;

  return _gdata;
}

//******************************************************************************
void
GDataStore::ReleaseGData()
{
#if (DEBUG >= 2)
  printf("GDataStore::ReleaseGData()\n");
#endif

  delete [] _gdata;
  _gdata = NULL;
}

//******************************************************************************
int
GDataStore::Compare(const void *rec1, const void *rec2)
{
  GDataSort *r1 = (GDataSort *)rec1;
  GDataSort *r2 = (GDataSort *)rec2;

  if (r1->_x < r2->_x) {
    return -1;
  } else if (r1->_x > r2->_x) {
    return 1;
  } else {
    return 0;
  }
}

//******************************************************************************
