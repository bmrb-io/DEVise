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

  _DisplayStats = false;
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
void ViewGraph::SetDisplayStats(Boolean stat)
{
  _DisplayStats = stat;

  if (_DisplayStats)
    _stats.Report();
  else
    Refresh();
}
