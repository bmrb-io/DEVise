/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.16  2009/05/13 22:41:23  wenger
  Merged x86_64_centos5_br_0 thru x86_64_centos5_br_1/dist_1_9_1x2 to
  the trunk.

  Revision 1.15.10.2  2009/05/06 20:19:13  wenger
  Got rid of extra debug output, cleaned up a few things.

  Revision 1.15.10.1  2009/05/01 22:26:34  wenger
  Debug code (and a few actual changes) trying to get DEVise to work
  on the x86_64/Centos 5 machines at BMRB (currently, opening
  histogram2.ds causes a core dump).

  Revision 1.15  2005/12/06 20:03:06  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.14.14.1  2003/04/18 17:07:41  wenger
  Merged gcc3_br_0 thru gcc3_br_1 to V1_7b0_br.

  Revision 1.14.30.1  2003/04/18 15:26:03  wenger
  Committing *some* of the fixes to get things to compile with gcc
  3.2.2; these fixes should be safe for earlier versions of the
  comiler.

  Revision 1.14  2000/03/14 17:05:07  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.13  1999/12/01 00:09:35  wenger
  Disabled extra debug logging for tracking down Omer's crash.

  Revision 1.12  1999/11/30 23:52:20  wenger
  A view can now be successfully inserted into a window which is in custom
  layout mode (fixes problem Omer had); more error checking in insertWindow
  command.

  Revision 1.11  1999/11/30 22:28:03  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.10  1999/04/20 14:13:31  wenger
  Improved debug output.

  Revision 1.9  1999/03/01 17:47:32  wenger
  Implemented grouping/ungrouping of views to allow custom view geometries.

  Revision 1.8  1999/02/11 19:54:31  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.7  1999/01/04 15:33:16  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.6.2.1  1999/02/11 18:23:59  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.6  1998/10/20 19:39:47  wenger
  Various small code cleanups.

  Revision 1.5  1998/02/26 00:18:57  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.4  1997/11/24 23:14:22  weaver
  Changes for the new ColorManager.

  Revision 1.3  1997/05/30 20:41:05  wenger
  Added GUI to allow user to specify windows to exclude from display
  print and/or print from pixmaps (for EmbeddedTk).  Exclusion is
  implemented but pixmap printing is not.

  Revision 1.2  1997/03/28 16:09:13  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

//******************************************************************************

#include <math.h>

#include "Layout.h"
#include "Display.h"
#include "Control.h"
#include "View.h"
#include "DebugLog.h"

//#define DEBUG

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

Layout::Layout(char* name, Coord x, Coord y, Coord w, Coord h,
			   Boolean printExclude, Boolean printPixmap)
	: ViewLayout(name)
{
#if defined(DEBUG)
  printf("Layout(0x%p)::Layout(%s, %f, %f, %f, %f)\n", this, name, x, y, w, h);
#endif

  _objectValid.Set();
  _mode = AUTOMATIC;
  Coord rootWidth, rootHeight;
  DeviseDisplay::DefaultDisplay()->Dimensions(rootWidth, rootHeight);
  Map((int) ( x * rootWidth), (int) (y * rootHeight),
      (unsigned) (w * rootWidth), (unsigned) (h * rootHeight));
  SetPrintExclude(printExclude);
  SetPrintPixmap(printPixmap);
}

Layout::Layout(char* name, int x, int y, unsigned w, unsigned h,
			   Boolean printExclude, Boolean printPixmap)
	: ViewLayout(name)
{
#if defined(DEBUG)
  printf("Layout(0x%p)::Layout(%s)\n", this, name);
#endif

  _objectValid.Set();
  _mode = AUTOMATIC;
  Map(x, y, w, h);
  SetPrintExclude(printExclude);
  SetPrintPixmap(printPixmap);
}

//******************************************************************************

/* for backward compatibility - to set STACKED, HOR and VERT layouts */
void Layout::SetPreferredLayout(int v, int h, Boolean stacked)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("Layout(%s, 0x%p)::SetPreferredLayout(%d, %d, %d)\n", GetName(),
    this, v, h, stacked);
