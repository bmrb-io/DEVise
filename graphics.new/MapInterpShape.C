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
  Revision 1.29  1996/12/18 18:52:08  wenger
  Devise requests Tasvir not to use subwindows for its images; sizing of
  Tasvir images now basically works like a RectX, except that the aspect
  ratio is fixed by the image itself; fixed a bug in action of +/- keys
  for RectX shapes; Devise won't request Tasvir images smaller than two
  pixels (causes error in Tasvir).

  Revision 1.28  1996/12/15 20:22:44  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.27  1996/12/04 18:12:47  wenger
  Unimplemented methods in PSWindowRep now report an error but do not
  abort when called; fixed code in cslib server example that caused problems
  for Anand; corrected the values returned by some of the NumShapeAttrs()
  methods in MapInterpShape.

  Revision 1.26  1996/12/03 17:03:04  jussi
  Added support for setting font size.

  Revision 1.25  1996/11/26 16:51:33  ssl
  Added support for piled viws

  Revision 1.24  1996/11/20 19:04:30  jussi
  Changed DrawPixelArray() call again.

  Revision 1.23  1996/11/20 01:09:17  jussi
  Changed threshold value for randomization from 0.5 to 0.15.

  Revision 1.22  1996/11/20 00:43:52  jussi
  Added support for X,Y randomization. Changed DrawPixelArray() call
  back so that +/- is used to control pixel size.

  Revision 1.21  1996/11/13 16:57:04  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.20  1996/11/07 22:40:28  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.19  1996/09/13 16:44:57  wenger
  PixelSize in GData now used as multiplier to the size when symbols are
  larger than one screen pixel (makes the '+' and '-' keys more useful).

  Revision 1.18  1996/09/12 18:38:06  wenger
  Added optional delay before drawing images.

  Revision 1.17  1996/09/06 07:00:11  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.16  1996/09/04 21:25:01  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.15  1996/08/29 18:24:41  wenger
  A number of Dali-related improvements: ShapeAttr1 now specifies image
  type when shape is 'image'; added new '-bytes' flag to Dali commands
  when sending images; TDataBinaryInterp now uses StringStorage so GData
  can access strings; fixed hash function for StringStorage so having the
  high bit set in a byte in the string doesn't crash the hash table;
  improved the error checking in some of the Dali code.

  Revision 1.14  1996/08/28 00:19:51  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.13  1996/08/23 16:56:18  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.12  1996/08/08 21:01:20  beyer
  Moved virtual functions from .h to .C file

  Revision 1.11  1996/08/03 15:38:02  jussi
  Flag _solid3D now has three values. Added line segment width.

  Revision 1.10  1996/07/20 18:49:41  jussi
  Added 3D line segment shape. Improved performance of line shade shape.

  Revision 1.9  1996/07/19 03:23:37  jussi
  Added LineShape and LineShadeShape.

  Revision 1.8  1996/07/15 21:32:07  jussi
  Added support for the 'size' gdata parameter.

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

#include <sys/param.h>

#include "MapInterpShape.h"
#include "Map3D.h"
#include "Init.h"
#include "Util.h"
#include "DevError.h"

//#define DEBUG

#define IMAGE_TYPE_GIF_LOCAL		(0)
#define IMAGE_TYPE_DALI_FILE		(1)
#define IMAGE_TYPE_DALI_FILE_SEND	(2)
#define IMAGE_TYPE_DALI_IMAGE		(3)

//---------------------------------------------------------------------------

int FullMapping_RectShape::NumShapeAttrs()
{
    return 2; 
}



