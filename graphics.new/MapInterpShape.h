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
  Revision 1.29  1996/07/19 13:27:27  jussi
  Fixed handling of stored points in LineShape.

  Revision 1.28  1996/07/19 03:29:46  jussi
  Fixed erroneous coordinate in LineShadeShape.

  Revision 1.27  1996/07/19 03:23:38  jussi
  Added LineShape and LineShadeShape.

  Revision 1.26  1996/07/15 21:33:01  jussi
  Added support for the 'size' gdata parameter. Fixed problem
  with patterns and Rect shapes.

  Revision 1.25  1996/07/15 18:22:29  jussi
  Fixed stupid bug introduced in last check-in.

  Revision 1.24  1996/07/15 17:20:51  jussi
  Added code to extract string data from StringStorage.

  Revision 1.23  1996/07/02 22:45:57  jussi
  The bounding box of symbols is now correctly computed. Scatter
  plots sometimes did not have all necessary data displayed in
  them, as bounding box used to be incorrectly computed.

  Revision 1.22  1996/06/27 22:54:26  jussi
  Added support for XOR color value.

  Revision 1.21  1996/06/27 19:06:52  jussi
  Merged 3D block shape into 2D rect shape, the appropriate shape
  is chosen based on current view setting. Removed Block and 3DVector
  shapes. Added empty default 3D drawing routine to all other
  shapes.

  Revision 1.20  1996/06/21 19:33:56  jussi
  Moved BlockShape private functions to Map3D.

  Revision 1.19  1996/06/16 01:52:24  jussi
  Added PolylineShape, PolylineFileShape, and TextLabelShape.
  Improved handling of GifImageShape.

  Revision 1.18  1996/05/22 21:05:08  jussi
  Added HighLowShape. Added tentative version of GifImageShape.

  Revision 1.17  1996/04/23 20:35:37  jussi
  Added Segment shape which just connects two end points.

  Revision 1.16  1996/04/16 20:44:08  jussi
  Added HorLineShape, a 2D horizontal line shape that is used
  by statistics views.

  Revision 1.15  1996/04/09 22:55:00  jussi
  Added View parameter to DrawGDataArray().

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

#include <iostream.h>

#include "Transform.h"
#include "Geom.h"
#include "RectShape.h"
#include "StringStorage.h"

//#define DEBUG

// -----------------------------------------------------------------

class FullMapping_RectShape: public RectShape {
public:
  virtual int NumShapeAttrs() { return 2; }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

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
      Pattern firstPattern = Pattern0;

      int count = 0;

      for(; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];

        Color color = GetColor(view, gdata, map, offset);
        Coord size = GetSize(gdata, map, offset);
        Pattern pattern = GetPattern(gdata, map, offset);

	if (count > 0 && (color != firstColor || pattern != firstPattern))
	  break;
	
	if (count == 0) {
	  firstColor = GetColor(view, gdata, map, offset);
          firstPattern = GetPattern(gdata, map, offset);
        }

	_width[count] = fabs(size * GetShapeAttr0(gdata, map, offset));
	_height[count] = fabs(size * GetShapeAttr1(gdata, map, offset));
	_x[count] = GetX(gdata, map, offset);
	if (_width[count] > pixelWidth)
	  _x[count] -= _width[count] / 2.0;
	_y[count] = GetY(gdata, map, offset);
	if (_height[count] > pixelHeight)
	  _y[count] -= _height[count] / 2.0;
	
	count++;
      }
      
      if (firstColor == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(firstColor);
      win->SetPattern(firstPattern);
      win->FillRectArray(_x, _y, _width, _height, count);
      if (firstColor == XorColor)
        win->SetCopyMode();
    }
  }

protected:
  virtual void Draw3DGDataArray(WindowRep *win, void **gdataArray,
                                int numSyms, TDataMap *map,
                                ViewGraph *view, int pixelSize);
};
 
// -----------------------------------------------------------------

