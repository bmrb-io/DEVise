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
#include<assert.h>

//#define DEBUG

ViewGraph::ViewGraph(char *name, VisualFilter &initFilter, 
		     AxisLabel *xAxis, AxisLabel *yAxis,
		     Color fg, Color bg,
		     Action *action) :
	View(name, action, initFilter, fg, bg, xAxis, yAxis)
{
  if (!action)
    SetAction(new ActionDefault("default"));

  // initialize statistics toggles to ASCII zero (not binary zero)
  memset(_DisplayStats, '0', STAT_NUM);

  // add terminating null
  _DisplayStats[STAT_NUM] = 0;

  _index = -1;
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
  InitMappingIterator();

  while(MoreMapping()) {
    MappingInfo *info = NextMapping();
    if (info->map == map) {
      DoneMappingIterator();
      _mappings.Delete(info);
      delete info->label;
      delete info;
      return;
    }
  }

  DoneMappingIterator();
}

char *ViewGraph::GetMappingLegend(TDataMap *map)
{
  InitMappingIterator();
  while(MoreMapping()) {
    MappingInfo *info = NextMapping();
    if (info->map == map) {
      DoneMappingIterator();
      return info->label;
    }
  }
  DoneMappingIterator();

  return "";
}

void ViewGraph::InitMappingIterator(Boolean backwards)
{
  DOASSERT(_index < 0, "Unexpected iterator index");
 // printf("Index = %d\n",_index);
 // assert(_index < 0 );
  _index = _mappings.InitIterator((backwards ? 1 : 0));
}

Boolean ViewGraph::MoreMapping()
{
  DOASSERT(_index >= 0, "Invalid iterator index");
  return _mappings.More(_index);
}

MappingInfo *ViewGraph::NextMapping()
{
  DOASSERT(_index >= 0, "Invalid iterator index");
  return _mappings.Next(_index);
}

void ViewGraph::DoneMappingIterator()
{
  DOASSERT(_index >= 0, "Invalid iterator index");
  _mappings.DoneIterator(_index);
  _index = -1;
  //printf("Done mapping Index = %d\n",_index);
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

  InitMappingIterator();

  while(MoreMapping()) {
    MappingInfo *info = NextMapping();
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

  DoneMappingIterator();

  win->PopTransform();
}

void ViewGraph::SetDisplayStats(char *stat)
{
  if (strlen(stat) != STAT_NUM) {
    fprintf(stderr, "ViewGraph::SetDisplayStats: incorrect length: %d\n",
	    (int)strlen(stat));
    return;
  }

  // Never redisplay data as long as the visual filter is the same
  // Before drawing the lines, need to figure out which of them to draw
  // If a line existing before needs to be deleted - draw line again
  // If a line not there before needs to be drawn - draw it
  // If line not there before, not to be drawn - dont draw it
  // If line there now, needs to remain - dont draw it
  // Basically XOR logic
  
  StatsXOR(_DisplayStats, stat, _DisplayStats);
  _stats.Report();
  strncpy(_DisplayStats, stat, STAT_NUM);
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
