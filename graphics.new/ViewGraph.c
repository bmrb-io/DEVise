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

//#define DEBUG

#include <assert.h>

#include "ViewGraph.h"
#include "TDataMap.h"
#include "ActionDefault.h"
#include "Init.h"
#include "RecordLink.h"
#include "TData.h"


ImplementDList(GStatList, int)

ViewGraph::ViewGraph(char *name, VisualFilter &initFilter, 
		     AxisLabel *xAxis, AxisLabel *yAxis,
		     Color fg, Color bg,
		     Action *action)
: View(name, initFilter, fg, bg, xAxis, yAxis),
  _updateLink("stats link")
{
    DO_DEBUG(printf("ViewGraph::ViewGraph(0x%p, %s)\n",
		    this, (name != NULL) ? name : "<null>"));
    _action = action;
    if (!_action)
      _action = new ActionDefault("default");

    memset(_DisplayStats, '0', STAT_NUM);

    // add terminating null
    _DisplayStats[STAT_NUM] = 0;
    
    // auto scaling is in effect by default
    _autoScale = true;

    _updateLink.SetMasterView(this);

    _statBuffer = new DataSourceFixedBuf(3072, "statBuffer");
    _statBuffer->AddRef();
    _statBuffer->SetControllingView(this);

    _histBuffer = new DataSourceFixedBuf(3072, "histBuffer");
    _histBuffer->AddRef();
    _histBuffer->SetControllingView(this);

    _gdataStatBuffer = new DataSourceFixedBuf(3072, "gdataStatBuffer");
    _gdataStatBuffer->AddRef();
    _gdataStatBuffer->SetControllingView(this);
}

ViewGraph::~ViewGraph()
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
    _gdataStatBuffer->RemoveControllingView(this);
    if( _gdataStatBuffer->DeleteRef() ) {
	delete _gdataStatBuffer;
    }

}

void ViewGraph::AddAsMasterView(RecordLink *link)
{
    // remove this view from the view list of the link; then add
    // the link as one of the links whose master this view is

    link->DeleteView(this);
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
      AttrInfo *yAttr = map->MapGAttr2TAttr("y");
      if( yAttr && yAttr->hasLoVal && yAttr->hasHiVal ) {
	// y min & max known for the file, so use those to define buckets
	double lo = AttrList::GetVal(&yAttr->loVal, yAttr->type);
	double hi = AttrList::GetVal(&yAttr->hiVal, yAttr->type);
	_allStats.SetHistWidth(lo, hi);
      } else {
	// global min & max are not known, so use filter hi & lo
        VisualFilter filter;
        GetVisualFilter(filter);
	_allStats.SetHistWidth(filter.yLow, filter.yHigh);
      }
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
    WindowRep *win = GetWindowRep();

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

        TDataMap *map = info->map;
        if (map->GetGDataOffset()->colorOffset < 0)
          win->SetFgColor(map->GetDefaultColor());
        else
          win->SetFgColor(BlueColor);
        win->AbsoluteText(label, x, y, w - 4, yInc, WindowRep::AlignEast,
                          true);
        y += yInc;
    }

    DoneMappingIterator(index);

    win->PopTransform();
}

