/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.57  1998/12/15 18:47:20  wenger
  New option in fixed text symbol: if size is <=1, it is assumed to be a
  fraction of the screen height, rather than the font size in points.

  Revision 1.56  1998/12/15 14:55:14  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.55  1998/11/06 17:59:49  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.54  1998/11/04 20:33:55  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.53  1998/06/23 17:50:19  wenger
  Fixed some compile warnings.

  Revision 1.52  1998/04/28 18:02:59  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.51  1998/04/16 21:51:36  wenger
  Committed Sanjay's text code.

  Revision 1.50  1998/04/13 22:26:29  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.49  1998/02/26 00:19:27  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.48  1998/01/08 19:46:06  wenger
  Added a little more debug code for text symbols.

  Revision 1.47  1997/12/19 19:43:19  wenger
  Fixed bug 250 (problems with colors.ds session) (actually three
  separate bugs).

  Revision 1.46  1997/12/16 17:57:50  zhenhai
  Added OpenGL features.

  Revision 1.45  1997/11/24 23:15:07  weaver
  Changes for the new ColorManager.

  Revision 1.44  1997/11/18 23:26:52  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.43  1997/08/20 22:10:59  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.42.2.3  1997/08/20 19:33:00  wenger
  Removed/disabled debug output for interruptible drawing.

  Revision 1.42.2.2  1997/08/15 23:06:28  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.42.2.1  1997/08/07 16:56:33  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.42  1997/07/18 20:24:53  wenger
  Orientation now works on Rect and RectX symbols; code also includes
  some provisions for locating symbols other than at their centers.

  Revision 1.41  1997/06/13 18:07:45  wenger
  Orientation is now working for text labels and fixed text labels.

  Revision 1.40.4.1  1997/05/21 20:40:37  weaver
  Changes for new ColorManager

  Revision 1.40  1997/05/06 17:29:03  wenger
  Tasvir image code gives warning if image filename or image is not a string.

  Revision 1.39  1997/04/29 17:35:10  wenger
  Minor fixes to new text labels; added fixed text label shape;
  CheckDirSpace() no longer prints an error message if it can't get disk
  status.

  Revision 1.38  1997/04/25 16:54:09  wenger
  Text labels are now sized in the same way as Rects; fixed font bug in
  middle button query popup; removed the dialog that warns you about your
  table name getting changed when you open an old data source.

  Revision 1.37  1997/04/16 18:53:35  wenger
  Text labels can now show non-string attributes; fixed a bug in constant
  strings in mapping (non-terminated string); added constant attributes to
  GData attribute list; commented out some debug code in mapping dialog.

  Revision 1.36  1997/04/14 17:45:17  beyer
  TextLabelShape now works with string constants in mapping.

  Revision 1.35  1997/04/11 18:49:14  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.34  1997/03/25 17:59:24  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.33  1997/03/19 19:41:47  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.32  1997/02/14 16:47:44  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.31.4.4  1997/03/07 20:04:15  wenger
  Tasvir images now work in PostScript output; Tasvir images now freed
  on a per-window basis; Tasvir timeout factor can be set on the command
  line; shared memory usage enabled by default.

  Revision 1.31.4.3  1997/02/11 23:32:19  weaver
  Changed vector shape drawing to have constant size arrowhead and beginning
  of vector at the GData center.

  Revision 1.31.4.2	 1997/02/11 15:18:10  wenger
  Fixed bug 154.

  Revision 1.31.4.1	 1997/02/09 20:14:08  wenger
  Fixed bug 147 (or at least some instances of it) -- found a bug in the
  query processor that caused it to miss records it should have found;
  fixed bugs 151 and 153.

  Revision 1.31	 1997/01/09 18:41:28  wenger
  Added workarounds for some Tasvir image bugs, added debug code.

  Revision 1.30	 1996/12/30 23:57:34  andyt
  First version with support for Embedded Tcl/Tk windows. Added new
  ETkWindow symbol shape. Improved the MappingInterp::MapGAttr2TAttr
  function to handle all GData attributes (used to only handle a subset).

  Revision 1.29	 1996/12/18 18:52:08  wenger
  Devise requests Tasvir not to use subwindows for its images; sizing of
  Tasvir images now basically works like a RectX, except that the aspect
  ratio is fixed by the image itself; fixed a bug in action of +/- keys
  for RectX shapes; Devise won't request Tasvir images smaller than two
  pixels (causes error in Tasvir).

  Revision 1.28	 1996/12/15 20:22:44  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.27	 1996/12/04 18:12:47  wenger
  Unimplemented methods in PSWindowRep now report an error but do not
  abort when called; fixed code in cslib server example that caused problems
  for Anand; corrected the values returned by some of the NumShapeAttrs()
  methods in MapInterpShape.

  Revision 1.26	 1996/12/03 17:03:04  jussi
  Added support for setting font size.

  Revision 1.25	 1996/11/26 16:51:33  ssl
  Added support for piled viws

  Revision 1.24	 1996/11/20 19:04:30  jussi
  Changed DrawPixelArray() call again.

  Revision 1.23	 1996/11/20 01:09:17  jussi
  Changed threshold value for randomization from 0.5 to 0.15.

  Revision 1.22	 1996/11/20 00:43:52  jussi
  Added support for X,Y randomization. Changed DrawPixelArray() call
  back so that +/- is used to control pixel size.

  Revision 1.21	 1996/11/13 16:57:04  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.20	 1996/11/07 22:40:28  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.19	 1996/09/13 16:44:57  wenger
  PixelSize in GData now used as multiplier to the size when symbols are
  larger than one screen pixel (makes the '+' and '-' keys more useful).

  Revision 1.18	 1996/09/12 18:38:06  wenger
  Added optional delay before drawing images.

  Revision 1.17	 1996/09/06 07:00:11  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
	negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.16	 1996/09/04 21:25:01  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.15	 1996/08/29 18:24:41  wenger
  A number of Dali-related improvements: ShapeAttr1 now specifies image
  type when shape is 'image'; added new '-bytes' flag to Dali commands
  when sending images; TDataBinaryInterp now uses StringStorage so GData
  can access strings; fixed hash function for StringStorage so having the
  high bit set in a byte in the string doesn't crash the hash table;
  improved the error checking in some of the Dali code.

  Revision 1.14	 1996/08/28 00:19:51  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.13	 1996/08/23 16:56:18  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.12	 1996/08/08 21:01:20  beyer
  Moved virtual functions from .h to .C file

  Revision 1.11	 1996/08/03 15:38:02  jussi
  Flag _solid3D now has three values. Added line segment width.

  Revision 1.10	 1996/07/20 18:49:41  jussi
  Added 3D line segment shape. Improved performance of line shade shape.

  Revision 1.9	1996/07/19 03:23:37	 jussi
  Added LineShape and LineShadeShape.

  Revision 1.8	1996/07/15 21:32:07	 jussi
  Added support for the 'size' gdata parameter.

  Revision 1.7	1996/07/10 19:00:40	 jussi
  Updated to reflect new transformation interface.

  Revision 1.6	1996/06/27 19:06:51	 jussi
  Merged 3D block shape into 2D rect shape, the appropriate shape
  is chosen based on current view setting. Removed Block and 3DVector
  shapes. Added empty default 3D drawing routine to all other
  shapes.

  Revision 1.5	1996/06/21 19:27:00	 jussi
  Moved computation of block vertices and edges to Map3D.C.
  Added support for drawing solid sides for blocks.

  Revision 1.4	1996/06/15 07:11:02	 yuc
  Small changes and clean-up DrawGDataArray, also add view and
  dimensions.

  Revision 1.3	1996/04/13 00:11:17	 jussi
  Added View * parameter to DrawGDataArray() and the calls to
  DrawPixelArray() and GetColor().

  Revision 1.2	1996/02/28 18:32:14	 yuc
  Added CVS header.
