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
  Revision 1.41  1997/12/16 17:57:52  zhenhai
  Added OpenGL features.

  Revision 1.40  1997/11/24 23:15:08  weaver
  Changes for the new ColorManager.

  Revision 1.39  1997/11/18 23:26:54  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.38  1997/08/20 22:11:00  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.37.2.1  1997/08/07 16:56:35  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.37  1997/07/18 20:24:55  wenger
  Orientation now works on Rect and RectX symbols; code also includes
  some provisions for locating symbols other than at their centers.

  Revision 1.36.6.1  1997/05/21 20:40:38  weaver
  Changes for new ColorManager

  Revision 1.36  1997/04/29 17:35:11  wenger
  Minor fixes to new text labels; added fixed text label shape;
  CheckDirSpace() no longer prints an error message if it can't get disk
  status.

  Revision 1.35  1996/11/13 16:57:04  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.34  1996/09/06 07:00:11  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.33  1996/08/08 21:01:21  beyer
  Moved virtual functions from .h to .C file

  Revision 1.32  1996/07/25 14:24:21  jussi
  Added capability to draw labels at (X,Y) data points.

  Revision 1.31  1996/07/20 18:49:42  jussi
  Added 3D line segment shape. Improved performance of line shade shape.

  Revision 1.30  1996/07/19 14:37:46  jussi
  Removed extra code.

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

#include "Transform.h"
#include "Geom.h"
#include "RectShape.h"
//#include "StringStorage.h"

#include "Color.h"

class ViewGraph;

//#define DEBUG

// -----------------------------------------------------------------

class FullMapping_RectShape
: public RectShape
{
  public:
    
    virtual int NumShapeAttrs();
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
    
  protected:
    
    virtual void Draw3DGDataArray(WindowRep *win, void **gdataArray,
                                int numSyms, TDataMap *map,
				  ViewGraph *view, int pixelSize,
				  int &recordsProcessed);
};
 
// -----------------------------------------------------------------

class FullMapping_RectXShape
: public RectXShape 
{
  public:
    
    virtual int NumShapeAttrs();
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_BarShape
: public BarShape 
{
  public:

    virtual int NumShapeAttrs();
    
    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_RegularPolygonShape
: public RegularPolygonShape
{
  public:

    virtual int NumShapeAttrs();
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_OvalShape
: public OvalShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
    virtual void Draw3DGDataArray(WindowRep *win, void **gdataArray,
				  int numSyms, TDataMap *map,
				  ViewGraph *view, int pixelSize,
				  int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_VectorShape
: public VectorShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_HorLineShape
: public HorLineShape
{
  public:

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_SegmentShape
: public SegmentShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);

  protected:

    virtual void Draw3DGDataArray(WindowRep *win, void **gdataArray,
				  int numSyms, TDataMap *map,
				  ViewGraph *view, int pixelSize,
				  int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_HighLowShape
: public HighLowShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);

    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};
 
// -----------------------------------------------------------------

class FullMapping_PolylineShape
: public PolylineShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};
 
// -----------------------------------------------------------------

class FullMapping_GifImageShape
: public GifImageShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);

    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};
 
// -----------------------------------------------------------------

class FullMapping_PolylineFileShape
: public PolylineFileShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};
 
// -----------------------------------------------------------------

class FullMapping_TextLabelShape
: public TextLabelShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_FixedTextLabelShape
: public FixedTextLabelShape
{
  public:

    virtual int NumShapeAttrs();

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_LineShape
: public LineShape
{
  public:

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);

    virtual void DrawConnectingLine(WindowRep *win, ViewGraph *view,
				    Pattern pattern, int line_width,
				    Coord x0, Coord y0, PColorID c0,
				    Coord x1, Coord y1, PColorID c1);

  protected:
    virtual void Draw3DGDataArray(WindowRep *win, void **gdataArray,
				  int numSyms, TDataMap *map,
				  ViewGraph *view, int pixelSize,
				  int &recordsProcessed);
};

// -----------------------------------------------------------------

class FullMapping_LineShadeShape
: public FullMapping_LineShape
{
  public:

    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawConnectingLine(WindowRep *win, ViewGraph *view,
				    Pattern pattern, int line_width,
				    Coord x0, Coord y0, PColorID c0,
				    Coord x1, Coord y1, PColorID c1);
};

//******************************************************************************
#endif
