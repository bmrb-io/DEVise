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
  Revision 1.4  1996/06/15 07:11:02  yuc
  Small changes and clean-up DrawGDataArray, also add view and
  dimensions.

  Revision 1.3  1996/04/13 00:11:17  jussi
  Added View * parameter to DrawGDataArray() and the calls to
  DrawPixelArray() and GetColor().

  Revision 1.2  1996/02/28 18:32:14  yuc
  Added CVS header.
*/

#include "MapInterpShape.h"
#include "Map3D.h"

// uncomment the following line if you want blocks drawn with wire
// frames, otherwise you'll get solid shapes
//#define WIRE_FRAME

void FullMapping_BlockShape::DrawGDataArray(WindowRep *win, 
					    void **gdataArray,
					    int numSyms, TDataMap *map, 
					    View *view, int pixelSize)
{
  view->SetNumDimensions(3);
  Map3D::CompViewingTransf(view->GetCamera());

  int x, y, w, h;
  view->GetDataArea(x, y, w, h);
  view->SetViewDir(w / 2, h / 2);

  GDataAttrOffset *offset = map->GetGDataOffset();

  for(int i = 0; i < numSyms; i++) {
    char *gdata = (char *)gdataArray[i];
    
    block_data[i].pt.x_ = GetX(gdata, map, offset);
    block_data[i].pt.y_ = GetY(gdata, map, offset);
    block_data[i].pt.z_ = GetZ(gdata, map, offset);
    block_data[i].W = fabs(GetShapeAttr0(gdata, map, offset));
    block_data[i].H = fabs(GetShapeAttr1(gdata, map, offset));
    block_data[i].D = fabs(GetShapeAttr2(gdata, map, offset));
    block_data[i].color = GetColor(view, gdata, map, offset);

    Map3D::AssignBlockVertices(block_data[i]);
#ifdef WIRE_FRAME
    Map3D::AssignBlockEdges(block_data[i]);
#else
    Map3D::AssignBlockSides(block_data[i]);
#endif

#ifdef DEBUG
    cout << "sym " << i << " of " << numSyms << endl
         << "  x = " << block_data[i].pt.x_
	 << "  y = " << block_data[i].pt.y_
	 << "  z = " << block_data[i].pt.z_ << endl;
    for(int j = 0; j < BLOCK_VERTEX; j++) {
      cout << "  "
	   << block_data[i].vt[j].x_ << "  "
	   << block_data[i].vt[j].y_ << "  "
	   << block_data[i].vt[j].z_ << endl;
    }
#endif
  }

  // map blocks to points, segments, and planes
  Map3D::MapBlockPoints(block_data, numSyms, view->GetCamera(), w, h);
#ifdef WIRE_FRAME
  Map3D::MapBlockSegments(block_data, numSyms, view->GetCamera(), w, h);
#else
  Map3D::MapBlockPlanes(block_data, numSyms, view->GetCamera(), w, h);
#endif

  // draw blocks
#ifdef WIRE_FRAME
  win->SetFgColor(BlackColor);
  Map3D::DrawSegments(win);
#else
  Map3D::DrawPlanes(win);
#endif

  // now draw reference axes
  win->SetFgColor(BlackColor);
  Map3D::DrawRefAxis(win, view->GetCamera());
}
