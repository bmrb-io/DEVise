/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of the ViewShape class.
 */

/*
  $Id$

  $Log$
  Revision 1.24  1999/08/23 21:23:31  wenger
  Removed Shape::NumShapeAttrs() method -- not used.

  Revision 1.23  1999/08/10 20:15:05  wenger
  Parent views can now control the titles of view symbols.

  Revision 1.22  1999/07/13 18:22:39  wenger
  Conditionaled out drawing of rectangle and view name to reduce flashing.

  Revision 1.21  1999/07/13 17:32:44  wenger
  Parent view can now control attribute(s) in child view's mapping;
  cleaned up some of the mapping-related code; better command logging.

  Revision 1.20  1999/06/30 17:38:51  wenger
  Data color of parent view's mapping (if specified) now controls the
  background color of view symbols; defined constant strings for GData
  attribute values to avoid potential problems.

  Revision 1.19  1999/06/22 18:33:28  wenger
  Visual filter values for view symbols can now be specified in the parent
  view's mapping.

  Revision 1.18  1999/05/28 16:32:47  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.17  1999/05/26 19:50:55  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.16  1999/05/20 15:17:56  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.15  1999/05/12 21:01:59  wenger
  Views containing view symbols can now be piled.

  Revision 1.14  1999/05/07 16:09:45  wenger
  Fixed bug in the ordering of viewsym piles.

  Revision 1.13  1999/05/07 14:13:55  wenger
  Piled view symbols now working: pile name is specified in parent view's
  mapping, views are piled by Z specified in parent's mapping; changes
  include improvements to the Dispatcher because of problems exposed by
  piled viewsyms; for now, view symbol piles are always linked (no GUI or
  API to change this).

  Revision 1.12  1999/04/14 15:30:20  wenger
  Improved 'switch TData': moved the code from Tcl to C++, functionality
  is more flexible -- schemas don't have to match exactly as long as the
  appropriate TData attributes are present; TData can now be specified for
  view symbols in parent view mapping; updated shape help.

  Revision 1.11  1999/03/03 18:22:07  wenger
  Fixed bugs 426 and 432 (problems with '5' (home) key); fixed bugs 466
  and 467 (query errors with sorted attributes); minor improvements to
  view symbols.

  Revision 1.10  1999/01/04 15:33:33  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

 */

#include <sys/param.h>
#include <iostream.h>

#include "ViewShape.h"
#include "TDataMap.h"
#include "Init.h"
#include "Util.h"
#include "DevError.h"
#include "ClassDir.h"
#include "Control.h"
#include "StringStorage.h"
#include "PileStack.h"
#include "Color.h"
#include "Parse.h"

//#define DEBUG

void
FullMapping_ViewShape::FindBoundingBoxes(void *gdataArray, int numRecs,
    TDataMap *tdMap, Coord &maxWidth, Coord &maxHeight)
{
#if defined(DEBUG)
  printf("FullMapping_ViewShape::FindBoundingBoxes(%d)\n", numRecs);
#endif

  GDataAttrOffset *offsets = tdMap->GetGDataOffset();

  if (offsets->_bbULxOffset < 0 && offsets->_bbULyOffset < 0 &&
      offsets->_bbLRxOffset < 0 && offsets->_bbLRyOffset < 0) {
#if defined(DEBUG)
  printf("  Bounding box is constant\n");
#endif
    // Just do one record, since they're all the same.
    numRecs = 1;
    gdataArray = NULL; // because accessing GData is an error here
  } else {
#if defined(DEBUG)
    printf("  Bounding box is variable\n");
#endif
  }

  char *dataP = (char *)gdataArray; // char * for ptr arithmetic
  int recSize = tdMap->GDataRecordSize();
  Coord tmpMaxW = 0.0;
  Coord tmpMaxH = 0.0;
  for (int recNum = 0; recNum < numRecs; recNum++) {
    Coord symSize = tdMap->GetSize(dataP);
    Coord symWidth =  symSize * tdMap->GetShapeAttr1(dataP);
    Coord symHeight =  symSize * tdMap->GetShapeAttr2(dataP);

    tmpMaxW = MAX(tmpMaxW, symWidth);
    tmpMaxH = MAX(tmpMaxH, symHeight);

    Coord halfWidth = symWidth / 2.0;
    Coord halfHeight = symHeight / 2.0;

    tdMap->SetBoundingBox(dataP, -halfWidth, halfHeight, halfWidth,
        -halfHeight);

    dataP += recSize;
  }

  maxWidth = tmpMaxW;
  maxHeight = tmpMaxH;
}

