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
  Revision 1.10  1999/01/04 15:33:33  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

 */

#include <sys/param.h>
#include <iostream.h>

#include "ViewShape.h"
#include "MappingInterp.h"
#include "Init.h"
#include "Util.h"
#include "DevError.h"
#include "ClassDir.h"
#include "Control.h"
#include "StringStorage.h"

#include "Color.h"
//#define DEBUG

int FullMapping_ViewShape::NumShapeAttrs()
{
  return 3;
}

void FullMapping_ViewShape::MaxSymSize(TDataMap *map, void *gdata, 
				       int numSyms, 
				       Coord &width, Coord &height)
{
  width = 0.0;
  height = 0.0;
}


void FullMapping_ViewShape::DrawGDataArray(WindowRep *win, 
					   void **gdataArray,
					   int numSyms, TDataMap *map, 
					   ViewGraph *view, 
					   int pixelSize,
					   int &recordsProcessed)
{
#if defined(DEBUG)
  printf("%s\n", __PRETTY_FUNCTION__);
#endif

  if (view->GetNumDimensions() == 3) {
    Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
      recordsProcessed);
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
    Coord dataX = ShapeGetX(gdata, map, offset);
    Coord dataY = ShapeGetY(gdata, map, offset);
    Coord dataSize = GetSize(gdata, map, offset);
    dataSize *= pixelSize; // +/- keys in view
    Coord dataWd = GetShapeAttr1(gdata, map, offset) * dataSize;
    Coord dataHt = GetShapeAttr2(gdata, map, offset) * dataSize;

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
      win->SetPattern(GetPattern(gdata, map, offset));
      win->SetLineWidth(GetLineWidth(gdata, map, offset));
      win->FillRect(dataX - dataWd / 2.0, dataY - dataHt / 2.0, dataWd, dataHt);
    }
#endif

    //
    // Find the view name for the symbol from the GData.
    //
    char *viewname = NULL;
    
    int key = (int)GetShapeAttr0(gdata, map, offset);
    int code = stringTable->Lookup(key, viewname);
    if (code < 0) {
      reportErrNosys("Can't find view name in string table");
      continue;
    }
    if (viewname == NULL) {
      reportError("No view name specified", devNoSyserr);
      continue;
    }

#if 1
    // Draw rectangle and view name for debugging.
    {
      PColorID	pcid = view->GetForeground();
      win->SetForeground(pcid);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->SetLineWidth(GetLineWidth(gdata, map, offset));
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

    if (view->GetDisplayDataValues()) {
      win->SetForeground(view->GetForeground());
      DisplayDataLabel(win, dataX, dataY, dataY);
    }


    //
    // Now put the view symbol into its parent view.
    //
    viewsym->AppendToParent(view);
    viewsym->Map(pixX, pixY, pixWd, pixHt);
    viewsym->Refresh();
  }
}
