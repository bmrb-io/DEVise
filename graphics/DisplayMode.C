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
  Implementation of DisplayMode class.
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

#include <stdio.h>

#include "DisplayMode.h"
#include "ViewWin.h"
#include "MappingInterp.h"
#include "Control.h"
#include "Util.h"

DisplayMode::Mode DisplayMode::_mode = DisplayMode::ModeNormal;
DisplayModeList DisplayMode::_objects;

#define DEBUG 0

//-----------------------------------------------------------------------------
void
DisplayMode::SetMode(Mode mode)
{
    if (DEBUG >= 1) {
        printf("DisplayMode::SetMode(%d)\n", (int)mode);
    }

    // Check for legal mode.
    switch (mode) {
    case ModeNormal:
    case ModeColorPrint:
    case ModeBWPrint:
	// If mode changes...
        if (mode != _mode) {
	    // ...save the new mode...
	    _mode = mode;

	    // ...refresh all of the DisplayMode objects...
	    int index = _objects.InitIterator();
	    while (_objects.More(index)) {
	        DisplayMode *mode = _objects.Next(index);
		mode->Refresh();
	    }
	    _objects.DoneIterator(index);

	    // ...notify GUI that the mode changed, in case the change was
	    // initiated from the back end...
	    ControlPanel *control = ControlPanel::Instance();
	    control->NotifyFrontEnd("global displayColorMode");

	    const int bufLen = 1024;
	    char buf[bufLen];
	    int formatted = snprintf(buf, bufLen,
	      "set displayColorMode %d", mode);
	    if (checkAndTermBuf(buf, bufLen, formatted).IsComplete()) {
	        control->NotifyFrontEnd(buf);
	    } else {
	        reportErrNosys("GUI was not updated because of buffer "
		  "length problem!");
	    }

	    // ...update the mapping dialog (if it's shown).
	    control->NotifyFrontEnd("UpdateMappingDialogCurrent");
	}
	break;

    default:
	// Note: *not* an assert here, because the mode value can be passed
	// in from the GUI.
	const int bufLen = 1024;
	char buf[bufLen];
	int formatted = snprintf(buf, bufLen, "Illegal display mode: %d",
	  (int)mode);
	reportErrNosys(buf);
        break;
    }
}

//-----------------------------------------------------------------------------
string
DisplayMode::WhiteColorCmd()
{
    if (DEBUG >= 1) {
        printf("DisplayMode::WhiteColorCmd()\n");
    }

    char buf[128];
    int formatted = snprintf(buf, sizeof(buf), "%d",
      (int)GetPColorID(whiteColor));
    checkAndTermBuf2(buf, formatted);

    string cmd(buf);

    if (DEBUG >= 2) {
        printf("  DisplayMode::WhiteColorCmd() returns <%s>\n", cmd.c_str());
    }

    return cmd;
}

//-----------------------------------------------------------------------------
DisplayMode::DisplayMode()
{
    if (DEBUG >= 1) {
        printf("DisplayMode::DisplayMode()\n");
    }

    _objects.Insert(this);
}

//-----------------------------------------------------------------------------
DisplayMode::~DisplayMode()
{
    if (DEBUG >= 1) {
        printf("DisplayMode::~DisplayMode()\n");
    }

    _objects.Delete(this);
}

//=============================================================================

//-----------------------------------------------------------------------------
DisplayModeView::DisplayModeView(ViewWin *viewWin) : DisplayMode()
{
    if (DEBUG >= 1) {
        printf("DisplayModeView::DisplayModeView(%s)\n", viewWin->GetName());
    }

    _viewWin = viewWin;
}

//-----------------------------------------------------------------------------
DisplayModeView::~DisplayModeView()
{
    if (DEBUG >= 1) {
        printf("DisplayModeView::~DisplayModeView()\n");
    }
}

//-----------------------------------------------------------------------------
void
DisplayModeView::SetColor(PColorID color)
{
    if (DEBUG >= 1) {
        printf("DisplayModeView::SetColor(%d)\n", (int)color);
    }

    *(Color4Mode(_mode)) = color;
}

