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
  Revision 1.3  1995/11/25 01:20:19  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.2  1995/09/05 22:16:05  jussi
  Added CVS header.
*/

#ifndef TDataViewX_h
#define TDataViewX_h

#include "QueryProc.h"
#include "ViewGraph.h"
#include "GDataBin.h"
#include "TDataCMap.h"
#include "Color.h"
#include "BasicStats.h"

class TDataViewX: public ViewGraph, private QueryCallback,
	private GDataBinCallback {
public:

  TDataViewX(char *name, VisualFilter &initFilter, QueryProc *qp, 
	     Color fg= BlackColor, Color bg= WhiteColor,
	     AxisLabel *xAxis=NULL, AxisLabel *yAxis=NULL,
	     Action *action=NULL);
	
  virtual void InsertMapping(TDataMap *map);

protected:
  /* from View */
  virtual void DerivedStartQuery(VisualFilter &filter, int timestamp);
  virtual void DerivedAbortQuery();
  
  virtual void PrintStat();

private:
  /* From GDataBinCallback */
  virtual void ReturnGDataBinRecs(TDataMap *map, void **recs, int numRecs);
  virtual void ReturnGDataBinConnectors(TDataCMap *cmap,
					Connector **connectors, int num);

  /* from QueryCallback */
  /* Query data ready to be returned. Do initialization here.*/
  virtual void QueryInit(void *userData);
  
  virtual void ReturnGData(TDataMap *mapping, RecId id,
			   void *gdata, int numGData);
  
  /* Done with query */
  virtual void QueryDone(int bytes, void *userData);
  
  VisualFilter _queryFilter;
  QueryProc *_queryProc;
  TDataMap *_map;
  TDataCMap *_cMap;
  GDataBin *_dataBin;
  int _totalGData, _numBatches;
  Boolean _batchRecs; /* true to batch records */
  BasicStats *_stats;
};
#endif
