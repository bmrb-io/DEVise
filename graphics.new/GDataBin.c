/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <math.h>
#include "Exit.h"
#include "GDataBin.h"
#include "Connector.h"
#include "TDataMap.h"
#include "TDataCMap.h"
#include "Init.h"

extern int debug;

/********************************************************************
Initializer
**********************************************************************/
GDataBin::GDataBin(){
	_numSyms = 0;
	_numSymsReturned = 0;
	int i;
	for (i=0; i < GDATA_BIN_MAX_PIXELS; i++)
		_iter[i] = 0;
	_iteration = 1;
	_returnIndex = 0;

	_elimOverlap = Init::ElimOverlap();


	/* Init space for connectors */
	for (i=0; i < GDATA_BIN_MAX_PIXELS; i++)
		_returnConnectors[i] = new Connector;
}

/**********************************************************************
Init before any data is returned from query processor 
************************************************************************/
void GDataBin::Init(TDataMap *mapping, Coord yLow, 
	Coord yHigh, Coord xLow, Coord xHigh, Coord yPerPixel, Coord xPerPixel, 
	Boolean dispConnector, TDataCMap *cMap, GDataBinCallback *callback){
	if (_returnIndex != 0){
		fprintf(stderr,"symbolBin::Init: _reuturnIndex <> 0\n");
		Exit::DoExit(2);
	}

/*
printf("GDataBinInit::xPerPixel = %f, yPerPixel = %f\n", xPerPixel, yPerPixel);
*/

	_mapping = mapping;
	_dispConnector = dispConnector;
	_cMap = cMap;
	_yLow = yLow; _yHigh = yHigh; _xLow = xLow; _xHigh = xHigh;
	_numPixels = (int)ceil((yHigh-yLow)/yPerPixel);
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
void GDataBin::InsertSymbol(RecId startRid,
	void *recs, int numRecs, int startIndex, int incr){
if (debug > 1)
	printf("GDataBin::InsertSymbol(%d,0x%x,%d,%d,%d)\n",
		startRid, recs, numRecs, startIndex, incr);

	int i;
	_numSyms += numRecs;

	/* pointer to next record to process */
	char *ptr = (char *)recs+ startIndex*_gRecSize; 

	/* amount to increment pointer each iter*/
	int ptrIncr = incr*_gRecSize;	

	if (!_elimOverlap){
		/* DO NOT eliminate overlap */
		for (i= startIndex; i < numRecs; i += incr){
			_returnSyms[_returnIndex++] = (GDataBinRec *)ptr;
			if (_returnIndex >= GDATA_BIN_MAX_PIXELS)
				ReturnSymbols();
			ptr += ptrIncr;
		}
		return;
	}

	/* Eliminate ovlerap here */
	GDataBinRec *sym;
	if (_needX){
		_xVal = _xLow+
			trunc((((GDataBinRec *)ptr)->x - _xLow)/_xPerPixel)*_xPerPixel;
		/*
		_xVal = ptr->x;
		*/
		_needX = false;
	}

	for (i= startIndex; i < numRecs; i += incr){
		sym = (GDataBinRec *)ptr;
		/*
		printf("(%f,%f)", sym->x, sym->y);
		*/
		if (fabs(sym->x-_xVal) > _xPerPixel){
			/*
			printf("fabs:sym: %f,xval: %f,xPerPix %f\n",sym->x,_xVal,_xPerPixel);
			*/
			/* another X value. Return the data */
			ReturnSymbols();
			_xVal = _xLow+
				trunc((((GDataBinRec *)ptr)->x - _xLow)/_xPerPixel)*_xPerPixel;
			/*
			_xVal = sym->x;
			*/
		}

		/* put symbol in the array */
		int index;
		if (sym->y < _yLow)
			index = 0;
		else if (sym->y >= _yHigh)
			index = _numPixels-1;
		else index = (int)((sym->y - _yLow)/_yPerPixel);
		/*
		printf("%d ",index);
		*/


		if (_iter[index] != _iteration){
			_iter[index] = _iteration;
			_returnSyms[_returnIndex++] = sym;
		}
		ptr += ptrIncr;
	}

	if (_dispConnector && incr == 1 ){
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
	ReturnSymbols();

}

/***********************************************************************
Return symbols
************************************************************************/
void GDataBin::ReturnSymbols(){
	int i;

	if (_returnIndex > 0){
		_numSymsReturned += _returnIndex;
		/*
		printf("returning %d symbols",_returnIndex);
		*/
extern int debug;
if (debug > 1){
	printf("GDataBin:returning %d symbols, x/y per pixel(%f,%f): ",
		_returnIndex,_xPerPixel, _yPerPixel);
	for (int i=0; i < _returnIndex; i++){
		GDataBinRec *sym = (GDataBinRec *)_returnSyms[i];
		printf("%f ", sym->x);
	}
	printf("\n");
}
		_callBack->ReturnGDataBinRecs(_mapping, (void **)_returnSyms,
			_returnIndex);
		_returnIndex = 0;
		_iteration++;
	}

}

/*********************************************************************
print statistics
**********************************************************************/
void GDataBin::PrintStat(){
	printf("GDataBin %d inserted, %d returned %f%%\n",
		_numSyms, _numSymsReturned, 
		(double)_numSymsReturned/(double)_numSyms*100.0);
}