void FullMapping_ViewShape::DrawGDataArray(WindowRep *win, 
					   void **gdataArray,
					   int numSyms, TDataMap *map, 
					   ViewGraph *view, 
					   int pixelSize,
					   int &recordsProcessed,
					   Boolean timeoutAllowed)
{
#if defined(DEBUG)
  printf("%s\n", __PRETTY_FUNCTION__);
#endif

  if (view->GetNumDimensions() == 3) {
    Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
      recordsProcessed, timeoutAllowed);
    return;
  }

//TEMP -- should we have the drawing timeout here??
  recordsProcessed = numSyms;

  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  //
  // Shape attributes should store:
  //   attr[0] = name of view
  //   attr[1] = width of symbol to be drawn 
  //   attr[2] = height of symbol to be drawn 
  //
  unsigned long attrflags = map->GetDynamicShapeAttrs();
  if (!(attrflags & (1 << 0))) {
    reportErrNosys("No view name specified in GData");
    return;
  } 

  GDataAttrOffset *offset = map->GetGDataOffset();
  StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);

  for (int i = 0; i < numSyms; i++) {
    char *gdata = (char *) gdataArray[i];
    
    //
    // Do transformations to get width and height.
    //
    Coord dataX = map->GetX(gdata);
    Coord dataY = map->GetY(gdata);

    Boolean hasZ = false;
    Coord dataZ;
    if (map->GDataAttrList()->Find("z")) {
      hasZ = true;
      dataZ = map->GetZ(gdata);
    }

    Coord dataSize = map->GetSize(gdata);
    dataSize *= pixelSize; // +/- keys in view
    Coord dataWd = map->GetShapeAttr1(gdata) * dataSize;
    Coord dataHt = map->GetShapeAttr2(gdata) * dataSize;

    Coord pixX1, pixY1, pixX2, pixY2;

    //
    // dataX, dataY are center; pixX, pixY are upper left corner.
    //
    win->Transform(dataX - dataWd / 2.0, dataY - dataHt / 2.0, pixX1, pixY1);
    win->Transform(dataX + dataWd / 2.0, dataY + dataHt / 2.0, pixX2, pixY2);
    int pixX = (int)MIN(pixX1, pixX2);
    int pixY = (int)MIN(pixY1, pixY2);
    unsigned pixWd = (unsigned)ABS(pixX1 - pixX2) + 1;
    unsigned pixHt = (unsigned)ABS(pixY1 - pixY2) + 1;

#if 0
    // Draw rectangle for debugging.
    {
      PColorID	pcid = view->GetForeground();
      win->SetForeground(pcid);
      win->SetPattern(map->GetPattern(gdata));
      win->SetLineWidth(map->GetLineWidth(gdata));
      win->FillRect(dataX - dataWd / 2.0, dataY - dataHt / 2.0, dataWd, dataHt);
    }
#endif

    //
    // Find the view name for the symbol from the GData.
    //
    char *viewname = NULL;
    
    int key = (int)map->GetShapeAttr0(gdata);
    int code = stringTable->Lookup(key, viewname);
    if (code < 0) {
      reportErrNosys("Can't find view name in string table");
      continue;
    }
    if (viewname == NULL) {
      reportError("No view name specified", devNoSyserr);
      continue;
    }

#if defined(DEBUG)
    printf("Drawing view symbol <%s> in view <%s>\n", viewname,
      view->GetName());
#endif

#if 0
    // Draw rectangle and view name for debugging.
    {
      PColorID	pcid = view->GetForeground();
      win->SetForeground(pcid);
      win->SetPattern(map->GetPattern(gdata));
      win->SetLineWidth(map->GetLineWidth(gdata));
      // 2.1, 0.9 are a kludge so rect is no larger than the view symbol.
      win->FillRect(dataX - dataWd / 2.0, dataY - dataHt / 2.1, dataWd * 0.9,
        dataHt * 0.9);

      pcid = view->GetBackground();
      win->SetForeground(pcid);
      win->ScaledText(viewname, dataX - dataWd / 2.0, dataY - dataHt / 2.0,
        dataWd, dataHt);
    }
#endif

    //
    // Find the actual view object for the symbol.
    //
    ViewGraph *viewsym = (ViewGraph *)classDir->FindInstance(viewname);
    if (!viewsym) {
      char errBuf[1024];
      sprintf(errBuf, "View <%s> not found", viewname);
      reportErrNosys(errBuf);
      continue;
    }

    // Set the view's TData if a TData is specified in the mapping.
    AttrList *attrList = map->GDataAttrList();
    SetTData(map, attrList, stringTable, gdata, viewsym);

    // Get the specified PileStack object, if any.
    PileStack *ps = GetPile(map, attrList, stringTable, gdata);

    // If a child view value is specified, set this value in the child
    // view's mapping.
    SetChildValue(map, attrList, stringTable, gdata, viewsym);

    // Set the view symbol's visual filter if filter values are specified in
    // the parent view's mapping.
    SetFilter(map, attrList, stringTable, gdata, viewsym);

    // Set the view symbol's title if a title value is specified in the
    // parent view's mapping.
    SetTitle(map, attrList, stringTable, gdata, viewsym);

    // Set the view symbol's background color to the color specified in
    // the parent view's mapping.
    if (map->GDataAttrList()->Find(gdataColorName)) {
      viewsym->SetBackground(map->GetColor(gdata));
    }

    if (view->GetDisplayDataValues()) {
      win->SetForeground(view->GetForeground());
      DisplayDataLabel(win, dataX, dataY, dataY);
    }

    //
    // Now put the view symbol into its parent view.
    //
    viewsym->AppendToParent(view);
    if (ps) {
      CheckPile(viewsym, ps, pixX, pixY, pixWd, pixHt);
    }
    viewsym->Map(pixX, pixY, pixWd, pixHt);
    viewsym->Refresh();

    if (hasZ) {
      viewsym->SetPileZ(dataZ);
    }

    if (ps) {
      ps->InsertView(viewsym);
    }
  }
}