class FullMapping_RectXShape: public RectXShape {
public:
  virtual int NumShapeAttrs() { return 1; }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {

    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

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
      win->Transform(fabs(GetSize(gdata, map, offset)
                          * GetShapeAttr0(gdata, map, offset)), 0.0,
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

      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->FillPixelRect(tx, ty, width, height);
      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};
 
// -----------------------------------------------------------------

class FullMapping_BarShape: public BarShape {
public:
  virtual int NumShapeAttrs() { return 1; }

  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    int gRecSize = map->GDataRecordSize();
    char *ptr = (char *)gdata;

    for(int i = 0; i < numSyms; i++) {
      Coord temp = fabs(GetSize(ptr, map, offset)
                        * GetShapeAttr0(ptr, map, offset));
      if (temp > width) width = temp;
      temp = fabs(GetY(ptr, map, offset));
      if (temp > height) height = temp;
      ptr += gRecSize;
    }
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {

    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

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
      Coord width = fabs(GetSize(gdata, map, offset)
                         * GetShapeAttr0(gdata, map, offset));
#if 0
      // experiment with 2-pixel wide bars to prevent white vertical
      // lines from showing up
      if (width < 2 * pixelWidth)
	width = 2 * pixelWidth;
#endif
      if (width > pixelWidth)
	x -= width / 2.0;
      Coord y = GetY(gdata, map, offset);

      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->FillRect(x, 0.0, width, y);
      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_RegularPolygonShape: public RegularPolygonShape {
public:
  virtual int NumShapeAttrs() { return 3; }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

      Coord x0, y0, x1, y1;
      win->Transform(0, 0, x0, y0);
      win->Transform(1, 1, x1, y1);
      Coord pixelWidth = 1 / fabs(x1 - x0);
      Coord pixelHeight = 1 / fabs(y1 - y0);

#ifdef DEBUG
      printf("RegularPolygonShape: maxW %.2f, maxH %.2f,\n",
	     maxWidth, maxHeight);
      printf("                     pixelW %.2f, pixelH %.2f\n",
	     pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord width = fabs(GetSize(gdata, map, offset)
                         * GetShapeAttr0(gdata, map, offset));
      Coord height = fabs(GetSize(gdata, map, offset)
                          * GetShapeAttr1(gdata, map, offset));
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
      
      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->FillPoly(points, segments);
      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_OvalShape: public OvalShape {
public:
  virtual int NumShapeAttrs() { return 2; }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

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
      Coord width = fabs(GetSize(gdata, map, offset)
                         * GetShapeAttr0(gdata, map, offset));
      Coord height = fabs(GetSize(gdata, map, offset)
                          * GetShapeAttr1(gdata, map, offset));
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->Arc(x, y, width, height, 0, 2 * PI);
      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_VectorShape: public VectorShape {
public:
  virtual int NumShapeAttrs() { return 2; }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

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
      Coord w = GetSize(gdata, map, offset)
                * GetShapeAttr0(gdata, map, offset);
      Coord h = GetSize(gdata, map, offset)
                * GetShapeAttr1(gdata, map, offset);
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->Line(x, y, x + w, y + h, 1);

      if (w == 0 && h == 0) {
        if (color == XorColor)
          win->SetCopyMode();
	continue;
      }

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

      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_HorLineShape: public HorLineShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 1e9;
    height = 0.0;
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {

    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    VisualFilter filter;
    view->GetVisualFilter(filter);
    Coord xLow = filter.xLow;
    Coord xHigh = filter.xHigh;

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->Line(xLow, y, xHigh, y, 2);
      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_SegmentShape: public SegmentShape {
public:
  virtual int NumShapeAttrs() { return 2; }

  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    int gRecSize = map->GDataRecordSize();
    char *ptr = (char *)gdata;

    for(int i = 0; i < numSyms; i++) {
      // double the width and height because segment starts at
      // at (X,Y) and is not centered at (X,Y)
          Coord w = 2 * GetSize(ptr, map, offset)
                      * GetShapeAttr0(ptr, map, offset);
      Coord h = 2 * GetSize(ptr, map, offset)
                  * GetShapeAttr1(ptr, map, offset);
      if (w > width) width = w;
      if (h > height) height = h;
      ptr += gRecSize;
    }
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 ? true : false);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

#ifdef DEBUG
      printf("SegmentShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord w = GetSize(gdata, map, offset)
                * GetShapeAttr0(gdata, map, offset);
      Coord h = GetSize(gdata, map, offset)
                * GetShapeAttr1(gdata, map, offset);
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->Line(x, y, x + w, y + h, 1);
      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_HighLowShape: public HighLowShape {
public:
  virtual int NumShapeAttrs() { return 3; }

  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    int gRecSize = map->GDataRecordSize();
    char *ptr = (char *)gdata;

    for(int i = 0; i < numSyms; i++) {
      Coord temp = fabs(GetSize(ptr, map, offset)
                        * GetShapeAttr0(ptr, map, offset));
      if (temp > width) width = temp;
      temp = GetShapeAttr1(ptr, map, offset);
      temp -= GetShapeAttr2(ptr, map, offset);
      if (temp > height) height = temp;
      ptr += gRecSize;
    }
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {

    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 &&
			    offset->shapeAttrOffset[2] < 0 ? true : false);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

#ifdef DEBUG
      printf("HighLowShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Coord width = fabs(GetSize(gdata, map, offset)
                         * GetShapeAttr0(gdata, map, offset));
      Coord high = GetShapeAttr1(gdata, map, offset);
      Coord low = GetShapeAttr2(gdata, map, offset);
      Coord tw = width / 20.0;
      Coord hw = width / 2.0;

      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));
      win->FillRect(x - tw, low, 2 * tw, high - low);
      win->Line(x - hw, y, x + hw, y, 1);
      win->Line(x - hw, low, x + hw, low, 1);
      win->Line(x - hw, high, x + hw, high, 1);
      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};
 
// -----------------------------------------------------------------

class FullMapping_PolylineShape: public PolylineShape {
public:
  virtual int NumShapeAttrs() { return MAX_GDATA_ATTRS; }

  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0;
    height = 0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    int gRecSize = map->GDataRecordSize();
    char *ptr = (char *)gdata;

    for(int i = 0; i < numSyms; i++) {
      int npOff = offset->shapeAttrOffset[0];
      if (npOff < 0) {
        ptr += gRecSize;
	continue;
      }
      int np = (int)*(Coord *)(ptr + npOff);
      for(int j = 1; j <= 2 * np; j++) {
	if (j >= MAX_GDATA_ATTRS)
	  continue;
	int off = offset->shapeAttrOffset[j];
	if (off < 0)
	  continue;
	Coord temp = *(Coord *)(ptr + off);
	// every other shape attribute is X, every other is Y
	if (j % 2) {
	  if (temp > width) width = temp;
	} else {
	  if (temp > height) height = temp;
	}
      }
      ptr += gRecSize;
    }
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0 &&
			    offset->shapeAttrOffset[1] < 0 &&
			    offset->shapeAttrOffset[2] < 0 ? true : false);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    if (fixedSymSize) {
      Coord maxWidth, maxHeight, maxDepth;
      map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

#ifdef DEBUG
      printf("PolylineShape: maxW %.2f, maxH %.2f, pixelW %.2f, pixelH %.2f\n",
	     maxWidth, maxHeight, pixelWidth, pixelHeight);
#endif

      if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
	DrawPixelArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
      }
    }

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));

      win->DrawPixel(x, y);

      int npOff = offset->shapeAttrOffset[0];
      if (npOff < 0) {
        if (color == XorColor)
          win->SetCopyMode();
	continue;
      }

      int np = (int)*(Coord *)(gdata + npOff);
#ifdef DEBUG
      printf("Drawing %d additional data points\n", np);
#endif
      for(int j = 0; j < np; j++) {
	if (1 + 2 * j + 1 >= MAX_GDATA_ATTRS)
	  break;
	int xOff = offset->shapeAttrOffset[1 + 2 * j];
	int yOff = offset->shapeAttrOffset[1 + 2 * j + 1];
	if (xOff < 0 || yOff < 0)
	  continue;
	Coord x1 = *(Coord *)(gdata + xOff);
	Coord y1 = *(Coord *)(gdata + yOff);
	win->Line(x, y, x1, y1, 1);
	x = x1;
	y = y1;
      }

      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};
 
// -----------------------------------------------------------------

class FullMapping_GifImageShape: public GifImageShape {
public:
  virtual int NumShapeAttrs() { return 1; }

  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;
  }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {

    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    // first draw a cross mark at each GIF image location;
    // if there is a problem in displaying the GIF image,
    // then at least the user sees some symbol in the window

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);
      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));

      Coord tx, ty;
      win->Transform(x, y, tx, ty);
      win->PushTop();
      win->MakeIdentity();
      win->Line(tx - 3, ty, tx + 3, ty, 1);
      win->Line(tx, ty - 3, tx, ty + 3, 1);
      win->PopTransform();

      if (color == XorColor)
        win->SetCopyMode();

      char *file = "image.gif";

      if (offset->shapeAttrOffset[0] >= 0) {
        int key = (int)GetShapeAttr0(gdata, map, offset);
        int code = StringStorage::Lookup(key, file);
#ifdef DEBUG
        printf("Key %d returns \"%s\", code %d\n", key, file, code);
#endif
      } else {
#ifdef DEBUG
        printf("Using default file \"%s\"\n", file);
#endif
      }

#ifdef DEBUG
      printf("Drawing GIF image %s at %.2f,%.2f\n", file, x, y);
#endif

      win->ImportImage(x, y, GIF, file);
    }
  }
};
 
