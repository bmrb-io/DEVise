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
  Revision 1.65  1998/01/14 16:39:23  wenger
  Merged cleanup_1_4_7_br_6 thru cleanup_1_4_7_br_7.

  Revision 1.64  1997/12/12 05:50:45  weaver
  *** empty log message ***

  Revision 1.63  1997/11/24 23:15:23  weaver
  Changes for the new ColorManager.

  Revision 1.62  1997/11/24 16:22:29  wenger
  Added GUI for saving GData; turning on GData to socket now forces
  redraw of view; GData to socket params now saved in session files;
  improvement to waitForQueries command.

  Revision 1.61  1997/11/18 23:27:05  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.60.4.1  1998/01/12 20:34:01  wenger
  Fixed duplicate frees in multi that caused core dump on Linux.

  Revision 1.60  1997/08/28 18:21:13  wenger
  Moved duplicate code from ViewScatter, TDataViewX, and ViewLens classes
  up into ViewGraph (parent class).

  Revision 1.59  1997/08/20 22:11:14  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.58.2.1  1997/08/15 23:06:33  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.58  1997/06/18 21:06:30  wenger
  Fixed problems saving to batch scripts.

  Revision 1.57  1997/06/10 19:22:08  wenger
  Removed (some) debug output.

  Revision 1.56  1997/06/05 21:08:41  wenger
  User-configurability of '4', '5', and '6' keys is now completed.

  Revision 1.55  1997/05/30 15:41:21  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.54.6.1  1997/05/21 20:40:51  weaver
  Changes for new ColorManager

  Revision 1.54  1997/04/30 21:45:40  wenger
  Fixed non-constant strings in complex mappings bug; TDataAsciiInterp
  no longer gives warning message on blank data lines; added makefile
  targets to make a Purify'd version of multi; fixed uninitialized memory
  read in the DList code; fixed bug that caused 1.4 version of multi to
  always crash; better error messages in DTE command parser; version is
  now 1.4.4.

  Revision 1.53  1997/04/21 23:01:29  guangshu
  Considered various cases of statistics, esp date type.

  Revision 1.52  1997/04/11 18:49:16  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.51  1997/03/28 16:10:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.50  1997/03/21 23:57:41  guangshu
  Write null records to statBuf when there is no record in the view to
  make the dataSourceBuf valid.

  Revision 1.49  1997/03/20 22:24:55  guangshu
  Enhanced statistics to support user specified number of buckets in histograms,
  group by X and Y, support for date type in group by.

  Revision 1.48  1997/03/19 19:41:52  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.47  1997/02/26 16:31:49  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.46  1997/02/03 19:45:37  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.45.4.1  1997/02/13 18:11:49  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.45  1997/01/24 22:18:40  wenger
  Fixed bug 136.

  Revision 1.44  1996/12/30 23:57:37  andyt
  First version with support for Embedded Tcl/Tk windows. Added new
  ETkWindow symbol shape. Improved the MappingInterp::MapGAttr2TAttr
  function to handle all GData attributes (used to only handle a subset).

  Revision 1.43  1996/11/26 16:51:41  ssl
  Added support for piled viws

  Revision 1.42  1996/11/20 16:51:09  jussi
  Replaced AbortAndReexecute() with AbortQuery() and Refresh().

  Revision 1.41  1996/11/13 16:57:13  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.40  1996/09/27 21:09:38  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.39  1996/08/29 19:07:29  ssl
  Same bug fix for reclinks slightly modified.

  Revision 1.37  1996/08/07 19:27:11  jussi
  Moved legends in merged view up a bit.

  Revision 1.36  1996/08/05 19:49:02  wenger
  Fixed compile errors caused by some of Kevin's recent changes; changed
  the attrproj stuff to make a .a file instead of a .o; added some more
  TData file writing stuff; misc. cleanup.

  Revision 1.35  1996/08/05 18:41:48  beyer
  - Color stats only print an entry for each color that is in it source view.
  (I.e., if the count for a color is zero, it doesn't have a record in the
  color stats.)
  - Improved safety of stats code.

  Revision 1.34  1996/08/04 21:59:57  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.33  1996/08/03 15:20:17  jussi
  Made "out of statistics buffer space" messages appear only when
  DEBUG is defined.

  Revision 1.32  1996/07/30 18:24:56  guangshu
  Commit the change as last one again.

  Revision 1.30  1996/07/25 14:32:55  guangshu
  Added linked list to keep track of the gstat records so it doesnot need to scann the range from xmin to xmax and fixed bugs for histograms

  Revision 1.29  1996/07/23 19:34:50  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.28  1996/07/23 17:26:07  jussi
  Added support for piled views.

  Revision 1.27  1996/07/22 23:44:31  guangshu
  Added statistics for gdata. The statistics includes count, ysum, max,
  mean, min.

  Revision 1.26  1996/07/20 17:07:57  guangshu
  Small fixes when wirte to HistBuffer.

  Revision 1.25  1996/07/19 18:00:31  guangshu
  Added support for histograms.

  Revision 1.24  1996/07/13 00:22:21  jussi
  ViewGraph writes only the minimum number of necessary records
  in the color statistics buffer.

  Revision 1.23  1996/07/12 19:40:12  jussi
  View statistics are now printed into a memory buffer.

  Revision 1.22  1996/06/27 15:46:12  jussi
  Moved key '5' functionality to ViewGraph::UpdateAutoScale().

  Revision 1.21  1996/06/24 19:44:30  jussi
  Added a win->Flush() to force statistics on the screen.

  Revision 1.20  1996/06/20 17:10:25  guangshu
  Added support for color statistics.

  Revision 1.19  1996/06/15 13:51:27  jussi
  Added SetMappingLegend() method.

  Revision 1.18  1996/06/13 00:16:32  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.17  1996/05/31 15:41:22  jussi
  Added support for record links.

  Revision 1.16  1996/05/07 16:33:23  jussi
  Moved Action member variable from View to ViewGraph. Moved
  implementation of HandleKey, HandlePress and HandlePopup to
  ViewGraph as well. Added IsScatterPlot() method.

  Revision 1.15  1996/04/22 21:38:09  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.14  1996/04/20 19:56:59  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.13  1996/04/16 20:39:54  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.12  1996/04/15 15:13:52  jussi
  A mapping label is now stored as part of the mapping list data
  structure. Added GetMappingLegend() accessor method.

  Revision 1.11  1996/04/10 15:27:22  jussi
  Added RemoveMapping() method.

  Revision 1.10  1996/04/10 02:23:34  jussi
  Added direction parameter to InitMappingIterator(), and added
  SwapMappings() method.

  Revision 1.9  1995/12/28 20:46:38  jussi
  Minor clean up.

  Revision 1.8  1995/12/18 03:15:15  ravim
  Data is never refreshed if the only changes are due to draw/undrawing the
  statistics. XOR logic used to accomplish this optimization.

  Revision 1.7  1995/12/14 22:03:49  jussi
  Added a couple of more checks in _DisplayStats handling.

  Revision 1.6  1995/12/14 20:18:52  jussi
  Fixed initialization of _DisplayStat and added more checking to
  SetViewStatistics (string passed to function can be shorter than
  _DisplayStats).

  Revision 1.5  1995/12/14 15:26:23  jussi
  Added a "return false" statement to ToRemoveStat.

  Revision 1.4  1995/12/07 02:20:51  ravim
  The set of stats to be displayed is given as a binary string. The
  data is refreshed only if necessary.

  Revision 1.3  1995/12/05 17:04:16  jussi
  Moved _stats from View (subclass) so that statistics can be turned
  on and displayed without having to redisplay the data itself.

  Revision 1.2  1995/09/05 22:16:17  jussi
  Added CVS header.