#endif

  _mode = AUTOMATIC;
  _stacked = stacked;       /* backward compatibility */

  if (stacked) {
    _mode = STACKED;
  } else { 
    if ((h == 0) && (v == 0)) {
      _mode = CUSTOM;
      horRequested = verRequested = 0;
    }
    horRequested = (h < 0 ? -1 : h);
    if (horRequested >= 1)  {
      verRequested = -1;
      _mode = VERTICAL;
    } else {
      verRequested = (v < 0 ? -1 : v);
      if (verRequested >= 1) {
	_mode = HORIZONTAL;
	horRequested = -1;
      }
    }
  }
  if (Mapped())  {
    if ( _mode != CUSTOM) {
      MapChildren(0, true);
    }
  }
}

void Layout::SetLayoutProperties(LayoutMode mode, int rows, int columns)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("Layout(%s, 0x%p)::SetLayoutProperties(%d, %d, %d)\n", GetName(),
    this, mode, rows, columns);
#endif
#if defined(DEBUG_LOG)
    char logBuf[1024];
    int formatted = snprintf(logBuf, sizeof(logBuf),
	  "Layout(%s, 0x%p)::SetLayoutProperties(%d, %d, %d)\n",
      GetName(), this, mode, rows, columns);
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif

  _mode = mode;
  if (_mode == HORIZONTAL) {
    verRequested = (rows < 1) ? 1 : columns;
  }
  if (_mode == VERTICAL) {
    horRequested = (columns < 1) ? 1 : rows;
  }
  if (Mapped())  {
    if ( mode != CUSTOM) {
      MapChildren(0, true);
    }
  }

#if defined(DEBUG_LOG)
    formatted = snprintf(logBuf, sizeof(logBuf),
	    "  Done with Layout::SetLayoutProperties()\n");
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif
}


void Layout::Append(ViewWin *child)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("Layout(%s, 0x%p)::Append(%s)\n", GetName(), this, child->GetName());
#endif

  /* Geometries of the children would have been set from the Layout
   * editor for CUSTOM MODE 
   * add the child to the list
   */
  if (_mode != CUSTOM) { 
     /* Resize existing children, compute left-over space for new child */
    if (Mapped()) {
      int x, y;
      unsigned int w, h;
      
      MapChildren(0, true, child->GetWeight(), &x, &y, &w, &h);
      child->Map(x, y, w, h);
    }
  } else {
    // Note: this is just a "dummy" size and location so that the view is
    // mapped *somewhere*.  For things to work reasonably well, the geometry
    // of the view should be set before anything else is done.  (This code
    // was probably reached from an insertWindow command; that should be
    // followed by a setViewGeometry command.)  RKW 1999-11-30.
    child->Map(0, 0, 10, 10);
  }
  ViewWin::Append(child);
}
  


void Layout::Delete(ViewWin *child)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("Layout(%s, 0x%p)::Delete(%s)\n", GetName(), this, child->GetName());
#endif
//TEMP -- should we make sure child is really a child of this object?

  ViewWin::Delete(child);
  if (_mode != CUSTOM) { 
    /* Geometries of the children would have been set from the Layout
     * editor 
     * add the child to the list
     */
    /* Inform layout editor of change */
    if (Mapped()) {
      child->Unmap();
      /* map other children */
      MapChildren(0, true);
    }
  }
}

void Layout::MapChildren(ViewWin *single, Boolean resize, 
			 int extraWeight, int *x, int *y, 
			 unsigned int *w, unsigned int *h)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("Layout(%s, 0x%p)::MapChildren(0x%p, %d)\n", GetName(),
    this, single, resize);
