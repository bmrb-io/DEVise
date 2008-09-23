/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.49  2005/12/06 20:03:12  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.48.10.3  2005/09/06 21:20:10  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.48.10.2  2003/11/19 19:40:14  wenger
  Display modes now work for symbol colors; also added some missing
  commands to the (horrible) Tcl code for copying views; minor
  improvement to error reporting.

  Revision 1.48.10.1  2003/11/05 17:01:45  wenger
  First part of display modes for printing is implemented (view foreground
  and background colors work, haven't done anything for symbol colors yet).

  Revision 1.48  2001/09/26 16:31:30  wenger
  Fixed bug 693 (DEVise rubberband line now reflects X-only zoom).

  Revision 1.47  2000/03/14 21:51:39  wenger
  Added more invalid object checking; had to take some memory checking
  out of client-side stuff for linking reasons.

  Revision 1.46  2000/03/14 17:05:15  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.45  1999/11/29 21:07:53  wenger
  Fixed bug 535 and partially fixed bug 532 (problems with view order in
  piles); removed (unused) replaceView command and related ViewWin methods

  Revision 1.44  1999/10/04 19:36:59  wenger
  Mouse location is displayed in "regular" DEVise.

  Revision 1.43  1999/09/02 17:25:51  wenger
  Took out the ifdefs around the MARGINS code, since DEVise won't compile
  without them; removed all of the TK_WINDOW code, and removed various
  unnecessary includes of tcl.h, etc.

  Revision 1.42  1999/08/12 16:02:48  wenger
  Implemented "inverse" zoom -- alt-drag zooms out instead of in.

  Revision 1.41  1999/08/05 21:42:37  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.40  1999/07/30 21:27:05  wenger
  Partway to cursor dragging: code to change mouse cursor when on a DEVise
  cursor is in place (but disabled).

  Revision 1.39  1999/06/15 18:09:44  wenger
  Added dumping of ViewWin objects to help with pile debugging.

  Revision 1.38  1999/05/12 21:01:03  wenger
  Views containing view symbols can now be piled.

  Revision 1.37  1999/05/07 16:09:39  wenger
  Fixed bug in the ordering of viewsym piles.

  Revision 1.36  1999/05/07 14:13:46  wenger
  Piled view symbols now working: pile name is specified in parent view's
  mapping, views are piled by Z specified in parent's mapping; changes
  include improvements to the Dispatcher because of problems exposed by
  piled viewsyms; for now, view symbol piles are always linked (no GUI or
  API to change this).

  Revision 1.35  1999/04/21 20:35:22  wenger
  Improved interface for changing fonts, titles, etc.:
  * Fonts can now be set on a window-wide basis.
  * Setting fonts, title, or axis date format in a piled view automatically
  changes all views in the pile accordingly.

  Revision 1.34  1999/02/22 19:07:37  wenger
  Piling of views with view symbols is not allowed; fixed bug 461 (redrawing
  of piles); fixed bug 464 (toggling axes in a pile); fixed dynamic memory
  problems in PileStack and ViewClassInfo classes.

  Revision 1.33  1999/02/11 19:54:39  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.32.2.1  1998/12/29 17:24:49  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.32  1998/10/20 19:46:05  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.31  1998/09/30 17:44:34  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.30  1998/09/10 23:24:29  wenger
  Fixed JavaScreen client switch GIF geometry problem.

  Revision 1.29  1998/05/21 18:18:32  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.28  1998/05/14 18:21:12  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.27  1998/02/04 20:22:04  zhenhai
  Fixed bugs of displaying date and customized text at axis.

  Revision 1.26  1997/12/12 05:50:23  weaver
  *** empty log message ***

  Revision 1.25  1997/11/24 23:14:36  weaver
  Changes for the new ColorManager.

  Revision 1.24  1997/06/04 15:50:31  wenger
  Printing windows to PostScript as pixmaps is now implemented, including
  doing so when printing the entire display.

  Revision 1.23  1997/05/30 20:42:28  wenger
  Added GUI to allow user to specify windows to exclude from display
  print and/or print from pixmaps (for EmbeddedTk).  Exclusion is
  implemented but pixmap printing is not.

  Revision 1.22.6.1  1997/05/21 20:40:06  weaver
  Changes for new ColorManager

  Revision 1.22  1997/04/21 22:48:35  guangshu
  Added _histViewExist.

  Revision 1.21  1997/03/25 17:58:59  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.20  1997/02/03 19:40:02  ssl
  1) Added a new Layout interface which handles user defined layouts
  2) Added functions to set geometry and remap views as changes in the
     layout editor
  3) Added a function to notify the front end of some change so that it
     can execute a Tcl command
  4) The old TileLayout.[Ch] files still exist but are commented out
     conditionally using #ifdef NEW_LAYOUT

  Revision 1.19.4.1  1997/03/15 00:31:08  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.19  1996/11/26 16:47:47  ssl
  Added support for Stacked Opaque and Transparent views

  Revision 1.18  1996/11/13 16:56:15  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.17  1996/11/07 22:40:18  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.16  1996/10/18 20:34:11  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.15  1996/09/19 20:11:55  wenger
  More PostScript output code (still disabled); some code for drawing
  view borders (disabled).

  Revision 1.14  1996/09/10 20:07:12  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.13  1996/07/14 16:52:30  jussi
  Added handling of window destroy events from window manager.

  Revision 1.12  1996/07/14 04:06:04  jussi
  Added HandleWindowDestroy() method.

  Revision 1.11  1996/07/13 17:26:31  jussi
  Moved Mapped() method to header file.

  Revision 1.10  1996/07/10 19:36:23  jussi
  Cleaned up code.

  Revision 1.9  1996/06/15 13:47:50  jussi
  Added SetFgBgColor() which allows Devise to change the view
  foreground and background colors at runtime.

  Revision 1.8  1996/04/11 17:54:04  jussi
  Added Raise() and Lower().

  Revision 1.7  1996/02/06 19:32:57  jussi
  Added top margin with Devise logo to root windows.

  Revision 1.6  1996/01/30 00:03:38  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.5  1995/12/29 22:41:54  jussi
  Corrected type of GetFgColor() method.

  Revision 1.4  1995/12/02 21:25:50  jussi
  Added support for TK_WINDOW i.e. Tk controls surrounding each
  view and window.

  Revision 1.3  1995/11/29 15:06:19  jussi
  Added copyright notice.

  Revision 1.2  1995/09/05 21:13:19  jussi
  Added/updated CVS header.
