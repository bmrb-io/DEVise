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

#define MARGINS

#ifndef ViewWin_h
#define ViewWin_h

#include "DList.h"
#include "DeviseTypes.h"
#include "WindowRep.h"
#include "Color.h"
#include "DualWindowRep.h"

#ifdef TK_WINDOW
#include <tcl.h>
#include <tk.h>
#endif

#define _windowRep _winReps.GetWindowRep()

class ViewWin;
DefinePtrDList(ViewWinList, ViewWin *);
class WindowRep;

class ViewWin: protected WindowRepCallback {
public:
    ViewWin(char *name, Color foreground = ForegroundColor,
            Color background = BackgroundColor, int weight = 1,
            Boolean boundary = true);
    virtual ~ViewWin();

    /* Iconify window, if top level. Not guaranteed to succeed */
    void Iconify();

    DevStatus ExportImage(DisplayExportFormat format, char *filename);

    /* Insert ViewWin into a parent */
    void AppendToParent(ViewWin *parent);

    /* Delete ViewWin from parent */
    void DeleteFromParent();
	
    /* These are called by parents */
    virtual void Map(int x, int y, unsigned w, unsigned h);
    virtual void Unmap();
    virtual void Iconify(Boolean iconified) = 0;

    Boolean Mapped() { return _mapped; }
    char *GetName(){ return _name; }
    int GetWeight() { return _weight; }

    virtual void Raise();
    virtual void Lower();

#if defined(MARGINS) || defined(TK_WINDOW)
    /* Get size of margin controls */
    void GetMargins(unsigned int &lm, unsigned int &rm,
                    unsigned int &tm, unsigned int &bm);
    /* Get real (non-margin-adjusted) geometry of window */
    void RealGeometry(int &x, int &y, unsigned &w, unsigned &h);
#endif

    /* Get current geometry of window */
    void Geometry(int &x, int &y, unsigned &w, unsigned &h);

    /* Get absolute origin of window, if mapped */
    void AbsoluteOrigin(int &x, int &y);

    /* Move/Resize self. Base class creates a new WindowRep
       if not already there and then resizes it */
    virtual void MoveResize(int x, int y, unsigned w, unsigned h);
    
    /* Get the Window Rep of this View window */
    WindowRep *GetWindowRep() { return _windowRep; }

    /* Detach all children from this view */
    void DetachChildren();

    /* Append child. Base class just inserts the child.
       Derived class should implement functions to MoveResize() child.*/
    virtual void Append(ViewWin *child);

    /* Delete child */
    virtual void Delete(ViewWin *child);

    /* Replace child1 by child2. child1 must be a valid child.
       Base class only updates list of children.*/
    virtual void Replace(ViewWin *child1, ViewWin *child2);

    /* Swap child1 and child2. Both must be valid. Base
       class updates only list of children. */
    virtual void SwapChildren(ViewWin *child1, ViewWin *child2);

    /* Clear parent without doing anything else. 
       This is to facilitate implementation of Replace() */
    void SetParent(ViewWin *win) { _parent = win; }

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
     * one that's on top when views are piled, stacked, etc. */
    ViewWin *GetFirstSibling() {
      ViewWin *parent = GetParent();
      DOASSERT(parent, "View has no parent");
      int index = parent->InitIterator();
      DOASSERT(parent->More(index), "Parent view has no children");
      ViewWin *sibling = parent->Next(index);
      parent->DoneIterator(index);
      return sibling;
    }

    
    ViewWin *GetParent() { return _parent; }
    
    /* Return true if window is iconified */
    Boolean Iconified();
    
    Color GetBgColor() { return _background; }
    Color GetFgColor() { return _foreground; }
    virtual void SetFgBgColor(Color fg, Color bg);

    virtual DevStatus PrintPS();
    void SetScreenOutput() { _winReps.SetScreenOutput(); }
    void SetFileOutput(const Rectangle &viewGeom,
      const Rectangle &parentGeom) {
      _winReps.SetFileOutput(viewGeom, parentGeom);
    }

protected:
    /* called by base class when it has been mapped/unmapped */
    virtual void SubClassMapped() = 0;
    virtual void SubClassUnmapped() = 0;

    virtual void HandleResize(WindowRep *w, int xlow, int ylow,
                              unsigned width, unsigned height);
    virtual void HandleWindowMappedInfo(WindowRep *w, Boolean mapped);
    virtual Boolean HandleWindowDestroy(WindowRep *w);

    /* Get total weight of children */
    int TotalWeight();

    Boolean _iconified;
    Boolean _winBoundary;
    Boolean _hasGeometry; /* TRUE if we have the geometry */
    int _x, _y;           /* current location */
    unsigned int _width;  /* current width */
    unsigned int _height; /* current height */
    ViewWinList _children;
    ViewWin *_parent;     /* parent window */
    DualWindowRep _winReps;
    char *_name;          /* name of window */
    int _weight;          /* relative weight of window */
    Boolean _mapped;      /* TRUE if this window is mapped */
    Color _background;    /* background color */
    Color _foreground;    /* foreground color */

#ifdef MARGINS
    void DrawMargins();
#endif

#ifdef TK_WINDOW
    void AddMarginControls();
    void DropMarginControls();
    void DestroyMarginControl(char *side);
    void ReparentMarginControl(char *side, int xoff, int yoff);
    void ResizeMargins(unsigned int w, unsigned int h);
    void ToggleMargins();
    
    Boolean _marginsOn;
    char    _tkPathName[32];
#endif
    
#if defined(MARGINS) || defined(TK_WINDOW)
    unsigned int _leftMargin;
    unsigned int _rightMargin;
    unsigned int _topMargin;
    unsigned int _bottomMargin;
#endif

private:
    Boolean _hasPrintIndex;
    int _printIndex;
};

#endif