void FullMapping_RectShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					   int numSyms, TDataMap *map,
					   ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
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

    Boolean fixedSymSize = (offset->shapeAttrOffset[0] < 0) &&
			    (offset->shapeAttrOffset[1] < 0);

    if (fixedSymSize) {
	Coord maxWidth, maxHeight, maxDepth;
	map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);
	maxWidth *= pixelSize;
	maxHeight *= pixelSize;
	maxDepth *= pixelSize;

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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, true);
	    return;
	}
    }

    ComputeDataLabelFrame(view);

    int i = 0;
    while (i < numSyms) {

	GlobalColor firstColor = BlackColor;
	Pattern firstPattern = Pattern0;
	int firstLineWidth = 0;

	int count = 0;

	for(; i < numSyms; i++) {
	    char *gdata = (char *)gdataArray[i];

	    GlobalColor color = GetColor(view, gdata, map, offset);
	    Coord size = GetSize(gdata, map, offset);
	    size *= pixelSize;
	    Pattern pattern = GetPattern(gdata, map, offset);

	    if (count > 0 && (color != firstColor || pattern != firstPattern))
	      break;
	    
	    if (count == 0) {
		firstColor = GetColor(view, gdata, map, offset);
		firstPattern = GetPattern(gdata, map, offset);
		firstLineWidth = int(GetLineWidth(gdata, map, offset)+0.5);
	    }

	    _width[count] = fabs(pixelSize * size
                                 * GetShapeAttr0(gdata, map, offset));
	    _height[count] = fabs(pixelSize * size
                                  * GetShapeAttr1(gdata, map, offset));
	    _x[count] = GetX(gdata, map, offset);
	    if (_width[count] > pixelWidth)
	      _x[count] -= _width[count] / 2.0;
	    _y[count] = GetY(gdata, map, offset);
	    if (_height[count] > pixelHeight)
	      _y[count] -= _height[count] / 2.0;

	    count++;
	}
	
        // Randomize X,Y coordinates if shape attribute 2 or 3 contains
        // a constant value of 0.15 or more.

        if (offset->shapeAttrOffset[2] < 0 && offset->shapeAttrOffset[3] < 0) {
            ShapeAttr *attrs = map->GetDefaultShapeAttrs();
            float cloudWidth = fabs(attrs[2]);
            float cloudHeight = fabs(attrs[3]);
            if (cloudWidth >= 0.15 || cloudHeight >= 0.15)
              RandomizePoints(_x, _y, count, cloudWidth, cloudHeight);
        }

	if (firstColor == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(firstColor);
	win->SetPattern(firstPattern);
	win->SetLineWidth(firstLineWidth);

	win->FillRectArray(_x, _y, _width, _height, count);

	if (view->GetDisplayDataValues()) {
	    for(int s = 0; s < count; s++)
	      DisplayDataLabel(win, _x[s] + _width[s] / 2,
			       _y[s] + _height[s] / 2,
			       _y[s] + _height[2] / 2);
	}

	if (firstColor == XorColor)
	  win->SetCopyMode();
    }
}
		 

void FullMapping_RectShape::Draw3DGDataArray(WindowRep *win,
                                             void **gdataArray,
                                             int numSyms, TDataMap *map,
                                             ViewGraph *view, int pixelSize)
{
  GDataAttrOffset *offset = map->GetGDataOffset();
  
  // 0 = wireframe only, 1 = solid, 2 = solid + wireframe
  Boolean wireframe = (view->GetSolid3D() == 0);
  Boolean solidFrame = (view->GetSolid3D() == 2);
  
  for(int i = 0; i < numSyms; i++) {
    char *gdata = (char *)gdataArray[i];
    
    Coord size = GetSize(gdata, map, offset);
    
    _object3D[i].pt.x_ = GetX(gdata, map, offset);
    _object3D[i].pt.y_ = GetY(gdata, map, offset);
    _object3D[i].pt.z_ = GetZ(gdata, map, offset);
    _object3D[i].W = fabs(size * GetShapeAttr0(gdata, map, offset));
    _object3D[i].H = fabs(size * GetShapeAttr1(gdata, map, offset));
    _object3D[i].D = fabs(size * GetShapeAttr2(gdata, map, offset));
    _object3D[i].segWidth = fabs(GetShapeAttr3(gdata, map, offset));
    _object3D[i].segWidth = MAX(_object3D[i].segWidth, 1);
    _object3D[i].color = GetColor(view, gdata, map, offset);

    Map3D::AssignBlockVertices(_object3D[i]);
    if (wireframe)
      Map3D::AssignBlockEdges(_object3D[i]);
    else
      Map3D::AssignBlockSides(_object3D[i]);
    
#ifdef DEBUG
    cout << "sym " << i << " of " << numSyms << endl
      << "  x = " << _object3D[i].pt.x_
	<< "  y = " << _object3D[i].pt.y_
	  << "  z = " << _object3D[i].pt.z_ << endl;
    for(int j = 0; j < BLOCK_VERTEX; j++) {
      cout << "  "
	<< _object3D[i].vt[j].x_ << "  "
	  << _object3D[i].vt[j].y_ << "  "
	    << _object3D[i].vt[j].z_ << endl;
    }
#endif
  }
  
  // get width and height of 3D display area
  int x, y, w, h;
  view->GetDataArea(x, y, w, h);
  
  // clip blocks
  Map3D::ClipBlocks(win, _object3D, numSyms, view->GetCamera(), w, h);
  
  // map blocks to points, segments, and planes and then draw them
  if (wireframe) {
    Map3D::MapBlockSegments(win, _object3D, numSyms,
			    view->GetCamera(), w, h);
    Map3D::DrawSegments(win);
  } else {
    Map3D::MapBlockPlanes(win, _object3D, numSyms,
			  view->GetCamera(), w, h);
    Map3D::DrawPlanes(win, solidFrame);
  }
}