#endif
#if defined(DEBUG_LOG)
    char logBuf[1024];
    int formatted = snprintf(logBuf, sizeof(logBuf),
	  "Layout(%s, 0x%p)::MapChildren(0x%p, %d)\n", GetName(),
      this, single, resize);
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif
  
  if ( _mode == CUSTOM) {
#if defined(DEBUG_LOG)
    formatted = snprintf(logBuf, sizeof(logBuf),
	    "  Done with Layout::MapChildren()\n");
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif
    return;
  }
  int totalWeight = TotalWeight() + extraWeight; 
  int _x, _y;
  unsigned int _w, _h;
  Geometry(_x, _y, _w, _h);
  
  if (_mode == STACKED) {
    // in a stacked view, all children get the total screen space;
    // only the top view is visible, and it's up to someone else
    // to decide which view is on top
#if defined(DEBUG)
    printf("Layout::MapChildren: stacking views\n");
#endif
    int index;
    for(index = InitIterator(); More(index);) {
      ViewWin *vw = Next(index);
      if (resize) {
	vw->MoveResize(_x, _y, _w, _h);
      } else {
	vw->Map(_x, _y, _w, _h);
      }
    }
    DoneIterator(index);
    if (x) {
      *x = _x;
      *y = _y;
      *w = _w;
      *h = _h;
    }
#if defined(DEBUG_LOG)
    formatted = snprintf(logBuf, sizeof(logBuf),
	    "  Done with Layout::MapChildren()\n");
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif
    return;
  }
  
  /******** VER or HOR views *********/
  const unsigned int numViews = NumChildren() + ( x ? 1 : 0);
  int horViews, verViews;
  ComputeLayout(_w, _h, numViews, horViews, verViews);
#ifdef DEBUG
  printf("Layout::MapChildren: using %dx%d layout for %d views\n",
	 verViews, horViews, numViews);
#endif
  DOASSERT((unsigned int) (verViews * horViews) >= numViews, "Incorrect number of views");
    // compute default, unweighted width and height of views
  unsigned int height = (int)(1.0 * _h / verViews);
  unsigned int width = (int)(1.0 * _w / horViews);

  unsigned int xoff = 0, yoff = 0;

  int index;
  for(index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);

    // if vertical stack of views, compute height based on weight
    if (horViews == 1) {
      height = (int)(1.0 * vw->GetWeight() / totalWeight * _h);
    }

    // if horizontal arrangement, compute width based on weight
    if (verViews == 1) {
      width = (int)(1.0 * vw->GetWeight() / totalWeight * _w);
    }

    // see if we're instructed to ignore all but one child
    if (!single || single == vw) {
      // see which method to call, resize or map
      if (resize) {
	vw->MoveResize(_x + xoff, _y + yoff, width, height);
      } else {
	vw->Map(_x + xoff, _y + yoff, width, height);
      }
    }

    // compute position of next view
    if (horViews == 1) {
      yoff += height;
    } else if (verViews == 1) {
      xoff += width;
    } else {
      xoff += width;
      // no more views fit in horizontally?
      if (_x + xoff + width > _x + _w) {
	xoff = 0;
	yoff += height;
      }
    }
  }
  DoneIterator(index);

  // see if we need to report back any unused space (for one more child)
  if (x) {
    DOASSERT(x && y && w && h, "Invalid window position or size");
    *x = _x + xoff;
    *y = _y + yoff;
    if (horViews == 1) {
      *h = _h - yoff;
    } else {
      *h = height;
    } if (verViews == 1) {
      *w = _w - xoff;
    } else {
      *w = width;
    }
  }
#if defined(DEBUG_LOG)
    formatted = snprintf(logBuf, sizeof(logBuf),
	    "  Done with Layout::MapChildren()\n");
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif
}


void Layout::ComputeLayout(unsigned int w, unsigned int h,
			   unsigned int numViews,
			   int &horViews, int &verViews)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("Layout::ComputeLayout(%s, 0x%p)::ComputeLayout(%u %u %u)\n",
    GetName(), this, w, h, numViews);
