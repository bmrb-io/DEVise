/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.12  1996/06/13 00:16:31  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.11  1996/05/31 15:41:25  jussi
  Added support for record links.

  Revision 1.10  1996/04/22 21:38:06  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.9  1996/04/10 02:24:39  jussi
  Added support for > 1 mappings in a view.

  Revision 1.8  1996/04/05 20:13:31  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.7  1996/01/30 00:02:09  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.6  1995/12/29 22:42:32  jussi
  Added support for line connectors.

  Revision 1.5  1995/12/05 17:07:58  jussi
  Statistics are now part of ViewGraph, the subclass of TDataviewX.

  Revision 1.4  1995/11/28  05:26:28  ravim
  Support for statistics.

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
#include "DList.h"

DefineDList(BStatList, BasicStats *)

class TDataViewX: public ViewGraph, private QueryCallback,
	private GDataBinCallback {
public:

  TDataViewX(char *name, VisualFilter &initFilter, QueryProc *qp, 
	     Color fg = ForegroundColor, Color bg = BackgroundColor,
	     AxisLabel *xAxis = NULL, AxisLabel *yAxis = NULL,
	     Action *action = NULL);

  ~TDataViewX();
	
  virtual void InsertMapping(TDataMap *map);

protected:
  /* from View */
  virtual void DerivedStartQuery(VisualFilter &filter, int timestamp);
  virtual void DerivedAbortQuery();
  
  /* Get record link */
  virtual RecordLinkList *GetRecordLinkList() { return &_slaveLink; }

  virtual Boolean DisplaySymbols() { return _dispSymbols; }
  virtual Boolean DisplaySymbols(Boolean state);
  virtual Boolean DisplayConnectors() { return _dispConnectors; }
  virtual Boolean DisplayConnectors(Boolean state);

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
  int          _timestamp;
  QueryProc    *_queryProc;
  TDataMap     *_map;
  GDataBin     *_dataBin;
  int          _index;
  int          _totalGData;
  int          _numBatches;
  Boolean      _batchRecs;

  Boolean      _dispSymbols;
  Boolean      _dispConnectors;
  TDataCMap    *_cMap;

  BStatList    _blist;	// Keep a list of BasicStats so we can delete them.
};

#endif