// -----------------------------------------------------------------

class FullMapping_PolylineFileShape: public PolylineFileShape {
public:
  virtual int NumShapeAttrs() { return 1; }

  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0;
    height = 0;
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);

      char *file = "polyline.dat";
      char *format = "%lf%lf";

      if (offset->shapeAttrOffset[0] >= 0) {
        int key = (int)GetShapeAttr0(gdata, map, offset);
        int code = StringStorage::Lookup(key, file);
#ifdef DEBUG
        printf("Key %d returns \"%s\", code %d\n", key, file, code);
#endif
      } else {
#ifdef DEBUG
        printf("Using default file \"%s\"\n", file);
#endif
      }

      if (offset->shapeAttrOffset[1] >= 0) {
        int key = (int)GetShapeAttr1(gdata, map, offset);
        int code = StringStorage::Lookup(key, format);
#ifdef DEBUG
        printf("Key %d returns \"%s\", code %d\n", key, format, code);
#endif
      } else {
#ifdef DEBUG
        printf("Using default format \"%s\"\n", format);
#endif
      }

      FILE *fp = fopen(file, "r");
      if (!fp) {
	printf("Cannot open polyline file %s\n", file);
	continue;
      }

#ifdef DEBUG
      printf("Drawing polyline file %s at %.2f,%.2f\n", file, x, y);
