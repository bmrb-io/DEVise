/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.55  1998/11/04 20:34:05  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.54  1998/10/20 19:46:19  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.53  1998/08/24 14:51:43  wenger
  Implemented support for JavaScreen drill-down.

  Revision 1.52  1998/07/30 15:31:24  wenger
  Fixed bug 381 (problem with setting master and slave of a link to the same
  view); generally cleaned up some of the master-slave link related code.

  Revision 1.51  1998/05/06 22:05:03  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.50  1998/04/30 14:24:24  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.49  1998/04/29 17:54:00  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.48  1998/04/10 18:29:34  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.47  1998/03/05 08:10:57  zhenhai
  Added ability to view 3D graphs from six directions. Use -gl option to run,
  and click key x,y,z and X,Y,Z to select the direction you are viewing.
  Use arrow keys to pan left right up and down.

  Revision 1.46  1998/02/26 22:59:58  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.45  1998/02/20 06:17:16  beyer
  resurected histograms

  Revision 1.44  1998/02/10 21:13:17  wenger
  Changed signatures of ReturnGData() in QueryCallback and its subclasses
  to pass back lists of records drawn (not implemented yet); moved
  declaration of ViewGraph_QueryCallback from ViewGraph.h to ViewGraph.c.

  Revision 1.43  1998/01/14 16:39:24  wenger
  Merged cleanup_1_4_7_br_6 thru cleanup_1_4_7_br_7.

  Revision 1.42  1997/12/12 05:50:46  weaver
  *** empty log message ***

  Revision 1.41  1997/11/24 23:15:24  weaver
  Changes for the new ColorManager.

  Revision 1.40  1997/11/24 16:22:30  wenger
  Added GUI for saving GData; turning on GData to socket now forces
  redraw of view; GData to socket params now saved in session files;
  improvement to waitForQueries command.

  Revision 1.39  1997/11/18 23:27:06  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.38.4.1  1998/01/12 20:34:10  wenger
  Fixed duplicate frees in multi that caused core dump on Linux.

  Revision 1.38  1997/08/28 18:21:14  wenger
  Moved duplicate code from ViewScatter, TDataViewX, and ViewLens classes
  up into ViewGraph (parent class).

  Revision 1.37  1997/06/05 21:08:42  wenger
  User-configurability of '4', '5', and '6' keys is now completed.

  Revision 1.36  1997/05/30 15:41:21  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.35.6.1  1997/05/21 20:40:52  weaver
  Changes for new ColorManager

  Revision 1.35  1997/04/21 23:01:56  guangshu
  Small changes.

  Revision 1.34  1997/04/03 16:36:48  wenger
  Reduced memory and CPU usage in statistics; fixed a memory leak in the
  statistics code; switched devised back to listening on port 6100
  (changed accidentally?); turned off a bunch of debug output.

  Revision 1.33  1997/03/20 22:25:37  guangshu
  Enhanced statistics in hitogram, group by.

  Revision 1.32  1997/02/14 16:47:48  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.31  1997/02/03 19:45:38  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.30.4.1  1997/02/11 23:24:22  guangshu
  Define BStatList upper one level in order to do all the stats in both
  scattered and sorted view.

  Revision 1.30  1996/11/26 16:51:42  ssl
  Added support for piled viws

  Revision 1.29  1996/11/13 16:57:14  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.28  1996/09/27 21:09:39  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.27  1996/08/04 21:59:57  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.26  1996/07/26 16:19:19  guangshu
  Remove unnecessary comment.

  Revision 1.25  1996/07/25 14:33:00  guangshu
  Added linked list to keep track of the gstat records so it doesnot need to scann the range from xmin to xmax and fixed bugs for histograms

  Revision 1.24  1996/07/23 17:26:08  jussi
  Added support for piled views.

  Revision 1.23  1996/07/22 23:44:42  guangshu
  Added statistics for gdata. The statistics includes count, ysum, max, mean, min.

  Revision 1.22  1996/07/19 18:00:32  guangshu
  Added support for histograms.

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

//******************************************************************************
//
//******************************************************************************

#ifndef __VIEWGRAPH_H
#define __VIEWGRAPH_H

//******************************************************************************
// Libraries
//******************************************************************************