//-----------------------------------------------------------------------------
void
DisplayModeView::SetColor(PColorID color, Mode mode)
{
    if (DEBUG >= 1) {
        printf("DisplayModeView::SetColor(%d, %d)\n", (int)color, (int)mode);
    }

    *(Color4Mode(mode)) = color;
}

//-----------------------------------------------------------------------------
PColorID
DisplayModeView::GetColor() const
{
    if (DEBUG >= 1) {
        printf("DisplayModeView::GetColor()\n");
    }

    return *Color4Mode(_mode);
}

//-----------------------------------------------------------------------------
PColorID
DisplayModeView::GetColor(Mode mode) const
{
    if (DEBUG >= 1) {
        printf("DisplayModeView::GetColor(%d)\n", (int)mode);
    }

    return *Color4Mode(mode);
}

//-----------------------------------------------------------------------------
PColorID *
DisplayModeView::Color4Mode(Mode mode)
{
    if (DEBUG >= 2) {
        printf("DisplayModeView::Color4Mode(%d)\n", (int)mode);
    }

    PColorID *result = NULL;

    switch (mode) {
    case ModeNormal:
	result = &_colorNormal;
        break;

    case ModeColorPrint:
	result = &_colorColorPrint;
        break;

    case ModeBWPrint:
	result = &_colorBWPrint;
        break;

    default:
	DOASSERT(false, "Illegal DisplayMode::Mode value");
    }

    return result;
}

//-----------------------------------------------------------------------------
const PColorID *
DisplayModeView::Color4Mode(Mode mode) const
{
    if (DEBUG >= 2) {
        printf("DisplayModeView::Color4Mode(%d)\n", (int)mode);
    }

    const PColorID *result = NULL;

    switch (mode) {
    case ModeNormal:
	result = &_colorNormal;
        break;

    case ModeColorPrint:
	result = &_colorColorPrint;
        break;

    case ModeBWPrint:
	result = &_colorBWPrint;
        break;

    default:
	DOASSERT(false, "Illegal DisplayMode::Mode value");
    }

    return result;
}

//=============================================================================

//-----------------------------------------------------------------------------
DisplayModeMapping::DisplayModeMapping(MappingInterp *mapping) : DisplayMode()
{
    if (DEBUG >= 1) {
        printf("DisplayModeMapping::DisplayModeMapping(%s)\n",
	  mapping->GetName());
    }

    _mapping = mapping;

    char buf[128];
    int formatted = snprintf(buf, sizeof(buf), "%d",
      (int)GetPColorID(blackColor));
    checkAndTermBuf2(buf, formatted);

    _colorNormal = buf;
    _colorColorPrint = buf;
    _colorBWPrint = buf;
}

//-----------------------------------------------------------------------------
DisplayModeMapping::~DisplayModeMapping()
{
    if (DEBUG >= 1) {
        printf("DisplayModeMapping::~DisplayModeMapping()\n");
    }
}

//-----------------------------------------------------------------------------
void
DisplayModeMapping::SetColor(const char *colorCmd)
{
    if (DEBUG >= 1) {
        printf("DisplayModeMapping::SetColor(%s)\n",
	  colorCmd ? colorCmd : "null");
    }

    if (!colorCmd) colorCmd = "";
    *(Color4Mode(_mode)) = colorCmd;
}

//-----------------------------------------------------------------------------
void
DisplayModeMapping::SetColor(const char *colorCmd, Mode mode)
{
    if (DEBUG >= 1) {
        printf("DisplayModeMapping::SetColor(%s, %d)\n",
	  colorCmd ? colorCmd : "null", (int)mode);
    }

    if (!colorCmd) colorCmd = "";
    *(Color4Mode(mode)) = colorCmd;
}

//-----------------------------------------------------------------------------
const char *
DisplayModeMapping::GetColor() const
{
    if (DEBUG >= 1) {
        printf("DisplayModeMapping::GetColor()\n");
    }

    return Color4Mode(_mode)->c_str();
}

