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

  Revision 1.18 1997/10/17 00:51:53 zhenhai
  Removed _transform. It was used to optimize drawing on the same pixel.
  However it needs to access the transformation of WindowRep. First,
  this is violation of encapsulation of the WindowRep classes. Second,
  the problem is that GDataBin assumes WindowRep classes uses Transform class
  to calculate coordinates. However, this is not the case for OpenGL WindowRep.
  Third, if such an optimization needs to be done, it should be done at
  WindowRep level instead of GDataBin level.

  Revision 1.17  1997/08/20 22:10:56  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.16.12.4  1997/08/20 18:36:22  wenger
  QueryProcFull and QPRange now deal correctly with interrupted draws.
  (Some debug output still turned on.)

  Revision 1.16.12.3  1997/08/20 15:33:04  wenger
  Interruptible drawing now working for TDataViewX class (including
  GDataBin class); improvements to reverseIndex in ViewScatter class.
  (Some debug output still turned on.)

  Revision 1.16.12.2  1997/08/15 23:06:26  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.16.12.1  1997/08/07 16:56:31  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.16  1997/01/14 15:48:22  wenger
  Fixed bug 105; changed '-noshm' flag to '-sharedMem 0|1' for more
  flexibility in overriding startup script default; fixed bug 116
  (off-by-one error in BufMgrFull caused buffer overflow in XWindowRep).

  Revision 1.15  1996/11/20 20:35:19  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.14  1996/11/07 22:40:28  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.13  1996/09/27 21:09:36  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.12  1996/09/27 15:53:18  wenger
  Fixed a number of memory leaks.

  Revision 1.11  1996/07/19 17:25:20  jussi
  Added canElimOverlap parameter to InsertSymbol().

  Revision 1.10  1996/04/16 20:08:30  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.9  1995/12/29 22:42:26  jussi
  Added support for line connectors.

  Revision 1.8  1995/12/28 19:52:02  jussi
  Small fixes to remove compiler warnings.

  Revision 1.7  1995/12/14 21:17:02  jussi
  *** empty log message ***

  Revision 1.6  1995/12/14 17:57:45  jussi
  *** empty log message ***

  Revision 1.5  1995/11/25 01:24:08  jussi
  Removed #ifdef CALCULATE_DIRECTLY which allowed one to quickly
  switch back to the xPerPixel and yPerPixel method for testing.

  Revision 1.4  1995/11/25  01:20:07  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.3  1995/11/24 21:25:51  jussi
  Added copyright notice and cleaned up code. Fixed inconsistency
  in computing exact pixel values in overlap elimination vs. pixel
  values computed by matrix transformations in XWindowRep.

  Revision 1.2  1995/09/05 22:14:48  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <math.h>

#include "Exit.h"
#include "GDataBin.h"
#include "Connector.h"
#include "TDataMap.h"
#include "TDataCMap.h"
#include "Init.h"

//#define DEBUG

#define ROUND(type, value) ((type)(value + 0.5))

/********************************************************************
Initializer
**********************************************************************/

GDataBin::GDataBin()
{
#if defined(DEBUG)
  printf("GDataBin::GDataBin(); new'ing %d connectors (%d each)\n",
    GDATA_BIN_MAX_PIXELS, sizeof(Connector));
#endif
  int i;

#ifdef 0
  for(i = 0; i < GDATA_BIN_MAX_PIXELS; i++)
    _timestamp[i] = 0;
#endif
  _iteration = 1;
  _returnIndex = 0;
  
  _elimOverlap = Init::ElimOverlap();

#if USE_CONNECTORS
  /* Init space for connectors */
  for(i = 0; i < GDATA_BIN_MAX_PIXELS; i++)
    _connectors[i] = new Connector;
#endif
}

/********************************************************************
Destructor.
**********************************************************************/

GDataBin::~GDataBin()
{
#if defined(DEBUG)
  printf("GDataBin::~GDataBin()\n");
#endif
#if USE_CONNECTORS
  int i;
  for(i = 0; i < GDATA_BIN_MAX_PIXELS; i++) {
    delete _connectors[i];
  }
#endif
}

/**********************************************************************
Init before any data is returned from query processor 
************************************************************************/

