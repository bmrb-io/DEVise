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
  Revision 1.6  1996/04/10 02:24:31  jussi
  Added support for > 1 mappings in a view.

  Revision 1.5  1996/04/05 20:13:35  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.4  1996/01/30 21:16:14  jussi
  Removed references to specific colors.

  Revision 1.3  1995/11/25  01:20:21  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

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
	      QueryProc *qp, Color fg = ForegroundColor,
	      Color bg = BackgroundColor, AxisLabel *xAxis = NULL,
	      AxisLabel *yAxis = NULL, Action *action = NULL);

  ~ViewScatter();
  
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
  int          _timestamp;
  QueryProc    *_queryProc;
  TDataMap     *_map;
  int          _index;
  void         *_recs[WINDOWREP_BATCH_SIZE]; /* max # of pointers */
};
#endif