*/

#include <sys/param.h>
#include <iostream.h>

#include "ViewGraph.h"
#include "MapInterpShape.h"
#include "Init.h"
#include "Util.h"
#include "DevError.h"
#include "StringStorage.h"
#include "DrawTimer.h"
#include "Display.h"

//#define DEBUG
#define USE_TIMER 1

#define IMAGE_TYPE_GIF_LOCAL		(0)
#define IMAGE_TYPE_DALI_FILE		(1)
#define IMAGE_TYPE_DALI_FILE_SEND	(2)
#define IMAGE_TYPE_DALI_IMAGE		(3)

// Avoid having to change all of the calls to these functions here.
#define GetX ShapeGetX
#define GetY ShapeGetY

//---------------------------------------------------------------------------

int FullMapping_RectShape::NumShapeAttrs()
{
	return 2; 
}

void FullMapping_RectShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					   int numSyms, TDataMap *map,
					   ViewGraph *view, int pixelSize, int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	GDataAttrOffset *offset = map->GetGDataOffset();

	Coord x0, y0, x1, y1;
	win->Transform(0, 0, x0, y0);
	win->Transform(1, 1, x1, y1);
	Coord pixelWidth = 1 / fabs(x1 - x0);
	Coord pixelHeight = 1 / fabs(y1 - y0);

#if defined(PIXELOPTIMIZE)
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
		recordsProcessed = numSyms;
		return;
	}
	}
#endif

	ComputeDataLabelFrame(view);

	int			i = 0;

	// This while loop IS!! needed, because we break out of the for loop
	// below if color, pattern, or orientation is non-constant.
	while (i < numSyms) {

	PColorID	fgid = GetPColorID(defForeColor);
	Pattern		firstPattern = Pattern0;
	int			firstLineWidth = 0;
	Coord		firstOrientation = 0.0;
	int			count = 0;

	Coord xArray[WINDOWREP_BATCH_SIZE], yArray[WINDOWREP_BATCH_SIZE];
	Coord widthArray[WINDOWREP_BATCH_SIZE], heightArray[WINDOWREP_BATCH_SIZE];
	for(; i < numSyms; i++) {
		char *gdata = (char *)gdataArray[i];
		PColorID	pcid = GetPColorID(gdata, map, offset);
		Coord size = GetSize(gdata, map, offset);
		size *= pixelSize;
		Pattern pattern = GetPattern(gdata, map, offset);
	    Coord orientation = GetOrientation(gdata, map, offset);
		if (count > 0 && (pcid != fgid || pattern != firstPattern ||
			orientation != firstOrientation))
		  break;
		
		if (count == 0) {
		fgid = pcid;
		firstPattern = pattern;
		firstLineWidth = int(GetLineWidth(gdata, map, offset)+0.5);
		firstOrientation = orientation;
		}
		widthArray[count] = fabs(pixelSize * size
								 * GetShapeAttr0(gdata, map, offset));
		heightArray[count] = fabs(pixelSize * size
								  * GetShapeAttr1(gdata, map, offset));
		xArray[count] = GetX(gdata, map, offset);
		yArray[count] = GetY(gdata, map, offset);

		count++;
    }

		// Randomize X,Y coordinates if shape attribute 2 or 3 contains
		// a constant value of 0.15 or more.

		if (offset->shapeAttrOffset[2] < 0 && offset->shapeAttrOffset[3] < 0)
		{
			ShapeAttr*	attrs = map->GetDefaultShapeAttrs();
			float		cloudWidth = fabs(attrs[2]);
			float		cloudHeight = fabs(attrs[3]);

			if (cloudWidth >= 0.15 || cloudHeight >= 0.15)
				RandomizePoints(xArray, yArray, count, cloudWidth, cloudHeight);
		}

		win->SetForeground(fgid);
		win->SetPattern(firstPattern);
		win->SetLineWidth(firstLineWidth);

	win->FillRectArray(xArray, yArray, widthArray, heightArray, count, WindowRep::AlignCenter,
	    firstOrientation);

	if (view->GetDisplayDataValues()) {
		for(int s = 0; s < count; s++)
		  DisplayDataLabel(win, xArray[s] + widthArray[s] / 2,
				   yArray[s] + heightArray[s] / 2,
				   yArray[s] + heightArray[2] / 2);
	}
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
}
		 
