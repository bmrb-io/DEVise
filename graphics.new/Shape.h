/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.35  1999/05/26 19:50:54  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.34  1999/05/21 14:52:42  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.33  1999/05/20 15:17:55  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.32  1998/12/15 14:55:22  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.31  1997/11/24 23:15:16  weaver
  Changes for the new ColorManager.

  Revision 1.30  1997/11/18 23:27:01  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.29  1997/08/20 22:11:09  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.28.8.1  1997/08/07 16:56:42  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.28.6.1  1997/05/21 20:40:45  weaver
  Changes for new ColorManager

  Revision 1.28  1997/02/26 16:31:45  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.27  1997/02/14 16:47:47  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.26.4.2  1997/02/20 20:50:44  wenger
  Changed mapping to use ShapeAttr4 for line width.

  Revision 1.26.4.1  1997/02/09 20:14:11  wenger
  Fixed bug 147 (or at least some instances of it) -- found a bug in the
  query processor that caused it to miss records it should have found;
  fixed bugs 151 and 153.

  Revision 1.26  1996/11/23 21:05:46  jussi
  Made randomized square centered at X,Y.

  Revision 1.25  1996/11/21 01:24:39  jussi
  Changed shape of cloud from oval to rectangular.

  Revision 1.24  1996/11/20 01:08:37  jussi
  Changed threshold value for randomization from 0.5 to 0.15.

  Revision 1.23  1996/11/20 00:42:33  jussi
  Added support for X,Y randomization.

  Revision 1.22  1996/11/13 16:57:08  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.21  1996/09/06 07:00:13  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.20  1996/08/03 15:38:29  jussi
  Added GetShapeAttr3() function.

  Revision 1.19  1996/07/25 14:24:22  jussi
  Added capability to draw labels at (X,Y) data points.

  Revision 1.18  1996/07/19 02:52:33  jussi
  Added LineShape and LineShadeShape.

  Revision 1.17  1996/07/15 21:33:23  jussi
  Added support for the 'size' gdata parameter. Added GetSize()
  and GetOrientation() functions.

  Revision 1.16  1996/07/12 23:30:12  jussi
  Added missing semicolon.

  Revision 1.15  1996/07/12 23:12:36  jussi
  Fixed bug in DrawGDataArray().

  Revision 1.14  1996/07/02 22:46:00  jussi
  The bounding box of symbols is now correctly computed. Scatter
  plots sometimes did not have all necessary data displayed in
  them, as bounding box used to be incorrectly computed.

  Revision 1.13  1996/06/27 19:06:58  jussi
  Merged 3D block shape into 2D rect shape, the appropriate shape
  is chosen based on current view setting. Removed Block and 3DVector
  shapes. Added empty default 3D drawing routine to all other
  shapes.

  Revision 1.12  1996/04/15 16:08:29  jussi
  Added GetShape() method.

  Revision 1.11  1996/04/09 22:53:55  jussi
  Added View parameter to DrawGDataArray().

  Revision 1.10  1996/01/13 23:10:13  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.9  1996/01/09 22:08:33  jussi
  Added GetZ() function.

  Revision 1.8  1995/12/28 19:30:37  jussi
  Small fix to remove compiler warning.

  Revision 1.7  1995/12/22 18:06:25  jussi
  Symbol width and height are taken as an absolute value.

  Revision 1.6  1995/11/28 00:23:36  jussi
  Elevated a couple of #include statements from MapInterpShape.h
  to this file.

  Revision 1.5  1995/11/28 00:07:25  jussi
  Moved some common code from MapInterpShape.h to this file.
  BoundingBoxGData is by default computed based on the width
  and height of a symbol. Small symbols are drawn as single
  pixels, and for this purpose derived classes can use the
  DrawPixelArray method.

  Revision 1.4  1995/11/25  01:20:15  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.3  1995/11/21 23:23:38  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:15:35  jussi
  Added CVS header.
*/

#ifndef Shape_h
#define Shape_h

#include <stdio.h>

#include "TDataMap.h"
#include "WindowRep.h"
#include "ViewGraph.h"

#include "Color.h"

class WindowRep;
class TDataMap;

class Shape {
 public:
  /* Initialize shape every time a view is refreshed */
  virtual void Init() {}

  /* Return # of shape attributes needed for this shape */
  virtual int NumShapeAttrs() { return 0; }

  virtual Boolean BBIsVariable(GDataAttrOffset *offsets) {
    Boolean result = false;
    // This is the most common case...
    if (offsets->_sizeOffset >= 0 ||
        offsets->_shapeAttrOffset[0] >= 0 ||
        offsets->_shapeAttrOffset[1] >= 0) {
      result = true;
    }
    return result;
  }

