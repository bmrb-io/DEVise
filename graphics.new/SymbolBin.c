/* SymbolBin.c */
#include <stdio.h>
#include <math.h>
#include "Exit.h"
#include "QueryProc.h"
#include "Symbol.h"
#include "SymbolBin.h"
#include "ams.h"
#include "TDataMap.h"


/********************************************************************
Initializer
**********************************************************************/
SymbolBin::SymbolBin(){
	_numSyms = 0;
	_numSymsReturned = 0;
	int i;
	for (i=0; i < MAX_PIXELS; i++)
		_iter[i] = 0;
	_iteration = 1;
	_returnIndex = 0;

	/* allocate symbols */
	for (i=0; i < MAX_PIXELS; i++){
		Symbol *sym = _returnSyms[i] = new Symbol;
		sym->Ref();

		sym = _saveSyms[i] = new Symbol;
		sym->Ref();
	}

}

/**********************************************************************
Init before any data is returned from query processor 
************************************************************************/
void SymbolBin::Init(TDataMap *mapping,
	Coord yLow, Coord yHigh, Coord yPerPixel, Coord xPerPixel,
	QueryCallback *callback){
	if (_returnIndex != 0){
		fprintf(stderr,"symbolBin::Init: _reuturnIndex <> 0\n");
		Exit::DoExit(2);
	}

/*
printf("SymbolBinInit::xPerPixel %f, yPerPixel %f\n", xPerPixel, yPerPixel);
*/

	_mapping = mapping;
	_yLow = yLow;
	_yHigh = yHigh;
	_numPixels = (int)ceil((yHigh-yLow)/yPerPixel);
	_needX = true;
	_callBack = callback;
	_xPerPixel = xPerPixel;
	_yPerPixel = yPerPixel;

	if (_numPixels > MAX_PIXELS){
		fprintf(stderr,"SymbolBin:: %d Pixels: too many\n");
		Exit::DoExit(2);
	}

}

#ifdef BATCH
/***************************************************************************
Insert symbol into bin. The bin will push the symbols to Gdata when done 
****************************************************************************/
void SymbolBin::InsertSymbol(TData *tdata, RecId startRid,
	void **recs, int numRecs, int startIndex, int incr){
	/*
	printf("new page %d recs\n", numRecs);
	*/

	int i;
	_numSyms += numRecs;

	/*
	int j;
	printf("SymbolBin:InsertSymbol: %d records, x:%f y:%f\n",numRecs,_xPerPixel,
		_yPerPixel);
	for (j=0; j < numRecs; j++){
		AmsData *data = (AmsData *)recs[j];
		printf("(%f,%f)",data->time,data->solDn);
	}
	printf("\n");
	*/

	/* instantiate all symbols */
	_mapping->MapToSymbolArray(tdata,startRid, recs, _saveSyms, numRecs,
		startIndex, incr);

	/*
	for (j=0; j < numRecs; j++){
		printf("(%f,%f)",_saveSyms[j]->x,_saveSyms[j]->y);
	}
	printf("\n");
	*/


	if (_needX){
		/*
		printf("need X: %f\n",sym->x);
		*/
		_xVal = _saveSyms[startIndex]->x;
		_needX = false;
	}

	Symbol *sym;
	for (i= startIndex; i < numRecs; i += incr){
		sym = _saveSyms[i];
		if (fabs(sym->x-_xVal) > _xPerPixel){
			/*
			printf("fabs:sym: %f,xval: %f,xPerPix %f\n",sym->x,_xVal,_xPerPixel);
			*/
			/* another X value. Return the data */
			ReturnSymbols();
			_xVal = sym->x;
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
	}

}
#else /* BATCH */

/***************************************************************************
Insert symbol into bin. The bin will push the symbols to Gdata when done 
****************************************************************************/
void SymbolBin::InsertSymbol(TData *tdata, RecId startRid,
	void **recs, int numRecs){

	int i;
	_numSyms += numRecs;

	for (i=0; i < numRecs; i++){
		AmsData *amsData = (AmsData *)recs[i];

		if (_needX){
			_xVal = amsData->time;
			_needX = false;
		}
		else if (fabs(amsData->time-_xVal) > _xPerPixel){
			/* another x value */
			ReturnSymbols();
			_xVal = amsData->time;
		}

		/* put symbol in the array */
		int index;
		if (amsData->solDn < _yLow)
			index = 0;
		else if (amsData->solDn >= _yHigh)
			index = _numPixels-1;
		else index = (int)trunc((amsData->solDn - _yLow)/_yPerPixel);


		if (_iter[index] != _iteration){
			_iter[index] = _iteration;
			_mapping->MapToSymbol(tdata,startRid+i,amsData,
					_returnSyms[_returnIndex++]);
		}
	}
}
#endif

/***********************************************************************
Return symbols
************************************************************************/
void SymbolBin::ReturnSymbols(){
	int i;

	if (_returnIndex > 0){
		_numSymsReturned += _returnIndex;
		/*
		printf("returning %d symbols",_returnIndex);
		*/
		_callBack->ReturnSymbols(_mapping,
			_returnSyms,_returnIndex,true, false,false);
		_returnIndex = 0;
		_iteration++;
	}

}

/*********************************************************************
print statistics
**********************************************************************/
void SymbolBin::PrintStat(){
	printf("SymbolBin %d inserted, %d returned %f%%\n",
		_numSyms, _numSymsReturned, 
		(double)_numSymsReturned/(double)_numSyms*100.0);
}