void FullMapping_RectShape::Draw3DGDataArray(WindowRep *win,
											 void **gdataArray,
											 int numSyms, TDataMap *map,
											 ViewGraph *view, int pixelSize,
											 int &recordsProcessed)
{
  fprintf(stderr,
    "Warning: 3D display of Rect shapes not currently implemented\n");

  recordsProcessed = numSyms;
}

//---------------------------------------------------------------------------

int FullMapping_RectXShape::NumShapeAttrs()
{
	return 2; 
}

void FullMapping_RectXShape::DrawGDataArray(WindowRep *win, void **gdataArray,
						int numSyms, TDataMap *map,
						ViewGraph *view, int pixelSize, int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif

	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	GDataAttrOffset *offset = map->GetGDataOffset();

#if defined(PIXELOPTIMIZE)
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
		recordsProcessed = numSyms;
		return;
	}
	}
#endif

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

	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	Coord orientation = GetOrientation(gdata, map, offset);

	win->FillPixelRect(tx, ty, width, height, 1.0, 1.0,
	  WindowRep::AlignCenter, orientation);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x, y, y);
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
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
				ViewGraph *view, int pixelSize, int &recordsProcessed) 
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
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

	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	win->FillRect(x, 0.0, width, y);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x + width / 2, y, y);
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
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
							 int pixelSize, int &recordsProcessed) 
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	GDataAttrOffset *offset = map->GetGDataOffset();

#if defined(PIXELOPTIMIZE)
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
	printf("					 pixelW %.2f, pixelH %.2f\n",
		   pixelWidth, pixelHeight);
#endif

	if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
		// The requested size of the shape is less than or equal to the
		// size of a screen pixel.
		DrawPixelArray(win, gdataArray, numSyms, map, view,
						   pixelSize, false);
		recordsProcessed = numSyms;
		return;
	}
	}
#endif

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
	
	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	win->FillPoly(points, segments);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x, y, y);
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
}


// -----------------------------------------------------------------


int FullMapping_OvalShape::NumShapeAttrs()
{
	return 2;
}


void FullMapping_OvalShape::DrawGDataArray(WindowRep *win, void **gdataArray,
					   int numSyms, TDataMap *map,
					   ViewGraph *view, int pixelSize, int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	GDataAttrOffset *offset = map->GetGDataOffset();

#if defined(PIXELOPTIMIZE)
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
		recordsProcessed = numSyms;
		return;
	}
	}

#endif

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

	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	win->SetLineWidth(GetLineWidth(gdata, map, offset));

	win->Arc(x, y, width, height, 0, 2 * PI);

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x, y, y);
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
}

void FullMapping_OvalShape::Draw3DGDataArray(WindowRep *win, void **gdataArray,
					   int numSyms, TDataMap *map,
					   ViewGraph *view, int pixelSize,
					   int &recordsProcessed)
{
  GDataAttrOffset *offset = map->GetGDataOffset();
  Coord x, y, z, r;
  // 0 = wireframe only, 1 = solid, 2 = solid + wireframe
  Boolean wireframe = (view->GetSolid3D() == 0);
  Boolean solidFrame = (view->GetSolid3D() == 2);
  
  for(int i = 0; i < numSyms; i++) {
    char *gdata = (char *)gdataArray[i];
    
    Coord size = GetSize(gdata, map, offset);
    
    x=GetX(gdata, map, offset);
    y=GetY(gdata, map, offset);
    z=GetZ(gdata, map, offset);
    r=size * GetShapeAttr0(gdata, map, offset)/2.0;
    PColorID colorid=GetPColorID(gdata, map, offset);
    win->SetForeground(colorid);
    win->FillSphere(x, y, z, r);
  }
  recordsProcessed = numSyms;
}
// -----------------------------------------------------------------


int FullMapping_VectorShape::NumShapeAttrs()
{
	return 2;
}


void FullMapping_VectorShape::DrawGDataArray(WindowRep *win, void **gdataArray,
						 int numSyms, TDataMap *map,
						 ViewGraph *view, int pixelSize, int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	GDataAttrOffset *offset = map->GetGDataOffset();

	Coord x0, y0, x1, y1;
	win->Transform(0, 0, x0, y0);
	win->Transform(1, 1, x1, y1);
	Coord pixelWidth = 1 / fabs(x1 - x0);
	Coord pixelHeight = 1 / fabs(y1 - y0);

#if defined(PIXELOPTIMIZE)
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
		recordsProcessed = numSyms;
		return;
	}
	}
