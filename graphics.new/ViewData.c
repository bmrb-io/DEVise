/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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

//#define DEBUG

struct SymbolInfo {
  Coord x;
  Coord y;
  ShapeID shape;
  Boolean inFilter;
  Boolean isComplex;
};

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewData::ViewData(char* name, VisualFilter& initFilter, QueryProc* qp,
						 PColorID fgid, PColorID bgid, 
						 AxisLabel* xAxis, AxisLabel* yAxis, Action* action)
	:	ViewGraph(name, initFilter, qp, xAxis, yAxis, fgid, bgid, action)
{
#if defined(DEBUG)
	printf("ViewData::ViewData(0x%p, %s)\n", this, name);
#endif
}

//******************************************************************************
ViewData::~ViewData()
{
#if defined(DEBUG)
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
#if defined(DEBUG)
  printf("ViewData::QueryInit()\n");
#endif

  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *table = _derivedTables.Next(index);
    if (!table->Initialize().IsComplete()) {
	  char errBuf[1024];
         sprintf(errBuf, "Error initializing table %s", table->GetName());
	  reportErrNosys(errBuf);
    }
  }
  _derivedTables.DoneIterator(index);

  ViewGraph::QueryInit(userData);
}

void
ViewData::QueryDone(int bytes, void* userData, TDataMap* map)
{
#if defined(DEBUG)
  printf("ViewData::QueryDone(%d)\n", bytes);
#endif

  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *table = _derivedTables.Next(index);
    if (!table->Done().IsComplete()) {
	  char errBuf[1024];
         sprintf(errBuf, "Error terminating table %s", table->GetName());
	  reportErrNosys(errBuf);
    }
  }
  _derivedTables.DoneIterator(index);

  ViewGraph::QueryDone(bytes, userData, map);
}

