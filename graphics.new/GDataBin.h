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
  Revision 1.3  1995/11/24 21:33:54  jussi
  Added copyright notice and cleaned up the code. Made _pixelX an
  int instead of Coord.

  Revision 1.2  1995/09/05 22:14:49  jussi
  Added CVS header.
*/

/* used to bin pixels together before returning the data */

#ifndef GDataBin_h
#define GDataBin_h

#include "DeviseTypes.h"
#include "VisualArg.h"
#include "Transform.h"
#include "RecId.h"

//#define CALCULATE_DIRECTLY

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
  virtual void ReturnGDataBinRecs(TDataMap *map, void **recs, int numRecs) {}
  virtual void ReturnGDataBinConnectors(TDataCMap *cmap,
					Connector **connectors, int num) {}
};

class GDataBin {

public:
  GDataBin();

  /* Init before any data is returned from query processor */
  void Init(TDataMap *map, VisualFilter *filter, Transform2D *transform,
	    Boolean dispConnector, TDataCMap *cMap,
	    GDataBinCallback *callback);

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

  Transform2D *_transform;
  int _maxYPixels;

  int _timestamp[GDATA_BIN_MAX_PIXELS];
  GDataBinRec *_returnSyms[GDATA_BIN_MAX_PIXELS];
  int _iteration;
  int _returnIndex;
  
  /* connectors to be returned */
  Connector *_returnConnectors[GDATA_BIN_MAX_PIXELS]; 
  
  Boolean _dispConnector; /* TRUE if display connector */
  TDataCMap *_cMap;	/* connector mapping */
  
  Boolean _needX; /* TRUE if we need to get X value for the bin */
  int _pixelX;	/* get the X value for the bin */
  GDataBinCallback *_callBack;
  TDataMap *_mapping;

#ifdef CALCULATE_DIRECTLY
  Coord _xLow, _yLow;
  Coord _xPerPixel, _yPerPixel;
#endif

  /* statistics */
  int _numSyms;			/* # of symbols encountered */
  int _numSymsReturned;	/* # of symbols returned */
  
  int _elimOverlap; 	/* true to eliminate overlapping GData */
};

#endif