#endif

	ComputeDataLabelFrame(view);

	for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord w = GetSize(gdata, map, offset)
	  * GetShapeAttr0(gdata, map, offset);
	w *= pixelSize;
	Coord h = GetSize(gdata, map, offset)
	  * GetShapeAttr1(gdata, map, offset);
	h *= pixelSize;

// CEW: Start vector at shape center instead of centering it there	
//	Coord x = GetX(gdata, map, offset) - w / 2;
//	Coord y = GetY(gdata, map, offset) - h / 2;
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);

	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	// ksb: should we SetLineWidth() or use width param on Line()
	win->Line(x, y, x + w, y + h, GetLineWidth(gdata, map, offset));

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x + w / 2, y + h / 2, y + h / 2);

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
// CEW: Constant, minimal size arrowheads (change later if needed)
//	if (arrowSize < 10)
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

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
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
						  ViewGraph *view, int pixelSize, int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
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

	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	win->Line(xLow, y, xHigh, y, GetLineWidth(gdata, map, offset));

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, (xLow + xHigh) / 2, y, y);
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
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
						  ViewGraph *view, int pixelSize, int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	ComputeDataLabelFrame(view);

	GDataAttrOffset *offset = map->GetGDataOffset();

	Coord x0, y0, x1, y1;
	win->Transform(0, 0, x0, y0);
	win->Transform(1, 1, x1, y1);
	Coord pixelWidth = 1 / fabs(x1 - x0);
	Coord pixelHeight = 1 / fabs(y1 - y0);

#if defined(PIXELOPTIMIZE)
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
		recordsProcessed = numSyms;
		return;
	}
	}
#endif

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

	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	win->Line(x, y, x + w, y + h, GetLineWidth(gdata, map, offset));

	if (view->GetDisplayDataValues())
	  DisplayDataLabel(win, x + w / 2, y + h / 2, y + h / 2);
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
}


void FullMapping_SegmentShape::
Draw3DGDataArray(WindowRep *win, void **gdataArray,
		 int numSyms, TDataMap *map,
		 ViewGraph *view, int pixelSize,
		 int &recordsProcessed)
{
	GDataAttrOffset *offset = map->GetGDataOffset();
	
	for(int i = 0; i < numSyms; i++) {
	  char *gdata = (char *)gdataArray[i];
	  
	  Coord size = GetSize(gdata, map, offset);
	  
	  Coord x = GetX(gdata, map, offset);
	  Coord y = GetY(gdata, map, offset);
	  Coord z = GetZ(gdata, map, offset);
	  Coord w = size * GetShapeAttr0(gdata, map, offset);
	  Coord h = size * GetShapeAttr1(gdata, map, offset);
	  Coord d = size * GetShapeAttr2(gdata, map, offset);
	  Coord width = fabs(GetShapeAttr3(gdata, map, offset));
	  width = MAX(width, 1);
	  win->SetForeground(GetPColorID(gdata, map, offset));
	  win->Line3D(x-w/2.0,y-h/2.0,z-d/2.0,
		      x+w/2.0,y+h/2.0,z+d/2.0,
		      width);
	}
	recordsProcessed = numSyms;
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
						  ViewGraph *view, int pixelSize,
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

	ComputeDataLabelFrame(view);

	GDataAttrOffset *offset = map->GetGDataOffset();
	
#if defined(PIXELOPTIMIZE)
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
		recordsProcessed = numSyms;
		return;
	}
	}
#endif

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

	win->SetForeground(GetPColorID(gdata, map, offset));
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
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
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
						   ViewGraph *view, int pixelSize,
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

	GDataAttrOffset *offset = map->GetGDataOffset();

#if defined(PIXELOPTIMIZE)
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
		recordsProcessed = numSyms;
		return;
	}
	}
#endif

	for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	win->SetForeground(GetPColorID(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	// LineWidth not currently compatible with PolyLine
	int width = 1;
	//int width = GetLineWidth(gdata, map, offset);
	//width *= pixelSize;
	//win->SetLineWidth(width);

	win->DrawPixel(x, y);

	int npOff = offset->shapeAttrOffset[0];
	if (npOff < 0)
		continue;

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
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
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
					       ViewGraph *view, int pixelSize,
						   int &recordsProcessed)
{
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif

    const Boolean sendImageOnSocket = false;
    
    if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
    }
    
    // Get default image filename.
    char defaultFile[MAXPATHLEN];
    char *directory = getenv("PWD");
    DOASSERT(directory != NULL, "Can't get current directory");
    sprintf(defaultFile, "%s/image.gif", directory);
    
    GDataAttrOffset *offset = map->GetGDataOffset();
	StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);
    
    // first draw a cross mark at each GIF image location;
    // if there is a problem in displaying the GIF image,
    // then at least the user sees some symbol in the window
    
    for (int i = 0; i < numSyms; i++) {
#if USE_TIMER
	  // Always draw at least one symbol so we are sure to make progress.
	  if ((i > 0) && DrawTimer::Expired()) {
#if defined(DEBUG)
        printf("Draw timed out\n");
#endif
		win->SetPattern(Pattern0);
		recordsProcessed = i;
		return;
	  }
#endif
	
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);
	
	// Transform the data X and Y to pixels.
	Coord tx, ty;
	win->Transform(x, y, tx, ty);
	
