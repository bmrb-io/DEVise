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

#include "ViewGraph.h"
#include "TDataMap.h"
#include "ActionDefault.h"
#include "Init.h"
#include "RecordLink.h"

//#define DEBUG

ViewGraph::ViewGraph(char *name, VisualFilter &initFilter, 
		     AxisLabel *xAxis, AxisLabel *yAxis,
		     Color fg, Color bg,
		     Action *action) :
	View(name, initFilter, fg, bg, xAxis, yAxis)
{
  _action = action;
  if (!_action)
    _action = new ActionDefault("default");

  memset(_DisplayStats, '0', STAT_NUM);

  // add terminating null
  _DisplayStats[STAT_NUM] = 0;
}

ViewGraph::~ViewGraph()
{
  // Drop view as master from all links.

  // Must delete link using DeleteCurrent() because SetMasterView()
  // calls DropAsMasterView() which calls _masterLink.Delete(link)
  // which is not allowed because we're inside an iterator
  
  int index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    RecordLink *link = _masterLink.Next(index);
    _masterLink.DeleteCurrent(index);
    link->SetMasterView(0);
  }
  _masterLink.DoneIterator(index);

  index = _slaveLink.InitIterator();
  while(_slaveLink.More(index)) {
    RecordLink *link = _slaveLink.Next(index);
    _slaveLink.DeleteCurrent(index);
    link->DeleteView(this);
  }
  _slaveLink.DoneIterator(index);
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

  info->map = map;
  info->label = CopyString(label);
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

  y += (int)(0.2 * h);
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
    win->AbsoluteText(label, x, y, w - 4, yInc, WindowRep::AlignEast, true);
    y += yInc;
  }

  DoneMappingIterator(index);

  win->PopTransform();
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

char *ViewGraph::MakeStatFileName()
{
  char *fname = StripPath(GetName());
  unsigned int nameLen = strlen(Init::WorkDir()) + strlen(fname) + 6 + 1;
  char *name = new char [nameLen];
  sprintf(name, "%s/%s.stat", Init::WorkDir(), fname);
  DOASSERT(strlen(name) < nameLen, "Name too long");
  return name;
}

void ViewGraph::WriteColorStatsToFile()
{
  char *statFile = MakeStatFileName();
  FILE *file = fopen(statFile, "w");

  if (!file) {
    printf("Cannot create statistics file %s: ", statFile);
    perror("fopen");
    delete statFile;
    return;
  }

  /* write column header */
  fprintf(file, "// Color Count Mean Max Min Z85L Z85H Z90L Z90H Z95L Z95H\n");
  fprintf(file, "\n");

  /* put the statistics in the stat file */
  for(int i = 0; i < MAXCOLOR; i++) {
    fprintf(file, "%d %d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
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
  }

  fclose(file);
  delete statFile;

  /* tell query processor to refresh views based on this file */
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

void ViewGraph::HandleKey(WindowRep *w, char key, int x, int y)
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

  if (GetNumDimensions() != 2)
    return false;

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
    FindWorld(x, y, x + 1, y - 1, worldXLow,worldYLow, worldXHigh, worldYHigh);
    return _action->PopUp(this, worldXLow, worldYLow,
			  worldXHigh, worldYHigh, button,
			  msgs, numMsgs);
  }

  return false;
}