#endif

  if (numViews <= 1) {
    horViews = verViews = 1;
#if defined(DEBUG)
    printf("  horViews = %d, viewViews = %d\n", horViews, verViews);
#endif
    return;
  }
  
  if (_mode == VERTICAL) {
    horViews = ((unsigned)horRequested < numViews) ? horRequested : numViews;
    verViews = numViews / horViews;
    if (numViews % horViews) {
      verViews++;
    }
#if defined(DEBUG)
    printf("  horViews = %d, viewViews = %d\n", horViews, verViews);
#endif
    return;
  }

  if (_mode == HORIZONTAL) {
    verViews = ((unsigned)verRequested < numViews) ? verRequested : numViews;
    horViews = numViews / verViews;
    if (numViews % verViews)  {
      horViews++;
    }
#if defined(DEBUG)
    printf("  horViews = %d, viewViews = %d\n", horViews, verViews);
#endif
    return;
  }
  
  /*****************AUTO LAYOUT *********************/
  
  // otherwise, we have total control over the layout, so...
  
  // if window is much taller than wide, stack views vertically
  // in a single column

  if (h >= 1.5 * w) {
    horViews = 1;
    verViews = numViews;
#if defined(DEBUG)
    printf("  horViews = %d, viewViews = %d\n", horViews, verViews);
#endif
    return;
  }

    // if window is much wider than tall, arrange views horizontally
  // in a single row -- because we typically display timeseries
  // data requiring more horizontal than vertical space, we have
  // a little more tendency to stack views vertically (see above)
  // than horizontally

  if (w >= 3 * h) {
    horViews = numViews;
    verViews = 1;
#if defined(DEBUG)
    printf("  horViews = %d, viewViews = %d\n", horViews, verViews);
#endif
    return;
  }

    // otherwise, arrange views so that there's roughly an equal number of
  // rows and columns; also try to make sure whole window is used
  // efficiently (no unused blocks)

  for(horViews = (int)sqrt((double)numViews); horViews >= 1; horViews--) {
    if (numViews % horViews == 0)
      break;
  }

  verViews = numViews / horViews;
#if defined(DEBUG)
    printf("  horViews = %d, viewViews = %d\n", horViews, verViews);
#endif
}


void Layout::ScaleChildren(int oldX, int oldY, unsigned oldW, unsigned oldH,
			   int oldX0, int oldY0)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  int newX, newY, newX0, newY0;
  unsigned newW, newH;
#ifdef DEBUG
  printf("ScaleChildren :: Old : (%d, %d) %u %u, abs(%d, %d) \n",
	 oldX, oldY, oldW, oldH, oldX0, oldY0);
#endif
  Geometry(newX,newY, newW, newH);
  AbsoluteOrigin(newX0,newY0);
#ifdef DEBUG
  printf("ScaleChildren :: New : (%d, %d) %u %u, abs(%d, %d) \n",
	 newX, newY, newW, newH, newX0, newY0);
#endif
  Boolean move, resize;
  
  move = resize = false;

  if ((oldW != newW) || (oldH != newH)) {
    resize = true;
  }
  if ((oldX0 != newX0) || (oldY0 != newY0)) {
    move = true;
  }
  if ( !move && !resize) {
    return;
  }
  char buf[100];
  if (!resize) {
    int formatted = snprintf(buf, sizeof(buf), "DEViseWindowMoved {%s}",
	    GetName());
	DevStatus tmpStatus = checkAndTermBuf2(buf, formatted);
	DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
    ControlPanel::Instance()->NotifyFrontEnd(buf);
    return;
  }
  /* window actually resized */
  
  double xscale = newW * 1.0 / oldW;
  double yscale = newH * 1.0 / oldH;
#ifdef DEBUG  
  printf("ScaleChildren : xscal = %f, yscale %f\n", xscale, yscale);
