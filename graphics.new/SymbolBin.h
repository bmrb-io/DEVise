/* SymbolBin: used to bin pixels together before returning the data */
#ifndef SymbolBin_h
#define SymbolBin_h
#include "DeviseTypes.h"
#include "RecId.h"

const int MAX_PIXELS = 2048;

class QueryCallback;
class TData;
class Symbol;
class SymbolBin {
class TDataMap;

public:
	SymbolBin();

	/* Init before any data is returned from query processor */
	void Init(TDataMap *map, Coord yLow, Coord yHigh, Coord yPerPixel,  
		Coord xPerPixel, QueryCallback *callback );

	/* finalize */
	void Final(){ ReturnSymbols();}

	/* Insert symbol into bin. The bin will push the symbols to Gdata
	when done */
	void InsertSymbol(TData *tdata, RecId rid, void **rec,
		int numRecs, int startIndex=0, int incr=1);
	void OldInsertSymbol(TData *tdata, TDataMap *map, RecId rid, void **rec,
		int numRecs);

	/* print statistics */
	void PrintStat();
private:

	/* Return symbols in the array */
	void ReturnSymbols();

	/* Free Symbols */
	Coord _yLow, _yHigh;
	int _numPixels;
	int _iter[MAX_PIXELS];
	Symbol *_returnSyms[MAX_PIXELS];
	Symbol *_saveSyms[MAX_PIXELS];
	int _iteration;
	int _returnIndex;

	Boolean _needX; /* TRUE if we need to get X value for the bin */
	Coord _xVal;	/* get the X value for the bin */
	QueryCallback *_callBack;
	Coord _xPerPixel, _yPerPixel;
	TDataMap *_mapping;

	/* statistics */
	int _numSyms;			/* # of symbols encountered */
	int _numSymsReturned;	/* # of symbols returned */
};

#endif