void	ViewData::ReturnGData(TDataMap* mapping, RecId recId,
								 void* gdata, int numGData,
								 int& recordsProcessed,
								 Boolean needDrawnList, int& recordsDrawn,
								 BooleanArray*& drawnList)
{
#if defined(DEBUG)
	printf("ViewData::ReturnGData(0x%p, %d)\n", gdata, numGData);
#endif

	// If window is iconified, say that we "processed" all records, even if
	// they didn't actually get drawn.
	recordsProcessed = numGData;

	drawnList = NULL;
	recordsDrawn = -1;

	// Check whether we have a mapping iterator open.
	if(_index < 0) {
		return;
	}
  
#if defined(DEBUG) || 0
	printf("ViewData %d recs buf start 0x%p\n", numGData, gdata);
#endif

	void*		recs[WINDOWREP_BATCH_SIZE];
	int			reverseIndex[WINDOWREP_BATCH_SIZE + 1];
	Coord		maxWidth, maxHeight, maxDepth;
	WindowRep*	win = GetWindowRep();

	reverseIndex[0] = 0;
	mapping->UpdateMaxSymSize(gdata, numGData);
	mapping->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

	if (IsInPileMode())
	{
		ViewWin*	vw = GetFirstSibling();

		win = vw->GetWindowRep();

#ifdef DEBUG
		printf("Drawing view %s in view %s, window 0x%p\n",
			   GetName(), vw->GetName(), win);
#endif
	}
  
	GDataAttrOffset*	offset = mapping->GetGDataOffset();
	int					gRecSize = mapping->GDataRecordSize();
	int					recIndex = 0;
	int					firstRec = 0;

    // Get info from GData records, figure out whether symbols are within
	// visual filter.
	char *dataP = (char *) gdata;
	SymbolInfo *symArray = new SymbolInfo[numGData];
	for (int recNum = 0; recNum < numGData; recNum++) {
      if (_countMapping) {
	    DevStatus result = _countMapping->ProcessRecord(dataP);
		DOASSERT(result.IsComplete(), "CountMapping::ProcessRecord() failed");
	  }

	  Coord x = symArray[recNum].x = ShapeGetX(dataP, mapping, offset);
	  Coord y = symArray[recNum].y = ShapeGetY(dataP, mapping, offset);
	  ShapeID shape = symArray[recNum].shape = GetShape(dataP, mapping, offset);
      symArray[recNum].isComplex = mapping->IsComplexShape(shape) ||
		  (GetNumDimensions() == 3);

	  // Note: here we're trying to find out whether _any part_ of the symbol
	  // is within the visual filter.  This sometimes fails, both by including
	  // symbols it shouldn't and by excluding symbols it shouldn't.
	  // RKW Feb. 12, 1998.
      // Use of maxWidth and maxHeight here is probably goofed up if symbols
	  // are rotated.  RKW Aug. 8, 1997.
	  symArray[recNum].inFilter = InVisualFilter(_queryFilter, x, y,
		  maxWidth, maxHeight);

	  dataP += gRecSize;
	}

	// Draw symbols
	char*		ptr = (char*)gdata;
	Boolean		timedOut = false;
	Boolean		recsFiltered = false;

	for (int i=0; i<numGData && !timedOut; i++)
	{
		// I don't know why the heck we draw a "complex" symbol here even if
		// it falls outside the visual filter; this also goofs up record links
		// if master view has complex symbols. RKW Aug. 8, 1997.
		if (symArray[i].isComplex || symArray[i].inFilter)
		{
			// reverseIndex is set up this way because if you have some records
			// outside the visual filter, you want to count all such records as
			// processed if they're before the last record actually drawn.
			reverseIndex[recIndex + 1] = i + 1;

			// Draw data only if window is not iconified
			if (!Iconified())
			{
				recs[recIndex++] = ptr;

				if (recIndex == WINDOWREP_BATCH_SIZE)
				{
					int		tmpRecs;

					mapping->DrawGDataArray(this, win, recs, recIndex, tmpRecs);

					// Note: if first records are outside filter, might
					// have _processed_ some records even if didn't draw any.
					recordsProcessed = reverseIndex[tmpRecs];

#if defined(DEBUG)
					printf("  tmpRecs = %d, recordsProcessed = %d\n", tmpRecs,
						   recordsProcessed);
#endif
					if (tmpRecs < recIndex)
						timedOut = true; // Draw timed out.

					recIndex = 0;
				}
			}
		}
		else
		{
			recsFiltered = true;
			reverseIndex[recIndex] = i + 1;

			// Put records _outside_ the visual filter into the record link.
			if (!MoreMapping(_index))
			{
				if (i > firstRec)
					WriteMasterLink(recId + firstRec, i - firstRec);

				// Next contiguous batch of record ids starts at i+1
				firstRec = i + 1;
			}
		}

		ptr += gRecSize;
	}

	if (!MoreMapping(_index) && (numGData > firstRec))
		WriteMasterLink(recId + firstRec, numGData - firstRec);

	if (!Iconified() && recIndex > 0 && !timedOut)
	{
		int		tmpRecs;

		mapping->DrawGDataArray(this, win, recs, recIndex, tmpRecs);
		recordsProcessed = reverseIndex[tmpRecs];

#if defined(DEBUG)
		printf("  tmpRecs = %d, recordsProcessed = %d\n", tmpRecs,
			   recordsProcessed);
#endif
	}


	// Update the list of records drawn.
	if (needDrawnList) {
	  if (!recsFiltered && recordsProcessed == numGData) {
		recordsDrawn = numGData;
	  } else {
        drawnList = new BooleanArray(numGData);
        drawnList->Clear();

	    int tmpRecsDrawn = 0;
	    for (int recNum = 0; recNum < recordsProcessed; recNum++)
	    {
	      if (symArray[recNum].isComplex || symArray[recNum].inFilter) {
		    tmpRecsDrawn++;
		    drawnList->Set(recNum, true);
	      }
	    }
	    recordsDrawn = tmpRecsDrawn;
	  }
	}

	// Do statistics only on the data corresponding to the symbols that
	// actually got drawn (note: if the window is iconified, we do the
	// statistics on all records).

	ptr = (char*)gdata;

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
		if (symArray[recNum].inFilter &&
			InVisualFilter(_queryFilter, x, y, 0.0, 0.0))
		{
			PColorID	pcid = mapping->GetPColorID(ptr);

			// Palette size variability warning...
			if ((pcid != nullPColorID) && (pcid < gMaxNumColors))
				_stats[pcid].Sample(x, y);

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

	delete [] symArray;
}

//******************************************************************************
Boolean
ViewData::HasDerivedTable()
{
#if defined(DEBUG)
	printf("ViewData::HasDerivedTable()\n");
#endif

	Boolean result = _derivedTables.Size() > 0;

	return result;
}

//******************************************************************************
void
ViewData::InsertValues(TData *tdata, int recCount, void **tdataRecs)
{
#if defined(DEBUG)
	printf("ViewData::InsertValues(%d)\n", recCount);
#endif

	int index = _derivedTables.InitIterator();
	while (_derivedTables.More(index)) {
	  DerivedTable *table = _derivedTables.Next(index);
	  if (!table->InsertValues(tdata, recCount, tdataRecs).IsComplete()) {
		char errBuf[1024];
        sprintf(errBuf, "Error inserting values into table %s",
		    table->GetName());
		reportErrNosys(errBuf);
	  }
	}
	_derivedTables.DoneIterator(index);
}

//******************************************************************************
char *
ViewData::CreateDerivedTable(char *namePrefix, char *masterAttrName)
{
#if defined(DEBUG)
	printf("ViewData::CreateDerivedTable(%s)\n", masterAttrName);
#endif

  //TEMP: we should check here to avoid creating two tables with the same
  //attribute.
  TDataMap *tdMap = GetFirstMap();
  if (tdMap == NULL) return NULL;
  TData *tdata = tdMap->GetPhysTData();
  if (tdata == NULL) return NULL;

  int namelen = strlen(namePrefix) + strlen(masterAttrName) + 1;
  char *name = new char[namelen + 1];
  sprintf(name, "%s:%s", namePrefix, masterAttrName);
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
#if defined(DEBUG)
  printf("ViewData::DestroyDerivedTable(%s)\n", tableName);
#endif

  Boolean found = false;
  int index = _derivedTables.InitIterator();
  while (_derivedTables.More(index)) {
    DerivedTable *table = _derivedTables.Next(index);
	if (!strcmp(tableName, table->GetName())) {
	  _derivedTables.DeleteCurrent(index);
	  found = true;
	  break;
	}
  }
  _derivedTables.DoneIterator(index);

  if (!found) {
	char errBuf[1024];
	sprintf(errBuf, "Table %s not found in table list", tableName);
	reportErrNosys(errBuf);
  }
}

//******************************************************************************
DerivedTable *
ViewData::GetDerivedTable(char *tableName)
{
#if defined(DEBUG)
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
