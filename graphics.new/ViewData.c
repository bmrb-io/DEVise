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
}

//******************************************************************************
// Callback Methods (QueryCallback)
//******************************************************************************

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