#if 0
	// Draw a cross at the center of each image.  This is now disabled
	// because of using Tasvir with no subwindows.	RKW 12/18/96.
	win->PushTop();
	win->MakeIdentity();
	win->Line(tx - 3, ty, tx + 3, ty, 1);
	win->Line(tx, ty - 3, tx, ty + 3, 1);
	win->PopTransform();
#endif
	
	// Get the name of the image file or the image itself.  (Print a warning
	// if this isn't a string attribute.)
    AttrList *attrList = map->GDataAttrList();
    AttrInfo *attrInfo = attrList->Find("shapeAttr_0");
    if (attrInfo == NULL) {
#if defined(DEBUG)
      reportErrNosys("Can't find AttrInfo for shapeAttr_0");
#endif
    } else {
	  if (attrInfo->type != StringAttr) {
		printf("Warning: image or image filename attribute is not a string attribute\n");
	  }
    }

	char *shapeAttr0 = NULL;
	if (offset->shapeAttrOffset[0] >= 0) {
	    int key = (int)GetShapeAttr0(gdata, map, offset);
	    int code = stringTable->Lookup(key, shapeAttr0);
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
		recordsProcessed = numSyms;
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
	    
	    // Increase Dali/Tasvir timeout by a factor of 10 if we're drawing
	    // a view that's in piled mode, because it's more important in
	    // this case to not time out so that the actual drawing of the
	    // views occurs in the correct order.
	    float timeoutFactor = 1.0;
	    View *view2 = View::FindViewByName(view->GetName());
	    if (view2->IsInPileMode()) timeoutFactor = 10.0;

	    Boolean maintainAspect = (Boolean) GetShapeAttr2(gdata, map,
	      offset);

	    win->DaliShowImage(tx, ty, width, height, file,
			       imageDataSize, image, timeoutFactor,
			       maintainAspect);
	}
	else
	{
	    reportError("Illegal image type", devNoSyserr);
	}
    }

	recordsProcessed = numSyms;
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
						   int pixelSize, int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
#endif
	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	GDataAttrOffset *offset = map->GetGDataOffset();
	StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);

	for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord x = GetX(gdata, map, offset);
	Coord y = GetY(gdata, map, offset);

	char *file = "polyline.dat";
	char *format = "%lf%lf";

	if (offset->shapeAttrOffset[0] >= 0) {
		int key = (int)GetShapeAttr0(gdata, map, offset);
		int code = stringTable->Lookup(key, file);
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
		int code = stringTable->Lookup(key, format);
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

	win->SetForeground(GetPColorID(gdata, map, offset));
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
	}

	win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
}

 
// -----------------------------------------------------------------


int FullMapping_TextLabelShape::NumShapeAttrs()
{
	return 4;
}


void FullMapping_TextLabelShape::MaxSymSize(TDataMap *map, void *gdata, 
						int numSyms,
						Coord &width, Coord &height)
{
	width = 0;
	height = 0;
}

static void
GetTextAttrInfo(AttrList *attrList,
    Boolean &labelAttrValid, AttrType &labelAttrType,
	Boolean &labelFormatValid, AttrType &labelFormatType)
{
  labelAttrType = IntAttr;
  AttrInfo *attrInfo = attrList->Find("shapeAttr_0");
  if (attrInfo == NULL) {
    labelAttrValid = false;
#if defined(DEBUG)
    reportErrNosys("Can't find AttrInfo for shapeAttr_0");
#endif
  } else {
    labelAttrValid = true;
    labelAttrType = attrInfo->type;
  }

  labelFormatType = IntAttr;
  attrInfo = attrList->Find("shapeAttr_1");
  if (attrInfo == NULL) {
    labelFormatValid = false;
#if defined(DEBUG)
    reportErrNosys("Can't find AttrInfo for shapeAttr_1");
#endif
  } else {
    labelFormatValid = true;
    labelFormatType = attrInfo->type;
  }
}

static char *
GetLabelText(char *gdata, TDataMap *map, 
    Boolean labelAttrValid, AttrType labelAttrType,
	Boolean labelFormatValid, AttrType labelFormatType, char labelBuf[])
{
    GDataAttrOffset *offset = map->GetGDataOffset();
    StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);

    char *label;

    if (!labelAttrValid) {
      label = "X";
    } else if (labelAttrType == StringAttr) {
      /* Label attribute is a string.  Get the string value from the
       * StringStorage class. */
      int key = (int) GetShapeAttr0(gdata, map, offset);
      int code = stringTable->Lookup(key, label);
      if( code < 0 ) {        // key not found
        label = "X";
#if defined(DEBUG)
        printf("Using default label \"%s\"\n", label);
#endif
      } else {
#if defined(DEBUG)
        printf("Key %d returns \"%s\", code %d\n", key, label, code);
#endif
      }
    } else if (labelAttrType == DateAttr) {
      /* Label attribute is a date.  Convert the UNIX date (stored as a
       * double in the GData) to a date string. */
      label = DateString((time_t) GetShapeAttr0(gdata, map, offset));
    } else {
      /* Label attribute is a numerical value (stored as a double in the
       * GData).  Print it with the appropriate format string. */
      char *labelFormat = NULL;
      if (labelFormatValid && (labelFormatType == StringAttr)) {
        int labelKey = (int) GetShapeAttr1(gdata, map, offset);
        int labelCode = stringTable->Lookup(labelKey, labelFormat);
        if (labelCode < 0) {	// key not found
          labelFormat = NULL;
        }
      }

      if (labelFormat == NULL) labelFormat = "%g";
      sprintf(labelBuf, labelFormat, GetShapeAttr0(gdata, map, offset));
      label = labelBuf;
    }

	return label;
}

