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
  int i;
  for(i = 0; i < GDATA_BIN_MAX_PIXELS; i++)
    _timestamp[i] = 0;
  _iteration = 1;
  _returnIndex = 0;
  
  _elimOverlap = Init::ElimOverlap();

  /* Init space for connectors */
  for(i = 0; i < GDATA_BIN_MAX_PIXELS; i++)
    _connectors[i] = new Connector;
}

/**********************************************************************
Init before any data is returned from query processor 
************************************************************************/

void GDataBin::Init(TDataMap *mapping, VisualFilter *filter,
		    Transform2D *transform, Boolean dispSymbol,
		    Boolean dispConnector, TDataCMap *cMap,
		    GDataBinCallback *callback)
{
#ifdef DEBUG
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
  _transform = transform;
  _transform->TransformY(filter->xLow, filter->yLow, ylow);
  _transform->TransformY(filter->xHigh, filter->yHigh, yhigh);
  _maxYPixels = ROUND(int, fabs(yhigh - ylow));

#ifdef DEBUG
  printf("GDataBin: _maxYPixels: %d\n", _maxYPixels);
#endif
  if (!(_maxYPixels > 0 && _maxYPixels < GDATA_BIN_MAX_PIXELS))
    printf("GDataBin: yhigh %.2f, ylow %.2f, _maxYPixels: %d\n",
	   yhigh, ylow, _maxYPixels);
  DOASSERT(_maxYPixels >= 0 && _maxYPixels < GDATA_BIN_MAX_PIXELS,
	   "Illegal value for maxYPixels");

  _needX = true;
  _callBack = callback;
  _gRecSize = mapping->GDataRecordSize();
}

/***************************************************************************
Insert symbol into bin. The bin will push the symbols to Gdata when done 
****************************************************************************/

void GDataBin::InsertSymbol(RecId startRid, void *recs, int numRecs,
			    int startIndex, int incr, Boolean canElimRecords)
{
#ifdef DEBUG
  printf("GDataBin::InsertSymbol(%ld,0x%p,%d,%d,%d,%d)\n",
	 startRid, recs, numRecs, startIndex, incr, canElimRecords);
#endif
  
  _numSyms += numRecs;

  if (_dispSymbol) {

    /* pointer to next record to process */
    char *ptr = (char *)recs + startIndex * _gRecSize; 
  
    /* amount to increment pointer each iter */
    int ptrIncr = incr * _gRecSize;	
  
    if (!_elimOverlap || !canElimRecords) {
      /* DO NOT eliminate overlap */
      for(int i = startIndex; i < numRecs; i += incr) {
	_returnSyms[_returnIndex++] = (GDataBinRec *)ptr;
	if (_returnIndex >= GDATA_BIN_MAX_PIXELS)
	  ReturnSymbols();
	ptr += ptrIncr;
      }
    } else {
      /* Eliminate overlap here */
      
      for(int i = startIndex; i < numRecs; i += incr) {
	
	GDataBinRec *sym = (GDataBinRec *)ptr;
	
	// compute X pixel value for this symbol and see if it differs
	// from X pixel value for current bin
	
	Coord x, y;
	_transform->Transform(sym->x, sym->y, x, y);
	int thisPixelX = ROUND(int, x);
	int thisPixelY = ROUND(int, y);
	
	if (_needX) {
	  _pixelX = thisPixelX;
	  _needX = false;
	} else if (thisPixelX != _pixelX) {
	  _iteration++;
	  _pixelX = thisPixelX;
	}
	
	/* put symbol in the array */
	if (thisPixelY < 0)
	  thisPixelY = 0;
	else if (thisPixelY > _maxYPixels)
	  thisPixelY = _maxYPixels;
	
	if (_timestamp[thisPixelY] != _iteration) {
	  _timestamp[thisPixelY] = _iteration;
	  DOASSERT(_returnIndex < GDATA_BIN_MAX_PIXELS,
		   "Illegal value for returnIndex");
	  _returnSyms[_returnIndex++] = sym;
	  
	  // flush cache of symbols to screen?
	  if (_returnIndex >= GDATA_BIN_MAX_PIXELS)
	    ReturnSymbols();
	} else {
#ifdef DEBUGx
	  printf("Not adding x %f, X pixel %d\n", sym->x, thisPixelX);
#endif
	}
	
	ptr += ptrIncr;
      }
    }
    
    // flush any remaining symbols to screen
    ReturnSymbols();
  }
  
  // return if connectors should not be displayed
  if (!_dispConnector || incr != 1)
    return;

  /* display connector has been set to true and
     we are looking at records one at a time */
    
  char *ptr = (char *)recs + startIndex * _gRecSize; 
  
  GDataBinRec *lastSym = (GDataBinRec *)ptr;
  Coord x, y;
  _transform->Transform(lastSym->x, lastSym->y, x, y);
  int lastPixelX = ROUND(int, x);
  int lastPixelY = ROUND(int, y);
  
  /* amount to increment pointer in each iteration */
  
  int ptrIncr = _gRecSize;	
  int index = 0;
  ptr += ptrIncr;
  
  for(int i = startIndex + 1; i < numRecs; i++) {
    GDataBinRec *sym = (GDataBinRec *)ptr;
    
    _transform->Transform(sym->x, sym->y, x, y);
    int thisPixelX = ROUND(int, x);
    int thisPixelY = ROUND(int, y);
    
    if (lastPixelX != thisPixelX || lastPixelY != thisPixelY) {
      
#ifdef DEBUG
      printf("mapping syms: (%d,%d) (%d,%d): ",
	     lastPixelX, lastPixelY, thisPixelX, thisPixelY);
#endif
      
      /* draw a connection */
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
    }
    
    lastSym = sym;
    lastPixelX = thisPixelX;
    lastPixelY = thisPixelY;
    
    ptr += ptrIncr;
  }
  
  if (index > 0)
    _callBack->ReturnGDataBinConnectors(_cMap, _connectors, index);
}

/***********************************************************************
Return symbols
************************************************************************/

void GDataBin::ReturnSymbols()
{
  if (_returnIndex <= 0)
    return;

  _numSymsReturned += _returnIndex;

#ifdef DEBUG
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
				_returnIndex);
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
