/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
// Defines a unique, global ColorManager and a unique, global, default Palette.
//
// The hard-coded default palette is for use when other palettes (default
// or otherwise) cannot be loaded from a file.
//
// To include basic color management, include this file. For TriColor use,
// include the TriColor classes.
//
// The three global color functions are the last vestage of the presence of
// "hard-coded" colors in the DEVise code. By hiding the actual colors used
// behind function calls, more flexible, context-dependent procedures can be
// introduced later.
//
// This interface is minimal, and will need to be expanded greatly for
// palette swapping, color reference counts, etc.
 */

/*
  $Id$

  $Log$
  Revision 1.4  1998/02/19 23:26:02  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

 */

//******************************************************************************
// Modification History:
//
// 970331 [weaver]: Original file.
// 970409 [weaver]: Modified to add globals gColorManager and gDefaultPalette.
// 971016 [weaver]: Added CM_GetRGBList(), CM_GetColorID().
// 971017 [weaver]: Added lensBackColor.
// 971106 [weaver]: Added basic palette-handling methods.
// 971107 [weaver]: Improved palette initialization, alloc/free and swapping.
// 971203 [weaver]: Added PM_GetRGBList().
// 971204 [weaver]: Added AP_GetRawXColorID().
// 971211 [weaver]: Changed InitColor() return type to bool.
//
//******************************************************************************

#ifndef __COLOR_H
#define __COLOR_H

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>

// Necessary color-related headers (management)
#include "Palette.h"

// Add these in later (color selection stuff)
//#include "TriColor.h"
//#include "RGBColor.h"
//#include "HSVColor.h"

// Coloring.h should probably be included here, but that causes problems
// because of circular dependencies.  RKW Feb. 17, 1998.
class Coloring;

//******************************************************************************
// Constants and Types
//******************************************************************************

const int	gMaxNumColors = 256;			// Artificial maximum palette size

enum GlobalColor
{
	minGlobalColor = 0,

	defForeColor = minGlobalColor,
	defBackColor,
	whiteColor,
	blackColor,

	gifWhiteColor,			// XDisplay.c (MakeAndWriteGif())
	gifBlackColor,			// XDisplay.c (MakeAndWriteGif())
	xWinBackColor,			// XDisplay.c (CreateWindowRep())
	xWinForeColor,			// XDisplay.c (CreateWindowRep())
	psDaliBlackColor,		// PSWindowRep.c (DaliShowImage())
	psDaliWhiteColor,		// PSWindowRep.c (DaliShowImage())
	viewBorderColor,		// View.c (Run(), twice)
	viewGraphLegendColor,	// ViewGraph.c (DrawLegend())

	kgraphForeColor,		// KGraph.C, was ForegroundColor
	kgraphBackColor,		// KGraph.C, was BackgroundColor
	kgraphCircleColor,		// KGraph.C, was BlueColor
	kgraphPointColor,		// KGraph.C, was GreenColor
	kgraphTextColor,		// KGraph.C, was RedColor
	kgraphLabelColor,		// KGraph.C, was BlueColor

	gdatasnapColor,			// GDataSnap.c, was ForegroundColor (still used?)
	map3DframeColor,		// Map3D.C, was BlackColor

	lensBackColor,			// ParseAPI.C (setViewLensParams)
	
	maxGlobalColor
};

typedef unsigned long	PaletteID;
const PaletteID			nullPaletteID = 0;

typedef unsigned long	ColorID;
const ColorID			nullColorID = 0;

typedef vector<RGB>		RGBList;

//******************************************************************************
// Externed Globals
//******************************************************************************

//******************************************************************************
// Function Prototypes (Initialization and Termination)
//******************************************************************************

// Terminates the ColorManager and supporting palettes. Any colors in the
// manager are freed in X.
void		TermColor(void);

//******************************************************************************
// Function Prototypes (Global Colors, Coloring)
//******************************************************************************

// Conversions to and from the color enumeration needed by DEVise for its
// private color needs. The GlobalColor enum is NOT related to its old use.
ColorID		GetColorID(GlobalColor color);
void		SetColorID(GlobalColor color, ColorID cid);

// Conversions to and from the color enumeration needed by DEVise for its
// private color needs. The GlobalColor enum is NOT related to its old use.
PColorID	GetPColorID(GlobalColor color);
void		SetPColorID(GlobalColor color, PColorID pcid);

// A kludge needed by the KGraph code to easily obtain several privately
// allocated colors.
const Coloring&		GetKGraphDefColoring(void);

//******************************************************************************
// Function Prototypes (Utilities, Color Manager)
//******************************************************************************

// Maps (via the ColorManager) a ColorID to an RGB. Returns false if the
// ColorID is invalid (not in the ColorManager), true if valid.
bool		CM_GetRGB(ColorID cid, RGB& rgb);

// Maps (via the ColorManager) an RGB to a ColorID. Returns false if the
// RGB is invalid (not in the ColorManager), true if valid.
bool		CM_GetColorID(const RGB& rgb, ColorID& cid);

// Returns a list of all RGBs currently allocated by the ColorManager.
RGBList		CM_GetRGBList(void);

//******************************************************************************
// Function Prototypes (Utilities, Palette Manager)
//******************************************************************************

// Maps (via the current palette) a PColorID to an RGB. Returns false if the
// PColorID is invalid (not in the current palette), true if valid.
bool		PM_GetRGB(PColorID pcid, RGB& rgb);

// Returns a list of all RGBs in the current palette.
RGBList		PM_GetRGBList(void);

// Maps (via the current palette) an RGB to a PColorID. Returns false if the
// RGB is invalid (not in the current palette), true if valid.
bool		PM_GetPColorID(const RGB& rgb, PColorID& pcid);

// If the requested RGB is in the current palette, returns its PColorID;
// otherwise, attempts to allocate it in the current palette.
bool		PM_GetOrAllocColor(const RGB& rgb, PColorID& pcid);

// Maps (via the PaletteManager) a PColorID to an ColorID. Returns nullColorID
// (invalid ColorID) if the PColorID is invalid (not in the current palette).
ColorID		PM_GetColorID(PColorID pcid);

// Returns the palette corresponding to the specified id, NULL upon failure.
Palette*	PM_GetPalette(PaletteID pid);

// Returns the id of the current palette.
PaletteID	PM_GetCurrentPalette(void);

// Changes the current palette to the one specified. Returns true upon success.
bool		PM_SetCurrentPalette(PaletteID pid);

// Creates a new palette from the \n-delimited string of colors. Does not
// allocate the colors in the palette.
PaletteID	PM_NewPalette(const string& s);

// Deletes a palette. Does not free colors in the palette.
bool		PM_DeletePalette(PaletteID pid);

//******************************************************************************
#endif