static Boolean // true if okay
GetAlignment(ViewGraph *view, WindowRep::SymbolAlignment &alignment)
{
    int align_num = view->GetAlign();
    
    Boolean okay = true;

    switch (align_num){
    case 0:
       alignment = WindowRep::AlignNorthWest;
       break;
  
     case 1:
       alignment = WindowRep::AlignWest;
       break;
   
    case 2:
       alignment = WindowRep::AlignSouthWest;
       break;
   
    case 3:
       alignment = WindowRep::AlignNorth;
       break;

    case 4:
       alignment = WindowRep::AlignCenter;
       break;

    case 5:
       alignment = WindowRep::AlignSouth;
       break;

    case 6:
       alignment = WindowRep::AlignNorthEast;
       break;

    case 7:
       alignment = WindowRep::AlignEast;
       break;

    case 8:
       alignment = WindowRep::AlignSouthEast;
       break;

    default:
       okay = false;
       break;
    }

	return okay;
}

void FullMapping_TextLabelShape::DrawGDataArray(WindowRep *win,
						void **gdataArray,
						int numSyms, TDataMap *map,
						ViewGraph *view, int pixelSize,
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

  /* Figure out the type of the label and label format attribute. */
  AttrList *attrList = map->GDataAttrList();
#if defined(DEBUG)
  attrList->Print();
#endif

  Boolean labelAttrValid, labelFormatValid;
  AttrType labelAttrType, labelFormatType;
  GetTextAttrInfo(attrList, labelAttrValid, labelAttrType,
	labelFormatValid, labelFormatType);

  GDataAttrOffset *offset = map->GetGDataOffset();
  StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);

  Coord oldPointSize = -9999.9;

  for(int i = 0; i < numSyms; i++) {
#if USE_TIMER
	  // Always draw at least one symbol so we are sure to make progress.
	  if ((i > 0) && DrawTimer::Expired()) {
#if defined(DEBUG)
        printf("Draw timed out\n");
#endif
		win->SetPattern(Pattern0);
		recordsProcessed = i;
		return;
	  }
#endif

    char *gdata = (char *)gdataArray[i];
    Coord x = GetX(gdata, map, offset);
    Coord y = GetY(gdata, map, offset);

	Coord size = GetSize(gdata, map, offset);
	size *= pixelSize;

	Coord width = fabs(size * GetShapeAttr2(gdata, map, offset));
	Coord height = fabs(size * GetShapeAttr3(gdata, map, offset));

	Coord orientation = GetOrientation(gdata, map, offset);

	/* Figure out a reasonable font size to use so that the amount of scaling
	 * is as small as possible.  I'm sure this isn't perfect, but it seems
	 * fairly close. RKW 4/25/97. */
	Coord x0, y0, x1, y1;
	win->Transform(0, 0, x0, y0);
	win->Transform(1, 1, x1, y1);
	Coord pointSize = height * fabs(y1 - y0);


	pointSize = MIN(pointSize, 16.0);
	pointSize = (Coord) ((int) (pointSize + 0.5));

    /* Find or generate the label string. */
    char labelBuf[128];
    char *label = GetLabelText(gdata, map, labelAttrValid, labelAttrType,
	  labelFormatValid, labelFormatType, labelBuf);


    win->SetForeground(GetPColorID(gdata, map, offset));
    win->SetPattern(GetPattern(gdata, map, offset));
    if (pointSize != oldPointSize) {
      win->SetFont("Courier", "Bold", "r", "Normal", pointSize);
      oldPointSize = pointSize;
    }

#if defined(DEBUG)
    printf("Text label: <%s>\n", label);
#endif
    
    WindowRep::SymbolAlignment alignment;

    if (GetAlignment(view, alignment)) {
      if  ((Boolean)GetShapeAttr4(gdata, map, offset)) {
        Pattern oldPattern = win->GetPattern();
        win->SetPattern((Pattern)-1);
        win->FillRectAlign(x, y, width, height, alignment, orientation);
        win->SetPattern(oldPattern);
      }
      win->ScaledDataText(label, x , y , width, height, alignment, true,
        orientation); 
#if 0 // For testing ScaledText() vs. ScaledDataText().
      win->SetForeground(GetPColorID(gdata, map, offset) + 5);
      win->ScaledText(label, x , y , width, height, alignment, true,
        orientation); 
      win->SetForeground(GetPColorID(gdata, map, offset));
#endif
     }
   }

   win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------


int FullMapping_TextDataLabelShape::NumShapeAttrs()
{
	return 4;
}


void FullMapping_TextDataLabelShape::MaxSymSize(TDataMap *map, void *gdata, 
						int numSyms,
						Coord &width, Coord &height)
{
	width = 0;
	height = 0;
}


/* this function is not much different from the scaled text version 
   for drawing variable sized text symbols..
   the difference is in the choice of the points for displaying the
   text -- while the previous version is more sort of a trial and 
   error .. chooses one pointsize and then tries to find if it can
   be scaled to fit the defining box, this method figures out a 
   reasonable point size by carrying out the pixel-> point conversion
   based on the display sizes and resolution of the screen and
   gets rid of width/height parameters in the display */