//#include "HashTable.h"
#include "View.h"
#include "DList.h"
#include "BasicStats.h"
#include "GdataStat.h"
#include "DataSourceFixedBuf.h"
#include "UpdateLink.h"
#include "AssoArray.h"
#include "QueryProc.h"
#include "GDataSock.h"
#include "Util.h"

#include "Color.h"

// Trouble, as the palette is now variable size. I've temporarily added a
// constant gMaxNumColors to Color.h to replace it (and replaced it
// everywhere it appears (this file and TDataViewX.c)). CEW 5/1/97
// const int MAXCOLOR = 43;

/* Set this to 1 to implement "big" statistics (this was needed for the
 * March '97 demo in Washington, but at least right now should generally
 * be turned off). RKW 4/3/97. */
#define BIG_STATS 0

#if BIG_STATS
  const int MAX_GSTAT = 10000;
  const int DERIVED_BUF_SIZE = 800000;
#else
  const int MAX_GSTAT = 199;
  const int DERIVED_BUF_SIZE = 9600;
#endif

const int HISTOGRAM_REC_SIZE = 24;
const int MAX_HISTOGRAM_BUCKETS = 100;
const int HIST_BUF_SIZE = MAX_HISTOGRAM_BUCKETS * HISTOGRAM_REC_SIZE;
const int STAT2D_BUF_SIZE = 131072;

#include "Action.h"
#include "RecId.h"
#include "PointStorage.h"
#include "TDataMap.h"

DefineDList(GStatList, double)
DefineDList(BStatList, BasicStats *)

class MasterSlaveLink;
class CountMapping;
class DerivedTable;
class SlaveTable;

struct MappingInfo {
  TDataMap *map;
  char *label;
};

enum PanDirection {
  PanDirInvalid = 0,
  PanDirLeft,
  PanDirRight,
  PanDirUp,
  PanDirDown
};

enum ViewHomeMode {
  HomeInvalid = 0,
  HomeAuto,
  HomeManual
};

struct ViewHomeInfo {
  ViewHomeMode mode;
  Coord autoXMargin, autoYMargin;
  Coord manXLo, manYLo;	// data units
  Coord manXHi, manYHi;	// data units
};

enum ViewPanMode {
  PanModeInvalid = 0,
  PanModeRelative,
  PanModeAbsolute
};

struct ViewPanInfo {
  ViewPanMode mode;
  Coord relPan;	// fraction of view width
  Coord absPan;	// data units
};

DefinePtrDList(MappingInfoList, MappingInfo *);
DefinePtrDList(MSLinkList, MasterSlaveLink *);

class GDataBin;

//******************************************************************************
// class ViewGraph
//******************************************************************************

class ViewGraph : public View
{
		friend class ViewGraph_QueryCallback;
		friend class SlaveTable;
		friend class JavaScreenCmd; // for HandlePopUp

	private:

		// Callback Adapters
		ViewGraph_QueryCallback*	queryCallback;

	public:

		// Constructors and Destructors
		ViewGraph(char* name, VisualFilter& initFilter, QueryProc* qp,
				  AxisLabel* xAxis, AxisLabel* yAxis,
				  PColorID fgid, PColorID bgid, Action* action = 0);
		virtual ~ViewGraph(void);

		// Utility Function (Color)
		double	CalcDataColorEntropy(void);

protected:
  friend class MasterSlaveLink;

  // Note: view and link data structures may not be consistent if these
  // methods are called by anything other than the MasterSlaveLink class.
  // RKW July 30, 1998.

  /* Make view a master/slave of a link */
  virtual void AddAsMasterView(MasterSlaveLink *link);
  virtual void DropAsMasterView(MasterSlaveLink *link);
  virtual void AddAsSlaveView(MasterSlaveLink *link);
  virtual void DropAsSlaveView(MasterSlaveLink *link);

  /* Unlink all MasterSlaveLinks connected to this view. */
  virtual void UnlinkMasterSlave();

public:
  // Stats update link access
  UpdateLink& GetUpdateLink() { return _updateLink; }
  // Record link list
  MSLinkList *SlaveLinkList() { return &_slaveLink;}

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

  //get the first mapping for this view in order to get the 
  //tdata name in TDataAsciiInterp.c
  TDataMap *GetFirstMap() {
    int index = InitMappingIterator(false);
    if (MoreMapping(index)) {
      TDataMap *firstMap = NextMapping(index)->map;
      DoneMappingIterator(index);
      return firstMap;
    } else {
      printf("No Mapping available\n");
      DoneMappingIterator(index);
      return NULL;
      }
  }

