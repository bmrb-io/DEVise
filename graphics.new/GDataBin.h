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
  Revision 1.7  1996/07/19 03:22:35  jussi
  Added RecId to GDataBinRec.

  Revision 1.6  1995/12/29 22:42:20  jussi
  Added support for line connectors.

  Revision 1.5  1995/11/25 01:24:25  jussi
  Removed #ifdef CALCULATE_DIRECTLY which allowed one to quickly
  switch back to the xPerPixel and yPerPixel method for testing.

  Revision 1.4  1995/11/25  01:20:09  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.
 
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

class QueryCallback;
class TData;
class GDataVar;
class TDataMap;
class TDataCMap;
struct Connector;

/* GDataBin assumes all records have at least x and y fields as well
   as the record ID; note that sizeof(RecId) < sizeof(Coord); we
   need to pad the struct so that GDataBinRec->x is at the same
   offset as in a real GData record */

struct GDataBinRec {
  union {
      RecId recId;
      Coord dummy;
  } r;
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
	    Boolean dispSymbol, Boolean dispConnector, TDataCMap *cMap,
	    GDataBinCallback *callback);

  /* Finalize */
  void Final() { ReturnSymbols(); }

  /* Insert symbol into bin. The bin will push the symbols to Gdata
     when done */
  void InsertSymbol(RecId rid, void *recs, int numRecs,
                    int startIndex = 0, int incr = 1,
                    Boolean canElimRecords = true);
  
  /* Print statistics */
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
  Connector *_connectors[GDATA_BIN_MAX_PIXELS]; 
  
  Boolean _dispSymbol;    /* TRUE if display symbol */
  Boolean _dispConnector; /* TRUE if display connector */
  TDataCMap *_cMap;	  /* connector mapping */
  
  Boolean _needX; /* TRUE if we need to get X value for the bin */
  int _pixelX;	/* get the X value for the bin */
  GDataBinCallback *_callBack;
  TDataMap *_mapping;

  /* statistics */
  int _numSyms;			/* # of symbols encountered */
  int _numSymsReturned;	/* # of symbols returned */
  
  int _elimOverlap; 	/* true to eliminate overlapping GData */
};

#endif