*/

//******************************************************************************

//#define DEBUG

#include <assert.h>

#include "ViewGraph.h"
#include "TDataMap.h"
#include "ActionDefault.h"
#include "Init.h"
#include "RecordLink.h"
#include "TData.h"
#include "Util.h"
#include "AssoArray.h"

#include "MappingInterp.h"

#define STEP_SIZE 20

ImplementDList(GStatList, double)

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewGraph::ViewGraph(char* name, VisualFilter& initFilter, QueryProc* qp,
					 AxisLabel* xAxis, AxisLabel* yAxis,
					 PColorID fgid, PColorID bgid, Action* action)
	: View(name, initFilter, fgid, bgid, xAxis, yAxis),
	  _updateLink("stats link")
{
    DO_DEBUG(printf("ViewGraph::ViewGraph(0x%p, %s)\n",
		    this, (name != NULL) ? name : "<null>"));

	queryCallback = new ViewGraph_QueryCallback(this);

    _action = action;
    _deleteAction = false;
    if (!_action)
    {
      _action = new ActionDefault("default");
      _deleteAction = true;
    }

    memset(_DisplayStats, '0', STAT_NUM);

    // add terminating null
    _DisplayStats[STAT_NUM] = 0;
    
    // auto scaling is in effect by default
    _autoScale = true;

    _updateLink.SetMasterView(this);

    _statBuffer = new DataSourceFixedBuf(DERIVED_BUF_SIZE, "statBuffer");
    _statBuffer->AddRef();
    _statBuffer->SetControllingView(this);

    _histBuffer = new DataSourceFixedBuf(HIST_BUF_SIZE, "histBuffer");
    _histBuffer->AddRef();
    _histBuffer->SetControllingView(this);

    _gdataStatBufferX = new DataSourceFixedBuf(DERIVED_BUF_SIZE, "gdataStatBufferX");
    _gdataStatBufferY = new DataSourceFixedBuf(DERIVED_BUF_SIZE, "gdataStatBufferY");
    _gdataStatBufferX->AddRef();
    _gdataStatBufferY->AddRef();
    _gdataStatBufferX->SetControllingView(this);
    _gdataStatBufferY->SetControllingView(this);

    _gstatInMem = true;
    histViewName = NULL;

    _homeInfo.mode = HomeAuto;
    _homeInfo.autoXMargin = 0.0;
    _homeInfo.autoYMargin = 0.0;
    _homeInfo.manXLo = 0.0;
    _homeInfo.manYLo = 0.0;
    _homeInfo.manXHi = 100.0;
    _homeInfo.manYHi = 100.0;

    _horPanInfo.mode = PanModeRelative;
    _horPanInfo.relPan = 0.5;
    _horPanInfo.absPan = 1.0;

  _queryProc = qp;
  _map = 0;
  _index = -1;
  _queryFilter = initFilter;

  _gds = NULL;
  _drawToScreen = true;
  _sendToSocket = false;
  _gdsParams.portNum = 0;
  _gdsParams.file = NULL;
  _gdsParams.sendText = true;
  _gdsParams.separator = ' ';
}

ViewGraph::~ViewGraph(void)
{
    DO_DEBUG(printf("ViewGraph::~ViewGraph(0x%p, %s)\n",
		    this, (GetName() != NULL) ? GetName() : "<null>"));

    // note: viewgraphs report their destruction twice, once during
    // ~ViewGraph() and once during ~View()
    ReportViewDestroyed();

    // disconnect from the stats buffers
    _statBuffer->RemoveControllingView(this);
    if( _statBuffer->DeleteRef() ) {
	delete _statBuffer;
    }
    _histBuffer->RemoveControllingView(this);
    if( _histBuffer->DeleteRef() ) {
	delete _histBuffer;
    }
    _gdataStatBufferX->RemoveControllingView(this);
    _gdataStatBufferY->RemoveControllingView(this);
    if( _gdataStatBufferX->DeleteRef() ) {
	delete _gdataStatBufferX;
    }
    if( _gdataStatBufferY->DeleteRef() ) {
	delete _gdataStatBufferY;
    }

    if (_deleteAction) delete _action;

    // SubClassUnmapped aborts any current query; this _must_ be done
    // before this destructor exits, or members needed to do the abort
    // will no longer be defined.
    SubClassUnmapped();

    int index = _blist.InitIterator();
    while (_blist.More(index)) {
      delete _blist.Next(index);
    }
    _blist.DoneIterator(index);
    _blist.DeleteAll();
    _gstatX.Clear();
    _gstatY.Clear();
    _glistX.DeleteAll();
    _glistY.DeleteAll();

    delete _gdsParams.file;
    delete _gds;
	delete queryCallback;
}

//******************************************************************************
// Utility Functions (Color)
//******************************************************************************