*/

/*
   A ViewWin is consist of two parts:
   1) WindowRep part that is its window
   2) children: subwindows

   The base class keeps track of the children, while the derived classes
   determine how to create the window representation of the
   children and how children are to be handled.

   Protocol for creating a new WindowRep in the constructor:
   Call the Map() function with the size of the window.
   1) If parent == NULL, a top level WindowRep is created.
   2) if parent != NULL: a lower level windowRep is created.
*/

//******************************************************************************
//
//******************************************************************************

#ifndef __VIEWWIN_H
#define __VIEWWIN_H

//******************************************************************************
// Libraries
//******************************************************************************

#include "DList.h"
#include "DeviseTypes.h"
#include "WindowRep.h"
#include "DualWindowRep.h"
#include "ObjectValid.h"

#include "Color.h"
#include "Coloring.h"
#include "DisplayMode.h"

class WindowRep;
class ViewWin;
class PileStack;

//******************************************************************************

DefinePtrDList(ViewWinList, ViewWin *);

//******************************************************************************
// class ViewWin
//******************************************************************************

class ViewWin_WindowRepCallback;

class ViewWin : public Coloring
{
		friend class ViewWin_WindowRepCallback;
		friend class PileStack;

	private:

		// Callback Adapters
		ViewWin_WindowRepCallback*	windowRepCallback;

