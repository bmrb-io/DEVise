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
  Revision 1.2  1996/02/28 18:32:14  yuc
  Added CVS header.
*/


#include "MapInterpShape.h"

// ---------------------------------------------------------- 

// sample block = unit cube
//    cube center at (0, 0, 0), edge size = 1
//    pt0 (-0.5, -0.5,  0.5)       pt1( 0.5, -0.5,  0.5)
//    pt2 ( 0.5, -0.5, -0.5)       pt3(-0.5, -0.5, -0.5)
//    pt4 (-0.5,  0.5,  0.5)       pt5(-0.5,  0.5, -0.5)
//    pt6 ( 0.5,  0.5,  0.5)       pt7( 0.5,  0.5, -0.5)
void
FullMapping_BlockShape::MapBlockVertex(int i)
{
	double W = _width[i]  / 2.0,
	       H = _height[i] / 2.0,
	       D = _depth[i]  / 2.0;

	block_data[i].vt[0].x_ = block_data[i].pt.x_ - W;
	block_data[i].vt[0].y_ = block_data[i].pt.y_ - D; 
	block_data[i].vt[0].z_ = block_data[i].pt.z_ + H;

	block_data[i].vt[1].x_ = block_data[i].pt.x_ + W;
	block_data[i].vt[1].y_ = block_data[i].pt.y_ - D;
	block_data[i].vt[1].z_ = block_data[i].pt.z_ + H;

	block_data[i].vt[2].x_ = block_data[i].pt.x_ + W;
	block_data[i].vt[2].y_ = block_data[i].pt.y_ - D;
	block_data[i].vt[2].z_ = block_data[i].pt.z_ - H;

	block_data[i].vt[3].x_ = block_data[i].pt.x_ - W;
	block_data[i].vt[3].y_ = block_data[i].pt.y_ - D;
	block_data[i].vt[3].z_ = block_data[i].pt.z_ - H;

	block_data[i].vt[4].x_ = block_data[i].pt.x_ - W;
	block_data[i].vt[4].y_ = block_data[i].pt.y_ + D;
	block_data[i].vt[4].z_ = block_data[i].pt.z_ + H;

	block_data[i].vt[5].x_ = block_data[i].pt.x_ - W;
	block_data[i].vt[5].y_ = block_data[i].pt.y_ + D;
	block_data[i].vt[5].z_ = block_data[i].pt.z_ - H;

	block_data[i].vt[6].x_ = block_data[i].pt.x_ + W;
	block_data[i].vt[6].y_ = block_data[i].pt.y_ + D;
	block_data[i].vt[6].z_ = block_data[i].pt.z_ + H;

	block_data[i].vt[7].x_ = block_data[i].pt.x_ + W;
	block_data[i].vt[7].y_ = block_data[i].pt.y_ + D;
	block_data[i].vt[7].z_ = block_data[i].pt.z_ - H;
} // end of MapBlockVertex()

// ---------------------------------------------------------- 

// #define YLC

void FullMapping_BlockShape::MapBlockEdges(int i)
{
	block_data[i].ed[0].p  = 0; block_data[i].ed[0].q  = 3; 
	block_data[i].ed[1].p  = 3; block_data[i].ed[1].q  = 2; 
	block_data[i].ed[2].p  = 2; block_data[i].ed[2].q  = 1; 
	block_data[i].ed[3].p  = 1; block_data[i].ed[3].q  = 0; 
	block_data[i].ed[4].p  = 0; block_data[i].ed[4].q  = 4; 
	block_data[i].ed[5].p  = 4; block_data[i].ed[5].q  = 5; 
	block_data[i].ed[6].p  = 5; block_data[i].ed[6].q  = 3; 
	block_data[i].ed[7].p  = 4; block_data[i].ed[7].q  = 6; 
	block_data[i].ed[8].p  = 6; block_data[i].ed[8].q  = 7; 
	block_data[i].ed[9].p  = 7; block_data[i].ed[9].q  = 5; 
	block_data[i].ed[10].p = 6; block_data[i].ed[10].q = 1; 
	block_data[i].ed[11].p = 2; block_data[i].ed[11].q = 7; 

} // end of MapBlockEdges()

void 
FullMapping_BlockShape::DrawGDataArray(WindowRep *win, void **gdataArray,
				       int numSyms, TDataMap *map,
				       View *view, int pixelSize)
{
    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, z0, x1, y1, z1;
    win->Transform(0, 0, 0, x0, y0, z0);
    win->Transform(1, 1, 1, x1, y1, z1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
                   offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight;
      map->MaxBoundingBox(maxWidth, maxHeight);

#ifdef DEBUG
      printf("BlockShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
          maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
         DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
         return;
      }
    } // end if-then

   int j;
   Color firstColor = 0;
   for(int i = 0; i < numSyms; i++) {

     char *gdata = (char *)gdataArray[i];
     if (i == 0)
          firstColor = GetColor(view, gdata, map, offset);

     _width[i]  = fabs(GetShapeAttr0(gdata, map, offset));
     _height[i] = fabs(GetShapeAttr1(gdata, map, offset));
     _depth[i]  = fabs(GetShapeAttr2(gdata, map, offset));

     block_data[i].pt.x_ = GetX(gdata, map, offset);
     block_data[i].pt.y_ = GetY(gdata, map, offset);
     block_data[i].pt.z_ = GetZ(gdata, map, offset);

     MapBlockVertex(i);
     MapBlockEdges (i);
         
#ifdef YLC
     cout << "numSyms = " << numSyms << endl << endl
          << "  x = " <<block_data[i].pt.x_
          << "  y = " <<block_data[i].pt.y_
          << "  z = " <<block_data[i].pt.z_ << "\n" ;
/*
     for (j = 0; j < BLOCK_VERTEX; j++) {
          cout << "\t" << block_data[i].vt[j].x_ << "  "
               << block_data[i].vt[j].y_ << "  "
               << block_data[i].vt[j].z_ << "\n";
     }
*/
     cout << "  ****************************** \n";
#endif

   } // end of for loop

   win->CompViewingTransf();
   win->MapAllPoints(block_data, numSyms);
   win->MapAllSegments(block_data, numSyms);

   win->SetFgColor(firstColor);
   win->DrawXSegments();
   win->DrawRefAxis();

} // end of DrawGDataArray()