double	ViewGraph::CalcDataColorEntropy(void)
{
	Coloring	coloring;
	IntVector	count;

	for(int i=0; i<gMaxNumColors; i++)
	{
		int		n = (int)(_stats[i].GetStatVal(STAT_COUNT));

		if (n > 0)
		{
			coloring.AddDataColor(i);
			count.push_back(n);
		}
	}

	return coloring.Entropy(count);
}

//******************************************************************************

void ViewGraph::AddAsMasterView(RecordLink *link)
{
    // remove this view from the slave view list of the link; then add
    // the link as one of the links whose master this view is
    
    if (link->VisualLink::DeleteView(this))
      DropAsSlaveView(link);
    if (!_masterLink.Find(link)) {
#ifdef DEBUG
        printf("View %s becomes master of record link %s\n", GetName(),
               link->GetName());
#endif
        _masterLink.Append(link);
      }
    Refresh();
}

void ViewGraph::DropAsMasterView(RecordLink *link)
{
    if (_masterLink.Find(link)) {
        _masterLink.Delete(link);
#ifdef DEBUG
        printf("View %s no longer master of record link %s\n", GetName(),
               link->GetName());
#endif
    }
}


void ViewGraph::AddAsSlaveView(RecordLink *link)
{
    // view cannot be a master
    DropAsMasterView(link);

    if (!_slaveLink.Find(link)) {
#ifdef DEBUG
        printf("View %s becomes slave of record link %s\n", GetName(),
               link->GetName());
#endif
        _slaveLink.Append(link);
    }
    Refresh();
}

void ViewGraph::DropAsSlaveView(RecordLink *link)
{
    if (_slaveLink.Find(link)) {
        _slaveLink.Delete(link);
#ifdef DEBUG
        printf("View %s no longer slave of record link %s\n", GetName(),
               link->GetName());
#endif
    }

    Refresh();
}

void ViewGraph::InsertMapping(TDataMap *map, char *label)
{
    MappingInfo *info = new MappingInfo;
    DOASSERT(info, "Could not create mapping information");
    DOASSERT(map, "null map");

    info->map = map;
    info->label = CopyString(label);

    int index = InitMappingIterator();
    if (!MoreMapping(index)) {
      // this is the first mapping
      // update the histogram bucket sizes
//      AttrInfo *yAttr = map->MapGAttr2TAttr(MappingCmd_Y);
//      if( yAttr && yAttr->hasLoVal && yAttr->hasHiVal ) {
	// y min & max known for the file, so use those to define buckets
//	double lo = AttrList::GetVal(&yAttr->loVal, yAttr->type);
//	double hi = AttrList::GetVal(&yAttr->hiVal, yAttr->type);
//	_allStats.SetHistWidth(lo, hi);
//       } else {
	// global min & max are not known, so use filter hi & lo
        VisualFilter filter;
        GetVisualFilter(filter);
	double yMax = _allStats.GetStatVal(STAT_MAX);
	double yMin = _allStats.GetStatVal(STAT_MIN);
	double hi = (yMax > filter.yHigh) ? yMax:filter.yHigh; 
	double lo = (yMin > filter.yLow) ? yMin:filter.yLow;
	_allStats.SetHistWidth(lo, hi);
#if defined(DEBUG) || 0
	printf("ViewGraph::yMax=%g,yMin=%g,filter.yHigh=%g,filter.yLow=%g,width=%g\n", 
		yMax, yMin, filter.yHigh, filter.yLow, _allStats.GetHistWidth());
#endif
//	_allStats.SetHistWidth(filter.yLow, filter.yHigh);
//      }
    }
    DoneMappingIterator(index);

    // determine if this view is dependent upon any other
    TData* tdata = map->GetTData();
    DOASSERT(tdata, "null tdata");
    DataSource* ds = tdata->GetDataSource();
    DOASSERT(ds, "null data source");
    ViewGraph* v = ds->ControllingView();
    if( v ) {
#ifdef DEBUG
        printf("View %s is slave of update link for %s\n", GetName(),
               v->GetName());
#endif
	v->GetUpdateLink().InsertView(this);
    }

    _mappings.Append(info);

    AttrList *attrList = map->GDataAttrList();
    if (attrList) {
        AttrInfo *info = attrList->Find("x");
        if (info && info->type == DateAttr)
          SetXAxisAttrType(DateAttr);
        info = attrList->Find("y");
        if (info && info->type == DateAttr)
          SetYAxisAttrType(DateAttr);
    }

//    if(IsXDateType()) {
//    	PrepareStatsBuffer(GetFirstMap());
#if defined(DEBUG)
//	printf("Need to re-prepare stat buffer in Insertmapping()\n");
#endif
//    	PrepareStatsBuffer(map);
//    }
//    GoHome();

    Refresh();
}

void ViewGraph::RemoveMapping(TDataMap *map)
{
    int index = InitMappingIterator();

    while(MoreMapping(index )) {
        MappingInfo *info = NextMapping(index);
        if (info->map == map) {
            DoneMappingIterator(index);
            _mappings.Delete(info);
            delete info->label;
            delete info;
            return;
        }
    }

    DoneMappingIterator(index);
}

char *ViewGraph::GetMappingLegend(TDataMap *map)
{
    int index = InitMappingIterator();

    while(MoreMapping(index)) {
        MappingInfo *info = NextMapping(index);
        if (info->map == map) {
            DoneMappingIterator(index);
            return info->label;
        }
    }

    DoneMappingIterator(index);

    return "";
}

void ViewGraph::SetMappingLegend(TDataMap *map, char *label)
{
    int index = InitMappingIterator();

    while(MoreMapping(index)) {
        MappingInfo *info = NextMapping(index);
        if (info->map == map) {
            delete info->label;
            info->label = CopyString(label);
            break;
        }
    }

    DoneMappingIterator(index);
}