	public:

		// Constructors and Destructors
//		ViewWin(char* name,
//				PColorID fgid = nullPColorID,
//				PColorID bgid = nullPColorID,
//				int weight = 1, Boolean boundary = true);
		ViewWin(char* name,
				PColorID fgid = GetPColorID(defForeColor),
				PColorID bgid = GetPColorID(defBackColor),
				int weight = 1, Boolean boundary = true);
		virtual ~ViewWin(void);

		// Getters and Setters
		WindowRep*			GetWindowRep(void)
		{ return _winReps.GetWindowRep(); }
		const WindowRep*	GetWindowRep(void) const
		{ return _winReps.GetWindowRep(); }

		// GetGifDirty() returns true iff the window's image has been
		// changed since the last time it was dumped as a gif.
		Boolean GetGifDirty();

		virtual void		SetForeground(PColorID fgid);
		virtual void		SetForeground(PColorID fgid,
		                      DisplayMode::Mode mode);
		virtual void		SetBackground(PColorID bgid);
		virtual void		SetBackground(PColorID bgid,
		                      DisplayMode::Mode mode);

		virtual PColorID	GetForeground(void) const;
		virtual PColorID	GetForeground(DisplayMode::Mode mode) const;
		virtual PColorID	GetBackground(void) const;
		virtual PColorID	GetBackground(DisplayMode::Mode mode) const;

    /* Iconify window, if top level. Not guaranteed to succeed */
    void Iconify();

    DevStatus ExportImage(DisplayExportFormat format, const char *filename);

    /* Insert ViewWin into a parent */
    void AppendToParent(ViewWin *parent);
    /* Delete ViewWin from parent */
    void DeleteFromParent();
	
    /* These are called by parents */
    virtual void Map(int x, int y, unsigned w, unsigned h);
    virtual void Unmap();
    virtual void Iconify(Boolean iconified) = 0;

    Boolean Mapped() const { return _mapped; }
    char *GetName() const { return _name; }
    int GetWeight() const { return _weight; }

    virtual void Raise();
    virtual void Lower();

    /* Get size of margin controls */
    void GetMargins(unsigned int &lm, unsigned int &rm,
                    unsigned int &tm, unsigned int &bm);
    /* Get real (non-margin-adjusted) geometry of window */
    void RealGeometry(int &x, int &y, unsigned &w, unsigned &h);
    
    /* Set geometry of window */
    virtual void SetGeometry(int x, int y, unsigned w, unsigned h);

    /* Get current geometry of window */
    void Geometry(int &x, int &y, unsigned &w, unsigned &h);

    /* Get absolute origin of window, if mapped */
    void AbsoluteOrigin(int &x, int &y);

    /* Move/Resize self. Base class creates a new WindowRep
       if not already there and then resizes it */
    virtual void MoveResize(int x, int y, unsigned w, unsigned h);
    
//    WindowRep *GetAltRep() { return _alternate;}
//    void SetAltRep(WindowRep *w) { _alternate = w;}
    /* Detach all children from this view */
    void DetachChildren();

    /* Append child. Base class just inserts the child.
       Derived class should implement functions to MoveResize() child.*/
    virtual void Append(ViewWin *child);

    /* Delete child */
    virtual void Delete(ViewWin *child);

    /* Swap child1 and child2. Both must be valid. Base
       class updates only list of children. */
    virtual void SwapChildren(ViewWin *child1, ViewWin *child2);

    /* Get the children */
    /* return # of children */
    int NumChildren() { return _children.Size(); }

    /* Iterator for children */
    int InitIterator(Boolean backwards = false) {
        return _children.InitIterator(backwards);
    }
    Boolean More(int index) { return _children.More(index); }
    virtual ViewWin *Next(int index) { return _children.Next(index); }
    void DoneIterator(int index) { _children.DoneIterator(index); }

