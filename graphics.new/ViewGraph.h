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
  Revision 1.21  1996/07/19 02:55:00  jussi
  Added point storage for storing missing data points.

  Revision 1.20  1996/07/12 19:40:13  jussi
  View statistics are now printed into a memory buffer.

  Revision 1.19  1996/07/08 20:30:49  jussi
  Added PixmapEnabled() method.

  Revision 1.18  1996/06/27 15:46:12  jussi
  Moved key '5' functionality to ViewGraph::UpdateAutoScale().

  Revision 1.17  1996/06/20 17:10:26  guangshu
  Added support for color statistics.

  Revision 1.16  1996/06/15 13:51:28  jussi
  Added SetMappingLegend() method.

  Revision 1.15  1996/06/13 00:16:33  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.14  1996/05/31 15:41:23  jussi
  Added support for record links.

  Revision 1.13  1996/05/07 16:33:35  jussi
  Moved Action member variable from View to ViewGraph. Moved
  implementation of HandleKey, HandlePress and HandlePopup to
  ViewGraph as well. Added IsScatterPlot() method.

  Revision 1.12  1996/04/22 21:38:09  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.11  1996/04/15 15:14:05  jussi
  A mapping label is now stored as part of the mapping list data
  structure. Added GetMappingLegend() accessor method.

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
#include "DerivedDataSource.h"

const int MAXCOLOR = 43;

#include "Action.h"
#include "RecId.h"
#include "PointStorage.h"

class TDataMap;
class RecordLink;

struct MappingInfo {
  TDataMap *map;
  char *label;
};

DefinePtrDList(MappingInfoList, MappingInfo *);
DefinePtrDList(RecordLinkList, RecordLink *);

class ViewGraph: public View, public DerivedDataSource {
public:
  ViewGraph(char *name, VisualFilter &initFilter, 
	    AxisLabel *xAxis, AxisLabel *yAxis,
	    Color fg, Color bg, Action *action = 0);

  virtual ~ViewGraph();

  /* Make view a master/slave of a link */
  virtual void AddAsMasterView(RecordLink *link);
  virtual void DropAsMasterView(RecordLink *link);
  virtual void AddAsSlaveView(RecordLink *link);
  virtual void DropAsSlaveView(RecordLink *link);

  /* Insert/remove mappings from view */
  virtual void InsertMapping(TDataMap *map, char *label = "");
  virtual void RemoveMapping(TDataMap *map);
  virtual char *GetMappingLegend(TDataMap *map);
  virtual void SetMappingLegend(TDataMap *map, char *label);

  int InitMappingIterator(Boolean backwards = false) {
    return _mappings.InitIterator((backwards ? 1 : 0));
  }

  Boolean MoreMapping(int index) {
    return _mappings.More(index);
  }

  MappingInfo *NextMapping(int index) {
    return _mappings.Next(index);
  }

  void DoneMappingIterator(int index) {
    _mappings.DoneIterator(index);
  }

  /* Return true if restoring pixmaps is allowed */
  virtual Boolean PixmapEnabled() {
    return (_masterLink.Size() > 0 || _slaveLink.Size() > 0 ? false : true);
  }

  /* Draw legend */
  virtual void DrawLegend();

  /* Update automatic scaling information */
  virtual void UpdateAutoScale();
  virtual void SetAutoScale(Boolean autoScale) { _autoScale = autoScale; }
  virtual Boolean GetAutoScale() { return _autoScale; }

  /* Toggle the value of DisplayStats */
  char *GetDisplayStats() { return _DisplayStats; }
  void SetDisplayStats(char *stat);
  
  /* Get pointer to statistics object */
  BasicStats *GetStatObj() { return &_allStats; }

  /* Return pointer to buffer with view statistics */
  virtual char *GetViewStatistics() { return _statBuffer; }
  virtual char *GetViewHistogram()  { return _histBuffer; }

  /* Set/get action */
  void SetAction(Action *action) { _action = action; }
  Action *GetAction() { return _action; }
	
  /* Return true if view is a scatter plot */
  Boolean IsScatterPlot();
      
  /* Return address of point storage */
  virtual PointStorage *GetPointStorage() { return &_pstorage; }

 protected:
  /* Write color statistics to memory buffer */
  void PrepareStatsBuffer();

  /* Insert records into record links whose master this view is */
  void WriteMasterLink(RecId start, int num);

  /* List of mappings displayed in this view */
  MappingInfoList _mappings;

  /* True if statistics need to be displayed along with data */
  char _DisplayStats[STAT_NUM + 1];

  BasicStats _allStats;            /* basic stats for all categories */
  BasicStats _stats[MAXCOLOR];     /* basic stats per category */
  char _statBuffer[3072];          /* view statistics */
  char _histBuffer[3072];	   /* histograms */
  Coord yMax, yMin;		   /* the ymax and ymin for _allStats */
  
  Action *_action;                 /* action in this view */
  RecordLinkList _masterLink;      /* links whose master this view is */
  RecordLinkList _slaveLink;       /* slave record link list */

  Boolean _autoScale;              /* true if auto scaling in effect */

  PointStorage _pstorage;          /* storage for missing data points */

 private:
  Boolean ToRemoveStats(char *oldset, char *newset);
  void StatsXOR(char *oldstat, char *newstat, char *result);

  /* Handle button press event */
  virtual void HandlePress(WindowRep * w, int xlow,
			   int ylow, int xhigh, int yhigh, int button);

  /* Handle key event */
  virtual void HandleKey(WindowRep *w ,char key, int x, int y);

  /* Handle pop-up */
  virtual Boolean HandlePopUp(WindowRep *, int x, int y, int button,
			      char **&msgs, int &numMsgs);
};

#endif