void ViewGraph::DrawLegend()
{
    WindowRep*	win = GetWindowRep();
	
    win->PushTop();
    win->MakeIdentity();
    
    int x, y;
    int w, h;
    GetDataArea(x, y, w, h);

    y += (int)(0.05 * h);
    int yInc = 12;

    int index = InitMappingIterator();

    while(MoreMapping(index)) {
        MappingInfo *info = NextMapping(index);
        char *label = info->label;
        if (!strlen(label))
          continue;

		TDataMap*	map = info->map;
		PColorID	fgid = GetPColorID(viewGraphLegendColor);
		
		if (map->GetGDataOffset()->colorOffset < 0)
			fgid = map->GetColoring().GetForeground();

		win->SetForeground(fgid);
		win->AbsoluteText(label, x, y, w - 4, yInc, WindowRep::AlignEast, true);
		y += yInc;
	}

    DoneMappingIterator(index);

    win->PopTransform();
}

void ViewGraph::GoHome()
{
#if defined(DEBUG)
    printf("ViewGraph::GoHome()\n");
#endif

    /* show all data records in view i.e. set filter to use the
       actual min/max X values and the actual min/max Y values;
       for 3D graphs, move camera to (0,0,Z) where Z is twice
       the min Z value */

    int index = InitMappingIterator();
    if (!MoreMapping(index)) {
        DoneMappingIterator(index);
        return;
    }

    TDataMap *map = NextMapping(index)->map;
    AttrInfo *xAttr = map->MapGAttr2TAttr(MappingCmd_X);
    AttrInfo *yAttr = map->MapGAttr2TAttr(MappingCmd_Y);
    AttrInfo *zAttr = map->MapGAttr2TAttr(MappingCmd_Z);

    if (GetNumDimensions() == 2) {
        VisualFilter filter;
        GetVisualFilter(filter);

	switch (_homeInfo.mode) {
	case HomeAuto: {
          if (xAttr) {
              if (xAttr->hasLoVal)
                filter.xLow = AttrList::GetVal(&xAttr->loVal, xAttr->type) -
		  _homeInfo.autoXMargin;
              if (xAttr->hasHiVal)
                filter.xHigh = AttrList::GetVal(&xAttr->hiVal, xAttr->type) +
		  _homeInfo.autoXMargin;

              if (filter.xHigh == filter.xLow) {
                  filter.xHigh += 1.0;
                  filter.xLow -= 1.0;
              }
          }
          if (yAttr) {
              if (yAttr->hasLoVal)
                filter.yLow = AttrList::GetVal(&yAttr->loVal, yAttr->type) -
		  _homeInfo.autoYMargin;
              if (yAttr->hasHiVal)
                filter.yHigh = AttrList::GetVal(&yAttr->hiVal, yAttr->type) +
		  _homeInfo.autoYMargin;

              if (filter.yHigh == filter.yLow) {
                  filter.yLow -= 1.0;
                  filter.yHigh += 1.0;
              }
          }
          if (!IsScatterPlot()) {
            filter.yLow = MIN(filter.yLow, 0.0);
	  }

	  break;
	}

	case HomeManual: {
	  filter.xLow = _homeInfo.manXLo;
	  filter.xHigh = _homeInfo.manXHi;
	  filter.yLow = _homeInfo.manYLo;
	  filter.yHigh = _homeInfo.manYHi;
	  break;
	}

	default:
	  DOASSERT(false, "Bad home mode");
	  return;
	  break;

	}

        SetVisualFilter(filter);
    } else {
        Camera c = GetCamera();
        c.fx = 0;
        c.fy = 0;
        c.fz = 0;
        double minZ = -4.0;
        if (zAttr && zAttr->hasLoVal)
          minZ = 2 * AttrList::GetVal(&zAttr->loVal, zAttr->type);
        if (minZ > -4.0)
          minZ = -4.0;
        if (!c.spherical_coord) {
            c.x_ = 0;
            c.y_ = 0;
            c.z_ = minZ;
        } else {
            c._theta = 0;
            c._phi = M_PI_2;
            c._rho = fabs(minZ);
        }
        SetCamera(c);
    }

    DoneMappingIterator(index);
}

void ViewGraph::PanLeftOrRight(PanDirection direction)
{
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::PanLeftOrRight(%d)\n", this, (int) direction);
#endif

    int panFactor;

    switch(direction) {
    case PanDirLeft:
      panFactor = -1;
      break;

    case PanDirRight:
      panFactor = 1;
      break;

    default:
      DOASSERT(false, "Illegal pan direction");
      return;
      break;
    }

    if (GetNumDimensions() == 2) {
      VisualFilter filter;
      GetVisualFilter(filter);
      Coord width = filter.xHigh - filter.xLow;

      Coord panDist;
      switch (_horPanInfo.mode) {
      case PanModeRelative:
        panDist = (filter.xHigh - filter.xLow) * _horPanInfo.relPan;
	break;

      case PanModeAbsolute:
	panDist = _horPanInfo.absPan;
	break;

      default:
	DOASSERT(false, "Bad pan mode");
	return;
	break;
      }

      filter.xLow += panFactor * panDist;
      filter.xHigh = filter.xLow + width;
      SetVisualFilter(filter);
    } else { 
      Camera camera = GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	/* when further away, it will move faster */
	incr_ = fabs(camera.x_ / STEP_SIZE);
	if (incr_ < 0.1)
	  incr_ = 0.1;
	camera.x_ += panFactor * incr_;
	if (!camera.fix_focus)
	  camera.fx += panFactor * incr_;
      } else {
	incr_ = M_PI / STEP_SIZE;
	camera._theta += panFactor * incr_;
      }
      SetCamera(camera);
    }

    return;
}

void ViewGraph::WriteMasterLink(RecId start, int num)
{
    // Insert records into record links whose master this view is
    int index = _masterLink.InitIterator();
    while(_masterLink.More(index)) {
        RecordLink *link = _masterLink.Next(index);
#if defined(DEBUG)
	printf("*********inserting recs (%ld, %ld) into %s\n", 
	       start, start + num - 1, link->GetName());
#endif
        link->InsertRecs(start, num);
    }
    _masterLink.DoneIterator(index);
}

Boolean ViewGraph::IsScatterPlot()
{
    int index = InitMappingIterator();

    if (MoreMapping(index)) {
        TDataMap *map = NextMapping(index)->map;
        int numDimensions;
        VisualFlag *dimensionInfo;
        numDimensions = map->DimensionInfo(dimensionInfo);
        if (!numDimensions) {
            /* a scatter plot has no sorted dimensions */
            DoneMappingIterator(index);
            return true;
        }
    }

    DoneMappingIterator(index);

    return false;
}