#endif

      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));

      Boolean hasPrev = false;
      Coord x0 = 0, y0 = 0;

      char line[255];
      while(fgets(line, sizeof line, fp)) {
	if (line[strlen(line) - 1] == '\n')
	  line[strlen(line) - 1] = 0;
	if (!strlen(line) || line[0] == '#') {
	  if (hasPrev)
	    win->DrawPixel(x0, y0);
	  hasPrev = false;
	} else {
	  Coord x1, y1;
	  if (sscanf(line, format, &x1, &y1) != 2) {
	    printf("Ignoring invalid line: %s\n", line);
	    continue;
	  }
	  x1 += x;
	  y1 += y;
	  if (hasPrev)
	    win->Line(x0, y0, x1, y1, 1);
	  else
	    hasPrev = true;
	  x0 = x1;
	  y0 = y1;
	}
      }

      fclose(fp);

      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};
 
// -----------------------------------------------------------------

class FullMapping_TextLabelShape: public TextLabelShape {
public:
  virtual int NumShapeAttrs() { return 1; }

  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0;
    height = 0;
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord x = GetX(gdata, map, offset);
      Coord y = GetY(gdata, map, offset);
      Color color = GetColor(view, gdata, map, offset);

      char *label = "X";

      if (offset->shapeAttrOffset[0] >= 0) {
        int key = (int)GetShapeAttr0(gdata, map, offset);
        int code = StringStorage::Lookup(key, label);
#ifdef DEBUG
        printf("Key %d returns \"%s\", code %d\n", key, label, code);
#endif
      } else {
#ifdef DEBUG
        printf("Using default label \"%s\"\n", label);
#endif
      }

      if (color == XorColor)
        win->SetXorMode();
      else
        win->SetFgColor(color);
      win->SetPattern(GetPattern(gdata, map, offset));

      // Pretend that there's a 500x500 box in which the text has to
      // be drawn; this is done because we don't know the size of the
      // the label in pixels, and if we pass a width or height that
      // is too tight, AbsoluteText() will try to scale the text.
      Coord fakeSize = 500;
      win->AbsoluteText(label, x - fakeSize / 2, y - fakeSize / 2,
			fakeSize, fakeSize, WindowRep::AlignCenter, true);

      if (color == XorColor)
        win->SetCopyMode();
    }
  }
};

// -----------------------------------------------------------------

