/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2003
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DisplayMode class (different modes for printing, etc.).
 
  This class implements multiple "display modes" that each have a different
  set of colors.  The current use, at least, is to be able to automatically
  change the colors to colors suitable for printing.  This is done by storing
  a separate color for each mode.  When the mode is changed, the objects
  of this class set up the views, etc., appropriately, and refresh them.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.3  2003/12/22 23:45:13  wenger
  Print mode symbol colors default to white instead of black for mappings
  whose only symbol type is 'view' (so child view backgrounds are white).

  Revision 1.1.2.2  2003/11/19 19:40:14  wenger
  Display modes now work for symbol colors; also added some missing
  commands to the (horrible) Tcl code for copying views; minor
  improvement to error reporting.

  Revision 1.1.2.1  2003/11/05 17:01:43  wenger
  First part of display modes for printing is implemented (view foreground
  and background colors work, haven't done anything for symbol colors yet).

 */

#ifndef _DisplayMode_
#define _DisplayMode_

#include <sys/types.h>

#include "Color.h"
#include "DList.h"

class ViewWin;
class MappingInterp;
class DisplayMode;
DefinePtrDList(DisplayModeList, DisplayMode *)

//=============================================================================

class DisplayMode
{
public: 
    enum Mode {
        ModeNormal = 0,
	ModeColorPrint = 1,
	ModeBWPrint = 2,
    };

    /**
     * Set the color mode, and update all ViewWin colors if the mode is
     * changed.
     * @param The display mode.
     */
    static void SetMode(Mode mode);

    /**
     * Get the current display mode.
     * @return The mode.
     */
    static Mode GetMode() { return _mode; }

    /**
     * Get the mapping command for a constant color of white.  (Note that
     * the command depends on the current color palette.)
     * @return The color command string.
     */
    static string WhiteColorCmd();

    /**
     * Constructor.
     */
    DisplayMode();

    /**
     * Destructor.
     */
    virtual ~DisplayMode();

    /**
     * Update the colors of the associated ViewWin, if there is one.
     */
    virtual void Refresh() = 0;

protected:
    static Mode _mode;
    static DisplayModeList _objects;
};

//=============================================================================

class DisplayModeView : public DisplayMode
{
public: 
    /**
     * Constructor.
     * @param The associated ViewWin (can be null).
     */
    DisplayModeView(ViewWin *viewWin = NULL);

    /**
     * Destructor.
     */
    virtual ~DisplayModeView();

protected:
    friend class ViewWin;

    /**
     * Set the color for the current mode.
     * @param The new color.
     */
    void SetColor(PColorID color);
    
    /**
     * Set the color for the given mode.
     * @param The new color.
     * @param The mode for which to set the color.
     */
    void SetColor(PColorID color, Mode mode);

    /**
     * Get the color for the current mode.
     * @return The color.
     */
    PColorID GetColor() const;

    /**
     * Get the color for the given mode.
     * @param The mode for which to get the color.
     * @return The color.
     */
    PColorID GetColor(Mode mode) const;

protected:
    /**
     * Get a pointer to this object's color for the given mode.
     * @param 
     * @param The mode for which to get the color.
     * @return A pointer to the color.
     */
    PColorID *Color4Mode(Mode mode);

    /**
     * Get a constant pointer to this object's color for the given mode.
     * @param 
     * @param The mode for which to get the color.
     * @return A constant pointer to the color.
     */
    const PColorID *Color4Mode(Mode mode) const;

    ViewWin *_viewWin;

    PColorID _colorNormal;
    PColorID _colorColorPrint;
    PColorID _colorBWPrint;
};

//=============================================================================

class DisplayModeMapping : public DisplayMode
{
public: 
    /**
     * Constructor.
     * @param The associated MappingInterp (can be null).
     */
    DisplayModeMapping(MappingInterp *mapping = NULL);

    /**
     * Destructor.
     */
    virtual ~DisplayModeMapping();

protected:
    friend class MappingInterp;

    /**
     * Set the color for the current mode.
     * @param The new color.
     */
    void SetColor(const char *colorCmd);
    
    /**
     * Set the color for the given mode.
     * @param The new color.
     * @param The mode for which to set the color.
     */
    void SetColor(const char *colorCmd, Mode mode);

    /**
     * Get the color for the current mode.
     * @return The color.
     */
    const char * GetColor() const;

    /**
     * Get the color for the given mode.
     * @param The mode for which to get the color.
     * @return The color.
     */
    const char * GetColor(Mode mode) const;

    /**
     * Update the colors of the associated MappingInterp, if there is one.
     */
    virtual void Refresh();

protected:
    /**
     * Get a pointer to this object's color for the given mode.
     * @param 
     * @param The mode for which to get the color.
     * @return A pointer to the color.
     */
    string *Color4Mode(Mode mode);

    /**
     * Get a constant pointer to this object's color for the given mode.
     * @param 
     * @param The mode for which to get the color.
     * @return A constant pointer to the color.
     */
    const string *Color4Mode(Mode mode) const;

    MappingInterp *_mapping;

    string _colorNormal;
    string _colorColorPrint;
    string _colorBWPrint;
};

//=============================================================================

class DisplayModeViewFG : public DisplayModeView
{
public:
    /**
     * Constructor.
     * @param The associated ViewWin (can be null).
     */
    DisplayModeViewFG(ViewWin *viewWin = NULL);

    /**
     * Destructor.
     */
    virtual ~DisplayModeViewFG();

    /**
     * Update the colors of the associated ViewWin, if there is one.
     */
    virtual void Refresh();
};

//=============================================================================

class DisplayModeViewBG : public DisplayModeView
{
public:
    /**
     * Constructor.
     * @param The associated ViewWin (can be null).
     */
    DisplayModeViewBG(ViewWin *viewWin = NULL);

    /**
     * Destructor.
     */
    virtual ~DisplayModeViewBG();

    /**
     * Update the colors of the associated ViewWin, if there is one.
     */
    virtual void Refresh();
};

#endif // _DisplayMode_

/*============================================================================*/