void FullMapping_TextDataLabelShape::DrawGDataArray(WindowRep *win,
						void **gdataArray,
						int numSyms, TDataMap *map,
						ViewGraph *view, int pixelSize,
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

  /* Figure out the type of the label and label format attribute. */
  AttrList *attrList = map->GDataAttrList();
#if defined(DEBUG)
  attrList->Print();
#endif

  Boolean labelAttrValid, labelFormatValid;
  AttrType labelAttrType, labelFormatType;
  GetTextAttrInfo(attrList, labelAttrValid, labelAttrType,
	labelFormatValid, labelFormatType);

  GDataAttrOffset *offset = map->GetGDataOffset();
  StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);

  Coord oldpixelperUnit = -9999.9;

  for(int i = 0; i < numSyms; i++) {
#if USE_TIMER
	  // Always draw at least one symbol so we are sure to make progress.
	  if ((i > 0) && DrawTimer::Expired()) {
#if defined(DEBUG)
        printf("Draw timed out\n");
#endif
		win->SetPattern(Pattern0);
		recordsProcessed = i;
		return;
	  }
#endif

    char *gdata = (char *)gdataArray[i];
    Coord x = GetX(gdata, map, offset);
    Coord y = GetY(gdata, map, offset);

	Coord size = GetSize(gdata, map, offset);
	size *= pixelSize;

	Coord width ;
	Coord height;

	Coord orientation = GetOrientation(gdata, map, offset);

	/* Figure out a reasonable font size to use so that the amount of scaling
	 * is as small as possible.  I'm sure this isn't perfect, but it seems
	 * fairly close. RKW 4/25/97. */
	Coord x0, y0, x1, y1;
	win->Transform(0, 0, x0, y0);
	win->Transform(1, 1, x1, y1);
	Coord pointSize =  fabs(y1 - y0);
	
    Coord pixelperUnitHeight     = fabs(y1 - y0);
	Coord pixelperUnitWidth      = fabs(x1 - x0);

#if defined(DEBUG)
        cout << "pixelper Unit (Height) = " << pixelperUnitHeight <<endl;
	cout << "pixelper Unit (Width) = " << pixelperUnitWidth <<endl;
#endif

    Coord pixelperUnit = size * (MIN(pixelperUnitWidth,pixelperUnitHeight));
	pixelperUnit  = (Coord) ((int) (pixelperUnit + 0.5));

    /* Find or generate the label string. */
    char labelBuf[128];
    char *label = GetLabelText(gdata, map, labelAttrValid, labelAttrType,
	  labelFormatValid, labelFormatType, labelBuf);


    win->SetForeground(GetPColorID(gdata, map, offset));
    win->SetPattern(GetPattern(gdata, map, offset));
    if (pixelperUnit != oldpixelperUnit) {
      win->SetFont("Courier", "Bold", "r", "Normal", pixelperUnit);
      oldpixelperUnit = pixelperUnit ;
    }

#if defined(DEBUG)
    printf("Text label: <%s>\n", label);
#endif

    height = pixelperUnitHeight * size;
    width  = strlen(label);

    WindowRep::SymbolAlignment alignment;

    if (GetAlignment(view, alignment)) {
      win->ScaledDataText(label, x , y , width, height,
                          alignment, true, orientation); 
    }
   }

   win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
}

// -----------------------------------------------------------------

// -----------------------------------------------------------------


int FullMapping_FixedTextLabelShape::NumShapeAttrs()
{
	return 2;
}


void FullMapping_FixedTextLabelShape::MaxSymSize(TDataMap *map, void *gdata, 
						int numSyms,
						Coord &width, Coord &height)
{
	width = 0;
	height = 0;
}