//---------------------------------------------------------------------------


int FullMapping_RectXShape::NumShapeAttrs()
{
    return 2; 
}


void FullMapping_RectXShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					    int numSyms, TDataMap *map,
					    ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, false);
	    return;
	}
    }

    Coord x0, y0, x1, y1;
    win->Transform(0.0, 0.0, x0, y0);
    
    ComputeDataLabelFrame(view);

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	Coord tx, ty;
	Coord width, height;
	
	win->Transform(x, y, tx, ty);

	win->Transform(
	  fabs(GetSize(gdata, map, offset) * GetShapeAttr0(gdata, map, offset)),
	  0.0, x1, y1);
	width = x1 - x0;
	width = ABS(width);

	height = fabs(GetShapeAttr1(gdata, map, offset)) /
	  fabs(GetShapeAttr0(gdata, map, offset)) * width;
	height = ABS(height);

        width *= pixelSize;
	height *= pixelSize;

	width = MAX(width, pixelSize);
	height = MAX(height, pixelSize);

	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	win->FillPixelRect(tx, ty, width, height);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x, y, y);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}


//---------------------------------------------------------------------------


int FullMapping_BarShape::NumShapeAttrs()
{ 
    return 1; 
}


void FullMapping_BarShape::MaxSymSize(TDataMap *map, void *gdata, int numSyms,
					      Coord &width, Coord &height)
{
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


void FullMapping_BarShape::DrawGDataArray(WindowRep *win, void **gdataArray,
			    int numSyms, TDataMap *map,
			    ViewGraph *view, int pixelSize) 
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
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

    ComputeDataLabelFrame(view);

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	Coord width = fabs(GetSize(gdata, map, offset)
			   * GetShapeAttr0(gdata, map, offset));
        width *= pixelSize;
#if 0
	// experiment with 2-pixel wide bars to prevent white vertical
	// lines from showing up
	if (width < 2 * pixelWidth)
	  width = 2 * pixelWidth;
#endif
	if (width > pixelWidth)
	  x -= width / 2.0;

	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	win->FillRect(x, 0.0, width, y);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x + width / 2, y, y);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}


//---------------------------------------------------------------------------


int FullMapping_RegularPolygonShape::NumShapeAttrs()
{
    return 3; 
}


void FullMapping_RegularPolygonShape::DrawGDataArray(WindowRep *win,
						     void **gdataArray,
						     int numSyms,
						     TDataMap *map,
						     ViewGraph *view,
						     int pixelSize) 
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
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
	maxWidth *= pixelSize;
	maxHeight *= pixelSize;
	maxDepth *= pixelSize;

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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, false);
	    return;
	}
    }

    ComputeDataLabelFrame(view);

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord width = fabs(GetSize(gdata, map, offset)
			   * GetShapeAttr0(gdata, map, offset));
	width *= pixelSize;
	Coord height = fabs(GetSize(gdata, map, offset)
			    * GetShapeAttr1(gdata, map, offset));
	height *= pixelSize;
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
	
	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	win->FillPoly(points, segments);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x, y, y);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}


// -----------------------------------------------------------------


int FullMapping_OvalShape::NumShapeAttrs()
{
    return 2;
}