#endif
  int index;
  /* check this */
  for (index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    int oldviewX, oldviewY, oldviewX0, oldviewY0;
    unsigned oldviewH, oldviewW;
    int newviewX, newviewY, newviewX0, newviewY0;
    unsigned newviewH, newviewW;

    vw->Geometry(oldviewX, oldviewY, oldviewW, oldviewH);
    vw->AbsoluteOrigin(oldviewX0, oldviewY0);
#ifdef DEBUG
    printf("ScaleChildren: View (%s) : (%d, %d) %u %u, abs %d, %d\n", 
	   GetName(), oldviewX, oldviewY, oldviewW, oldviewH, 
	   oldviewX0, oldviewY0);
    printf("ScaleChildren :: Old : (%d, %d) %u %u, abs(%d, %d) \n",
	 oldX, oldY, oldW, oldH, oldX0, oldY0);
#endif
    newviewW = (unsigned) (oldviewW * xscale);
    newviewH = (unsigned) (oldviewH * yscale);
    newviewX = (int) (newX + xscale * (oldviewX0 - oldX - oldX0));
    newviewY = (int) (newY + yscale * (oldviewY0 - oldY - oldY0));
#ifdef DEBUG
    printf("ScaleChildren: View (%s) SetGeometry(%d, %d, %u %u). \n",
	   vw->GetName(), newviewX, newviewY,  newviewW, newviewH );
#endif
    /* Note that SetGeometry takes parameters like MoveResize and 
     * these do not correspond to the GetGeometry parameters 
     */
    ((View *)vw)->SetGeometry(newviewX, newviewY, newviewW, newviewH);
    /* To check get the geometry again and see the values */
    
  }
  DoneIterator(index);
  return;
}

//******************************************************************************
// Callback Methods (WindowRepCallback)
//******************************************************************************

void	Layout::HandleResize(WindowRep* win, int x, int y,
							 unsigned w, unsigned h)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#ifdef DEBUG
	printf("Layout(%s, 0x%p)::HandleResize at %d,%d, size %u,%u\n",
		   GetName(), this, x, y, w, h);
#endif
#if defined(DEBUG_LOG)
    char logBuf[1024];
	int formatted = snprintf(logBuf, sizeof(logBuf),
	  "Layout(%s, 0x%p)::HandleResize at %d,%d, size %u,%u\n",
      GetName(), this, x, y, w, h);
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif

	int			oldX, oldY, oldX0, oldY0;
	unsigned	oldH, oldW;
	
	Geometry(oldX,oldY, oldW, oldH);
	AbsoluteOrigin(oldX0,oldY0);
	ViewWin::HandleResize(win, x, y, w, h);

	if (Mapped())
	{
		if (_mode == CUSTOM) {
			ScaleChildren(oldX, oldY, oldW, oldH, oldX0, oldY0);
		} else {
			MapChildren(0, true);
        }

		char	buf[100];
		int formatted = snprintf(buf, sizeof(buf),
		    "DEViseWindowResize {%s}", GetName());
        DevStatus tmpStatus = checkAndTermBuf2(buf, formatted);
		DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
#ifdef DEBUG
		printf("%s\n", buf);
#endif
		ControlPanel::Instance()->NotifyFrontEnd(buf);
	}
#if defined(DEBUG_LOG)
    formatted = snprintf(logBuf, sizeof(logBuf),
	    "  Done with Layout::HandleResize()\n");
	checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif
}

void	Layout::HandleWindowMappedInfo(WindowRep* win, Boolean mapped)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");

	char	buf[100];
	ViewWin::HandleWindowMappedInfo(win, mapped);
	int formatted = snprintf(buf, sizeof(buf), "DEViseWindowMapped {%s}",
	    GetName());
    DevStatus tmpStatus = checkAndTermBuf2(buf, formatted);
	DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
#ifdef DEBUG
	printf("%s\n", buf);
#endif
	ControlPanel::Instance()->NotifyFrontEnd(buf);
}

Boolean		Layout::HandleWindowDestroy(WindowRep*	win)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");

	char	buf[100];
	ViewWin::HandleWindowDestroy(win);
	int formatted = snprintf(buf, sizeof(buf), "DEViseWindowDestroy {%s}",
	    GetName());
    DevStatus tmpStatus = checkAndTermBuf2(buf, formatted);
	DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
#ifdef DEBUG
	printf("%s\n", buf);
#endif
	ControlPanel::Instance()->NotifyFrontEnd(buf);
	return true;
}

//******************************************************************************
