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

  Revision 1.13 1997/10/16 zhenhai
  Removed _transform. It was used to optimize drawing on the same pixel.
  However it needs to access the transformation of WindowRep. First,
  this is violation of encapsulation of the WindowRep classes. Second,
  the problem is that GDataBin assumes WindowRep classes uses Transform class
  to calculate coordinates. However, this is not the case for OpenGL WindowRep.
  Third, if such an optimization needs to be done, it should be done at
  WindowRep level instead of GDataBin level.

  Revision 1.12  1997/09/05 22:36:18  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.11  1997/08/20 22:10:56  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.10.12.2  1997/08/15 23:06:26  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.10.12.1  1997/08/07 16:56:31  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.10  1996/11/20 20:35:19  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.9  1996/09/27 15:53:19  wenger
  Fixed a number of memory leaks.

  Revision 1.8  1996/07/19 17:24:40  jussi
  Added canElimRecords parameter to InsertSymbol().

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

#define USE_CONNECTORS 0

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
  virtual void ReturnGDataBinRecs(TDataMap *map, void **recs, int numRecs,
				  int &recordsProcessed) {}
  virtual void ReturnGDataBinConnectors(TDataCMap *cmap,
					Connector **connectors, int num) {}
};

class GDataBin {

public:
  GDataBin();
  ~GDataBin();

  /* Init before any data is returned from query processor */

  void Init(TDataMap *map, VisualFilter *filter, 
	    Boolean dispSymbol, Boolean dispConnector, TDataCMap *cMap,
	    GDataBinCallback *callback);

  /* Finalize */
  void Final(int &recordsProcessed) { ReturnSymbols(recordsProcessed); }

  /* Insert symbol into bin. The bin will push the symbols to Gdata
     when done */
  void InsertSymbol(RecId rid, void *recs, int numRecs,
                    int &recordsProcessed, int startIndex = 0,
		    int incr = 1, Boolean canElimRecords = true);
  
  /* Print statistics */
  void PrintStat();

private:

  /* Return symbols in the array */
  void ReturnSymbols(int &recordsProcessed);

  int _gRecSize;	/* size of GData record */

//  Transform2D *_transform;
//  int _maxYPixels;

  int _timestamp[GDATA_BIN_MAX_PIXELS];
  GDataBinRec *_returnSyms[GDATA_BIN_MAX_PIXELS];
  int _iteration;
  int _returnIndex;
  
  /* connectors to be returned */
  Connector *_connectors[GDATA_BIN_MAX_PIXELS]; 
  
  Boolean _dispSymbol;    /* TRUE if display symbol */
  Boolean _dispConnector; /* TRUE if display connector */
  TDataCMap *_cMap;	  /* connector mapping */
  
//  Boolean _needX; /* TRUE if we need to get X value for the bin */
//  int _pixelX;	/* get the X value for the bin */
  GDataBinCallback *_callBack;
  TDataMap *_mapping;

  /* statistics */
  int _numSyms;			/* # of symbols encountered */
  int _numSymsReturned;	/* # of symbols returned */
  
  int _elimOverlap; 	/* true to eliminate overlapping GData */
};

#endif