void FullMapping_OvalShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					   int numSyms, TDataMap *map,
					   ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, false);
	    return;
	}
    }

    ComputeDataLabelFrame(view);

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord width = fabs(GetSize(gdata, map, offset)
			   * GetShapeAttr0(gdata, map, offset));
        width *= pixelSize;
	Coord height = fabs(GetSize(gdata, map, offset)
			    * GetShapeAttr1(gdata, map, offset));
        height *= pixelSize;
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	win->Arc(x, y, width, height, 0, 2 * PI);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x, y, y);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}


// -----------------------------------------------------------------


int FullMapping_VectorShape::NumShapeAttrs()
{
    return 2;
}


void FullMapping_VectorShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					     int numSyms, TDataMap *map,
					     ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, false);
	    return;
	}
    }

    ComputeDataLabelFrame(view);

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord w = GetSize(gdata, map, offset)
	  * GetShapeAttr0(gdata, map, offset);
	w *= pixelSize;
	Coord h = GetSize(gdata, map, offset)
	  * GetShapeAttr1(gdata, map, offset);
	h *= pixelSize;
	Coord x = GetX(gdata, map, offset) - w / 2;
	Coord y = GetY(gdata, map, offset) - h / 2;
	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	// ksb: should we SetLineWidth() or use width param on Line()
	win->Line(x, y, x + w, y + h, GetLineWidth(gdata, map, offset));

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x + w / 2, y + h / 2, y + h / 2);

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


// -----------------------------------------------------------------


void FullMapping_HorLineShape::MaxSymSize(TDataMap *map, void *gdata, 
					  int numSyms,
					  Coord &width, Coord &height)
{
    width = 1e9;
    height = 0.0;
}
  

void FullMapping_HorLineShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					      int numSyms, TDataMap *map,
					      ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
    }

    ComputeDataLabelFrame(view);

    VisualFilter filter;
    view->GetVisualFilter(filter);
    Coord xLow = filter.xLow;
    Coord xHigh = filter.xHigh;

    GDataAttrOffset *offset = map->GetGDataOffset();

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord y = GetY(gdata, map, offset);
	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));

	win->Line(xLow, y, xHigh, y, GetLineWidth(gdata, map, offset));

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, (xLow + xHigh) / 2, y, y);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}


// -----------------------------------------------------------------


int FullMapping_SegmentShape::NumShapeAttrs()
{
    return 2;
}



void FullMapping_SegmentShape::MaxSymSize(TDataMap *map, void *gdata,
					  int numSyms,
					  Coord &width, Coord &height)
{
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
  

void FullMapping_SegmentShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					      int numSyms, TDataMap *map,
					      ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
    }

    ComputeDataLabelFrame(view);

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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, false);
	    return;
	}
    }

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord w = GetSize(gdata, map, offset)
	  * GetShapeAttr0(gdata, map, offset);
	w *= pixelSize;
	Coord h = GetSize(gdata, map, offset)
	  * GetShapeAttr1(gdata, map, offset);
	h *= pixelSize;
	Coord x = GetX(gdata, map, offset) - w / 2;
	Coord y = GetY(gdata, map, offset) - h / 2;
	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));

	win->Line(x, y, x + w, y + h, GetLineWidth(gdata, map, offset));

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x + w / 2, y + h / 2, y + h / 2);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}


void FullMapping_SegmentShape::Draw3DGDataArray(WindowRep *win,
                                                void **gdataArray,
                                                int numSyms, TDataMap *map,
                                                ViewGraph *view, int pixelSize)
{
    GDataAttrOffset *offset = map->GetGDataOffset();

    for(int i = 0; i < numSyms; i++) {
        char *gdata = (char *)gdataArray[i];
	
        Coord size = GetSize(gdata, map, offset);

        _object3D[i].pt.x_ = GetX(gdata, map, offset);
        _object3D[i].pt.y_ = GetY(gdata, map, offset);
        _object3D[i].pt.z_ = GetZ(gdata, map, offset);
        _object3D[i].W = size * GetShapeAttr0(gdata, map, offset);
        _object3D[i].H = size * GetShapeAttr1(gdata, map, offset);
        _object3D[i].D = size * GetShapeAttr2(gdata, map, offset);
        _object3D[i].segWidth = fabs(GetShapeAttr3(gdata, map, offset));
        _object3D[i].segWidth = MAX(_object3D[i].segWidth, 1);
        _object3D[i].color = GetColor(view, gdata, map, offset);

#ifdef DEBUG
        cout << "sym " << i << " of " << numSyms << endl
	  << "  x = " << _object3D[i].pt.x_
	  << "  y = " << _object3D[i].pt.y_
	  << "  z = " << _object3D[i].pt.z_ << endl;
#endif
    }

    // get width and height of 3D display area
    int x, y, w, h;
    view->GetDataArea(x, y, w, h);

    // clip, map, and draw segments
    Map3D::ClipLineSegments(win, _object3D, numSyms, view->GetCamera(), w, h);
    Map3D::MapLineSegments(win, _object3D, numSyms, view->GetCamera(), w, h);
    Map3D::DrawSegments(win);
}