  // Take converted GData records and plug in bounding box information.
  // The default is that ShapeAttr0 is width and ShapeAttr1 is height.
  virtual void FindBoundingBoxes(void *gdataArray, int numRecs,
      TDataMap *tdMap, Coord &maxWidth, Coord &maxHeight) {
#if defined(DEBUG)
    printf("Shape::FindBoundingBoxes(%d)\n", numRecs);
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
      Coord symWidth = symSize * tdMap->GetShapeAttr0(dataP);
      Coord symHeight = symSize * tdMap->GetShapeAttr1(dataP);

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

  /* Draw GData symbols. */
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize,
			      int &recordsProcessed,
			      Boolean timeoutAllowed) {}

 protected:

  /* By default, the shapes cannot display themselves in 3D */
  virtual void Draw3DGDataArray(WindowRep *win, void **gdataArray,
                                int numSyms, TDataMap *map,
                                ViewGraph *view, int pixelSize,
				int &recordsProcessed, Boolean timeoutAllowed)
				{ recordsProcessed = numSyms; }

  /* Draw each GData symbol as a single pixel. Used by derived classes
     as a common method in cases where symbols are smaller than one
     pixel. */
  virtual void DrawPixelArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize,
                              Boolean canRandomize) {
    GDataAttrOffset *offset = map->GetGDataOffset();
    int i = 0;
    Coord xArray[WINDOWREP_BATCH_SIZE], yArray[WINDOWREP_BATCH_SIZE];
    while (i < numSyms) {
      char *gdata = (char *)gdataArray[i];
      int count = 1;
      xArray[0] = map->GetX(gdata);
      yArray[0] = map->GetY(gdata);

      PColorID	fgid = map->GetPColorID(gdata);
      
      int colorIndex;
      for(colorIndex = i + 1; colorIndex < numSyms; colorIndex++) {
	char *colorGData = (char *)gdataArray[colorIndex];

	if (map->GetPColorID(colorGData) != fgid)
	  break;

	xArray[count] = map->GetX(colorGData);
	yArray[count] = map->GetY(colorGData);
        count++;
      }
      
      // Randomize X,Y coordinates if shape attribute 2 or 3 contains
      // a constant value of 0.15 or more.

      if (canRandomize && offset->_shapeAttrOffset[2] < 0
          && offset->_shapeAttrOffset[3] < 0) {
        const ShapeAttr *attrs = map->GetDefaultShapeAttrs();
        float cloudWidth = fabs(attrs[2]);
        float cloudHeight = fabs(attrs[3]);
        if (cloudWidth >= 0.15 || cloudHeight >= 0.15)
          RandomizePoints(xArray, yArray, count, cloudWidth, cloudHeight);
      }

      win->SetForeground(fgid);
      win->DrawPixelArray(xArray, yArray, count, pixelSize);
      
      i = colorIndex;
    }
  }

  virtual void ComputeDataLabelFrame(View *view) {
    VisualFilter filter;
    view->GetVisualFilter(filter);
    dataLabelWidth = filter.xHigh - filter.xLow;
    dataLabelHeight = filter.yHigh - filter.yLow;
    dataLabelYOffset = dataLabelHeight * 0.05;
  }

  virtual void DisplayDataLabel(WindowRep *win, Coord x,
                                Coord y, Coord value) {
    win->SetPattern(Pattern0);
    char label[32];
    sprintf(label, "%g", value);
    win->AbsoluteText(label, x - dataLabelWidth / 2,
                      y + dataLabelYOffset - dataLabelHeight / 2,
                      dataLabelWidth, dataLabelHeight,
                      WindowRep::AlignCenter, true);
  }

  virtual void RandomizePoints(Coord *x, Coord *y, int count,
                               Coord cloudWidth, Coord cloudHeight) {
    // Randomize points in X, Y, or both directions.
    for(int cnt = 0; cnt < count; cnt++) {
      if (cloudWidth == 0.0) {
        y[cnt] += ((rand() % 1000) / 1000.0 - 0.5) * cloudHeight;
      } else if (cloudHeight == 0.0) {
        x[cnt] += ((rand() % 1000) / 1000.0 - 0.5) * cloudWidth;
      } else {
#if defined(OVAL_CLOUD)
        float length = (rand() % 1000) / 1000.0 * cloudWidth;
        float dir = (rand() % 360) / 360.0 * 2 * 3.14;
        x[cnt] += length * sin(dir);
        y[cnt] += length * cos(dir) * cloudHeight / cloudWidth;
#else
        x[cnt] += ((rand() % 1000) / 1000.0 - 0.5) * cloudWidth;
        y[cnt] += ((rand() % 1000) / 1000.0 - 0.5) * cloudHeight;
#endif
      }
    }
  }

  Coord dataLabelWidth;
  Coord dataLabelHeight;
  Coord dataLabelYOffset;
};

#endif