void ViewGraph::UpdateAutoScale()
{
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
    AttrInfo *xAttr = map->MapGAttr2TAttr("x");
    AttrInfo *yAttr = map->MapGAttr2TAttr("y");
    AttrInfo *zAttr = map->MapGAttr2TAttr("z");

    if (GetNumDimensions() == 2) {
        VisualFilter filter;
        GetVisualFilter(filter);
        if (xAttr) {
            if (xAttr->hasHiVal)
              filter.xHigh = AttrList::GetVal(&xAttr->hiVal, xAttr->type);
            if (xAttr->hasLoVal)
              filter.xLow = AttrList::GetVal(&xAttr->loVal, xAttr->type);
            if (filter.xHigh == filter.xLow) {
                filter.xHigh += 1.0;
                filter.xLow -= 1.0;
            }
        }
        if (yAttr) {
            if (yAttr->hasHiVal)
              filter.yHigh = AttrList::GetVal(&yAttr->hiVal, yAttr->type);
            if (yAttr->hasLoVal)
              filter.yLow = AttrList::GetVal(&yAttr->loVal, yAttr->type);
            if (filter.yHigh == filter.yLow) {
                filter.yHigh += 1.0;
                filter.yLow -= 1.0;
            }
        }
        if (!IsScatterPlot())
          filter.yLow = 0;
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

void ViewGraph::WriteMasterLink(RecId start, int num)
{
    // Insert records into record links whose master this view is
    int index = _masterLink.InitIterator();
    while(_masterLink.More(index)) {
        RecordLink *link = _masterLink.Next(index);
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
    printf("ViewGraph::MasterRecomputed[%s]\n", GetName());
#endif
    AbortAndReexecuteQuery();
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

void ViewGraph::PrepareStatsBuffer()
{
    /* initialize statistics buffer */
    _statBuffer->Clear();
    _histBuffer->Clear();
    _gdataStatBuffer->Clear();

    /* put the statistics in the stat buffer */
    char line[1024];
    int i;
    for(i = 0; i < MAXCOLOR; i++) {
	if( _stats[i].GetStatVal(STAT_COUNT) > 0 ) {
	    sprintf(line, "%d %d %g %g %g %g %g %g %g %g %g\n",
		      i, (int)_stats[i].GetStatVal(STAT_COUNT),	
		      _stats[i].GetStatVal(STAT_MEAN),
		      _stats[i].GetStatVal(STAT_MAX),
		      _stats[i].GetStatVal(STAT_MIN),
		      _stats[i].GetStatVal(STAT_ZVAL85L),
		      _stats[i].GetStatVal(STAT_ZVAL85H),
		      _stats[i].GetStatVal(STAT_ZVAL90L),
		      _stats[i].GetStatVal(STAT_ZVAL90H),
		      _stats[i].GetStatVal(STAT_ZVAL95L),
		      _stats[i].GetStatVal(STAT_ZVAL95H));
	    int len = strlen(line);
	    DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	    if( (int) _statBuffer->Write(line, len) != len ) {
#ifdef DEBUG
		fprintf(stderr, "Out of statistics buffer space\n");
#endif
		break;
	    }
	}
    }

    double width = _allStats.GetHistWidth();
#if defined(DEBUG)
    printf("histogram width: %g\n", width);
#endif
    if( width > 0 ) {
	double pos = _allStats.GetHistMin() + width / 2.0;
	for(i = 0; i < HIST_NUM; i++) {
	    sprintf(line, "%g %d\n", pos, _allStats.GetHistVal(i));
	    int len = strlen(line);
	    DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	    if( (int) _histBuffer->Write(line, len) != len ) {
#ifdef DEBUG
	        fprintf(stderr, "Out of histogram buffer space\n");
#endif
	        break;
	    }
	    pos += width;
	}
    }

    BasicStats *bs;
    int index = _glist.InitIterator();
    while(_glist.More(index)) {
	int i = _glist.Next(index); 
	if (_gstat.Lookup(i, bs)) {
	   DOASSERT(bs,"HashTable lookup error\n");
	   sprintf(line, "%d %d %g %g %g %g\n",
			     i, (int)bs->GetStatVal(STAT_COUNT),
			     bs->GetStatVal(STAT_YSUM),
			     bs->GetStatVal(STAT_MAX),
			     bs->GetStatVal(STAT_MEAN),
			     bs->GetStatVal(STAT_MIN));
	   int len = strlen(line);
	   DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	   if( (int) _gdataStatBuffer->Write(line, len) != len ) {
#ifdef DEBUG
	       fprintf(stderr, "Out of GData Stat Buffer space\n");
#endif
	       break;
	   }
       }
    }	
    _glist.DoneIterator(index);
}

/* Handle button press event */

void ViewGraph::HandlePress(WindowRep *w, int xlow, int ylow,
			    int xhigh, int yhigh, int button)
{
    if (xlow == xhigh && ylow == yhigh) {
        if (CheckCursorOp(w, xlow, ylow, button))
          /* was a cursor event */
          return;
    }
    
    ControlPanel::Instance()->SelectView(this);
    
    if (_action) {
        /* transform from screen to world coordinates */
        Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
        FindWorld(xlow, ylow, xhigh, yhigh,
                  worldXLow, worldYLow, worldXHigh, worldYHigh);
        
        _action->AreaSelected(this, worldXLow, worldYLow, worldXHigh, 
                              worldYHigh, button);
    }
}

/* handle key event */

void ViewGraph::HandleKey(WindowRep *w, int key, int x, int y)
{
    ControlPanel::Instance()->SelectView(this);

    if (_action) {
        /* xform from screen to world coord */
        Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
        FindWorld(x, y, x, y,
                  worldXLow, worldYLow, worldXHigh, worldYHigh);
        
        _action->KeySelected(this, key, worldXLow, worldYLow);
    }
}

Boolean ViewGraph::HandlePopUp(WindowRep *win, int x, int y, int button,
			       char **&msgs, int &numMsgs)
{
#ifdef DEBUG
    printf("View::HandlePopUp at %d,%d, action = 0x%p\n", x, y, _action);
#endif

    ControlPanel::Instance()->SelectView(this);

    int labelX, labelY, labelW, labelH;
    GetLabelArea(labelX, labelY, labelW, labelH);

    static char *buf[10];

    if (x >= labelX && x <= labelX + labelW - 1
        && y >= labelY && y <= labelY + labelH - 1) {
        buf[0] = GetName();
        msgs = buf;
        numMsgs = 1;
        return true;
    }
    
    if (_action) {
        /* transform from screen x/y into world x/y */
        Coord worldXLow, worldYLow, worldXHigh, worldYHigh;
        FindWorld(x, y, x + 1, y - 1, worldXLow,worldYLow,
                  worldXHigh, worldYHigh);
        return _action->PopUp(this, worldXLow, worldYLow,
                              worldXHigh, worldYHigh, button,
                              msgs, numMsgs);
    }

    return false;
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
	    AttrInfo *yAttr = info->map->MapGAttr2TAttr("y");
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
    AbortAndReexecuteQuery();
}