// -----------------------------------------------------------------


int FullMapping_HighLowShape::NumShapeAttrs()
{
    return 3;
}


void FullMapping_HighLowShape::MaxSymSize(TDataMap *map, void *gdata,
					  int numSyms,
					  Coord &width, Coord &height)
{
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
  

void FullMapping_HighLowShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					      int numSyms, TDataMap *map,
					      ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
    }

    ComputeDataLabelFrame(view);

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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, false);
	    return;
	}
    }

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	Coord width = fabs(GetSize(gdata, map, offset)
			   * GetShapeAttr0(gdata, map, offset));
	width *= pixelSize;
	Coord high = GetShapeAttr1(gdata, map, offset);
	Coord low = GetShapeAttr2(gdata, map, offset);
	Coord tw = width / 20.0;
	Coord hw = width / 2.0;

	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	int line_width = GetLineWidth(gdata, map, offset);
	win->SetLineWidth(line_width);

	// ksb: should the width be applied to the rect, lines, or both?
	win->FillRect(x - tw, low, 2 * tw, high - low);
	win->Line(x - hw, y, x + hw, y, line_width);
	win->Line(x - hw, low, x + hw, low, line_width);
	win->Line(x - hw, high, x + hw, high, line_width);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x, y, y);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}

 
// -----------------------------------------------------------------


int FullMapping_PolylineShape::NumShapeAttrs()
{ 
    return MAX_GDATA_ATTRS;
}


void FullMapping_PolylineShape::MaxSymSize(TDataMap *map, void *gdata,
					   int numSyms,
					   Coord &width, Coord &height)
{
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
  

void FullMapping_PolylineShape::DrawGDataArray(WindowRep *win,
					       void **gdataArray,
					       int numSyms, TDataMap *map,
					       ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
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
	    // The requested size of the shape is less than or equal to the
	    // size of a screen pixel.
	    DrawPixelArray(win, gdataArray, numSyms, map, view,
                           pixelSize, false);
	    return;
	}
    }

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	GlobalColor color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	// LineWidth not currently compatible with PolyLine
	int width = 1;
	//int width = GetLineWidth(gdata, map, offset);
	//width *= pixelSize;
	//win->SetLineWidth(width);

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
	    win->Line(x, y, x1, y1, width);
	    x = x1;
	    y = y1;
	}

	if (color == XorColor)
	  win->SetCopyMode();
    }
}

 
// -----------------------------------------------------------------


int FullMapping_GifImageShape::NumShapeAttrs()
{
    return 3;
}


void FullMapping_GifImageShape::MaxSymSize(TDataMap *map, void *gdata,
					   int numSyms,
					   Coord &width, Coord &height)
{
    width = 0.0;
    height = 0.0;
}


