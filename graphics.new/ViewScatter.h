/*
  $Id$

  $Log$*/

#ifndef ViewScatter_h
#define ViewScatter_h
#include "QueryProc.h"
#include "ViewGraph.h"
#include "GDataBin.h"
#include "WindowRep.h"
#include "Color.h"

class ViewScatter: public ViewGraph, private QueryCallback {
public:

	ViewScatter(char *name, VisualFilter &initFilter,
		QueryProc *qp, 
		Color fg= BlackColor, Color bg = WhiteColor,
		AxisLabel *xAxis=NULL, AxisLabel *yAxis = NULL,
		Action *action=NULL);
	
	virtual void InsertMapping(TDataMap *map);
protected:
	/* from View */
	virtual void DerivedStartQuery(VisualFilter &filter, int timestamp);
	virtual void DerivedAbortQuery();

private:

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
Coord _xPerPixel, _yPerPixel;
void *_recs[WINDOWREP_BATCH_SIZE]; /* max # of pointers */
};
#endif