void FullMapping_FixedTextLabelShape::DrawGDataArray(WindowRep *win,
						void **gdataArray,
						int numSyms, TDataMap *map,
						ViewGraph *view, int pixelSize,
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

  /* Figure out the type of the label and label format attribute. */
  AttrList *attrList = map->GDataAttrList();

  Boolean labelAttrValid, labelFormatValid;
  AttrType labelAttrType, labelFormatType;
  GetTextAttrInfo(attrList, labelAttrValid, labelAttrType,
	labelFormatValid, labelFormatType);

  VisualFilter filter;
  view->GetVisualFilter(filter);
  Coord filterWidth = filter.xHigh - filter.xLow;
  Coord filterHeight = filter.yHigh - filter.yLow;

  GDataAttrOffset *offset = map->GetGDataOffset();
  StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);

  Coord oldPointSize = -9999.9;

  for(int i = 0; i < numSyms; i++) {
#if USE_TIMER
	  // Always draw at least one symbol so we are sure to make progress.
	  if ((i > 0) && DrawTimer::Expired()) {
#if defined(DEBUG)
        printf("Draw timed out\n");
#endif
		win->SetPattern(Pattern0);
		recordsProcessed = i;
		return;
	  }
#endif

    char *gdata = (char *)gdataArray[i];
    Coord x = GetX(gdata, map, offset);
    Coord y = GetY(gdata, map, offset);
    Coord pointSize = GetSize(gdata, map, offset);
    if (pointSize <= 1.0) {
	  // Size of font is defined as a fraction of the screen height.
      Coord displayWidth, displayHeight;
	  DeviseDisplay::DefaultDisplay()->Dimensions(displayWidth, displayHeight);
	  pointSize = pointSize * displayHeight *
		DeviseDisplay::DefaultDisplay()->PointsPerPixel();
	}
	Coord orientation = GetOrientation(gdata, map, offset);

    /* Find or generate the label string. */
    char labelBuf[128];
    char *label = GetLabelText(gdata, map, labelAttrValid, labelAttrType,
	  labelFormatValid, labelFormatType, labelBuf);


	win->SetForeground(GetPColorID(gdata, map, offset));
    win->SetPattern(GetPattern(gdata, map, offset));
    if (pointSize != oldPointSize) {
      win->SetFont("Helvetica", "Bold", "r", "Normal", pointSize);
      oldPointSize = pointSize;
    }



    WindowRep::SymbolAlignment alignment;

    if (GetAlignment(view, alignment)) {
      // Pretend that there's a large box in which the text has to
      // be drawn; this is done because we don't know the size of the
      // the label in pixels, and if we pass a width or height that
      // is too tight, AbsoluteText() will try to scale the text.
      win->AbsoluteDataText(label, x , y , filterWidth, filterHeight,
                            alignment, true, orientation); 
    }
  }

  win->SetPattern(Pattern0);

	recordsProcessed = numSyms;
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
					   ViewGraph *view, int pixelSize,
					   int &recordsProcessed)
{
#if defined(DEBUG)
	printf("%s\n", __PRETTY_FUNCTION__);
	printf("  win = 0x%p; numSyms = %d\n", win, numSyms);
#endif

	if (view->GetNumDimensions() == 3) {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed);
	return;
	}

	ComputeDataLabelFrame(view);

	GDataAttrOffset *offset = map->GetGDataOffset();

	/* get coordinates of first data point in this batch */

	char*		gdata = (char *)gdataArray[0];
	RecId		recId = GetRecId(gdata, map, offset);
	Coord		x0 = GetX(gdata, map, offset);
	Coord		y0 = GetY(gdata, map, offset);
	PColorID	c0 = GetPColorID(gdata, map, offset);

	// How should line width be handled for line types?
	int width = GetLineWidth(gdata, map, offset);

	/* draw line connecting last point of previous batch to
	   first point of this batch */

	if (recId > 0)
	{
		Coord		xp, yp;
		PColorID	cp;

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

	for(int i = 1; i < numSyms; i++)
	{
		char*		gdata = (char *)gdataArray[i];
		Coord		x = GetX(gdata, map, offset);
		Coord		y = GetY(gdata, map, offset);
		PColorID	color = GetPColorID(gdata, map, offset);

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

	// Draw line connecting last point of this batch to first point of next

	Coord		xn, yn;
	PColorID	cn;

	if (view->GetPointStorage()->Find(recId + numSyms, xn, yn, cn)) {
		DrawConnectingLine(win, view,
						   Pattern0, width, x0, y0, c0, xn, yn, cn);
		(void)view->GetPointStorage()->Remove(recId + numSyms);
	} else {
		view->GetPointStorage()->Insert(recId + numSyms - 1, x0, y0, c0);
	}

	recordsProcessed = numSyms;
}

void FullMapping_LineShape::Draw3DGDataArray(WindowRep *win, void **gdataArray,
					   int numSyms, TDataMap *map,
					   ViewGraph *view, int pixelSize,
					   int &recordsProcessed)
{
  	GDataAttrOffset *offset = map->GetGDataOffset();

	char*	gdata = (char *)gdataArray[0];
	Coord	x0 = GetX(gdata, map, offset);
	Coord	y0 = GetY(gdata, map, offset);
	Coord	z0 = GetZ(gdata, map, offset);
	Coord	width = GetLineWidth(gdata, map, offset);
    	PColorID color = GetPColorID(gdata, map, offset);

	for(int i = 1; i < numSyms; i++) {
		gdata = (char *)gdataArray[i];
		Coord	x = GetX(gdata, map, offset);
		Coord	y = GetY(gdata, map, offset);
		Coord	z = GetZ(gdata, map, offset);
    		win->SetForeground(color);
    		win->Line3D(x0, y0, z0, x, y, z, width);
		x0=x;
		y0=x;
		z0=x;
		color=GetPColorID(gdata, map, offset);
		width = GetLineWidth(gdata, map, offset);
	}
	recordsProcessed=numSyms;
}


void FullMapping_LineShape::DrawConnectingLine(WindowRep *win, ViewGraph *view,
					   Pattern pattern, int line_width,
					   Coord x0, Coord y0, PColorID c0,
					   Coord x1, Coord y1, PColorID c1)
{
	win->SetPattern(pattern);
	win->SetForeground(c0);

	if (c0 == c1)
	{
		win->Line(x0, y0, x1, y1, line_width);
		return;
	}

	win->Line(x0, y0, (x0 + x1) / 2, (y0 + y1) / 2, line_width);

	win->SetForeground(c1);
	win->Line((x0 + x1) / 2, (y0 + y1) / 2, x1, y1, line_width);

	win->SetPattern(Pattern0);
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
							ViewGraph *view, Pattern pattern, int line_width,
							Coord x0, Coord y0, PColorID c0,
							Coord x1, Coord y1, PColorID c1)
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
	win->SetForeground(c0);

	Point points[4];

	if (c0 == c1) {
	if (y0 == y1) {
		/* area is a rectangle -- optimize for speed */
		win->FillRect(x0, 0, x1 - x0, y0);
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

	win->SetForeground(c1);

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
						  x0, y0, c0, x1, y1, c1);

    win->SetPattern(Pattern0);
}

//******************************************************************************
