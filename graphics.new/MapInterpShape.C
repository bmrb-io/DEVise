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
  Revision 1.3  1996/04/13 00:11:17  jussi
  Added View * parameter to DrawGDataArray() and the calls to
  DrawPixelArray() and GetColor().

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

	block_data[i].W = W;
	block_data[i].H = H;
	block_data[i].D = D;

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

// #define YLC

// ---------------------------------------------------------- 
void 
FullMapping_BlockShape::DrawGDataArray(WindowRep *win, 
	void **gdataArray, int numSyms, TDataMap *map, 
	View *view, int pixelSize)
{
    view->SetNumDimensions(3);
    win->CompViewingTransf(view->GetCamera());

    int X=0, Y=0, H=0, V=0;
    view->GetDataArea (X, Y, H, V);
#ifdef YLC
	cout << "win pixel -> X = "<<X<<" Y = "<<Y<<" H = "<<H<<" V = "<<V<<endl;
#endif
    view->SetViewDir(H / 2, V / 2);
//    view->SetLabelInfo(1); // ture = labe occupies top of view

    GDataAttrOffset *offset = map->GetGDataOffset();
    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
                   offset->shapeAttrOffset[1] < 0 ? true : false);

   Color firstColor = 0;
   int i = 0, count;

   while (i < numSyms) {
	count = 0;

	for (; i < numSyms; i++) {
		char *gdata = (char *)gdataArray[i];
		if (count == 0)
			firstColor = GetColor(view, gdata, map, offset);
		else if (count > 0 && 
			GetColor(view, gdata, map, offset) != firstColor)
			break;

		_width[count]  = fabs(GetShapeAttr0(gdata, map, offset));
		_height[count] = fabs(GetShapeAttr1(gdata, map, offset));
		_depth[count]  = fabs(GetShapeAttr2(gdata, map, offset));

		block_data[count].pt.x_ = GetX(gdata, map, offset);
		block_data[count].pt.y_ = GetY(gdata, map, offset);
		block_data[count].pt.z_ = GetZ(gdata, map, offset);

		MapBlockVertex(count);
		MapBlockEdges (count);
		count++;				// get the next pt
#ifdef YLC1
     cout << "numSyms = " << numSyms <<"\n\n"<<"  x = "
		<<block_data[i].pt.x_ << "  y = " <<block_data[i].pt.y_
          << "  z = " <<block_data[i].pt.z_ << "\n" ;
     for (int j = 0; j < BLOCK_VERTEX; j++) {
          cout << "\t" << block_data[i].vt[j].x_ << "  "
               << block_data[i].vt[j].y_ << "  "
               << block_data[i].vt[j].z_ << "\n";
     }
	cout<<"numSyms = "<<numSyms<<"  i = "<<i<<" firstC = "<<firstColor<<endl;
#endif
	} // end of for loop

     win->SetFgColor(0);         // 0 = black
	win->MapAllXPoints(block_data, count, view->GetCamera(), H, V);
	win->MapAllXSegments(block_data, count, view->GetCamera(), H, V);
	win->SetFgColor(firstColor);
	win->DrawXSegments();
   } // end of while-loop
   win->SetFgColor(0);         // 0 = black
   // draw the x,y,z axis
   win->DrawRefAxis(view->GetCamera());
} // end of DrawGDataArray()


