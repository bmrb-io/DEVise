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

#include <assert.h>

#include "ViewGraph.h"
#include "TDataMap.h"
#include "ActionDefault.h"

ViewGraph::ViewGraph(char *name, VisualFilter &initFilter, 
		     AxisLabel *xAxis, AxisLabel *yAxis,
		     Color fg, Color bg,
		     Action *action) :
	View(name,action,initFilter, fg, bg, xAxis, yAxis)
{
  if (action == NULL)
    SetAction(new ActionDefault("default"));

  // initialize statistics toggles to ASCII zero (not binary zero)
  memset(_DisplayStats, '0', STAT_NUM);

  // add terminating null
  _DisplayStats[STAT_NUM] = 0;
}

void ViewGraph::InsertMapping(TDataMap *map)
{
  _mappings.Append(map);

  AttrList *attrList = map->GDataAttrList();
  if (attrList != NULL) {
    AttrInfo *info = attrList->Find("x");
    if (info != NULL && info->type == DateAttr)
      SetXAxisAttrType(DateAttr);
    info = attrList->Find("y");
    if (info != NULL && info->type == DateAttr)
      SetYAxisAttrType(DateAttr);
  }
  
  Refresh();
}

void ViewGraph::InitMappingIterator()
{
  _index = _mappings.InitIterator();
}

Boolean ViewGraph::MoreMapping()
{
  return _mappings.More(_index);
}

TDataMap *ViewGraph::NextMapping()
{
  return _mappings.Next(_index);
}

void ViewGraph::DoneMappingIterator()
{
  _mappings.DoneIterator(_index);
}

/* Turn On/Off DisplayStats */
void ViewGraph::SetDisplayStats(char *stat)
{
  if (strlen(stat) != STAT_NUM) {
    fprintf(stderr, "ViewGraph::SetDisplayStats: incorrect length: %d\n",
	    (int)strlen(stat));
    return;
  }

/*
  if (ToRemoveStats(_DisplayStats, stat) == true)
  {
    strncpy(_DisplayStats, stat, STAT_NUM);
    Refresh();
  }
  else 
  {
    strncpy(_DisplayStats, stat, STAT_NUM);
    _stats.Report();
  }
*/

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
  assert(strlen(oldset) == STAT_NUM && strlen(newset) == STAT_NUM);

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
