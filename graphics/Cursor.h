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
  Revision 1.23  1999/08/17 19:46:54  wenger
  Converted Condor UserMonth session from high/low symbols to piles for
  better representation of data; fixed some cursor/pile drawing bugs and
  TData switching bugs that I found in the process.

  Revision 1.22  1999/08/05 21:42:34  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.21  1999/07/30 21:27:02  wenger
  Partway to cursor dragging: code to change mouse cursor when on a DEVise
  cursor is in place (but disabled).

  Revision 1.20  1999/04/29 17:36:33  wenger
  Implemented 'fixed cursor size' option (for the sake of the JavaScreen).

  Revision 1.19  1999/01/29 21:09:42  wenger
  Fixed bug 451 (dragging cursor in JS bypasses cursor grid).

  Revision 1.18  1998/06/12 19:55:15  wenger
  Attribute of TAttr/set links can now be changed; GUI has menu of available
  attributes; attribute is set when master view is set instead of at link
  creation; misc. debug code added.

  Revision 1.17  1998/06/10 18:04:02  wenger
  Improved new cursor drawing (bug in Mesa implementation sometimes makes
  it look ugly); cursor color now saved in sessions.

  Revision 1.16  1998/06/09 20:06:01  wenger
  2D OpenGL cursor now drawn as shaded outline plus every-other-point
  "mesh"; OpenGL CursorStore and GLWindowRep on SGI now use color indices
  instead of RGB so that they work the same as the other architectures;
  added user interface to allow changing cursor color (merged through
  cursor_test_br_1).

  Revision 1.15.2.1  1998/06/09 18:15:28  wenger
  Added cursor color-changing capability.

  Revision 1.15  1998/05/05 15:14:39  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.14  1998/04/13 22:24:43  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.13  1998/03/27 15:08:40  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

  Revision 1.12  1998/03/26 15:21:25  zhenhai
  The cursor drawings now use CursorStore as backup instead of using
  XOR mode for drawing and erasing.

  Revision 1.11  1997/11/24 23:14:12  weaver
  Changes for the new ColorManager.

  Revision 1.10  1997/07/22 15:36:22  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.9  1997/06/09 14:46:34  wenger
  Added cursor grid; fixed bug 187; a few minor cleanups.

  Revision 1.8.10.1  1997/05/21 20:39:36  weaver
  Changes for new ColorManager

  Revision 1.8  1996/11/13 16:56:05  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.7  1996/07/13 17:21:29  jussi
  Removed unnecessary code.

  Revision 1.6  1996/06/13 00:14:30  jussi
  Added support for XY cursors. All types of cursors can now
  be moved by clicking on their new location in the data area.
  Previously only the X label area was sensitive for cursor
  movement.

  Revision 1.5  1996/02/02 21:51:47  jussi
  Replaced HightlightColor with ForegroundColor. Cursors are
  drawn with xor so foreground color doesn't matter.

  Revision 1.4  1996/01/30 21:14:08  jussi
  Removed references to specific colors.

  Revision 1.3  1995/12/29 18:26:36  jussi
  Added FilterAboutToChange() to facilitate new cursor mechanism.
  Also added the copyright notice.

  Revision 1.2  1995/09/05 21:12:34  jussi
  Added/updated CVS header.
*/

#ifndef DeviseCursor_h
#define DeviseCursor_h

#include "DList.h"
#include "VisualArg.h"
#include "ViewCallback.h"
#include "CursorStore.h"
#include "WindowRep.h"

#include "Color.h"

class DeviseCursor;
class View;
DefinePtrDList(DeviseCursorList, DeviseCursor *)

//******************************************************************************
// class DeviseCursor
//******************************************************************************

class DeviseCursor : private ViewCallback
{
	public:

		// Constructors and Destructors
		DeviseCursor(char* name, VisualFlag flag,
					 Boolean useGrid = false,
					 Coord gridX = 1.0, Coord gridY = 1.0,
					 Boolean edgeGrid = false);
		virtual ~DeviseCursor(void);

