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
  Revision 1.7  1996/07/10 19:00:40  jussi
  Updated to reflect new transformation interface.

  Revision 1.6  1996/06/27 19:06:51  jussi
  Merged 3D block shape into 2D rect shape, the appropriate shape
  is chosen based on current view setting. Removed Block and 3DVector
  shapes. Added empty default 3D drawing routine to all other
  shapes.

  Revision 1.5  1996/06/21 19:27:00  jussi
  Moved computation of block vertices and edges to Map3D.C.
  Added support for drawing solid sides for blocks.

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

//#define DEBUG

// uncomment the following line if you want blocks drawn with wire
// frames, otherwise you'll get solid shapes
//#define WIRE_FRAME

void FullMapping_RectShape::Draw3DGDataArray(WindowRep *win,
                                             void **gdataArray,
                                             int numSyms, TDataMap *map,
                                             View *view, int pixelSize)
{
  GDataAttrOffset *offset = map->GetGDataOffset();

  for(int i = 0; i < numSyms; i++) {
    char *gdata = (char *)gdataArray[i];
    
    Coord size = GetSize(gdata, map, offset);

    block_data[i].pt.x_ = GetX(gdata, map, offset);
    block_data[i].pt.y_ = GetY(gdata, map, offset);
    block_data[i].pt.z_ = GetZ(gdata, map, offset);
    block_data[i].W = fabs(size * GetShapeAttr0(gdata, map, offset));
    block_data[i].H = fabs(size * GetShapeAttr1(gdata, map, offset));
    block_data[i].D = fabs(size * GetShapeAttr2(gdata, map, offset));
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

  // get width and height of 3D display area
  int x, y, w, h;
  view->GetDataArea(x, y, w, h);

  // map blocks to points, segments, and planes
  Map3D::MapBlockPoints(win, block_data, numSyms, view->GetCamera(), w, h);
#ifdef WIRE_FRAME
  Map3D::MapBlockSegments(win, block_data, numSyms, view->GetCamera(), w, h);
#else
  Map3D::MapBlockPlanes(win, block_data, numSyms, view->GetCamera(), w, h);
#endif

  // draw blocks
#ifdef WIRE_FRAME
  win->SetFgColor(BlackColor);
  Map3D::DrawSegments(win);
#else
  Map3D::DrawPlanes(win);
#endif
}