void FullMapping_GifImageShape::DrawGDataArray(WindowRep *win,
					       void **gdataArray,
					       int numSyms, TDataMap *map,
					       ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
    const Boolean sendImageOnSocket = false;

    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
    }

    // Get default image filename.
    char defaultFile[MAXPATHLEN];
    char *directory = getenv("PWD");
    DOASSERT(directory != NULL, "Can't get current directory");
    sprintf(defaultFile, "%s/image.gif", directory);

    GDataAttrOffset *offset = map->GetGDataOffset();

    // first draw a cross mark at each GIF image location;
    // if there is a problem in displaying the GIF image,
    // then at least the user sees some symbol in the window

    for(int i = 0; i < numSyms; i++) {

	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	GlobalColor color = GetColor(view, gdata, map, offset);

	// Transform the data X and Y to pixels.
	Coord tx, ty;
	win->Transform(x, y, tx, ty);

#if 0
	// Draw a cross at the center of each image.  This is now disabled
	// because of using Tasvir with no subwindows.  RKW 12/18/96.
	win->PushTop();
	win->MakeIdentity();
	win->Line(tx - 3, ty, tx + 3, ty, 1);
	win->Line(tx, ty - 3, tx, ty + 3, 1);
	win->PopTransform();
#endif

	if (color == XorColor)
	  win->SetCopyMode();
	
	// Get the name of the image file or the image itself.
	char *shapeAttr0 = NULL;
	if (offset->shapeAttrOffset[0] >= 0) {
	    int key = (int)GetShapeAttr0(gdata, map, offset);
	    int code = StringStorage::Lookup(key, shapeAttr0);
#ifdef DEBUG
	    printf("Key %d returns \"%s\", code %d\n", key, shapeAttr0, code);
#endif
	} else {
#ifdef DEBUG
	    printf("Using default file \"%s\"\n", defaultFile);
#endif
	}


	// Now decide how to deal with it.
	char *file;
	int imageDataSize;
	char *image;
	Boolean dali;

        int imageType = int(GetShapeAttr1(gdata, map, offset) + 0.5);
	switch (imageType)
	{
	case IMAGE_TYPE_GIF_LOCAL:
#if defined(DEBUG)
          printf("GIF image local\n");
#endif
	  dali = false;
	  file = shapeAttr0 != NULL ? shapeAttr0 : defaultFile;
	  break;

	case IMAGE_TYPE_DALI_FILE:
#if defined(DEBUG)
          printf("Tasvir file, sending filename to Tasvir\n");
#endif
	  dali = true;
	  file = shapeAttr0 != NULL ? shapeAttr0 : defaultFile;
	  imageDataSize = 0;
	  image = NULL;
	  break;

	case IMAGE_TYPE_DALI_FILE_SEND:
#if defined(DEBUG)
          printf("Tasvir file, sending image to Tasvir\n");
#endif
	  dali = true;
	  file = shapeAttr0 != NULL ? shapeAttr0 : defaultFile;
          (void) ReadFile(file, imageDataSize, image);
	  file = "-";
	  break;

	case IMAGE_TYPE_DALI_IMAGE:
#if defined(DEBUG)
          printf("Tasvir image\n");
#endif
	  DOASSERT(shapeAttr0 != NULL, "Can't get image");
	  dali = true;
	  file = "-";
	  // Note: I'm not sure that using strlen() here is safe.  RKW
	  // 8/29/96.
	  imageDataSize = strlen(shapeAttr0);
	  image = shapeAttr0;
	  break;

	default:
	  reportError("Illegal image type", devNoSyserr);
	  return;
	  break;
	}

#ifdef DEBUG
	printf("Drawing image %s at %.2f,%.2f\n", file, tx, ty);
#endif

	if (Init::ImageDelay() != 0) sleep(Init::ImageDelay());

	// Display the image.
	if (dali)
	{
	  Coord size = GetSize(gdata, map, offset);
	  Coord x0, y0, x1, y1;
	  win->Transform(0.0, 0.0, x0, y0);
	  win->Transform(size, size, x1, y1);

	  Coord width = fabs(x1 - x0);
	  Coord height = fabs(y1 - y0);

          width *= pixelSize;
	  height *= pixelSize;

	  width = MAX(width, pixelSize);
	  height = MAX(height, pixelSize);

#if 0
	  // Enable this line to have the large rather than the smaller
	  // dimension control the image size.
	  width = height = MAX(width, height);
#endif

	  win->DaliShowImage(tx, ty, width, height, file, imageDataSize, image);
	}
	else
	{
	  win->ImportImage(tx, ty, GIF, file);
	}
    }
}

 
// -----------------------------------------------------------------


int FullMapping_PolylineFileShape::NumShapeAttrs()
{
    return 1;
}


void FullMapping_PolylineFileShape::MaxSymSize(TDataMap *map, void *gdata,
					       int numSyms,
					       Coord &width, Coord &height)
{
    width = 0;
    height = 0;
}


