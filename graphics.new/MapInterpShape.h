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
  Revision 1.14  1996/02/28 17:30:32  yuc
  Add 3D BlockShape.

  Revision 1.13  1996/02/05 02:53:48  yuc
  Update MaxInterpShapes to 8, for adding
  3d vector

  Revision 1.12  1996/01/09 22:14:34  jussi
  Added a skeleton for 3D block object.

  Revision 1.11  1995/12/22 18:07:03  jussi
  Added Vector shape.

  Revision 1.10  1995/12/14 17:35:39  jussi
  Made small fixes to get rid of g++ -Wall warnings.

  Revision 1.9  1995/12/13 22:07:28  jussi
  Added debugging statements. If symbol width or height is non-constant
  (depends on user data), then we cannot use the pixel array display
  optimization because there's no good way to determine the maximum
  bounding box and hence to determine whether ALL symbols reduce to
  single pixels.

  Revision 1.8  1995/11/28 00:24:01  jussi
  Moved a couple of #include statements to Shape.h.

  Revision 1.7  1995/11/28 00:04:33  jussi
  Added polygon and oval shape. The former is drawn as a small number of
  segments, the latter as a perfect, round shape.

  Revision 1.6  1995/11/25 01:20:10  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.5  1995/11/24 20:13:40  jussi
  Added missing closing brace.

  Revision 1.4  1995/11/24 20:07:32  jussi
  Fixed calculation of X and Y starting position for cases where
  pixel width (or height) is at most 1. In such cases one should not
  adjust X/Y coordinates by one half of the width/height.

  Revision 1.3  1995/11/21 23:32:07  jussi
  Added copyright notice and cleaned up the code significantly.

  Revision 1.2  1995/09/05 22:15:01  jussi
  Added CVS header.
*/

#ifndef MapInterShape_H
#define MapInterShape_H

#include <assert.h>
#include <iostream.h>
#include "Transform.h"
#include "Geom.h"
#include "RectShape.h"

//#define DEBUG

// -----------------------------------------------------------------

class FullMapping_RectShape: public RectShape {
public:
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		 
    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight;
      map->MaxBoundingBox(maxWidth, maxHeight);

      // pixelWidth is how many X units one pixel corresponds to
      // pixelHeight is how many Y units one pixel corresponds to
      // maxWidth is the maximum width of rectangles, measured in X units
      // maxHeight is the maximum width of rectangles, measured in Y units
      //
      // see if one pixel is enough to cover the area whose width is
      // pixelWidth and height is pixelHeight

#ifdef DEBUG
      printf("RectShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    int i = 0;
    while (i < numSyms) {

      Color firstColor = 0;
      int count = 0;

      for(; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];

	if (count > 0 && GetColor(view, gdata, map, offset) != firstColor)
	  break;
	
	if (count == 0)
	  firstColor = GetColor(view, gdata, map, offset);

	_width[count] = fabs(GetShapeAttr0(gdata, map, offset));
	_height[count] = fabs(GetShapeAttr1(gdata, map, offset));
	_x[count] = GetX(gdata, map, offset);
	if (_width[count] > pixelWidth)
	  _x[count] -= _width[count] / 2.0;
	_y[count] = GetY(gdata, map, offset);
	if (_height[count] > pixelHeight)
	  _y[count] -= _height[count] / 2.0;
	
	count++;
      }
      
      win->SetFgColor(firstColor);
      win->FillRectArray(_x, _y, _width, _height, count);
    }
  }
};
 
// -----------------------------------------------------------------

class FullMapping_RectXShape: public RectXShape {
public:
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {

    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight;
      map->MaxBoundingBox(maxWidth, maxHeight);

      Coord x0, y0, x1, y1;
      win->Transform(0, 0, x0, y0);
      win->Transform(1, 1, x1, y1);
      Coord pixelWidth = 1 / fabs(x1 - x0);

#ifdef DEBUG
      printf("RectXShape: maxW %.2f, pixelW %.2f\n", maxWidth, pixelWidth);
#endif

      if (maxWidth <= pixelWidth) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    Coord x0, y0, x1, y1;
    win->Transform(0.0, 0.0, x0, y0);
    
    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord tx, ty;
      Coord width, height;
      win->Transform(GetX(gdata, map, offset), GetY(gdata, map, offset),
		     tx, ty);
      win->Transform(fabs(GetShapeAttr0(gdata, map, offset)), 0.0,
		     x1, y1);
      width = x1 - x0;
      if (width < 0.0)
	width = -width;
      height = fabs(GetShapeAttr1(gdata, map, offset)) /
	fabs(GetShapeAttr0(gdata, map, offset)) * width;
      if (width < pixelSize)
	width = pixelSize;
      if (height < pixelSize)
	height = pixelSize;

      win->SetFgColor(GetColor(view, gdata, map, offset));
      win->SetPattern(GetPattern(gdata, map, offset));
      win->FillPixelRect(tx, ty, width, height);
    }
  }
};
 
// -----------------------------------------------------------------

class FullMapping_BarShape: public BarShape {
public:
  virtual void BoundingBoxGData(TDataMap *map, void **gdataArray, int numSyms,
				Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord temp = fabs(GetShapeAttr0(gdata, map, offset));
      if (temp > width) width = temp;
      temp = fabs(GetY(gdata, map, offset));
      if (temp > height) height = temp;
    }
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {

    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);

#ifdef DEBUG
    printf("BarShape: pixelW %.2f\n", pixelWidth);
#endif

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord x = GetX(gdata, map, offset);
      Coord width = fabs(GetShapeAttr0(gdata, map, offset));
#if 0
      // experiment with 2-pixel wide bars to prevent white vertical
      // lines from showing up
      if (width < 2 * pixelWidth)
	width = 2 * pixelWidth;
#endif
      if (width > pixelWidth)
	x -= width / 2.0;
      Coord y = GetY(gdata, map, offset);

