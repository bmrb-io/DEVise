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
  Revision 1.2  1995/09/05 22:16:17  jussi
  Added CVS header.
*/

#ifndef ViewGraph_h
#define ViewGraph_h

#include "View.h"
#include "DList.h"
#include "Color.h"
#include "BasicStats.h"

class TDataMap;
DefinePtrDList(TDataMapList, TDataMap *)

class ViewGraph: public View {
public:
  ViewGraph(char *name, VisualFilter &initFilter, 
	    AxisLabel *xAxis, AxisLabel *yAxis,
	    Color fg, Color bg,
	    Action *action = NULL);
	
  virtual void InsertMapping(TDataMap *map);

  void InitMappingIterator();
  Boolean MoreMapping();
  TDataMap *NextMapping();
  void DoneMappingIterator();

  /* Toggle the value of DisplayStats */
  Boolean GetDisplayStats() {return _DisplayStats; }
  void SetDisplayStats(Boolean stat);

protected:
  TDataMapList _mappings;
  int _index;

  /* TRUE if Statistics need to be displayed along with data */
  Boolean _DisplayStats;
  BasicStats _stats;
};
#endif