	 /* Set source view. Changing this view's visual filter
     changes in the cursor's position. Can also set to NULL */
  void SetSource(View *view);
  View *GetSource() { return _src; }

  /* Set destination: this is where the cursor will be drawn.
     Can also set to NULL. */
  void SetDst(View *view);
  View *GetDst() { return _dst; }
  
  /* Get current visual filter. return TRUE if it exists. */
  Boolean GetVisualFilter(VisualFilter *&filter);

  VisualFlag GetFlag() { return _visFlag; }

  /* Move the X and Y coords of source; X and Y are the center of the cursor */
  void MoveSource(Coord x, Coord y, Coord width = -1.0, Coord height = -1.0);

  /* Get or set the grid parameters. */
  void GetGrid(Boolean &useGrid, Coord &gridX, Coord &gridY,
      Boolean &edgeGrid) {
    useGrid = _useGrid;
    gridX = _gridX;
    gridY = _gridY;
    edgeGrid = _edgeGrid;
  }
  void SetGrid(Boolean useGrid, Coord gridX, Coord gridY, Boolean edgeGrid) {
    _useGrid = useGrid;
    _gridX = gridX;
    _gridY = gridY;
    _edgeGrid = edgeGrid;
  }
  void ReadCursorStore(WindowRep*);
  void DrawCursorStore(WindowRep*);
  void DrawCursorFill(WindowRep*);
  void DrawCursorBorder(WindowRep*);

  void SetCursorColor(PColorID color);
  PColorID GetCursorColor() { return _cursorColor; }

  const char *GetName() { return _name; }

  // Get or set "fixed size".  This doesn't do anything, except get passed
  // to the JavaScreen to tell it to not allow the user to change the size
  // of the cursor.
  Boolean GetFixedSize() { return _fixedSize; }
  void SetFixedSize(Boolean fixed) { _fixedSize = fixed; }

  // Figure out whether the given location is on the cursor, and if so,
  // where.
  CursorHit::HitType IsOnCursor(Coord dataX, Coord dataY, Coord dataXTol,
      Coord dataYTol);

  // Figure out the resulting new cursor position if the mouse is dragged
  // from (pixX1, pixY1) to (pixX2, pixY2).
  void FindNewPosition(int pixX1, int pixY1, int pixX2, int pixY2,
      CursorHit::HitType hitType, Coord &dataXLow, Coord &dataYLow,
	  Coord &dataXHigh, Coord &dataYHigh);

  // Get the pixels that should be used to draw the cursor in the
  // destination view; returns true if values are valid.
  Boolean GetDestPixels(int &pixX1, int &pixY1, int &pixX2, int &pixY2);
  Boolean GetDestPixels(Coord dataXLow, Coord dataYLow, Coord dataXHigh,
      Coord dataYHigh, int &pixX1, int &pixY1, int &pixX2, int &pixY2);

  void SetOldDestPixels(int pixX1, int pixY1, int pixX2, int pixY2);
  Boolean GetOldDestPixels(int &pixX1, int &pixY1, int &pixX2, int &pixY2);
  void InvalidateOldDestPixels();

private:
  virtual void FilterAboutToChange(View *view);
  virtual void FilterChanged(View *view, VisualFilter &filter, int flushed);
  virtual void ViewDestroyed(View *view);
  void MatchGrid(Coord x, Coord y, Coord width, Coord height,
      VisualFilter &filter);

  char *_name;                          /* name of cursor */
  View *_src, *_dst;                    /* source and destination views */
  VisualFilter _filter;                 /* current filter */
  VisualFlag _visFlag;                  /* valid components in filter */
  Boolean _useGrid;
  Coord _gridX;
  Coord _gridY;
  Boolean _edgeGrid;
  CursorStore _cursor_store[12];
  PColorID _cursorColor;
  Boolean _fixedSize;

  Boolean _oldPixelsValid;
  int _oldPixX1;
  int _oldPixY1;
  int _oldPixX2;
  int _oldPixY2;
};

//******************************************************************************
#endif