class FullMapping_LineShape: public LineShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		 
    if (view->GetNumDimensions() == 3) {
      Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
      return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    /* get coordinates of first data point in this batch */

    char *gdata = (char *)gdataArray[0];
    RecId recId = GetRecId(gdata, map, offset);
    Coord x0 = GetX(gdata, map, offset);
    Coord y0 = GetY(gdata, map, offset);
    Color c0 = GetColor(view, gdata, map, offset);

    /* draw line connecting last point of previous batch to
       first point of this batch */

    if (recId > 0) {
        Coord xp, yp;
        Color cp;
        if (view->GetPointStorage()->Find(recId - 1, xp, yp, cp)) {
            DrawConnectingLine(win, GetPattern(gdata, map, offset),
                               xp, yp, cp, x0, y0, c0);
            (void)view->GetPointStorage()->Remove(recId - 1);
        } else {
            view->GetPointStorage()->Insert(recId, x0, y0, c0);
        }
    }

    /* now draw line connecting rest of points */

    for(int i = 1; i < numSyms; i++) {
        char *gdata = (char *)gdataArray[i];
        Coord x = GetX(gdata, map, offset);
        Coord y = GetY(gdata, map, offset);
        Color color = GetColor(view, gdata, map, offset);
        DrawConnectingLine(win, GetPattern(gdata, map, offset),
                           x0, y0, c0, x, y, color);
        x0 = x;
        y0 = y;
        c0 = color;
    }

    /* draw line connecting last point of this batch to
       first point of next batch */

    Coord xn, yn;
    Color cn;
    if (view->GetPointStorage()->Find(recId + numSyms, xn, yn, cn)) {
        DrawConnectingLine(win, Pattern0, x0, y0, c0, xn, yn, cn);
        (void)view->GetPointStorage()->Remove(recId + numSyms);
    } else {
        view->GetPointStorage()->Insert(recId + numSyms - 1, x0, y0, c0);
    }
  }

  virtual void DrawConnectingLine(WindowRep *win, Pattern pattern,
                                  Coord x0, Coord y0, Color c0,
                                  Coord x1, Coord y1, Color c1) {
      win->SetPattern(pattern);
      if (c0 == XorColor)
          win->SetXorMode();
      else
          win->SetFgColor(c0);

      if (c0 == c1) {
          win->Line(x0, y0, x1, y1, 1);
          if (c0 == XorColor)
              win->SetCopyMode();
          return;
      }

      win->Line(x0, y0, (x0 + x1) / 2, (y0 + y1) / 2, 1);
      if (c0 == XorColor)
          win->SetCopyMode();

      if (c1 == XorColor)
          win->SetXorMode();
      else
          win->SetFgColor(c1);
      win->Line((x0 + x1) / 2, (y0 + y1) / 2, x1, y1, 1);
      if (c1 == XorColor)
          win->SetCopyMode();
  }
};

// -----------------------------------------------------------------

class FullMapping_LineShadeShape: public FullMapping_LineShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    int gRecSize = map->GDataRecordSize();
    char *ptr = (char *)gdata;

    for(int i = 0; i < numSyms; i++) {
      Coord temp = fabs(GetY(ptr, map, offset));
      if (temp > height) height = temp;
      ptr += gRecSize;
    }
  }
  
  virtual void DrawConnectingLine(WindowRep *win, Pattern pattern,
                                  Coord x0, Coord y0, Color c0,
                                  Coord x1, Coord y1, Color c1) {
      win->SetPattern(pattern);
      if (c0 == XorColor)
          win->SetXorMode();
      else
          win->SetFgColor(c0);

      Point points[4];

      if (c0 == c1) {
          points[0].x = x0;
          points[0].y = y0;
          points[1].x = x1;
          points[1].y = y1;
          points[2].x = x1;
          points[2].y = 0;
          points[3].x = x0;
          points[3].y = 0;
          win->FillPoly(points, 4);
          if (c0 == XorColor)
              win->SetCopyMode();
          return;
      }

      points[0].x = x0;
      points[0].y = y0;
      points[1].x = (x0 + x1) / 2;
      points[1].y = (y0 + y1) / 2;
      points[2].x = (x0 + x1) / 2;
      points[2].y = 0;
      points[3].x = x0;
      points[3].y = 0;
      win->FillPoly(points, 4);
      if (c0 == XorColor)
          win->SetCopyMode();

      if (c1 == XorColor)
          win->SetXorMode();
      else
          win->SetFgColor(c1);
      points[0].x = (x0 + x1) / 2;
      points[0].y = (y0 + y1) / 2;
      points[1].x = x1;
      points[1].y = y1;
      points[2].x = x1;
      points[2].y = 0;
      points[3].x = (x0 + x1) / 2;
      points[3].y = 0;
      win->FillPoly(points, 4);
      if (c1 == XorColor)
          win->SetCopyMode();
  }
};

#endif