void ViewGraph::SetDisplayStats(char *stat)
{
    if (strlen(stat) > STAT_NUM) {
        fprintf(stderr, "Incorrect view statistics length: %d\n",
                (int)strlen(stat));
        return;
    }

    // Never redisplay data as long as the visual filter is the same.
    // Before drawing the lines, need to figure out which of them to draw.
    // If a line existing before needs to be deleted - draw line again.
    // If a line not there before needs to be drawn - draw it.
    // If line not there before, not to be drawn - don't draw it.
    // If line there now, needs to remain - don't draw it.
    // Basically XOR logic.
    
    StatsXOR(_DisplayStats, stat, _DisplayStats);
    _allStats.Report();

    // use memcpy to avoid copying the terminating null in stat
    memcpy(_DisplayStats, stat, strlen(stat));

    // flush new statistics to the screen
    WindowRep *win = GetWindowRep();
    if (win)
      win->Flush();
}


void ViewGraph::MasterRecomputed(ViewGraph* master)
{
#ifdef DEBUG
    printf("ViewGraph::MasterRecomputed [%s]\n", GetName());
#endif
    AbortQuery();
    Refresh();
}

Boolean ViewGraph::ToRemoveStats(char *oldset, char *newset)
{
    DOASSERT(strlen(oldset) == STAT_NUM && strlen(newset) == STAT_NUM,
             "Invalid statistics flags");

    /* Check if a 1 in "old" has become 0 in "new" - means that a stat
       previously being displayed should now be removed. */
    for (int i = 0; i < STAT_NUM; i++)
      if ((oldset[i] == '1') && (newset[i] == '0'))
        return true;
    return false;
}

void ViewGraph::StatsXOR(char *oldstat, char *newstat, char *result)
{
    for (int i = 0; i < STAT_NUM; i++)
      result[i] = ((oldstat[i] - '0')^(newstat[i] - '0')) + '0';
}

/*
AssoArray<int> month(12);

month["Jan"] = 1;
month["Feb"] = 2;
month["Mar"] = 3;
month["Apr"] = 4;
month["May"] = 5;
month["Jun"] = 6;
month["Jul"] = 7;
month["Aug"] = 8;
month["Sep"] = 9;
month["Oct"] = 10;
month["Nov"] = 11;
month["Dec"] = 12;
*/

/* The following two methods areonly temporary solutions, a good solution
   is an associative array which I am working on. */

int findMonth(const char *mon){
	if(!strcmp(mon, "Jan")) return 1;
	if(!strcmp(mon, "Feb")) return 2;
	if(!strcmp(mon, "Mar")) return 3;
	if(!strcmp(mon, "Apr")) return 4;
	if(!strcmp(mon, "May")) return 5;
	if(!strcmp(mon, "Jun")) return 6;
	if(!strcmp(mon, "Jul")) return 7;
	if(!strcmp(mon, "Aug")) return 8;
	if(!strcmp(mon, "Sep")) return 9;
	if(!strcmp(mon, "Oct")) return 10;
	if(!strcmp(mon, "Nov")) return 11;
	if(!strcmp(mon, "Dec")) return 12;
	else printf("***********No such month**********\n");
	return 0;
}

char **ExtractDate(char *string) {
	char **date = new char*[3];
	date[0] = new char[4];
	date[1] = new char[3];
	date[2] = new char[5];

	char *p = string;
	strncpy(date[0], p, 4);
	date[0][3] = '\0';
	strncpy(date[1], p+4, 3);
	if(date[1][0]==' ') { date[1]++; date[1][1]='\0'; }
	else date[1][2] = '\0';
	strncpy(date[2], p+7, 5);
	date[2][4] = '\0';

	return date;
}


