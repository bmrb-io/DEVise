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

  Coord rootWidth, rootHeight;
  DeviseDisplay::DefaultDisplay()->Dimensions(rootWidth, rootHeight);
  Map((int) ( x * rootWidth), (int) (y * rootHeight),
      (unsigned) (w * rootWidth), (unsigned) (h * rootHeight));
  _mode = AUTOMATIC;
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

  Map(x, y, w, h);
  _mode = AUTOMATIC;
  SetPrintExclude(printExclude);
  SetPrintPixmap(printPixmap);
}

//******************************************************************************

/* for backward compatibility - to set STACKED, HOR and VERT layouts */
void Layout::SetPreferredLayout(int v, int h, Boolean stacked)
{
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
#if defined(DEBUG)
  printf("Layout(%s, 0x%p)::SetLayoutProperties(%d, %d, %d)\n", GetName(),
    this, mode, rows, columns);
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
}


void Layout::Append(ViewWin *child)
{
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
  }
  ViewWin::Append(child);
}
  


void Layout::Delete(ViewWin *child)
{
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
#if defined(DEBUG)
  printf("Layout(%s, 0x%p)::MapChildren(0x%p, %d)\n", GetName(),
    this, single, resize);
#endif
  
  if ( _mode == CUSTOM) {
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
      if (resize)
	vw->MoveResize(_x, _y, _w, _h);
      else
	vw->Map(_x, _y, _w, _h);
    }
    DoneIterator(index);
    if (x) {
      *x = _x;
      *y = _y;
      *w = _w;
      *h = _h;
    }
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
    if (horViews == 1)
      height = (int)(1.0 * vw->GetWeight() / totalWeight * _h);

    // if horizontal arrangement, compute width based on weight
    if (verViews == 1)
      width = (int)(1.0 * vw->GetWeight() / totalWeight * _w);

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
    if (horViews == 1)
      yoff += height;
    else if (verViews == 1)
      xoff += width;
    else {
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
    if (horViews == 1)
      *h = _h - yoff;
    else
      *h = height;
    if (verViews == 1)
      *w = _w - xoff;
    else
      *w = width;
  }
}


void Layout::ComputeLayout(unsigned int w, unsigned int h,
			   unsigned int numViews,
			   int &horViews, int &verViews)
{
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

  for(horViews = (int)sqrt(numViews); horViews >= 1; horViews--) {
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
    sprintf(buf,"DEViseWindowMoved {%s}", GetName());
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
#ifdef DEBUG
	printf("Layout(%s, 0x%p)::HandleResize at %d,%d, size %u,%u\n",
		   GetName(), this, x, y, w, h);
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
		sprintf(buf,"DEViseWindowResize {%s}", GetName());
#ifdef DEBUG
		printf("%s\n", buf);
#endif
		ControlPanel::Instance()->NotifyFrontEnd(buf);
	}
}

void	Layout::HandleWindowMappedInfo(WindowRep* win, Boolean mapped)
{
	char	buf[100];

	ViewWin::HandleWindowMappedInfo(win, mapped);
	sprintf(buf,"DEViseWindowMapped {%s}", GetName());

#ifdef DEBUG
	printf("%s\n", buf);
#endif

	ControlPanel::Instance()->NotifyFrontEnd(buf);
}

Boolean		Layout::HandleWindowDestroy(WindowRep*	win)
{
	char	buf[100];

	ViewWin::HandleWindowDestroy(win);
	sprintf(buf,"DEViseWindowDestroy {%s}", GetName());

#ifdef DEBUG
	printf("%s\n", buf);
#endif

	ControlPanel::Instance()->NotifyFrontEnd(buf);
	return true;
}

//******************************************************************************