  /* Return true if restoring pixmaps is allowed */
  virtual Boolean PixmapEnabled() {
    return (_masterLink.Size() > 0 || _slaveLink.Size() > 0 ? false : true);
  }

  /* Draw legend */
  virtual void DrawLegend();

  /* Update automatic scaling information */
  virtual void SetAutoScale(Boolean autoScale) { _autoScale = autoScale; }
  virtual Boolean GetAutoScale() { return _autoScale; }
  virtual Boolean IsGStatInMem() { return _gstatInMem; }
  virtual void ResetGStatInMem() { _gstatInMem = true; }
  virtual Boolean IsXDateType();
  virtual Boolean IsYDateType();

  /* Update visual filter in various ways. */
  virtual void GoHome();
  virtual void PanLeftOrRight(PanDirection direction);
  virtual void PanUpOrDown(PanDirection direction);

  /* Get and set the home and panning info. */
  virtual void GetHomeInfo(ViewHomeInfo &info) { info = _homeInfo; }
  virtual void SetHomeInfo(const ViewHomeInfo &info) { _homeInfo = info; }
  virtual void GetHorPanInfo(ViewPanInfo &info) { info = _horPanInfo; }
  virtual void SetHorPanInfo(const ViewPanInfo &info) { _horPanInfo = info; }

  /* Toggle the value of DisplayStats */
  char *GetDisplayStats() { return _DisplayStats; }
  void SetDisplayStats(char *stat);
  
  /* Get pointer to statistics object */
  BasicStats *GetStatObj() { return &_allStats; }
  void SetHistogram(Coord min, Coord max, int buckets);
  Coord GetHistogramMin() { return _allStats.GetHistMin(); }     
  Coord GetHistogramMax() { return _allStats.GetHistMax(); }     
  int GetHistogramBuckets() { return _allStats.GetnumBuckets(); }     

  /* Return pointer to buffer with view statistics */
  DataSourceBuf* GetViewStatistics() { return _statBuffer; }
  DataSourceBuf* GetViewHistogram()  { return _histBuffer; }
  DataSourceBuf* GetGdataStatisticsX() { return _gdataStatBufferX; }
  DataSourceBuf* GetGdataStatisticsY() { return _gdataStatBufferY; }

  // uses filter''s y range to set the histogram upper & lower bounds
  void SetHistogramWidthToFilter();

  /* Set/get action */
  void SetAction(Action *action);
  Action *GetAction() { return _action; }
	
  /* Return true if view is a scatter plot */
  Boolean IsScatterPlot();
      
  /* Return address of point storage */
  virtual PointStorage *GetPointStorage() { return &_pstorage; }

  // the view master has recomputed its view, this view is called if
  // it depends upon master
  void MasterRecomputed(ViewGraph* master);

  /* Write color statistics to memory buffer */
  void PrepareStatsBuffer(TDataMap *map);
  void setHistViewname(char *name) { histViewName = name; }
  char *getHistViewname() { return histViewName; }

  Boolean GetDrawToScreen() { return _drawToScreen; }
  Boolean GetSendToSocket() { return _sendToSocket; }
  void GetSendParams(GDataSock::Params &params) { params = _gdsParams; }

  void SetDrawToScreen(Boolean drawToScreen);
  void SetSendToSocket(Boolean sendToSocket);
  void SetSendParams(const GDataSock::Params &params);

  DevStatus Send(void **gdataArray, TDataMap *map, int recCount) {
    if (_gds != NULL) {
      return _gds->Send(this, gdataArray, map, recCount);
    } else {
      reportErrNosys("No GDataSock object");
      return StatusFailed;
    }
  }

  virtual char *CreateDerivedTable(char *namePrefix, char *masterAttrName) {
    return NULL; }
  virtual void DestroyDerivedTable(char *tableName) {}
  virtual DerivedTable *GetDerivedTable(char *tableName) { return NULL; }

  virtual void TAttrLinkChanged();

  void GetCountMapping(Boolean &enabled, char *&countAttr, char *&putAttr);
  DevStatus SetCountMapping(Boolean enabled, char *countAttr, char *putAttr);