void GDataBin::Init(TDataMap *mapping, VisualFilter *filter,
		    Boolean dispSymbol,
		    Boolean dispConnector, TDataCMap *cMap,
		    GDataBinCallback *callback)
{
#if defined(DEBUG)
  printf("GDataBin::Init: dispConnector = %s, cMap = 0x%p\n",
	 (dispConnector ? "on" : "off"), cMap);
  printf("  Transform: ");
  transform->Print();
  printf("\n");
#endif

  DOASSERT(!_returnIndex, "Invalid return index value");
  
  _numSyms = 0;
  _numSymsReturned = 0;

  _mapping = mapping;
  _dispSymbol = dispSymbol;
  _dispConnector = dispConnector;
  _cMap = cMap;

  Coord ylow, yhigh;
//  _transform = transform;
//  _transform->TransformY(filter->xLow, filter->yLow, ylow);
//  _transform->TransformY(filter->xHigh, filter->yHigh, yhigh);
//  _maxYPixels = ROUND(int, fabs(yhigh - ylow));

#ifdef DEBUG
  printf("GDataBin: _maxYPixels: %d\n", _maxYPixels);
#endif
#ifdef 0
  if (!(_maxYPixels > 0 && _maxYPixels < GDATA_BIN_MAX_PIXELS))
    printf("GDataBin: yhigh %.2f, ylow %.2f, _maxYPixels: %d\n",
	   yhigh, ylow, _maxYPixels);
  DOASSERT(_maxYPixels >= 0 && _maxYPixels < GDATA_BIN_MAX_PIXELS,
	   "Illegal value for maxYPixels");

  _needX = true;
#endif
  _callBack = callback;
  _gRecSize = mapping->GDataRecordSize();
}

/***************************************************************************
Insert symbol into bin. The bin will push the symbols to Gdata when done 
****************************************************************************/

