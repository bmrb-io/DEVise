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
  Revision 1.6  1995/12/14 20:51:03  jussi
  Added one char to _DisplayStats for terminating null.

  Revision 1.5  1995/12/07 02:21:22  ravim
  New function ToRemoveStats to detect if data needs to be refreshed.

  Revision 1.4  1995/12/06 05:43:55  ravim
  Added function which returns the stat object.

  Revision 1.3  1995/12/05 17:06:37  jussi
  Moved _stats from View (subclass) so that statistics can be turned
  on and displayed without having to redisplay the data itself.

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
  char *GetDisplayStats() { return _DisplayStats; }
  void SetDisplayStats(char *stat);
  BasicStats *GetStatObj() { return &_stats; }

protected:
  TDataMapList _mappings;
  int _index;

  /* TRUE if Statistics need to be displayed along with data */
  char _DisplayStats[STAT_NUM + 1];
  BasicStats _stats;

 private:
  Boolean ToRemoveStats(char *oldset, char *newset);
  void StatsXOR(char *oldstat, char *newstat, char *result);

};
#endif