void ViewGraph::PrepareStatsBuffer(TDataMap *map)
{
    /* initialize statistics buffer */
    _statBuffer->Clear();
    _histBuffer->Clear();
    _gdataStatBufferX->Clear();
    _gdataStatBufferY->Clear();

    VisualFilter filter;
    GetVisualFilter(filter);
    char *date_string;
    char **date;

    /* put the statistics in the stat buffer */
    char line[1024];
    int i;
    for(i = 0; i < gMaxNumColors; i++) {
//	if( _stats[i].GetStatVal(STAT_COUNT) > 0 ) {
	    sprintf(line, "%d %d %g %g %g %g %g %g %g %g %g %g\n",
		      i, (int)_stats[i].GetStatVal(STAT_COUNT),	
		      _stats[i].GetStatVal(STAT_YSUM),
		      _stats[i].GetStatVal(STAT_MIN),
		      _stats[i].GetStatVal(STAT_MEAN),
		      _stats[i].GetStatVal(STAT_MAX),
		      _stats[i].GetStatVal(STAT_ZVAL85L),
		      _stats[i].GetStatVal(STAT_ZVAL85H),
		      _stats[i].GetStatVal(STAT_ZVAL90L),
		      _stats[i].GetStatVal(STAT_ZVAL90H),
		      _stats[i].GetStatVal(STAT_ZVAL95L),
		      _stats[i].GetStatVal(STAT_ZVAL95H));
	    int len = strlen(line);
#if defined(DEBUG) || 0
    printf("Color stat buf line is %s\n", line);
#endif

	    DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	    if( (int) _statBuffer->Write(line, len) != len ) {
#ifdef DEBUG
		fprintf(stderr, "Out of statistics buffer space\n");
#endif
		break;
	    }
//	  }
      }
    Boolean y_is_date = false;
    if(map) {
        AttrList *gAttrList = map->GDataAttrList();
	if (gAttrList) {
          gAttrList->InitIterator();
          while(gAttrList->More()) {
                AttrInfo *info = gAttrList->Next();
                if(!strcmp(info->name, "y") && info->type==DateAttr) {
                        y_is_date = true;
                }
          }
          gAttrList->DoneIterator();
	}
    }

    double width = _allStats.GetHistWidth();
#if defined(DEBUG) || 0
    printf("histogram width: %g\n", width);
#endif
    if( width == 0 ) {
        printf("Hist width should have been set at this point\n");
/*        _allStats.SetHistWidth(filter.yLow, filter.yHigh);
#if defined(DEBUG) || 0
        printf("Hist width in ViewGraph after set is %g\n", _allStats.GetHistWidth());
#endif
*/    }
	int size = 32 * _allStats.GetnumBuckets(); // 24 is the max size for each line
	_histBuffer->Resize(size);
	double pos = _allStats.GetHistMin() + width / 2.0;
#if defined(DEBUG) || 0
	printf("Histogram min is %g, max is %g\n", _allStats.GetHistMin(), _allStats.GetHistMax());
#endif
	for(i = 0; i < _allStats.GetnumBuckets(); i++) {
	    if (y_is_date) {
		date_string = DateString(pos);
		date = ExtractDate(date_string);
		sprintf(line, "%d %s %s %d\n", findMonth(date[0]),
                 	date[1], date[2], _allStats.GetHistVal(i));
	    } else sprintf(line, "%.4e %d\n", pos, _allStats.GetHistVal(i));
	    int len = strlen(line);
	    DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	    if( (int) _histBuffer->Write(line, len) != len ) {
	        fprintf(stderr, "*****Warning: Out of histogram buffer space\n");
	        break;
	    }
#if defined(DEBUG) || 0
	    printf("Hist buf line is %s, pos is %g\n", line, pos);
#endif
	    pos += width;
	}
#if defined(DEBUG) || 0
	printf("Buf has %d lines\n", i);
#endif

    BasicStats *bs;
    Boolean x_is_date = false;
    if(map){
    	AttrList *gAttrList = map->GDataAttrList();
	if (gAttrList) {
    	  gAttrList->InitIterator();
    	  while(gAttrList->More()) {
		AttrInfo *info = gAttrList->Next();
		if(!strcmp(info->name, "x") && info->type==DateAttr) {
			x_is_date = true;
		}
    	  }
          gAttrList->DoneIterator();
	}
    }

    int getXRecs = 0;
    int len;
    int total = 0;

    int index = _glistX.InitIterator();
    while(_glistX.More(index)) {
	double i = _glistX.Next(index); 
	if (_gstatX.Lookup(i, bs)) {
	   DOASSERT(bs,"HashTable lookup error\n");
	   if (x_is_date) {
		date_string = DateString(i);
		date = ExtractDate(date_string);
		sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]), 
			     date[1], date[2], (int)bs->GetStatVal(STAT_COUNT),
			     bs->GetStatVal(STAT_YSUM),
			     bs->GetStatVal(STAT_MIN),
			     bs->GetStatVal(STAT_MEAN),
			     bs->GetStatVal(STAT_MAX));
	   } else sprintf(line, "%g %d %g %g %g %g\n",
			     i, (int)bs->GetStatVal(STAT_COUNT),
			     bs->GetStatVal(STAT_YSUM),
			     bs->GetStatVal(STAT_MIN),
			     bs->GetStatVal(STAT_MEAN),
			     bs->GetStatVal(STAT_MAX));
	   len = strlen(line);
	   total += len;
	   DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
#if defined(DEBUG) || 0
	    	printf("_gstatX buf line is %s\n", line);