void
FullMapping_ViewShape::SetTData(TDataMap *map, AttrList *attrList,
    StringStorage *stringTable, char *gdata, ViewGraph *viewsym)
{
  AttrInfo *info = attrList->Find(gdataShapeAttr3Name);
  if (info && info->type == StringAttr) {
    int key = (int)map->GetShapeAttr3(gdata);

    char *tdName = NULL;
    int code = stringTable->Lookup(key, tdName);
    if (code < 0) {
      reportErrNosys("Can't find TData name in string table");
    } else {
      viewsym->SwitchTData(tdName);
    }
  }
}

PileStack *
FullMapping_ViewShape::GetPile(TDataMap *map, AttrList *attrList,
    StringStorage *stringTable, char *gdata)
{
  // Figure out whether anything is specified for shapeAttr_4.
  PileStack *ps = NULL;
  AttrInfo *info = attrList->Find(gdataShapeAttr4Name);
  if (info && info->type == StringAttr) {
    int key = (int)map->GetShapeAttr4(gdata);

    char *pileName = NULL;
    int code = stringTable->Lookup(key, pileName);
    if (code < 0) {
      reportErrNosys("Can't find pile name in string table");
    } else {
      ps = PileStack::FindByName(pileName);
      if (!ps) {
        ps = new PileStack(pileName, NULL);
        ps->SetState(PileStack::PSPiledLinked);
      }
    }
  }

  return ps;
}

void
FullMapping_ViewShape::SetChildValue(TDataMap *map, AttrList *attrList,
    StringStorage *stringTable, char *gdata, ViewGraph *viewsym)
{
#if defined(DEBUG)
  printf("FullMapping_ViewShape::SetChildValue(%s)\n", viewsym->GetName());
#endif

  AttrInfo *info = attrList->Find(gdataShapeAttr9Name);
  if (info) {
    char *mapStr = NULL;
    char valBuf[256];
    if (info->type == StringAttr) {
      int key = (int)map->GetShapeAttr(gdata, 9);

      int code = stringTable->Lookup(key, mapStr);
      if (code < 0) {
        reportErrNosys("Can't find mapStr in string table");
#if defined(DEBUG)
      } else {
        printf("mapStr = <%s>\n", mapStr);
#endif
      }
    } else {
      sprintf(valBuf, "%g", map->GetShapeAttr(gdata, 9));
      mapStr = valBuf;
    }

    TDataMap *map = viewsym->GetFirstMap();
    map->SetParentValue(mapStr);
  }
}

