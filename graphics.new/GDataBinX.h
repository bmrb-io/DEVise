/*
  $Id$

  $Log$*/

/* used to bin pixels together before returning the data */

#ifndef GDataBinX_h
#define GDataBinX_h
#include "SymbolBin.h"
#include "DeviseTypes.h"
#include "RecId.h"


class QueryCallback;
class TData;
class GDataVar;
class TDataMap;
class TDataCMap;
struct Connector;

/* GDataBin assumes all records have at least x and y fields */
struct GDataBinXRec {
	Coord x;
	Coord y;
};

class GDataBinX {


public:
	GDataBinX();

	/* Init before any data is returned from query processor */
	void Init(TDataMap *map, Coord xLow, Coord xHigh, Coord yPerPixel,
		Coord xPerPixel, Boolean dispConnector, 
		TDataCMap *cMap, QueryCallback *callback);

	/* finalize */
	void Final(){ ReturnSymbols();}

	/* Insert symbol into bin. The bin will push the symbols to Gdata
	when done */
	void InsertSymbol(RecId rid, void *rec,
		int numRecs, int startIndex=0, int incr=1);

	/* print statistics */
	void PrintStat();
private:

	/* Return symbols in the array */
	void ReturnSymbols();

	/* Free Symbols */
	int _gRecSize; /* size of GData records */
	Coord _xLow, _xHigh;
	int _numPixels;
	int _iter[MAX_PIXELS];
	GDataBinXRec *_returnSyms[MAX_PIXELS];
	int _iteration;
	int _returnIndex;
	Connector *_returnConnectors[MAX_PIXELS]; /* connectors to be returned */
	Boolean _dispConnector; /* TRUE if display connector */
	TDataCMap *_cMap;	/* connector mapping */

	Boolean _needY; /* TRUE if we need to get Y value for the bin */
	Coord _yVal;	/* get the X value for the bin */
	Coord _yPerPixel; 
	Coord _xPerPixel;
	QueryCallback *_callBack;
	TDataMap *_mapping;

	/* statistics */
	int _numSyms;			/* # of symbols encountered */
	int _numSymsReturned;	/* # of symbols returned */
};

#endif