      win->SetFgColor(GetColor(view, gdata, map, offset));
      win->SetPattern(GetPattern(gdata, map, offset));
      win->FillRect(x, 0.0, width, y);
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_PolygonShape: public PolygonShape {
public:
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		 
    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight;
      map->MaxBoundingBox(maxWidth, maxHeight);

      Coord x0, y0, x1, y1;
      win->Transform(0, 0, x0, y0);
      win->Transform(1, 1, x1, y1);
      Coord pixelWidth = 1 / fabs(x1 - x0);
      Coord pixelHeight = 1 / fabs(y1 - y0);

#ifdef DEBUG
      printf("PolygonShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord width = fabs(GetShapeAttr0(gdata, map, offset));
      Coord height = fabs(GetShapeAttr1(gdata, map, offset));
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      
      // int segments = GetShapeAttr2(gdata, map, offset);
      int segments = 16;
      Point points[segments];
      double increment = 360.0 / segments;
      for(int seg = 0; seg < segments; seg++) {
	double angle = ToRadian((seg - 0.5) * increment);
	points[seg].x = x + width / 2 * cos(angle);
	points[seg].y = y + height / 2 * sin(angle);
      }
      
      win->SetFgColor(GetColor(view, gdata, map, offset));
      win->SetPattern(GetPattern(gdata, map, offset));
      win->FillPoly(points, segments);
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_OvalShape: public OvalShape {
public:
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		 
    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight;
      map->MaxBoundingBox(maxWidth, maxHeight);

      Coord x0, y0, x1, y1;
      win->Transform(0, 0, x0, y0);
      win->Transform(1, 1, x1, y1);
      Coord pixelWidth = 1 / fabs(x1 - x0);
      Coord pixelHeight = 1 / fabs(y1 - y0);

#ifdef DEBUG
      printf("OvalShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord width = fabs(GetShapeAttr0(gdata, map, offset));
      Coord height = fabs(GetShapeAttr1(gdata, map, offset));
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      win->SetFgColor(GetColor(view, gdata, map, offset));
      win->SetPattern(GetPattern(gdata, map, offset));
      win->Arc(x, y, width, height, 0, 2 * PI);
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_VectorShape: public VectorShape {
public:
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		 
    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight;
      map->MaxBoundingBox(maxWidth, maxHeight);

#ifdef DEBUG
      printf("VectorShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Color color = GetColor(view, gdata, map, offset);
      Coord w = GetShapeAttr0(gdata, map, offset);
      Coord h = GetShapeAttr1(gdata, map, offset);
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->Line(x, y, x + w, y + h, 1);

      if (w == 0 && h == 0)
	continue;

      // compute pixel locations
      Coord tx, ty, tw, th;
      win->Transform(x + w, y + h, tx, ty);
      win->Transform(w, h, tw, th);
      tw -= x0;
      th -= y0;

      // draw arrow head
      Coord arrowSize = 0.15 * sqrt(tw * tw + th * th);
      if (arrowSize < 10)
	arrowSize = 10;
      const Coord angle = atan2(th, tw);
      const Coord arrowSteepness = 0.1 * PI;
      const Coord leftAngle = angle - arrowSteepness;
      const Coord rightAngle = angle + arrowSteepness;
      
      Point points[3];
      points[0].x = tx;
      points[0].y = ty;
      points[1].x = tx - arrowSize * cos(leftAngle);
      points[1].y = ty - arrowSize * sin(leftAngle);
      points[2].x = tx - arrowSize * cos(rightAngle);
      points[2].y = ty - arrowSize * sin(rightAngle);

      win->PushTop();
      win->MakeIdentity();
      win->FillPoly(points, 3);
      win->PopTransform();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_BlockShape: public BlockShape {
public:
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize);

private:
	void MapBlockVertex(int);
	void MapBlockEdges (int);
}; // end of FullMapping_BlockShape


// -----------------------------------------------------------------

class FullMapping_3DVectorShape: public VectorShape {
public:
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		 
    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight;
      map->MaxBoundingBox(maxWidth, maxHeight);

#ifdef DEBUG
      printf("VectorShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Color color = GetColor(view, gdata, map, offset);
      Coord w = GetShapeAttr0(gdata, map, offset);
      Coord h = GetShapeAttr1(gdata, map, offset);
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->Line(x, y, x + w, y + h, 1);

      if (w == 0 && h == 0)
	continue;

      // compute pixel locations
      Coord tx, ty, tw, th;
      win->Transform(x + w, y + h, tx, ty);
      win->Transform(w, h, tw, th);
      tw -= x0;
      th -= y0;

      // draw arrow head
      Coord arrowSize = 0.15 * sqrt(tw * tw + th * th);
      if (arrowSize < 10)
	arrowSize = 10;
      const Coord angle = atan2(th, tw);
      const Coord arrowSteepness = 0.1 * PI;
      const Coord leftAngle = angle - arrowSteepness;
      const Coord rightAngle = angle + arrowSteepness;
      
      Point points[3];
      points[0].x = tx;
      points[0].y = ty;
      points[1].x = tx - arrowSize * cos(leftAngle);
      points[1].y = ty - arrowSize * sin(leftAngle);
      points[2].x = tx - arrowSize * cos(rightAngle);
      points[2].y = ty - arrowSize * sin(rightAngle);

      win->PushTop();
      win->MakeIdentity();
      win->FillPoly(points, 3);
      win->PopTransform();
    }
  }
}; // FullMapping_3DVectorShape


#endif


