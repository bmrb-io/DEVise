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
  Revision 1.2  1995/09/05 22:16:21  jussi
  Added CVS header.
*/

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
  void *_recs[WINDOWREP_BATCH_SIZE]; /* max # of pointers */
};
#endif
