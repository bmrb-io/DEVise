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
  Revision 1.4  1995/12/06 21:23:15  jussi
  Added ClosestColor() method.

  Revision 1.3  1995/12/02 21:30:38  jussi
  Tried if letting Tcl/Tk handle all X events and pass a copy
  of each event to us would solve the problem with TK_WINDOW_old.
  This event handling mechanism didn't turn out to work right
  so I renamed it TK_WINDOW_EV2.

  Revision 1.2  1995/09/05 21:13:39  jussi
  Added/updated CVS header.
*/

#ifndef XDisplay_h
#define XDisplay_h

#include "Display.h"
#include "Xdef.h"
#include "DList.h"
#include "XBitmap.h"
#include "Pattern.h"

#ifdef TK_WINDOW_EV2
#include <tcl.h>
#include <tk.h>
#endif

class XWindowRep;

DefinePtrDList(XWindowRepList, XWindowRep *);

class XDisplay: public DeviseDisplay {
public:
    XDisplay(char *name=NULL);

#ifdef TK_WINDOW_EV2
    virtual ~XDisplay();
    virtual int HandleXEvent(XEvent &event);
#endif

    virtual int NumPlanes(){
		return DisplayPlanes(_display,DefaultScreen(_display));
	}
	virtual void Dimensions(Coord &width, Coord &height){
		int screen = DefaultScreen(_display);
		width = (Coord) DisplayWidth(_display,screen);
		height = (Coord) DisplayHeight(_display,screen);
	};

    /* Create a new window Rep. Dimensions are in normatlized
	coord from (0,0) to (1,1) */
	virtual WindowRep *CreateWindowRep(char *name,
		Coord x, Coord y, Coord width, Coord height, 
		Color fgnd=WhiteColor, Color bgnd=BlackColor,
		WindowRep *parentRep=NULL, Coord min_width = 0.05, 
		Coord min_height = 0.05, Boolean relative= true,
		Boolean winBoundary = true);

	/* Destroy a window rep*/
	virtual void DestroyWindowRep(WindowRep *win);

	/* Do internal event processing, but do not block while doing it */
	virtual void InternalProcessing();

protected:
	/* from DeviseDisplay */
        Boolean ClosestColor(Colormap &map, XColor &color, Color &c,
			     float &error);
	virtual void AllocColor(char *name, Color globalColor);
	virtual void AllocColor(double r, double g, double b, Color globalColor);

	friend class XWindowRep;

	/* Get stipple for pattern */
	Pixmap GetPatternStipple(Pattern p){ return _stipples[p]; };

	/* get structure for font */
	XFontStruct *GetFontStruct(){ return _fontStruct; };

	/* Get width and height of X window */
	void WinDimensions(Window win,Coord &winWidth, Coord &winHeight){
		Window root;
		int x,y;
		unsigned int width, height;
		unsigned int border_width, depth;
		XGetGeometry(_display,win,&root,&x,&y,&width,&height,
			&border_width, &depth);
		winWidth = (Coord)width;
		winHeight = (Coord)height;
	}


	/* translate from normalized dimensions of (0,0) to (1,1) into
	real window coordinates.  */
	void RealWindowDimensions(Window win,
		Coord x, Coord y, Coord w, Coord h,
		Coord &realX, Coord &realY, Coord &realWidth, Coord &realHeight){
		Coord winWidth, winHeight;
		WinDimensions(win,winWidth, winHeight);

		realX = x*winWidth;
		realY = y*winHeight;
		realWidth = w*winWidth;
		realHeight = h *winHeight;
	};

	void RealWindowWidthHeight(Window win,Coord width, Coord height,
			Coord &realWidth, Coord &realHeight){
		Coord winWidth, winHeight;
		WinDimensions(win,winWidth, winHeight);
		realWidth = width*winWidth;
		realHeight = height*winHeight;
	};
		

private:
	Display *_display;		/* X display */
	/* DList<XWindowRep *> _winList; */
	XWindowRepList _winList; /* list of X windows created */
	Pixmap _stipples[XNumBitmaps]; /* bitmaps for patterns */
	XFontStruct *_fontStruct;

	Color baseColor;                /* base color */
};

#endif