    /* Get the first sibling of this ViewWin (the first sibling is the
     * one that's on the *bottom* when views are piled, stacked, etc.,
     * because the views are drawn in the order that they're on the
     * list). */
    ViewWin *GetFirstSibling() {
      ViewWin *sibling = NULL;
      ViewWin *parent = GetParent();
      if (parent) {
        int index = parent->InitIterator();
	if (parent->More(index)) {
          sibling = parent->Next(index);
	}
        parent->DoneIterator(index);
      }
      return sibling;
    }

    ViewWin *GetParent() { return _parent; }
    
    /* Return true if window is iconified */
    Boolean Iconified();

    virtual DevStatus PrintPS();
    void SetScreenOutput() { _winReps.SetScreenOutput(); }
    void SetFileOutput(const Rectangle &viewGeom,
      const Rectangle &parentGeom) {
      _winReps.SetFileOutput(viewGeom, parentGeom);
    }
    Boolean IsPrinting() { return _hasPrintIndex; }

    void SetPrintExclude(Boolean exclude) { _excludeFromPrint = exclude; }
    Boolean GetPrintExclude() { return _excludeFromPrint; }
    void SetPrintPixmap(Boolean pixmap) { _printAsPixmap = pixmap; }
    Boolean GetPrintPixmap() { return _printAsPixmap; }

	PileStack *GetMyPileStack() { return _myPileStack; }
	void SetMyPileStack(PileStack *ps);

	PileStack *GetParentPileStack() { return _parentPileStack; }

	void Dump(FILE *fp);

protected:
	void SetParentPileStack(PileStack *ps);

public:
	virtual void Refresh(Boolean refreshPile = true) {}

    virtual void SetFont(const char *which, int family, float pointSize,
      Boolean bold, Boolean italic, Boolean notifyPile = true);

	// Z value for piled view symbols.  Perhaps this should be combined
	// with the Z in the View class.
	void SetPileZ(Coord z) { _pileZValid = true; _pileZValue = z; }
	void UnsetPileZ() { _pileZValid = false; }
	const Coord* GetPileZ() { return _pileZValid ? &_pileZValue : NULL; }

protected:
    /* called by base class when it has been mapped/unmapped */
    virtual void SubClassMapped() = 0;
    virtual void SubClassUnmapped() = 0;

    /* Get total weight of children */
    int TotalWeight();

    Boolean _iconified;
    Boolean _winBoundary;
    Boolean _hasGeometry; /* TRUE if we have the geometry */
    Boolean _histViewExist; /* TRUE if the histogram view exist */
    int _x, _y;           /* current location */
    unsigned int _width;  /* current width */
    unsigned int _height; /* current height */
    ViewWinList _children;
    ViewWin *_parent;     /* parent window */
//    WindowRep *_alternate;  /* testing pile view */
    DualWindowRep _winReps;
    char *_name;          /* name of window */
    int _weight;          /* relative weight of window */
    Boolean _mapped;      /* TRUE if this window is mapped */

    void DrawMargins();

    unsigned int _leftMargin;
    unsigned int _rightMargin;
    unsigned int _topMargin;
    unsigned int _bottomMargin;

private:
    Boolean _hasPrintIndex;
    int _printIndex;

    Boolean _excludeFromPrint;
    Boolean _printAsPixmap;

	// The PileStack, if any, that "owns" this object.
	PileStack *_myPileStack;

	// The PileStack, if any, that this object "owns".
	PileStack *_parentPileStack;

	// Z value for piled view symbols.
	Boolean _pileZValid;
	Coord _pileZValue;


	protected:

		// Callback methods (WindowRepCallback)
		virtual void	HandleExpose(WindowRep* w, int x, int y, 
									 unsigned width, unsigned height);

#ifdef RAWMOUSEEVENTS
		virtual void	HandleButton(WindowRep* w, int x, int y,
									 int button, int state, int type) {}
#else
		virtual void	HandlePress(WindowRep* w, int x1, int y1,
									int x2, int y2, int button,
									int state) {}
#endif