void GDataBin::InsertSymbol(RecId startRid, void *recs, int numRecs,
			    int &recordsProcessed, int startIndex,
			    int incr, Boolean canElimRecords)
{
#if defined(DEBUG)
  printf("GDataBin::InsertSymbol(%ld,0x%p,%d,%d,%d,%d)\n",
	 startRid, recs, numRecs, startIndex, incr, canElimRecords);
#endif
  
  _numSyms += numRecs;

  if (_dispSymbol) {
    Boolean timedOut = false;

    /* pointer to next record to process */
    char *ptr = (char *)recs + startIndex * _gRecSize; 
  
    /* amount to increment pointer each iter */
    int ptrIncr = incr * _gRecSize;	
  
    if (!_elimOverlap || !canElimRecords) {
      /* DO NOT eliminate overlap */
      recordsProcessed = 0;
      for(int i = startIndex; i < numRecs && !timedOut; i += incr) {
	_returnSyms[_returnIndex++] = (GDataBinRec *)ptr;
	if (_returnIndex >= GDATA_BIN_MAX_PIXELS) {
	  int recordsToProcess = _returnIndex;
	  int tmpRecProc;
	  ReturnSymbols(tmpRecProc);
	  if (tmpRecProc < recordsToProcess) timedOut = true;
	  if (timedOut) printf("%s: %d: Draw timed out\n", __FILE__, __LINE__);
	  recordsProcessed += tmpRecProc;
	}
	ptr += ptrIncr;
      }
      // flush any remaining symbols to screen
      if (!timedOut) {
        int tmpRecProc;
        ReturnSymbols(tmpRecProc);
        recordsProcessed += tmpRecProc;
      }
    } else {
      /* Eliminate overlap here */
      
      int reverseIndex[GDATA_BIN_MAX_PIXELS + 1];
      reverseIndex[0] = 0;
      recordsProcessed = 0;
      for(int i = startIndex; i < numRecs && !timedOut; i += incr) {
	
	GDataBinRec *sym = (GDataBinRec *)ptr;
	
	// compute X pixel value for this symbol and see if it differs
	// from X pixel value for current bin
	
#ifdef 0
	Coord x, y;
	_transform->Transform(sym->x, sym->y, x, y);
	int thisPixelX = ROUND(int, x);
	int thisPixelY = ROUND(int, y);
	
	if (_needX) {
	  _pixelX = thisPixelX;
	  _needX = false;
	}
	else if (thisPixelX != _pixelX) {
	  _iteration++;
	  _pixelX = thisPixelX;
	}
	
	/* put symbol in the array */
	if (thisPixelY < 0)
	  thisPixelY = 0;
	else if (thisPixelY > _maxYPixels)
	  thisPixelY = _maxYPixels;
#endif
	
#ifdef 0
	if (_timestamp[thisPixelY] != _iteration) {
	  _timestamp[thisPixelY] = _iteration;
#endif
	  DOASSERT(_returnIndex < GDATA_BIN_MAX_PIXELS,
		   "Illegal value for returnIndex");
	  _returnSyms[_returnIndex++] = sym;
	  reverseIndex[_returnIndex] = i + 1;
	  
	  // flush cache of symbols to screen?
	  if (_returnIndex >= GDATA_BIN_MAX_PIXELS) {
	    int recordsToProcess = _returnIndex;
	    int tmpRecProc;
	    ReturnSymbols(tmpRecProc);
	    if (tmpRecProc < recordsToProcess) timedOut = true;
	    if (timedOut) printf("%s: %d: Draw timed out\n", __FILE__, __LINE__);
	    recordsProcessed += reverseIndex[tmpRecProc];
	  }
#ifdef 0
	} else {
#ifdef DEBUGx
//	  printf("Not adding x %f, X pixel %d\n", sym->x, thisPixelX);
#endif
	  reverseIndex[_returnIndex] = i + 1;
	}
#endif
	
	ptr += ptrIncr;
      }
    
      // flush any remaining symbols to screen
      if (!timedOut) {
        int tmpRecProc;
        ReturnSymbols(tmpRecProc);
        recordsProcessed += reverseIndex[tmpRecProc];
      }
    }
  }
  
  // return if connectors should not be displayed
  if (!_dispConnector || incr != 1)
    return;

  /* display connector has been set to true and
     we are looking at records one at a time */
    
  char *ptr = (char *)recs + startIndex * _gRecSize; 
  
  GDataBinRec *lastSym = (GDataBinRec *)ptr;
#ifdef 0
  Coord x, y;
  _transform->Transform(lastSym->x, lastSym->y, x, y);
  int lastPixelX = ROUND(int, x);
  int lastPixelY = ROUND(int, y);
#endif
  
  /* amount to increment pointer in each iteration */
  
  int ptrIncr = _gRecSize;	
  int index = 0;
  ptr += ptrIncr;
  
  // Draw connectors only for records that were drawn.
  for(int i = startIndex + 1; i < recordsProcessed; i++) {
    GDataBinRec *sym = (GDataBinRec *)ptr;
    
#ifdef 0
    _transform->Transform(sym->x, sym->y, x, y);
    int thisPixelX = ROUND(int, x);
    int thisPixelY = ROUND(int, y);
#endif
    
#ifdef 0
    if (lastPixelX != thisPixelX || lastPixelY != thisPixelY) {
#endif
      
#ifdef DEBUG
      printf("mapping syms: (%d,%d) (%d,%d): ",
	     lastPixelX, lastPixelY, thisPixelX, thisPixelY);
#endif
      
      /* draw a connection */
#if !USE_CONNECTORS
      DOASSERT(0, "Trying to use _connectors\n");
#endif
      if (_cMap->MapToConnection(lastSym, sym, _connectors[index])) {
#ifdef DEBUG
	printf("accepted\n");
#endif
	if (++index >= GDATA_BIN_MAX_PIXELS) {
	  _callBack->ReturnGDataBinConnectors(_cMap, _connectors, index);
	  index = 0;
	}
      } else {
#ifdef DEBUG
	printf("rejected\n");
#endif
      }
#ifdef 0
    }
#endif
    
    lastSym = sym;
#ifdef 0
    lastPixelX = thisPixelX;
    lastPixelY = thisPixelY;
#endif
    
    ptr += ptrIncr;
  }
  
  if (index > 0) {
#if !USE_CONNECTORS
      DOASSERT(0, "Trying to use _connectors\n");
#endif
    _callBack->ReturnGDataBinConnectors(_cMap, _connectors, index);
  }
}

/***********************************************************************
Return symbols
************************************************************************/

void GDataBin::ReturnSymbols(int &recordsProcessed)
{
  if (_returnIndex <= 0) {
    recordsProcessed = 0;
    return;
  }

  _numSymsReturned += _returnIndex;

#if defined(DEBUG)
  printf("GDataBin: returning %d symbols:\n", _returnIndex);
#endif

#ifdef DEBUG_SYMBOLS
  for(int i = 0; i < _returnIndex; i++){
    GDataBinRec *sym = (GDataBinRec *)_returnSyms[i];
    printf("%f ", sym->x);
  }
  printf("\n");
#endif

  _callBack->ReturnGDataBinRecs(_mapping, (void **)_returnSyms,
				_returnIndex, recordsProcessed);

  _returnIndex = 0;
  _iteration++;
}

/*********************************************************************
print statistics
**********************************************************************/

void GDataBin::PrintStat()
{
  printf("GDataBin: %d inserted, %d returned (%.2f%%)\n",
	 _numSyms, _numSymsReturned, 
	 (double)_numSymsReturned / (double)_numSyms * 100.0);
}