void FullMapping_PolylineFileShape::DrawGDataArray(WindowRep *win,
						   void **gdataArray,
						   int numSyms, TDataMap *map,
						   ViewGraph *view,
						   int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
    }

    GDataAttrOffset *offset = map->GetGDataOffset();

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	GlobalColor color = GetColor(view, gdata, map, offset);

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
	int width = GetLineWidth(gdata, map, offset);
	//width *= pixelSize;
	win->SetLineWidth(width);

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
		  win->Line(x0, y0, x1, y1, width);
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

 
// -----------------------------------------------------------------


int FullMapping_TextLabelShape::NumShapeAttrs()
{
    return 1;
}


void FullMapping_TextLabelShape::MaxSymSize(TDataMap *map, void *gdata, 
					    int numSyms,
					    Coord &width, Coord &height)
{
    width = 0;
    height = 0;
}


void FullMapping_TextLabelShape::DrawGDataArray(WindowRep *win,
						void **gdataArray,
						int numSyms, TDataMap *map,
						ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
    }

    VisualFilter filter;
    view->GetVisualFilter(filter);
    Coord filterWidth = filter.xHigh - filter.xLow;
    Coord filterHeight = filter.yHigh - filter.yLow;

    GDataAttrOffset *offset = map->GetGDataOffset();

    for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
        Coord pointSize = GetSize(gdata, map, offset);
        if (pointSize <= 1.0)
            pointSize = 12.0;
	GlobalColor color = GetColor(view, gdata, map, offset);

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
        win->SetFont("Helvetica", "Bold", "r", "Normal",
                     pointSize);

	// Pretend that there's a large box in which the text has to
	// be drawn; this is done because we don't know the size of the
	// the label in pixels, and if we pass a width or height that
	// is too tight, AbsoluteText() will try to scale the text.
	win->AbsoluteText(label, x - filterWidth / 2, y - filterHeight / 2,
			  filterWidth, filterHeight,
			  WindowRep::AlignCenter, true);

	if (color == XorColor)
	  win->SetCopyMode();
    }
}


// -----------------------------------------------------------------


void FullMapping_LineShape::MaxSymSize(TDataMap *map, void *gdata, int numSyms,
				       Coord &width, Coord &height)
{
    width = 0.0;
    height = 0.0;
}
  

void FullMapping_LineShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					   int numSyms, TDataMap *map,
					   ViewGraph *view, int pixelSize)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("  win = 0x%p; numSyms = %d\n", win, numSyms);
#endif

    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
	return;
    }

    ComputeDataLabelFrame(view);

    GDataAttrOffset *offset = map->GetGDataOffset();

    /* get coordinates of first data point in this batch */

    char *gdata = (char *)gdataArray[0];
    RecId recId = GetRecId(gdata, map, offset);
    Coord x0 = GetX(gdata, map, offset);
    Coord y0 = GetY(gdata, map, offset);
    GlobalColor c0 = GetColor(view, gdata, map, offset);

    // How should line width be handled for line types?
    int width = GetLineWidth(gdata, map, offset);

    /* draw line connecting last point of previous batch to
       first point of this batch */

    if (recId > 0) {
        Coord xp, yp;
        GlobalColor cp;
        if (view->GetPointStorage()->Find(recId - 1, xp, yp, cp)) {
            DrawConnectingLine(win, view,
                               GetPattern(gdata, map, offset), width,
                               xp, yp, cp, x0, y0, c0);
            if (view->GetDisplayDataValues())
              DisplayDataLabel(win, x0, y0, y0);
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
        GlobalColor color = GetColor(view, gdata, map, offset);
	width = GetLineWidth(gdata, map, offset);
        DrawConnectingLine(win, view,
                           GetPattern(gdata, map, offset), width,
                           x0, y0, c0, x, y, color);
        if (view->GetDisplayDataValues())
          DisplayDataLabel(win, x, y, y);
        x0 = x;
        y0 = y;
        c0 = color;
    }

    /* draw line connecting last point of this batch to
       first point of next batch */

    Coord xn, yn;
    GlobalColor cn;
    if (view->GetPointStorage()->Find(recId + numSyms, xn, yn, cn)) {
        DrawConnectingLine(win, view,
                           Pattern0, width, x0, y0, c0, xn, yn, cn);
        (void)view->GetPointStorage()->Remove(recId + numSyms);
    } else {
        view->GetPointStorage()->Insert(recId + numSyms - 1, x0, y0, c0);
    }
}