void
FullMapping_ViewShape::SetFilter(TDataMap *map, AttrList *attrList,
    StringStorage *stringTable, char *gdata, ViewGraph *viewsym)
{
#if defined(DEBUG)
  printf("FullMapping_ViewShape::SetFilter(%s)\n", viewsym->GetName());
#endif

  VisualFilter filter;
  viewsym->GetVisualFilter(filter);

  Coord xlo, ylo, xhi, yhi;

  if (ShapeAttrToFilterVal(map, attrList, stringTable, gdata, 5,
      viewsym->IsXDateType(), xlo)) {
    filter.xLow = xlo;
  }

  if (ShapeAttrToFilterVal(map, attrList, stringTable, gdata, 6,
      viewsym->IsYDateType(), ylo)) {
    filter.yLow = ylo;
  }

  if (ShapeAttrToFilterVal(map, attrList, stringTable, gdata, 7,
      viewsym->IsXDateType(), xhi)) {
    filter.xHigh = xhi;
  }

  if (ShapeAttrToFilterVal(map, attrList, stringTable, gdata, 8,
      viewsym->IsYDateType(), yhi)) {
    filter.yHigh = yhi;
  }

  viewsym->SetVisualFilter(filter);
}

Boolean
FullMapping_ViewShape::ShapeAttrToFilterVal(TDataMap *map, AttrList *attrList,
    StringStorage *stringTable, char *gdata, int attrNum, Boolean isDate,
    Coord &value)
{
#if defined(DEBUG)
  printf("FullMapping_ViewShape::ShapeAttrToFilterVal(%d)\n", attrNum);
#endif

  DOASSERT(attrNum < MAX_SHAPE_ATTRS, "Illegal shape attribute number");

  Boolean gotValue = false;

  char attrName[128];
  sprintf(attrName, "%s%d", gdataShapeAttrName, attrNum);
  AttrInfo *info = attrList->Find(attrName);
  if (info) {
    if (info->type == StringAttr) {
      char *tmpStr = NULL;
      int key = (int)map->GetShapeAttr(gdata, attrNum);
      int code = stringTable->Lookup(key, tmpStr);
      if (code < 0) {
        reportErrNosys("Can't find filter value in string table");
      } else {
        if (isDate) {
	  if (ParseFloatDate(tmpStr, value)) {
            gotValue = true;
	  }
	} else {
          value = atof(tmpStr);
          gotValue = true;
	}
      }
    } else { // Note: no special case here for date because internally at
             // date is really just an int.
      value = map->GetShapeAttr(gdata, attrNum);
      gotValue = true;
    }
  }

  return gotValue;
}

void
FullMapping_ViewShape::SetTitle(TDataMap *map, AttrList *attrList,
    StringStorage *stringTable, char *gdata, ViewGraph *viewsym)
{
#if defined(DEBUG)
  printf("FullMapping_ViewShape::SetTitle(%s)\n", viewsym->GetName());
#endif

  AttrInfo *info = attrList->Find(gdataShapeAttr10Name);
  if (info && info->type == StringAttr) {
    int key = (int)map->GetShapeAttr(gdata, 10);

    char *title = NULL;
    int code = stringTable->Lookup(key, title);
    if (code < 0) {
      reportErrNosys("Can't find view title in string table");
    } else {
      Boolean occupyTop;
      int extent;
      char *string;
      viewsym->GetLabelParam(occupyTop, extent, string);
      viewsym->SetLabelParam(occupyTop, extent, title);
#if defined(DEBUG)
  printf("  Set title of view <%s> to <%s>\n", viewsym->GetName(), title);
#endif
    }
  }
}
 
void
FullMapping_ViewShape::CheckPile(ViewGraph *viewsym, PileStack *&ps, int pixX,
    int pixY, unsigned pixWd, unsigned pixHt)
{
  ViewWin *firstView = ps->GetFirstView();
  if (firstView && firstView->Mapped()) {
    int pileX, pileY, tmpXP, tmpYP;
    unsigned tmpW, tmpH;
    firstView->Geometry(pileX, pileY, tmpW, tmpH);
    firstView->AbsoluteOrigin(pileX, pileY);
    firstView->GetParent()->AbsoluteOrigin(tmpXP, tmpYP);
    pileX -= tmpXP;
    pileY -= tmpYP;

    if (!PileStack::SameViewOrSamePile(firstView->GetParent(),
        viewsym->GetParent()) ||
        pixX != pileX || pixY != pileY || pixWd != tmpW || pixHt != tmpH) {
      if (false) {
        // Force all views in pile to the same geometry.
#if defined(DEBUG)
        printf("View <%s> has different geometry or parent than first "
            "view in pile;\n"
            " geometry changed to match first view\n", viewsym->GetName());
#endif
        pixX = pileX;
        pixY = pileY;
        pixWd = tmpW;
        pixHt = tmpH;
      } else {
        // Don't pile views if geometry differs.
#if defined(DEBUG)
        printf("View <%s> has different geometry or parent than first "
            "view in pile;\n"
            " view not piled\n", viewsym->GetName());
#endif
        ps = NULL;
      }
    }
  }
}
