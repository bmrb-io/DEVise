//******************************************************************************
// Color Module
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: Color.h
// Last modified: Thu Dec 11 18:45:17 1997 by Chris Weaver
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
//
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
//
//******************************************************************************

#ifndef __COLOR_H
#define __COLOR_H

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

// All color-related headers (management)
#include "RGB.h"
#include "ColorManager.h"
#include "XColorManager.h"
#include "Palette.h"
#include "PaletteColor.h"
#include "PaletteManager.h"
#include "ActivePalette.h"

// Add these in later (color selection stuff)
//#include "TriColor.h"
//#include "RGBColor.h"
//#include "HSVColor.h"

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

//******************************************************************************
// Externed Globals
//******************************************************************************

//******************************************************************************
// Function Prototypes (Initialization and Termination)
//******************************************************************************

// Initializes the ColorManager and supporting default and active palettes
// given an X display (which must be set previously). The colors in the default
// palette are allocated in X. Returns true if initialization completed, false
// if an error occurred (some colors in the default palette could not be
// allocated).
bool		InitColor(Display* display);

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

// Maps (via the ColorManager) a ColorID to an XColorID. Returns nullXColorID
// (invalid XColorID) if the ColorID is invalid (not in the ColorManager).
XColorID	CM_GetXColorID(ColorID cid);

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

//******************************************************************************
#endif
