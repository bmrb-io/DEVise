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
  _numSyms = 0;
  _numSymsReturned = 0;

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

void GDataBin::Init(TDataMap *mapping, Coord yLow,
		    Coord yHigh, Coord xLow, Coord xHigh,
		    Coord yPerPixel, Coord xPerPixel, 
		    Boolean dispConnector, TDataCMap *cMap,
		    GDataBinCallback *callback)
{
  if (_returnIndex != 0) {
    fprintf(stderr,"symbolBin::Init: _returnIndex <> 0\n");
    Exit::DoExit(2);
  }
  
#ifdef DEBUG
  printf("GDataBinInit::xPerPixel = %f, yPerPixel = %f\n",
	 xPerPixel, yPerPixel);
#endif

  _mapping = mapping;
  _dispConnector = dispConnector;
  _cMap = cMap;
  _yLow = yLow; _yHigh = yHigh;
  _xLow = xLow; _xHigh = xHigh;
  _numPixels = ROUND(int, (yHigh - yLow) / yPerPixel);
  _needX = true;
  _callBack = callback;
  _xPerPixel = xPerPixel;
  _yPerPixel = yPerPixel;
  _gRecSize = mapping->GDataRecordSize();
  
  if (_numPixels > GDATA_BIN_MAX_PIXELS){
    fprintf(stderr,"GDataBin:: %d Pixels: too many\n");
    Exit::DoExit(2);
  }
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
  char *ptr = (char *)recs+ startIndex * _gRecSize; 
  
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

  GDataBinRec *sym = (GDataBinRec *)ptr;

  if (_needX) {
    _pixelX = ROUND(int, (sym->x - _xLow) / _xPerPixel);
    _needX = false;
  }

  for(int i = startIndex; i < numRecs; i += incr) {

    GDataBinRec *sym = (GDataBinRec *)ptr;

    // compute X pixel value for this symbol and see if it differs
    // from X pixel value for current bin

    int thisPixelX = ROUND(int, (sym->x - _xLow) / _xPerPixel);
    if (thisPixelX != _pixelX) {
      _iteration++;
      _pixelX = thisPixelX;
    }
    
    /* put symbol in the array */
    int pixelY;
    if (sym->y < _yLow)
      pixelY = 0;
    else if (sym->y >= _yHigh)
      pixelY = _numPixels - 1;
    else
      pixelY = ROUND(int, (sym->y - _yLow) / _yPerPixel);

    if (_timestamp[pixelY] != _iteration) {
      _timestamp[pixelY] = _iteration;
      assert(_returnIndex < GDATA_BIN_MAX_PIXELS);
      _returnSyms[_returnIndex++] = sym;

      // flush cache of symbols to screen?
      if (_returnIndex >= GDATA_BIN_MAX_PIXELS)
	ReturnSymbols();
    }
#ifdef DEBUG
    else
      printf("Not adding x %f, X pixel %d\n", sym->x, thisPixelX);
#endif

    ptr += ptrIncr;
  }
  
  if (_dispConnector && incr == 1 ) {
    /* display connector has been set to true and
       we are looking at records one at a time */
    
    char *ptr = (char *)recs + startIndex*_gRecSize; 
    GDataBinRec *lastSym = (GDataBinRec *)ptr;
    
    /* amount to increment pointer each iter*/
    int ptrIncr = _gRecSize;	
    int index = 0;
    ptr += ptrIncr;
    for (i= startIndex+1; i < numRecs; i++){
      sym = (GDataBinRec *)ptr;
      /*
	 printf("testing syms: (%f,%f) (%f,%f)\n",
	 lastSym->x, lastSym->y, sym->x, sym->y);
      */
      if (fabs(lastSym->x- sym->x) >= _xPerPixel || 
	  fabs(lastSym->y - sym->y) >= _yPerPixel)
	/* draw a connection */
	if (_cMap->MapToConnection(lastSym, sym, 
				   _returnConnectors[index])){
	  /*
	     printf("accepted\n");
	  */
	  if (++index >= GDATA_BIN_MAX_PIXELS){
	    _callBack->ReturnGDataBinConnectors(_cMap,
						_returnConnectors, index);
	    index =0;
	  }
	}
      
      lastSym = sym;
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
  printf("GDataBin:returning %d symbols, x/y per pixel(%f,%f):\n",
	 _returnIndex, _xPerPixel, _yPerPixel);
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
  printf("GDataBin %d inserted, %d returned %f%%\n",
	 _numSyms, _numSymsReturned, 
	 (double)_numSymsReturned/(double)_numSyms*100.0);
}