#endif
           getXRecs = 1;
	   if( (int) _gdataStatBufferX->Write(line, len) != len ) {
#ifdef DEBUG
	       fprintf(stderr, "******Out of GData Stat Buffer space\n");
#endif
	       break;
	   }
       }
    }
    _glistX.DoneIterator(index);

    if(getXRecs == 0 ) {
        double low = filter.xLow;
	double high = filter.xHigh;

	date_string = DateString(low);
	date = ExtractDate(date_string);
	sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
		 date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
       	len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	if( (int) _gdataStatBufferX->Write(line, len) != len ) 
    		fprintf(stderr, "******Out of GData Stat Buffer space\n");
// 	printf("_gstatX buf line is %s\n", line);

	sprintf(line, "%g %d %g %g %g %g\n", low, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
        printf("_gstatX buf line is %s\n", line);
#endif 
	if( (int) _gdataStatBufferX->Write(line, len) != len ) 
	    fprintf(stderr, "******Out of GData Stat Buffer space\n");
//        printf("_gstatX buf line is %s\n", line);

        date_string = DateString(high);
        date = ExtractDate(date_string); 
        sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
                   date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
        if( (int) _gdataStatBufferX->Write(line, len) != len ) 
        	fprintf(stderr, "******Out of GData Stat Buffer space\n");
//           printf("_gstatX buf line is %s\n", line);

        sprintf(line, "%g %d %g %g %g %g\n", high, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
           printf("_gstatX buf line is %s\n", line);
#endif
        if( (int) _gdataStatBufferX->Write(line, len) != len ) 
            fprintf(stderr, "******Out of GData Stat Buffer space\n");
    }

    int getYRecs = 0;
    index = _glistY.InitIterator();
    while(_glistY.More(index)) {
	double i = _glistY.Next(index); 
	if (_gstatY.Lookup(i, bs)) {
	   DOASSERT(bs,"HashTable lookup error\n");
	   if (y_is_date) {
        	date_string = DateString(i);
        	date = ExtractDate(date_string);
		sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]), 
                             date[1], date[2], (int)bs->GetStatVal(STAT_COUNT),
                             bs->GetStatVal(STAT_YSUM),
                             bs->GetStatVal(STAT_MIN),
                             bs->GetStatVal(STAT_MEAN),
                             bs->GetStatVal(STAT_MAX));
	   } else sprintf(line, "%g %d %g %g %g %g\n",
			     i, (int)bs->GetStatVal(STAT_COUNT),
			     bs->GetStatVal(STAT_YSUM),
			     bs->GetStatVal(STAT_MIN),
			     bs->GetStatVal(STAT_MEAN),
			     bs->GetStatVal(STAT_MAX));
	   len = strlen(line);
	   DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
#if defined(DEBUG) || 0
	    printf("_gstatY buf line is %s\n", line);
#endif
           getYRecs = 1; 
	   if( (int) _gdataStatBufferY->Write(line, len) != len ) {
#ifdef DEBUG
	       fprintf(stderr, "Out of GData Stat Buffer space\n");
#endif
	       break;
	   }
       }
    }	
    _glistY.DoneIterator(index);

    if(getYRecs == 0) {
        double low = filter.yLow;
        double high = filter.yHigh;

	date_string = DateString(low);
        date = ExtractDate(date_string);
        sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
                 date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
        if( (int) _gdataStatBufferY->Write(line, len) != len ) 
                fprintf(stderr, "******Out of GData Stat Buffer space\n");

        sprintf(line, "%g %d %g %g %g %g\n", low, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
        printf("_gstatY buf line is %s\n", line);
#endif
        if( (int) _gdataStatBufferY->Write(line, len) != len ) 
            fprintf(stderr, "******Out of GData Stat Buffer space\n");

        date_string = DateString(high);
        date = ExtractDate(date_string); 
        sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
                   date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
        if( (int) _gdataStatBufferY->Write(line, len) != len ) 
                fprintf(stderr, "******Out of GData Stat Buffer space\n");

        sprintf(line, "%g %d %g %g %g %g\n", high, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
        printf("_gstatY buf line is %s\n", line);
#endif
        if( (int) _gdataStatBufferY->Write(line, len) != len )
            fprintf(stderr, "******Out of GData Stat Buffer space\n");
    }
}

/* Check if the X attr of GData is DateAttr */
Boolean ViewGraph::IsXDateType(){
	AttrList *gAttrList = GetFirstMap()->GDataAttrList();
	if (gAttrList) {
	  gAttrList->InitIterator();
	  while(gAttrList->More()) {
		AttrInfo *info = gAttrList->Next();
		if(!strcmp(info->name, "x") && info->type==DateAttr) 
			return true;
	  }
	  gAttrList->DoneIterator();
	}
	return false;
}

/* Check if the Y attr of GData is DateAttr */
Boolean ViewGraph::IsYDateType(){
	AttrList *gAttrList = GetFirstMap()->GDataAttrList();
	if (gAttrList) {
	  gAttrList->InitIterator();
	  while(gAttrList->More()) {
		AttrInfo *info = gAttrList->Next();
		if(!strcmp(info->name, "y") && info->type==DateAttr) 
			return true;
	  }
	  gAttrList->DoneIterator();
	}
	return false;
}

void ViewGraph::SetAction(Action *action)
{
  if (_deleteAction) delete _action;
  _action = action;
  _deleteAction = false;
}

void ViewGraph::SetHistogramWidthToFilter()
{
    VisualFilter filter;
    GetVisualFilter(filter);
    Coord lo = filter.yLow;
    Coord hi = filter.yHigh;
    bool is_filter = true;
    if( lo == _allStats.GetHistMin() && hi == _allStats.GetHistMax() ) {
	// histogram already at these values so switch to the
	// global file min & max if available
	int index = InitMappingIterator();
	if (MoreMapping(index)) {
	    MappingInfo *info = NextMapping(index);
	    AttrInfo *yAttr = info->map->MapGAttr2TAttr(MappingCmd_Y);
	    if( yAttr && yAttr->hasLoVal && yAttr->hasHiVal ) {
		lo = AttrList::GetVal(&yAttr->loVal, yAttr->type);
		hi = AttrList::GetVal(&yAttr->hiVal, yAttr->type);
		is_filter = false;
	    }
	}
	DoneMappingIterator(index);
    }
    printf("Histogram for view %s set to (%g, %g) from %s range\n",
	   GetName(), lo, hi, is_filter ? "filter" : "file");
    _allStats.SetHistWidth(lo, hi);
    ResetGStatInMem();
    AbortQuery();
    Refresh();
}

void ViewGraph::DerivedStartQuery(VisualFilter &filter, int timestamp)
{
#if defined(DEBUG)
  printf("ViewGraph(%s)::DerivedStartQuery()\n", GetName());
#endif

  _queryFilter = filter;
  _timestamp = timestamp;

  // Initialize statistics collection
  _allStats.Init(this);

  for(int i = 0; i < gMaxNumColors; i++)
    _stats[i].Init(this);

  int index = _blist.InitIterator();
  while (_blist.More(index)) {
    delete _blist.Next(index);
  }
  _blist.DoneIterator(index);
  _blist.DeleteAll();
  _gstatX.Clear();     /* Clear the hashtable and calculate it again */
  _gstatY.Clear();     /* Clear the hashtable and calculate it again */
  _glistX.DeleteAll(); /* Clear the gdata list */
  _glistY.DeleteAll(); /* Clear the gdata list */

  // Initialize record links whose master this view is
  index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    RecordLink *link = _masterLink.Next(index);
    link->Initialize();
  }
  _masterLink.DoneIterator(index);

  _index = InitMappingIterator(true);   // open iterator backwards
  if (MoreMapping(_index)) {
    _map = NextMapping(_index)->map;
#ifdef DEBUG
    printf("Submitting query 1 of %d: 0x%p\n", _mappings.Size(), _map);
#endif
    _pstorage.Clear();
    _queryProc->BatchQuery(_map, _queryFilter, queryCallback, 0, _timestamp);

    if (_sendToSocket) {
      if (_gds != NULL) {
        reportErrNosys("Duplicate GDataSock creation");
      } else {
        _gds = new GDataSock(_gdsParams);
        if (!_gds->GetStatus().IsComplete()) {
	  reportErrNosys("Error creating GDataSock object");
	  delete _gds;
	  _gds = NULL;
        }
      }
    }
  } else {
#ifdef DEBUG
    printf("View has no mappings; reporting query as done\n");
#endif
    ReportQueryDone(0);
    DoneMappingIterator(_index);
    _map = 0;
    _index = -1;
  }
}

void ViewGraph::DerivedAbortQuery()
{
#if defined(DEBUG)
    printf("ViewGraph::DerivedAbortQuery(), index = %d\n", _index);
#endif

  if (_map) {
    _queryProc->AbortQuery(_map, queryCallback);
    DOASSERT(_index >= 0, "Invalid iterator index");
    DoneMappingIterator(_index);
    _map = 0;
    _index = -1;
  }

  // Abort record links whose master this view is
  int index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    RecordLink *link = _masterLink.Next(index);
    link->Abort();
  }
  _masterLink.DoneIterator(index);
}