//-----------------------------------------------------------------------------
const char *
DisplayModeMapping::GetColor(Mode mode) const
{
    if (DEBUG >= 1) {
        printf("DisplayModeMapping::GetColor(%d)\n", (int)mode);
    }

    return Color4Mode(mode)->c_str();
}

//-----------------------------------------------------------------------------
void
DisplayModeMapping::Refresh()
{
    if (DEBUG >= 1) {
        printf("DisplayModeMapping::Refresh()\n");
    }

    if (_mapping) {
        _mapping->SetCurrentColor(GetColor());
    }
}

//-----------------------------------------------------------------------------
string *
DisplayModeMapping::Color4Mode(Mode mode)
{
    if (DEBUG >= 2) {
        printf("DisplayModeMapping::Color4Mode(%d)\n", (int)mode);
    }

    string *result = NULL;

    switch (mode) {
    case ModeNormal:
	result = &_colorNormal;
        break;

    case ModeColorPrint:
	result = &_colorColorPrint;
        break;

    case ModeBWPrint:
	result = &_colorBWPrint;
        break;

    default:
	DOASSERT(false, "Illegal DisplayMode::Mode value");
    }

    return result;
}

//-----------------------------------------------------------------------------
const string *
DisplayModeMapping::Color4Mode(Mode mode) const
{
    if (DEBUG >= 2) {
        printf("DisplayModeMapping::Color4Mode(%d)\n", (int)mode);
    }

    const string *result = NULL;

    switch (mode) {
    case ModeNormal:
	result = &_colorNormal;
        break;

    case ModeColorPrint:
	result = &_colorColorPrint;
        break;

    case ModeBWPrint:
	result = &_colorBWPrint;
        break;

    default:
	DOASSERT(false, "Illegal DisplayMode::Mode value");
    }

    return result;
}

//=============================================================================

//-----------------------------------------------------------------------------
DisplayModeViewFG::DisplayModeViewFG(ViewWin *viewWin) :
  DisplayModeView(viewWin)
{
    if (DEBUG >= 1) {
        printf("DisplayModeViewFG::DisplayModeViewFG(%s)\n",
	  viewWin->GetName());
    }

    _colorNormal = GetPColorID(blackColor);
    _colorColorPrint = GetPColorID(blackColor);
    _colorBWPrint = GetPColorID(blackColor);
}

//-----------------------------------------------------------------------------
DisplayModeViewFG::~DisplayModeViewFG()
{
    if (DEBUG >= 1) {
        printf("DisplayModeViewFG::~DisplayModeViewFG()\n");
    }
}

//-----------------------------------------------------------------------------
void
DisplayModeViewFG::Refresh()
{
    if (DEBUG >= 1) {
        printf("DisplayModeViewFG::Refresh()\n");
    }

    // Note: this call will end up calling SetColor() on this object -- I
    // don't think that really hurts anything.  wenger 2003-11-03.
    if (_viewWin) {
        _viewWin->SetForeground(GetColor());
    }
}

//=============================================================================

//-----------------------------------------------------------------------------
DisplayModeViewBG::DisplayModeViewBG(ViewWin *viewWin) : DisplayModeView(viewWin)
{
    if (DEBUG >= 1) {
        printf("DisplayModeViewBG::DisplayModeViewBG(%s)\n",
	  viewWin->GetName());
    }

    _colorNormal = GetPColorID(whiteColor);
    _colorColorPrint = GetPColorID(whiteColor);
    _colorBWPrint = GetPColorID(whiteColor);
}

//-----------------------------------------------------------------------------
DisplayModeViewBG::~DisplayModeViewBG()
{
    if (DEBUG >= 1) {
        printf("DisplayModeViewBG::~DisplayModeViewBG()\n");
    }
}

//-----------------------------------------------------------------------------
void
DisplayModeViewBG::Refresh()
{
    if (DEBUG >= 1) {
        printf("DisplayModeViewBG::Refresh()\n");
    }

    // Note: this call will end up calling SetColor() on this object -- I
    // don't think that really hurts anything.  wenger 2003-11-03.
    if (_viewWin) {
        _viewWin->SetBackground(GetColor());
    }
}

/*===========================================================================*/
