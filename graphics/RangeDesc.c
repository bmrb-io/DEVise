/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of RangeDesc class.
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>
#include <sys/param.h>

#include "RangeDesc.h"
#include "DevError.h"
#include "View.h"
#include "ViewGraph.h"
#include "VisualLinkClassInfo.h"
#include "DeviseLink.h"
#include "Control.h"
#include "TData.h"
#include "TDataMap.h"
#include "MappingInterp.h"

//#define DEBUG

struct foo {
  // One or the other must be NULL.
  View *_view;
  DeviseLink *_link;
};

static void PrintViewList(FILE *file, RangeDesc::ViewList &views);
static void PrintLinkList(FILE *file, RangeDesc::LinkList &links);

/*------------------------------------------------------------------------------
 * function: RangeDesc::RangeDesc
 * Constructor.
 */
RangeDesc::RangeDesc()
{
#if defined(DEBUG)
  printf("RangeDesc(0x%p)::RangeDesc()\n", this);
#endif
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::~RangeDesc
 * Destructor.
 */
RangeDesc::~RangeDesc()
{
#if defined(DEBUG)
  printf("RangeDesc(0x%p)::~RangeDesc()\n", this);
#endif

  Clear();
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::Dump
 * Dump a text description to the given file.
 */
DevStatus
RangeDesc::Dump(const char *filename)
{
#if defined(DEBUG)
  printf("RangeDesc::Dump()\n");
#endif

  DevStatus result = StatusOk;

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    char errBuf[MAXPATHLEN * 2];
    sprintf(errBuf, "Unable to open file: %s", filename);
    reportErrSys(errBuf);
    result += StatusFailed;
  } else {
    RangeDesc desc;
    DevStatus tmpResult = desc.Build();
    result += tmpResult;
    if (tmpResult.IsComplete()) {
      desc.Print(file);
    }

    if (fclose(file) != 0) {
      reportErrSys("Error closing file");
      result += StatusWarn;
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::Build
 * Build up the data structures of ranges, tables, and views.
 */
DevStatus
RangeDesc::Build()
{
#if defined(DEBUG)
  printf("RangeDesc(0x%p)::Build()\n", this);
#endif

  DevStatus result = StatusOk;

  result += BuildViewLists();
  result += BuildLinkLists();

#if defined(DEBUG)
  printf("Unassigned X views: ");
  PrintViewList(stdout, _unassignedXViews);
  printf("Unassigned Y views: ");
  PrintViewList(stdout, _unassignedYViews);
  printf("Unassigned X links: ");
  PrintLinkList(stdout, _unassignedXLinks);
  printf("Unassigned Y links: ");
  PrintLinkList(stdout, _unassignedYLinks);
#endif

  int rangeCount = 0;
  while (_unassignedXViews.Size() > 0) {
    char rangeName[128];
    sprintf(rangeName, "X%d", ++rangeCount);
    Range *xRange = new Range(rangeName, RangeX);
    DOASSERT(xRange != NULL, "Out of memory");

    _xRanges.Append(xRange);
    View *view = _unassignedXViews.GetFirst();
    AssignViewToRange(xRange, view, _unassignedXViews, _unassignedXLinks);
  }

  rangeCount = 0;
  while (_unassignedYViews.Size() > 0) {
    char rangeName[128];
    sprintf(rangeName, "Y%d", ++rangeCount);
    Range *yRange = new Range(rangeName, RangeY);
    DOASSERT(yRange != NULL, "Out of memory");

    _yRanges.Append(yRange);
    View *view = _unassignedYViews.GetFirst();
    AssignViewToRange(yRange, view, _unassignedYViews, _unassignedYLinks);
  }

#if defined(DEBUG)
  int linkIndex = _unassignedXLinks.InitIterator();
  while (_unassignedXLinks.More(linkIndex)) {
    DeviseLink *link = _unassignedXLinks.Next(linkIndex);
    int viewIndex = link->InitIterator();
    DOASSERT(!link->More(viewIndex), "Unassigned link has views");
    link->DoneIterator(viewIndex);
  }
  _unassignedXLinks.DoneIterator(linkIndex);
  
  linkIndex = _unassignedYLinks.InitIterator();
  while (_unassignedYLinks.More(linkIndex)) {
    DeviseLink *link = _unassignedYLinks.Next(linkIndex);
    int viewIndex = link->InitIterator();
    DOASSERT(!link->More(viewIndex), "Unassigned link has views");
    link->DoneIterator(viewIndex);
  }
  _unassignedYLinks.DoneIterator(linkIndex);
#endif

  result += BuildTDataList();
  result += BuildViewInfoList();

  return result;
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::Clear
 * Clear all lists, etc.
 */
void
RangeDesc::Clear()
{
#if defined(DEBUG)
  printf("RangeDesc(0x%p)::Clear()\n", this);
#endif

  int index = _xRanges.InitIterator();
  while (_xRanges.More(index)) {
    delete _xRanges.Next(index);
  }
  _xRanges.DoneIterator(index);

  index = _yRanges.InitIterator();
  while (_yRanges.More(index)) {
    delete _yRanges.Next(index);
  }
  _yRanges.DoneIterator(index);

  index = _tDatas.InitIterator();
  while (_tDatas.More(index)) {
    delete _tDatas.Next(index);
  }
  _tDatas.DoneIterator(index);

  index = _views.InitIterator();
  while (_views.More(index)) {
    delete _views.Next(index);
  }
  _views.DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::Print
 * Print out this object.
 */
void
RangeDesc::Print(FILE *file)
{
#if defined(DEBUG)
  printf("RangeDesc(0x%p)::Print()\n", this);
#endif

  fprintf(file, "X Ranges:\n");
  int index = InitXRangeIterator();
  while (MoreXRanges(index)) {
    Range *xRange = NextXRange(index);
    fprintf(file, "  %s: ", xRange->_name);
    PrintViewList(file, xRange->_views);
  }
  XRangeDone(index);

  fprintf(file, "Y Ranges:\n");
  index = InitYRangeIterator();
  while (MoreYRanges(index)) {
    Range *yRange = NextYRange(index);
    fprintf(file, "  %s: ", yRange->_name);
    PrintViewList(file, yRange->_views);
  }
  YRangeDone(index);

  fprintf(file, "Tables:\n");
  index = InitTDIterator();
  while (MoreTD(index)) {
    TdInfo *info = NextTD(index);
    fprintf(file, "  %s: ", info->_tData->GetName());
    PrintViewList(file, info->_views);
  }
  TDDone(index);

  fprintf(file, "Views:\n");
  index = InitViewIterator();
  while (MoreViews(index)) {
    ViewInfo *info = NextView(index);
    fprintf(file, "  %s: {%s} {%s}\n", info->_view->GetName(),
      info->_xMapping, info->_yMapping);
  }
  ViewsDone(index);
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::BuildViewLists
 * Build lists of views not yet assigned to ranges.
 */
DevStatus
RangeDesc::BuildViewLists()
{
  DevStatus result = StatusOk;

  //
  // Go through all views, put them into the lists of views not assigned
  // X ranges and views not assigned Y ranges.
  //
  int index = View::InitViewIterator();
  while (View::MoreView(index)) {
    View *view = View::NextView(index);
    _unassignedXViews.Append(view);
    _unassignedYViews.Append(view);
  }
  View::DoneViewIterator(index);

  return result;
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::BuildLinkLists
 * Build lists of views not yet assigned to ranges.
 */
DevStatus
RangeDesc::BuildLinkLists()
{
  DevStatus result = StatusOk;

  //
  // Go through all links, put visual links into the lists of links not
  // assigned X ranges and/or links not assigned Y ranges, as appropriate.
  //
  int index = DevLink::InitIterator();
  while (DevLink::More(index)) {
    VisualLinkClassInfo *info = DevLink::Next(index);
    DeviseLink *link = (DeviseLink *)info->GetInstance();
    if (link != NULL) {
      if (link->GetFlag() & VISUAL_X) {
        _unassignedXLinks.Append(link);
      }
      if (link->GetFlag() & VISUAL_Y) {
        _unassignedYLinks.Append(link);
      }
    }
  }
  DevLink::DoneIterator(index);

  return result;
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::AssignViewToRange
 * Assign the given view to the given range.
 * Note: this method is called recursively.
 */
void
RangeDesc::AssignViewToRange(Range *range, View *view,
  ViewList &unassignedViewList, LinkList &unassignedLinkList)
{
#if defined(DEBUG)
  printf("RangeDesc(0x%p)::AssignViewToRange(%s, %s)\n", this, range->_name,
    view->GetName());
#endif

  //
  // Move this view from the unassigned list to the range's view list.
  //
  unassignedViewList.Delete(view);
  range->_views.Append(view);

  //
  // Now find all unassigned links connected to this view, and assign *them*
  // to the range.
  //
  int index = unassignedLinkList.InitIterator();
  while (unassignedLinkList.More(index)) {
    DeviseLink *link = unassignedLinkList.Next(index);
    if (link->ViewInLink((ViewGraph *)view)) {
      //
      // Goofiness with the iterator here to make sure we don't try to
      // delete with an interator open.
      //
      unassignedLinkList.DoneIterator(index);
      AssignLinkToRange(range, link, unassignedViewList, unassignedLinkList);
      index = unassignedLinkList.InitIterator();
    }
  }
  unassignedLinkList.DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::AssignLinkToRange
 * Assign the given link to the given range.
 * Note: this method is called recursively.
 */
void
RangeDesc::AssignLinkToRange(Range *range, DeviseLink *link,
  ViewList &unassignedViewList, LinkList &unassignedLinkList)
{
#if defined(DEBUG)
  printf("RangeDesc(0x%p)::AssignLinkToRange(%s, %s)\n", this, range->_name,
    link->GetName());
#endif

#if defined(DEBUG)
  switch (range->_type) {
  case RangeX:
    DOASSERT(link->GetFlag() & VISUAL_X,
      "Range type does not match link type");
    break;

  case RangeY:
    DOASSERT(link->GetFlag() & VISUAL_Y,
      "Range type does not match link type");
    break;

  default:
    DOASSERT(false, "Invalid range type");
    break;
  }
#endif

  //
  // Remove the link from the unassigned list.
  //
  unassignedLinkList.Delete(link);

  //
  // Now find all unassigned views connected to this link, and assign them
  // to the range.
  // 
  int index = link->InitIterator();
  while (link->More(index)) {
    View *view = link->Next(index);
    if (unassignedViewList.Find(view)) {
      link->DoneIterator(index);
      AssignViewToRange(range, view, unassignedViewList, unassignedLinkList);
      index = link->InitIterator();
    }
  }
  link->DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::BuildTDataList
 * Build list of all TDatas.
 */
DevStatus
RangeDesc::BuildTDataList()
{
  DevStatus result = StatusOk;

  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  int classCount;
  char **classNames;
  classDir->ClassNames("tdata", classCount, classNames);

#if defined(DEBUG)
  printf("Class names for tdata: ");
  PrintArgs(stdout, classCount, classNames, true);
#endif

  //
  // Make a list of all of the active TDatas.
  //
  for (int classNum = 0; classNum < classCount; classNum++) {
    int instanceCount;
    char **instancePtrs;
    classDir->InstancePointers("tdata", classNames[classNum], instanceCount,
      instancePtrs);
    for (int instanceNum = 0; instanceNum < instanceCount; instanceNum++) {
      TData *tdata = (TData *)instancePtrs[instanceNum];
      TdInfo *tdInfo = new TdInfo(tdata);
      DOASSERT(tdInfo != NULL, "Out of memory");
      _tDatas.Append(tdInfo);
    }
  }

  //
  // Now go through the views and match up the views with their TData(s).
  //
  int viewIndex = View::InitViewIterator();
  while (View::MoreView(viewIndex)) {
    ViewGraph *view = (ViewGraph *)View::NextView(viewIndex);

    int mappingCount = 0;
    int mapIndex = view->InitMappingIterator();
    while (view->MoreMapping(mapIndex)) {
      mappingCount++;
      MappingInfo *mapInfo = view->NextMapping(mapIndex);

      TData *tdata = mapInfo->map->GetLogTData();
      if (tdata != NULL) {
        int tdIndex = _tDatas.InitIterator();
	while (_tDatas.More(tdIndex)) {
	  TdInfo *tdInfo = _tDatas.Next(tdIndex);
	  if (tdInfo->_tData == tdata) {
	    tdInfo->_views.Append(view);
	  }
	}
	_tDatas.DoneIterator(tdIndex);
      }
    }
    view->DoneMappingIterator(mapIndex);

    if (mappingCount > 1) {
      fprintf(stderr, "Warning: view {%s} has multiple mappings\n",
        view->GetName());
      result += StatusWarn;
    }
  }
  View::DoneViewIterator(viewIndex);

  return result;
}

/*------------------------------------------------------------------------------
 * function: RangeDesc::BuildViewInfoList
 * Build list of all views, with X and Y mapping information.
 */
DevStatus
RangeDesc::BuildViewInfoList()
{
  DevStatus result = StatusOk;

  int viewIndex = View::InitViewIterator();
  while (View::MoreView(viewIndex)) {
    ViewGraph *view = (ViewGraph *)View::NextView(viewIndex);

    int mappingCount = 0;
    int mapIndex = view->InitMappingIterator();
    while (view->MoreMapping(mapIndex)) {
      mappingCount++;
      MappingInfo *mapInfo = view->NextMapping(mapIndex);
      MappingInterpCmd *mapCmd =
        ((MappingInterp *)mapInfo->map)->GetMappingCmd();

      ViewInfo *viewInfo = new ViewInfo(view, mapCmd->xCmd, mapCmd->yCmd);
      DOASSERT(viewInfo != NULL, "Out of memory");
      _views.Append(viewInfo);
    }
    view->DoneMappingIterator(mapIndex);
    if (mappingCount > 1) {
      fprintf(stderr, "Warning: view {%s} has multiple mappings\n",
        view->GetName());
      result += StatusWarn;
    }
  }
  View::DoneViewIterator(viewIndex);

  return result;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*------------------------------------------------------------------------------
 * function: PrintViewList
 * Print a ViewList.
 */
void
PrintViewList(FILE *file, RangeDesc::ViewList &views)
{
  Boolean first = true;
  int index = views.InitIterator();
  while (views.More(index)) {
    View *view = views.Next(index);
    if (first) {
      first = false;
    } else {
      fprintf(file, ", ");
    }
    fprintf(file, "{%s}", view->GetName());
  }
  views.DoneIterator(index);
  fprintf(file, "\n");
}

/*------------------------------------------------------------------------------
 * function: PrintLinkList
 * Print a LinkList.
 */
void
PrintLinkList(FILE *file, RangeDesc::LinkList &links)
{
  Boolean first = true;
  int index = links.InitIterator();
  while (links.More(index)) {
    DeviseLink *link = links.Next(index);
    if (first) {
      first = false;
    } else {
      fprintf(file, ", ");
    }
    fprintf(file, "{%s}", link->GetName());
  }
  links.DoneIterator(index);
  fprintf(file, "\n");
}

/*============================================================================*/