void
ViewGraph::SetDrawToScreen(Boolean drawToScreen)
{
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::SetDrawToScreen(%d)\n", this, drawToScreen);
#endif

  if (drawToScreen && !_drawToScreen) {
    // We should make this a non-propagating redraw when that's possible.
    Refresh();
  }

  _drawToScreen = drawToScreen;
}

void
ViewGraph::SetSendToSocket(Boolean sendToSocket)
{
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::SetSendToSocket(%d)\n", this, sendToSocket);
#endif

  if (sendToSocket && !_sendToSocket) {
    // We should make this a non-propagating redraw when that's possible.
    Refresh();
  }

  _sendToSocket = sendToSocket;
}


void
ViewGraph::SetSendParams(const GDataSock::Params &params)
{
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::SetSendParams(%d, %s, %d, '%c')\n", this,
    params.portNum, params.file ? params.file : "NULL", params.sendText,
    params.separator);
#endif

  _gdsParams = params;
  _gdsParams.file = CopyString(_gdsParams.file);
}

//******************************************************************************
// Callback Methods (QueryCallback)
//******************************************************************************

void	ViewGraph::QueryDone(int bytes, void* userData, TDataMap* map)
{
#if defined(DEBUG)
	printf("ViewGraph::QueryDone(), index = %d, bytes = %d\n", _index, bytes);
#endif

	_pstorage.Clear();

	if (_index < 0)
		return;

	if (MoreMapping(_index))
	{
#ifdef DEBUG
		printf("Submitting next query 0x%p\n", _map);
#endif
		_map = NextMapping(_index)->map;
		_queryProc->BatchQuery(_map, _queryFilter, queryCallback, 0,
							   _timestamp);

		return;
	}

	DoneMappingIterator(_index);
	_map = 0;
	_index = -1;

    delete _gds;
    _gds = NULL;

    _allStats.Done();
    _allStats.Report();

	for(int i=0; i<gMaxNumColors; i++)
		_stats[i].Done();

	PrepareStatsBuffer(map);
	DrawLegend();

	// Finish record links whose master is this view
	int		index = _masterLink.InitIterator();

	while(_masterLink.More(index))
		_masterLink.Next(index)->Done();

	_masterLink.DoneIterator(index);
	ReportQueryDone(bytes);
}

void	ViewGraph::PrintLinkInfo(void) 
{
	printf("View : %s \n", GetName());
	printf("Master of ");

	int		index = _masterLink.InitIterator();

	while(_masterLink.More(index))
	{
		RecordLink*		reclink = (RecordLink*)_masterLink.Next(index);

		if (reclink)
			reclink->Print();
	}

	_masterLink.DoneIterator(index);
	printf("\nSlave of ");

	index = _slaveLink.InitIterator();

	while(_slaveLink.More(index))
	{
		RecordLink*		reclink = (RecordLink*)_slaveLink.Next(index);

		if (reclink) 
			reclink->Print();
	}

	_slaveLink.DoneIterator(index);
	printf("\nUpdate Link");
	_updateLink.Print();
}

//******************************************************************************
// Callback Methods (WindowCallback)
//******************************************************************************

void	ViewGraph::HandlePress(WindowRep* w, int xlow, int ylow,
							   int xhigh, int yhigh, int button)
{
#if defined(DEBUG)
    printf("ViewGraph(0x%p %s)::HandlePress(%d, %d, %d, %d, %d)\n", this,
	  GetName(), xlow, ylow, xhigh, yhigh, button);
#endif

	if ((xlow == xhigh) && (ylow == yhigh) &&
		CheckCursorOp(w, xlow, ylow, button))	// Was a cursor event?
          return;

	// Note: doing the unhighlight and highlight here breaks the dependency
	// we had on the client doing this for us.  RKW Jan 27, 1998.

	// Unhighlight previously-selected view, if any, and highlight the
	// newly-selected view (do nothing if they are the same view).
	View *prevSelView = NULL;
	int index = InitViewIterator();
	while (MoreView(index) && prevSelView == NULL) {
	  View *tmpView = NextView(index);
	  if (tmpView->IsHighlighted()) prevSelView = tmpView;
	}
	DoneViewIterator(index);

	if (prevSelView != this) {
	  if (prevSelView != NULL) prevSelView->Highlight(false);
	  Highlight(true);
	}

	ControlPanel::Instance()->SelectView(this);

	if (_action)				// Convert from screen to world coordinates
	{
		Coord	worldXLow, worldYLow, worldXHigh, worldYHigh;

		FindWorld(xlow, ylow, xhigh, yhigh,
				  worldXLow, worldYLow, worldXHigh, worldYHigh);
		
		_action->AreaSelected(this, worldXLow, worldYLow, worldXHigh, 
							  worldYHigh, button);
	}
}

void	ViewGraph::HandleKey(WindowRep* w, int key, int x, int y)
{
	ControlPanel::Instance()->SelectView(this);

	if (_action)				// Convert from screen to world coordinates
	{
		Coord	worldXLow, worldYLow, worldXHigh, worldYHigh;

		FindWorld(x, y, x, y, worldXLow, worldYLow, worldXHigh, worldYHigh);
		_action->KeySelected(this, key, worldXLow, worldYLow);
	}
}

Boolean		ViewGraph::HandlePopUp(WindowRep* win, int x, int y, int button,
								   char**& msgs, int& numMsgs)
{
#ifdef DEBUG
	printf("ViewGraph::HandlePopUp at %d,%d, action = 0x%p\n", x, y, _action);
#endif

	int 			labelX, labelY, labelW, labelH;
	static char*	buf[10];

	ControlPanel::Instance()->SelectView(this);
	GetLabelArea(labelX, labelY, labelW, labelH);

	if ((x >= labelX) && (x <= labelX + labelW - 1) &&
		(y >= labelY) && (y <= labelY + labelH - 1))
	{
		buf[0] = GetName();
		msgs = buf;
		numMsgs = 1;
		return true;
	}

	if (_action)				// Convert from screen to world coordinates
	{
		Coord	worldXLow, worldYLow, worldXHigh, worldYHigh;

		FindWorld(x, y, x + 1, y - 1,
				  worldXLow, worldYLow, worldXHigh, worldYHigh);
		return _action->PopUp(this, worldXLow, worldYLow, worldXHigh,
							  worldYHigh, button, msgs, numMsgs);
	}

	return false;
}

//******************************************************************************