void FullMapping_LineShape::DrawConnectingLine(WindowRep *win, ViewGraph *view,
					   Pattern pattern, int line_width,
					   Coord x0, Coord y0, GlobalColor c0,
					   Coord x1, Coord y1, GlobalColor c1)
{
    win->SetPattern(pattern);
    if (c0 == XorColor)
      win->SetXorMode();
    else
      win->SetFgColor(c0);

    if (c0 == c1) {
	win->Line(x0, y0, x1, y1, line_width);
	if (c0 == XorColor)
	  win->SetCopyMode();
	return;
    }

    win->Line(x0, y0, (x0 + x1) / 2, (y0 + y1) / 2, line_width);
    if (c0 == XorColor)
      win->SetCopyMode();

    if (c1 == XorColor)
      win->SetXorMode();
    else
      win->SetFgColor(c1);
    win->Line((x0 + x1) / 2, (y0 + y1) / 2, x1, y1, line_width);
    if (c1 == XorColor)
      win->SetCopyMode();
}


// -----------------------------------------------------------------


void FullMapping_LineShadeShape::MaxSymSize(TDataMap *map, 
					    void *gdata, int numSyms,
					    Coord &width,
					    Coord &height)
{
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
  

void FullMapping_LineShadeShape::DrawConnectingLine(WindowRep *win,
						    ViewGraph *view,
						    Pattern pattern,
						    int line_width,
						    Coord x0, Coord y0,
						    GlobalColor c0,
						    Coord x1, Coord y1,
						    GlobalColor c1)
{
    /* clip top of shape with filter (filled polygons are
       really slow if y0 or y1 is far outside of the screen */
    VisualFilter filter;
    view->GetVisualFilter(filter);
    y0 = MIN(filter.yHigh, y0);
    y1 = MIN(filter.yHigh, y1);
    y0 = MAX(filter.yLow, y0);
    y1 = MAX(filter.yLow, y1);

    win->SetPattern(pattern);
    win->SetLineWidth(-1);	// no edge lines
    if (c0 == XorColor)
      win->SetXorMode();
    else
      win->SetFgColor(c0);

    Point points[4];

    if (c0 == c1) {
	if (y0 == y1) {
	    /* area is a rectangle -- optimize for speed */
	    win->FillRect(x0, 0, x1 - x0, y0);
	    if (c0 == XorColor)
	      win->SetCopyMode();
	} else {
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
	}
    } else {

	if (y0 == y1) {
	    /* area is a rectangle -- optimize for speed */
	    win->FillRect(x0, 0, (x1 - x0) / 2, y0);
	} else {
	    points[0].x = x0;
	    points[0].y = y0;
	    points[1].x = (x0 + x1) / 2;
	    points[1].y = (y0 + y1) / 2;
	    points[2].x = (x0 + x1) / 2;
	    points[2].y = 0;
	    points[3].x = x0;
	    points[3].y = 0;
	    win->FillPoly(points, 4);
	}
	if (c0 == XorColor)
	  win->SetCopyMode();

	if (c1 == XorColor)
	  win->SetXorMode();
	else
	  win->SetFgColor(c1);

	if (y0 == y1) {
	    /* area is a rectangle -- optimize for speed */
	    win->FillRect((x0 + x1) / 2, 0, (x1 - x0) / 2, y0);
	} else {
	    points[0].x = (x0 + x1) / 2;
	    points[0].y = (y0 + y1) / 2;
	    points[1].x = x1;
	    points[1].y = y1;
	    points[2].x = x1;
	    points[2].y = 0;
	    points[3].x = (x0 + x1) / 2;
	    points[3].y = 0;
	    win->FillPoly(points, 4);
	}
	if (c1 == XorColor)
	  win->SetCopyMode();

    }

    /* very slim filled polygons have the problem that depending
       on the fill_rule (see GC description in an Xlib manual)
       the interior might not be filled in all cases; to compensate
       for this, we draw a 1-pixel wide line underneath the
       polygon.

       The line is now variable width & placed on top of the fill so that
       you get a solid line to cap the line & the fill pattern below.
    */

    FullMapping_LineShape::DrawConnectingLine(win, view, Pattern0, line_width,
					      x0, y0, c0,
					      x1, y1, c1);
}


//---------------------------------------------------------------------------


