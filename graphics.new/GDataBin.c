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
  Revision 1.3  1995/11/24 21:25:51  jussi
  Added copyright notice and cleaned up code. Fixed inconsistency
  in computing exact pixel values in overlap elimination vs. pixel
  values computed by matrix transformations in XWindowRep.

  Revision 1.2  1995/09/05 22:14:48  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>

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
  for(int i = 0; i < GDATA_BIN_MAX_PIXELS; i++)
    _timestamp[i] = 0;
  _iteration = 1;
  _returnIndex = 0;
  
  _elimOverlap = Init::ElimOverlap();

  /* Init space for connectors */
  for(i = 0; i < GDATA_BIN_MAX_PIXELS; i++)
    _returnConnectors[i] = new Connector;
}

/**********************************************************************
Init before any data is returned from query processor 
************************************************************************/

void GDataBin::Init(TDataMap *mapping, VisualFilter *filter,
		    Transform2D *transform, Boolean dispConnector,
		    TDataCMap *cMap, GDataBinCallback *callback)
{
  if (_returnIndex != 0) {
    fprintf(stderr,"symbolBin::Init: _returnIndex <> 0\n");
    Exit::DoExit(2);
  }
  
#ifdef DEBUG
  printf("GDataBinInit::Transform: ");
  transform->Print();
  printf("\n");
#endif

  _numSyms = 0;
  _numSymsReturned = 0;

  _mapping = mapping;
  _dispConnector = dispConnector;
  _cMap = cMap;

#ifdef CALCULATE_DIRECTLY
  Coord xlow, ylow, xhigh, yhigh;
  _transform = transform;
  _transform->Transform(filter->xLow, filter->yLow, xlow, ylow);
  _transform->Transform(filter->xHigh, filter->yHigh, xhigh, yhigh);
  _xPerPixel = (filter->xHigh - filter->xLow) / fabs(xhigh - xlow);
  _yPerPixel = (filter->yHigh - filter->yLow) / fabs(yhigh - ylow);
  _xLow = filter->xLow;
  _yLow = filter->yLow;
  _maxYPixels = ROUND(int, fabs(yhigh - ylow));
#ifdef DEBUG
  printf("GDataBin: x/y per pixel: %.2f, %.2f\n", _xPerPixel, _yPerPixel);
#endif
#else
  Coord ylow, yhigh;
  _transform = transform;
  _transform->TransformY(filter->xLow, filter->yLow, ylow);
  _transform->TransformY(filter->xHigh, filter->yHigh, yhigh);
  _maxYPixels = ROUND(int, fabs(yhigh - ylow));
#endif

#ifdef DEBUG
  printf("GDataBin: _maxYPixels: %d\n", _maxYPixels);
#endif
  assert(_maxYPixels > 0 && _maxYPixels < GDATA_BIN_MAX_PIXELS);

  _needX = true;
  _callBack = callback;
  _gRecSize = mapping->GDataRecordSize();
}

/***************************************************************************
Insert symbol into bin. The bin will push the symbols to Gdata when done 
****************************************************************************/

void GDataBin::InsertSymbol(RecId startRid, void *recs, int numRecs,
			    int startIndex, int incr)
{
#ifdef DEBUG
  printf("GDataBin::InsertSymbol(%d,0x%x,%d,%d,%d)\n",
	 startRid, recs, numRecs, startIndex, incr);
#endif
  
  _numSyms += numRecs;

  /* pointer to next record to process */
  char *ptr = (char *)recs + startIndex * _gRecSize; 
  
  /* amount to increment pointer each iter*/
  int ptrIncr = incr * _gRecSize;	
  
  if (!_elimOverlap){
    /* DO NOT eliminate overlap */
    for(int i = startIndex; i < numRecs; i += incr) {
      _returnSyms[_returnIndex++] = (GDataBinRec *)ptr;
      if (_returnIndex >= GDATA_BIN_MAX_PIXELS)
	ReturnSymbols();
      ptr += ptrIncr;
    }
    if (_returnIndex > 0)
      ReturnSymbols();
    return;
  }

  /* Eliminate overlap here */

  for(int i = startIndex; i < numRecs; i += incr) {

    GDataBinRec *sym = (GDataBinRec *)ptr;

    // compute X pixel value for this symbol and see if it differs
    // from X pixel value for current bin

#ifdef CALCULATE_DIRECTLY
    int thisPixelX = ROUND(int, (sym->x - _xLow) / _xPerPixel);
    int thisPixelY = ROUND(int, (sym->y - _yLow) / _yPerPixel);
#else
    Coord x, y;
    _transform->Transform(sym->x, sym->y, x, y);
    int thisPixelX = ROUND(int, x);
    int thisPixelY = ROUND(int, y);
#endif

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
      assert(_returnIndex < GDATA_BIN_MAX_PIXELS);
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
  
  if (_dispConnector && incr == 1 ) {

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

    for(i = startIndex+1; i < numRecs; i++) {
      GDataBinRec *sym = (GDataBinRec *)ptr;

      _transform->Transform(sym->x, sym->y, x, y);
      int thisPixelX = ROUND(int, x);
      int thisPixelY = ROUND(int, y);

#ifdef DEBUG
      printf("testing syms: (%d,%d) (%d,%d)\n",
	     lastPixelX, lastPixelY, thisPixelX, thisPixelY);
#endif

      if (lastPixelX != thisPixelX || lastPixelY != thisPixelY) {

	/* draw a connection */
	if (_cMap->MapToConnection(lastSym, sym, 
				   _returnConnectors[index])) {
#ifdef DEBUG
	  printf("accepted\n");
#endif
	  if (++index >= GDATA_BIN_MAX_PIXELS){
	    _callBack->ReturnGDataBinConnectors(_cMap,
						_returnConnectors, index);
	    index = 0;
	  }
	}
      }
      
      lastSym = sym;
      lastPixelX = thisPixelX;
      lastPixelY = thisPixelY;

      ptr += ptrIncr;
    }
    
    if (index > 0)
      _callBack->ReturnGDataBinConnectors(_cMap,
					  _returnConnectors, index);
  }

  // flush any remaining symbols to screen
  ReturnSymbols();
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
