/*
  $Id$

  $Log$*/

#ifndef TDataViewX_h
#define TDataViewX_h
#include "QueryProc.h"
#include "ViewGraph.h"
#include "GDataBin.h"
#include "TDataCMap.h"
#include "Color.h"

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
Coord _xPerPixel, _yPerPixel;
int _totalGData, _numBatches;
Boolean _batchRecs; /* true to batch records */
};
#endif