		virtual void	HandleResize(WindowRep* w, int xlow, int ylow,
									 unsigned width, unsigned height);
		virtual void	HandleKey(WindowRep* w, int key, int x, int y) {}

#ifndef RAWMOUSEEVENTS
		virtual Boolean	HandlePopUp(WindowRep* w, int x, int y, int button,
									const char**& msgs, int& numMsgs)
		{ return false; }
#endif

		virtual void	HandleWindowMappedInfo(WindowRep* w, Boolean mapped);
		virtual Boolean	HandleWindowDestroy(WindowRep *w);

		virtual void IsOnCursor(int pixX, int pixY, CursorHit &cursorHit) {
		  cursorHit._hitType = CursorHit::CursorNone;
		  cursorHit._cursor = NULL; }

        virtual void MouseDrag(int x1, int y1, int x2, int y2, int button) {}
		
		// Don't do anything in a window that doesn't have a view.
		virtual void ShowMouseLocation(int *mouseX, int *mouseY) {}

    private:
        ObjectValid _objectValid;
		DisplayModeViewBG _bgColors;
		DisplayModeViewFG _fgColors;
};

//******************************************************************************
// class ViewWin_WindowRepCallback
//******************************************************************************

class ViewWin_WindowRepCallback : public WindowRepCallback
{
	private:

		ViewWin*	_parent;
		
	public:

		ViewWin_WindowRepCallback(ViewWin* parent)
			: _parent(parent) {}
		virtual ~ViewWin_WindowRepCallback() {}

		virtual void	HandleExpose(WindowRep* w, int x, int y, 
									 unsigned width, unsigned height)
		{
			_parent->HandleExpose(w, x, y, width, height);
		}

#ifdef RAWMOUSEEVENTS
		virtual void	HandleButton(WindowRep* w, int x, int y,
									 int button, int state, int type)
		{
			_parent->HandleButton(w, x, y, button, state, type);
		}
#else
		virtual void	HandlePress(WindowRep* w, int xlow, int ylow,
									int xhigh, int yhigh, int button,
									int state)
		{
			_parent->HandlePress(w, xlow, ylow, xhigh, yhigh, button, state);
		}
#endif

		virtual void	HandleResize(WindowRep* w, int xlow, int ylow,
									 unsigned width, unsigned height)
		{
			_parent->HandleResize(w, xlow, ylow, width, height);
		}

		virtual void	HandleKey(WindowRep* w, int key, int x, int y)
		{
			_parent->HandleKey(w, key, x, y);
		}
  
#ifndef RAWMOUSEEVENTS
		virtual Boolean		HandlePopUp(WindowRep* w, int x, int y, int button,
										const char**& msgs, int& numMsgs)
		{
			return _parent->HandlePopUp(w, x, y, button, msgs, numMsgs);
		}
#endif

		virtual void	HandleWindowMappedInfo(WindowRep* w, Boolean mapped)
		{
			_parent->HandleWindowMappedInfo(w, mapped);
		}

		virtual Boolean		HandleWindowDestroy(WindowRep* w)
		{
			return _parent->HandleWindowDestroy(w);
		}

		virtual void IsOnCursor(int pixX, int pixY, CursorHit &cursorHit)
		{
			_parent->IsOnCursor(pixX, pixY, cursorHit);
		}

		virtual void MouseDrag(int x1, int y1, int x2, int y2, int button)
        {
		    _parent->MouseDrag(x1, y1, x2, y2, button);
		}

		virtual void ShowMouseLocation(int *mouseX, int *mouseY)
        {
		    _parent->ShowMouseLocation(mouseX, mouseY);
		}
};

//******************************************************************************
#endif