  void SetStringTable(TDataMap::TableType type, char *name);
  char *GetStringTable(TDataMap::TableType type) {
    return *TableType2NameP(type);
  }

 protected:
  virtual void ReturnGDataBinRecs(TDataMap *map, void **recs, int numRecs){};

  /* Insert records into record links whose master this view is */
  void WriteMasterLink(RecId start, int num);

  virtual void UpdatePhysTData();

  /* List of mappings displayed in this view */
  MappingInfoList _mappings;

  virtual void DerivedStartQuery(VisualFilter &filter,
                                 int timestamp);
  virtual void DerivedAbortQuery();

  char **TableType2NameP(TDataMap::TableType type);

  /* True if statistics need to be displayed along with data */
  char _DisplayStats[STAT_NUM + 1];
  char *histViewName;

  BasicStats _allStats;            	/* basic stats for all categories */
  BasicStats _stats[gMaxNumColors];     /* basic stats per category */

  DataSourceFixedBuf* _statBuffer;         /* view statistics */
  DataSourceFixedBuf* _histBuffer;	   /* histograms */
  DataSourceFixedBuf* _gdataStatBufferX;   /* the statistics for each x */
  DataSourceFixedBuf* _gdataStatBufferY;   /* the statistics for each x */

  Boolean _gstatInMem;			/* true if gdata is in memory */

  GdataStat _gstatX;
  GdataStat _gstatY;
  GStatList _glistX;                /* List to keep track of all X values */ 
  GStatList _glistY;                /* List to keep track of all Y values */ 

  Action *_action;                 /* action in this view */
  Boolean _deleteAction;           /* delete _action when this is destroyed? */
  MSLinkList _masterLink;      /* links whose master this view is */
  MSLinkList _slaveLink;       /* slave record link list */
  int _slaveTAttrTimestamp;	// changed when linked or unlinked from
				// TAttrLink, or master views TData changed
  SlaveTable *_slaveTable;

  UpdateLink _updateLink;	// link to view that want to know when
				// this view has changed

  Boolean _autoScale;              /* true if auto scaling in effect */

  PointStorage _pstorage;          /* storage for missing data points */

  //TEMP -- why do we have _queryFilter here, when View already has one?
  VisualFilter _queryFilter;
  int          _timestamp;
  QueryProc    *_queryProc;
  TDataMap     *_map;
  GDataBin     *_dataBin;
  int          _index;

  BStatList    _blist;  // Keep a list of BasicStats so we can delete them.

  CountMapping *_countMapping;

 private:
  Boolean ToRemoveStats(char *oldset, char *newset);
  void StatsXOR(char *oldstat, char *newstat, char *result);

  ViewHomeInfo _homeInfo;
  ViewPanInfo _horPanInfo;
  GDataSock *_gds;
  Boolean _drawToScreen;
  Boolean _sendToSocket;
  GDataSock::Params _gdsParams;
  char *_stringXTableName;
  char *_stringYTableName;
  char *_stringZTableName;
  char *_stringGenTableName;

	protected:

		// Callback methods (QueryCallback)
		virtual void	QueryInit(void* userData) {}
		virtual void	QueryDone(int bytes, void* userData,
								  TDataMap* map = NULL);
		virtual void*	GetObj(void) { return this; }
		virtual MSLinkList*		GetMasterLinkList()
		{ DOASSERT(false, "Call in derived class only"); return NULL; }
		virtual MSLinkList*		GetRecordLinkList()
		{ DOASSERT(false, "Call in derived class only"); return NULL; }
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList,
									int& recordsDrawn,
									BooleanArray*& drawnList)
		{ DOASSERT(false, "Call in derived class only"); }
		virtual void	PrintLinkInfo(void);
		virtual Boolean HasDerivedTable()
		{ DOASSERT(false, "Call in derived class only"); return false; }
		virtual void InsertValues(TData *tdata, int recCount, void **tdataRecs)
		{ DOASSERT(false, "Call in derived class only"); }

		// Callback methods (WindowRepCallback)
		virtual void	HandlePress(WindowRep* w, int xlow, int ylow,
									int xhigh, int yhigh, int button);
		virtual void	HandleKey(WindowRep* w ,int key, int x, int y);
		virtual Boolean HandlePopUp(WindowRep*, int x, int y, int button,
									char**& msgs, int& numMsgs);
};

//******************************************************************************
#endif
