/* GDataBin: used to bin pixels together before returning the data */
#ifndef GDataBin_h
#define GDataBin_h
#include "DeviseTypes.h"
#include "RecId.h"

class QueryCallback;
class TData;
class GDataVar;
class TDataMap;
class TDataCMap;
struct Connector;

/* GDataBin assumes all records have at least x and y fields */
struct GDataBinRec {
	Coord x;
	Coord y;
};

const int GDATA_BIN_MAX_PIXELS = 2048;

class GDataBinCallback {
public:
	virtual void ReturnGDataBinRecs(TDataMap *map, void **recs, int numRecs){};
	virtual void ReturnGDataBinConnectors(TDataCMap *cmap,
		Connector **connectors, int num){};
};

class GDataBin {

public:
	GDataBin();

	/* Init before any data is returned from query processor */
	void Init(TDataMap *map, Coord yLow, Coord yHigh, 
		Coord xLow, Coord xHigh, 
		Coord yPerPixel, Coord xPerPixel, Boolean dispConnector, 
		TDataCMap *cMap, GDataBinCallback *callback);

	/* finalize */
	void Final(){ ReturnSymbols();}

	/* Insert symbol into bin. The bin will push the symbols to Gdata
	when done */
	void InsertSymbol(RecId rid, void *recs,
		int numRecs, int startIndex=0, int incr=1);

	/* print statistics */
	void PrintStat();
private:

	/* Return symbols in the array */
	void ReturnSymbols();

	int _gRecSize;	/* size of GData record */
	/* Free Symbols */
	Coord _yLow, _yHigh, _xLow, _xHigh;
	int _numPixels;
	int _iter[GDATA_BIN_MAX_PIXELS];
	GDataBinRec *_returnSyms[GDATA_BIN_MAX_PIXELS];
	int _iteration;
	int _returnIndex;

	/* connectors to be returned */
	Connector *_returnConnectors[GDATA_BIN_MAX_PIXELS]; 

	Boolean _dispConnector; /* TRUE if display connector */
	TDataCMap *_cMap;	/* connector mapping */

	Boolean _needX; /* TRUE if we need to get X value for the bin */
	Coord _xVal;	/* get the X value for the bin */
	Coord _yPerPixel; 
	Coord _xPerPixel;
	GDataBinCallback *_callBack;
	TDataMap *_mapping;

	/* statistics */
	int _numSyms;			/* # of symbols encountered */
	int _numSymsReturned;	/* # of symbols returned */

	int _elimOverlap; 	/* true to eliminate overlapping GData */
};

#endif
