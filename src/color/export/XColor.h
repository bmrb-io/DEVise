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
  X Windows-specific parts of color manager interface.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/02/20 20:44:08  wenger
  Changed color and utils libraries to new export directory scheme for
  dealing with include files (email with complete explanation forthcoming).

  Revision 1.1  1998/02/19 23:26:11  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

 */

#ifndef _XColor_h_
#define _XColor_h_

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

//******************************************************************************
// Types and Constants
//******************************************************************************

// The 'X' in 'XColorID' is supposed to be for 'external', not 'X Windows'.
// However, I'm leaving it in here because it is somewhat system-specific
// in that it assumes that a color can be represented by an unsigned long.
// RKW Feb. 17, 1998.
typedef unsigned long	XColorID;
const XColorID		nullXColorID = 0;

//******************************************************************************
// Function Prototypes (Initialization and Termination)
//******************************************************************************

// Initializes the ColorManager and supporting default and active palettes
// given an X display (which must be set previously). The colors in the default
// palette are allocated in X. Returns true if initialization completed, false
// if an error occurred (some colors in the default palette could not be
// allocated).
bool		InitColor(Display * d, int depth, int ncmap, Colormap * cmap);


//******************************************************************************
// Function Prototypes (Utilities, Color Manager)
//******************************************************************************

// Maps (via the ColorManager) a ColorID to an XColorID. Returns nullXColorID
// (invalid XColorID) if the ColorID is invalid (not in the ColorManager).
XColorID	CM_GetXColorID(ColorID cid);


//******************************************************************************
// Function Prototypes (Utilities, Active Palette)
//******************************************************************************

// Maps (via the Active Palette) a PColorID (modulus the current palette size)
// to an XColorID. ALWAYS returns a valid XColorID.
// The mapping is an array lookup operation - use whenever possible for speed.
XColorID	AP_GetXColorID(PColorID cid);

// Maps (via the Active Palette) to an XColorID. Returns nullXColorID (invalid
// XColorID) if the PColorID is invalid (not in the active palette). This is a
// simple array lookup operation - use whenever possible for speed.
XColorID	AP_GetRawXColorID(PColorID pcid);


#endif /* _XColor_h_ */

/*============================================================================*/
