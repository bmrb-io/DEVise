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
  Revision 1.10  1996/04/10 15:27:09  jussi
  Added RemoveMapping() method.

  Revision 1.9  1996/04/10 02:24:14  jussi
  Added direction parameter to InitMappingIterator(), and
  added SwapMappings() method.

  Revision 1.8  1996/04/05 20:13:32  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.7  1995/12/18 03:16:13  ravim
  Function to compute XOR of statistic strings.

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

struct MappingInfo {
  TDataMap *map;
  char *label;
};

DefinePtrDList(MappingInfoList, MappingInfo *);

class ViewGraph: public View {
public:
  ViewGraph(char *name, VisualFilter &initFilter, 
	    AxisLabel *xAxis, AxisLabel *yAxis,
	    Color fg, Color bg,
	    Action *action = NULL);

  virtual void InsertMapping(TDataMap *map, char *label = "");
  virtual void RemoveMapping(TDataMap *map);
  virtual char *GetMappingLegend(TDataMap *map);

  void InitMappingIterator(Boolean backwards = false);
  Boolean MoreMapping();
  MappingInfo *NextMapping();
  void DoneMappingIterator();

  /* Draw legend */
  virtual void DrawLegend();

  /* Toggle the value of DisplayStats */
  char *GetDisplayStats() { return _DisplayStats; }
  void SetDisplayStats(char *stat);
  BasicStats *GetStatObj() { return &_stats; }

protected:
  MappingInfoList _mappings;
  int _index;

  /* TRUE if Statistics need to be displayed along with data */
  char _DisplayStats[STAT_NUM + 1];
  BasicStats _stats;

 private:
  Boolean ToRemoveStats(char *oldset, char *newset);
  void StatsXOR(char *oldstat, char *newstat, char *result);
};

#endif
