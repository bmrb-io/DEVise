/* GDataBinX.c */
#include <stdio.h>
#include <math.h>
#include "Exit.h"
#include "QueryProc.h"
#include "GDataBinX.h"
#include "Connector.h"
#include "TDataMap.h"
#include "TDataCMap.h"


/********************************************************************
Initializer
**********************************************************************/
GDataBinX::GDataBinX(){
	_numSyms = 0;
	_numSymsReturned = 0;
	int i;
	for (i=0; i < MAX_PIXELS; i++)
		_iter[i] = 0;
	_iteration = 1;
	_returnIndex = 0;

	/* Init space for connectors */
	for (i=0; i < MAX_PIXELS; i++)
		_returnConnectors[i] = new Connector;
}

/**********************************************************************
Init before any data is returned from query processor 
************************************************************************/
void GDataBinX::Init(TDataMap *mapping, Coord xLow, 
	Coord xHigh, Coord yPerPixel, Coord xPerPixel, 
	Boolean dispConnector, TDataCMap *cMap, QueryCallback *callback){
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
	_xLow = xLow;
	_xHigh = xHigh;
	_numPixels = (int)ceil((xHigh-xLow)/xPerPixel);
	_needY = true;
	_callBack = callback;
	_xPerPixel = xPerPixel;
	_yPerPixel = yPerPixel;
	_gRecSize = mapping->GDataRecordSize();

	if (_numPixels > MAX_PIXELS){
		fprintf(stderr,"GDataBin:: %d Pixels: too many\n");
		Exit::DoExit(2);
	}

}

/***************************************************************************
Insert symbol into bin. The bin will push the symbols to Gdata when done 
****************************************************************************/
void GDataBinX::InsertSymbol(RecId startRid,
	void *recs, int numRecs, int startIndex, int incr){
	/*
	printf("new page %d recs\n", numRecs);
	*/

	int i;
	_numSyms += numRecs;

	GDataBinXRec *sym;
	char *ptr = (char *)recs+ startIndex*_gRecSize;
	int ptrIncr = incr*_gRecSize;
	if (_needY){
		/*
		printf("need Y: %f\n",sym->y);
		*/
		_yVal = ((GDataBinXRec *)ptr)->y;
		_needY = false;
	}

	for (i= startIndex; i < numRecs; i += incr){
		sym = (GDataBinXRec *)ptr;
		if (fabs(sym->y-_yVal) > _yPerPixel){
			/*
			printf("fabs:sym: %f,yval: %f,yPerPix %f\n",sym->y,_yVal,_yPerPixel);
			*/
			/* another Y value. Return the data */
			ReturnSymbols();
			_yVal = sym->y;
		}

		/* put symbol in the array */
		int index;
		if (sym->x < _xLow)
			index = 0;
		else if (sym->x >= _xHigh)
			index = _numPixels-1;
		else index = (int)((sym->x - _xLow)/_xPerPixel);
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
		ptr = (char *)recs+startIndex*_gRecSize;
		GDataBinXRec *lastSym = (GDataBinXRec *)ptr;
		ptr += ptrIncr;
		int index = 0;
		for (i= startIndex+1; i < numRecs; i++){
			sym = (GDataBinXRec *)ptr;
			if (fabs(lastSym->x- sym->x) >= _xPerPixel || 
				fabs(lastSym->y - sym->y) >= _yPerPixel)
				/* draw a connection */
				if (_cMap->MapToConnection(lastSym, sym, 
						_returnConnectors[index]))
					index++;
			
			lastSym = sym;

			ptr += ptrIncr;
		}

		/*
		if (index > 0)
			_callBack->ReturnConnector(_cMap,_returnConnectors, index);
		*/
	}
	ReturnSymbols();

}

/***********************************************************************
Return symbols
************************************************************************/
void GDataBinX::ReturnSymbols(){
	int i;

	if (_returnIndex > 0){
		_numSymsReturned += _returnIndex;
		/*
		printf("returning %d symbols",_returnIndex);
		*/
		/*
		_callBack->ReturnGData(_mapping, (void **)_returnSyms,_returnIndex);
		*/
		_returnIndex = 0;
		_iteration++;
	}

}

/*********************************************************************
print statistics
**********************************************************************/
void GDataBinX::PrintStat(){
	printf("GDataBin %d inserted, %d returned %f%%\n",
		_numSyms, _numSymsReturned, 
		(double)_numSymsReturned/(double)_numSyms*100.0);
}
