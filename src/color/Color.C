//******************************************************************************
// Color Module
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: Color.h
// Last modified: Mon Nov 24 14:09:48 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970409 [weaver]: Original file.
// 970513 [weaver]: Updated to standard C++ class String.
// 970513 [weaver]: Added trace points.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971016 [weaver]: Added CM_GetRGBList(), CM_GetColorID().
// 971017 [weaver]: Added lensBackColor.
// 971106 [weaver]: Added basic palette-handling methods.
// 971107 [weaver]: Improved palette initialization, alloc/free and swapping.
//
//******************************************************************************

#include "Color.h"

//******************************************************************************
// Libraries
//******************************************************************************

#include <iostream.h>

#include "Coloring.h"

//#define INLINE_TRACE
#include "debug.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

static const ColorID	gNumCoreColors	= 6;
static const ColorID	gNumDefColors	= 38;

static const string		gCoreColors[gNumCoreColors] =
{
	"#000000000000 black",
	"#00000000ff00 blue",
	"#000064000000 DarkGreen",
	"#0000ff000000 green",
	"#ff0000000000 red",
	"#ffffffffffff white",
};

static const string		gDefColors[gNumDefColors] =
{
	"#000000000000 black",
	"#000000008b00 DarkBlue",
	"#00000000cd00 MediumBlue",
	"#00000000ff00 blue",
	"#000064000000 DarkGreen",
	"#00008b000000 green4",
	"#0000bf00ff00 DeepSkyBlue1",
	"#0000ff000000 green",
	"#0000ff00ff00 cyan",
	"#190019007000 MidnightBlue",
	"#1e009000ff00 DodgerBlue",
	"#1f001f001f00 gray12",
	"#22008b002200 ForestGreen",
	"#400040004000 gray25",
	"#610061006100 gray38",
	"#7f007f007f00 gray50",
	"#8b0000000000 DarkRed",
	"#8b0045001300 SaddleBrown",
	"#99003200cc00 DarkOrchid",
	"#a100a100a100 gray63",
	"#b20022002200 firebrick",
	"#b4005200cd00 MediumOrchid3",
	"#bf00bf00bf00 gray75",
	"#c000ff003e00 OliveDrab",
	"#e000e000e000 gray88",
	"#ee00ad000e00 Goldenrod2",
	"#ee00e800aa00 PaleGoldenrod",
	"#f000e6008c00 khaki",
	"#f500de00b300 wheat",
	"#fa00fa00d200 LightGoldenrodYellow",
	"#fd00f500e600 OldLace",
	"#ff0000000000 red",
	"#ff000000ff00 magenta",
	"#ff007f000000 DarkOrange1",
	"#ff00a5000000 orange",
	"#ff00d7000000 gold",
	"#ff00ff000000 yellow",
	"#ffffffffffff white",
};

//******************************************************************************
// Globals
//******************************************************************************

static ColorManager*	gColorManager	= NULL;		// Unique ColorManager
static PaletteManager*	gPaletteManager	= NULL;		// Unique PaletteManager

static PaletteID		gCorePaletteID;				// Core PaletteID
static Palette*			gCorePalette;				// Core Palette

static PaletteID		gDefaultPaletteID;			// Default PaletteID
static Palette*			gDefaultPalette	= NULL;		// Default Palette

static PaletteID		gCurrentPaletteID;			// Current PaletteID
static Palette*			gCurrentPalette	= NULL;		// Current Palette

static ActivePalette*	gActivePalette	= NULL;		// ActivePalette

// These are more devise-specific than the others
static ColorID			globalColors[maxGlobalColor];
static Coloring			kgraphDefColoring;

//******************************************************************************
// Function Prototypes
//******************************************************************************

static void		SetupColors(void);
static void		InitKGraphDefaultColoring(void);

// Allocates the colors in the palette. All or nothing.
static bool		AllocPalette(PaletteID pid);

// Frees the colors in the palette.
static bool		FreePalette(PaletteID pid);

//******************************************************************************
// Initialization Functions
//******************************************************************************

void	InitColor(Display* display)
{
	Trace("InitColor()");

	Palette*	palette;
	PaletteID	pid;

	// Initialize managers, active palette
	gColorManager		= new XColorManager(display);	// Color manager
	gPaletteManager		= new PaletteManager();			// Palette manager
	gActivePalette		= new ActivePalette();			// Active palette

	// Initialize core palette
	pid					= gPaletteManager->NewPalette();
	palette				= (*gPaletteManager)[pid];

	for (ColorID i=0; i<gNumCoreColors; i++)
	{
		PaletteColor	pc;

		pc.FromString(gCoreColors[i]);
		palette->NewColor(pc);
	}

	if (!AllocPalette(pid))
		return;

	gCorePaletteID		= pid;
	gCorePalette		= palette;

	// Initialize default palette
	pid					= gPaletteManager->NewPalette();
	palette				= (*gPaletteManager)[pid];

	for (ColorID i=0; i<gNumDefColors; i++)
	{
		PaletteColor	pc;

		pc.FromString(gDefColors[i]);
		palette->NewColor(pc);
	}

	if (!AllocPalette(pid))
		return;

	gDefaultPaletteID	= pid;
	gDefaultPalette		= palette;

	// Allocate and install active palette (using default palette)
	gCurrentPaletteID	= gDefaultPaletteID;
	gCurrentPalette		= gDefaultPalette;

	gActivePalette->SetColors(*gCurrentPalette, *gColorManager);

	// Initialize "hard-coded" color scheme
	SetupColors();
	InitKGraphDefaultColoring();
}

void	TermColor(void)
{
	Trace("TermColor()");

	delete gActivePalette;
	delete gDefaultPalette;
	delete gPaletteManager;
	delete gColorManager;
}

// Allocates the default palette then assigns global colors.
void	SetupColors(void)
{
	Trace("SetupColors()");

	RGB			rgb;
	ColorID		cid;

	// Alloc black
	rgb = gCorePalette->GetColor(0)->GetColor();
	gColorManager->GetColorID(rgb, cid);

	SetColorID(blackColor, cid);
	SetColorID(gifBlackColor, cid);
	SetColorID(map3DframeColor, cid);
	SetColorID(xWinForeColor, cid);
	SetColorID(psDaliBlackColor, cid);
	SetColorID(lensBackColor, cid);

	// Alloc blue
	rgb = gCorePalette->GetColor(1)->GetColor();
	gColorManager->GetColorID(rgb, cid);

	SetColorID(kgraphCircleColor, cid);
	SetColorID(kgraphLabelColor, cid);

	// Alloc dark green
	rgb = gCorePalette->GetColor(2)->GetColor();
	gColorManager->GetColorID(rgb, cid);

	SetColorID(defForeColor, cid);
	SetColorID(kgraphForeColor, cid);
	SetColorID(gdatasnapColor, cid);
	SetColorID(viewBorderColor, cid);
	SetColorID(viewGraphLegendColor, cid);

	// Alloc green
	rgb = gCorePalette->GetColor(3)->GetColor();
	gColorManager->GetColorID(rgb, cid);

	SetColorID(kgraphPointColor, cid);

	// Alloc red
	rgb = gCorePalette->GetColor(4)->GetColor();
	gColorManager->GetColorID(rgb, cid);

	SetColorID(kgraphTextColor, cid);

	// Alloc white
	rgb = gCorePalette->GetColor(5)->GetColor();
	gColorManager->GetColorID(rgb, cid);

	SetColorID(defBackColor, cid);
	SetColorID(whiteColor, cid);
	SetColorID(gifWhiteColor, cid);
	SetColorID(kgraphBackColor, cid);
	SetColorID(xWinBackColor, cid);
	SetColorID(psDaliWhiteColor, cid);
}

// This is a workaround for now. There should be a single default global
// Coloring object for KGraphs.
void	InitKGraphDefaultColoring(void)
{
	Trace("InitKGraphDefaultColoring()");

	kgraphDefColoring.SetForeground(GetColorID(kgraphForeColor));
	kgraphDefColoring.SetBackground(GetColorID(kgraphBackColor));

	kgraphDefColoring.AddDataColor(GetColorID(kgraphCircleColor));
	kgraphDefColoring.AddDataColor(GetColorID(kgraphPointColor));
	kgraphDefColoring.AddDataColor(GetColorID(kgraphTextColor));
	kgraphDefColoring.AddDataColor(GetColorID(kgraphLabelColor));
}

//******************************************************************************
// Utility Functions (Global Colors, Colorings)
//******************************************************************************

ColorID		GetColorID(GlobalColor color)
{
	if ((minGlobalColor <= color) && (color < maxGlobalColor))
		return globalColors[color];
	else
		return globalColors[defForeColor];
}

void		SetColorID(GlobalColor color, ColorID cid)
{
	if ((minGlobalColor <= color) && (color < maxGlobalColor))
		globalColors[color] = cid;
}

PColorID	GetPColorID(GlobalColor color)
{
	ColorID		cid = GetColorID(color);
	RGB			rgb;
	PColorID	pcid;

	if (gColorManager == NULL)
		return nullPColorID;

	gColorManager->GetRGB(cid, rgb);

	if (PM_GetPColorID(rgb, pcid))
		return pcid;
	else
		return nullPColorID;
}

void		SetPColorID(GlobalColor color, PColorID pcid)
{
	ColorID		cid = GetColorID(color);
	RGB			rgb;

	gColorManager->GetRGB(cid, rgb);

	PColorID	fake = pcid;
}

const Coloring&		GetKGraphDefColoring(void)
{
	return kgraphDefColoring;
}

//******************************************************************************
// Utility Functions (Color Manager)
//******************************************************************************

// Watch out for the nullXColorID case.
XColorID	CM_GetXColorID(ColorID cid)
{
	XColorID	xcid;
	
	if (gColorManager->GetXColorID(cid, xcid))
		return xcid;

	return nullXColorID;
}

bool		CM_GetRGB(ColorID cid, RGB& rgb)
{
	return gColorManager->GetRGB(cid, rgb);
}

bool		CM_GetColorID(const RGB& rgb, ColorID& cid)
{
	return gColorManager->GetColorID(rgb, cid);
}

RGBList		CM_GetRGBList()
{
	RGBList		rgbList;

	gColorManager->GetRGBList(rgbList);

	return rgbList;
}

//******************************************************************************
// Utility Functions (Palette Manager)
//******************************************************************************

bool		PM_GetRGB(PColorID pcid, RGB& rgb)
{
	Trace("PM_GetRGB()");

	const PaletteColor*		pc = (*gCurrentPalette)[pcid];

	if (pc == NULL)
		return false;					// Couldn't find id in palette
	
	rgb = pc->GetColor();

	return true;
}

bool		PM_GetPColorID(const RGB& rgb, PColorID& pcid)
{
	Trace("PM_GetPColorID()");

	for (PColorID i=0; i<gCurrentPalette->GetSize(); i++)
	{
		if (gCurrentPalette->GetColor(i)->GetColor() == rgb)
		{
			pcid = i;
			return true;
		}
	}

	return false;
}

ColorID		PM_GetColorID(PColorID pcid)
{
	Trace("PM_GetColorID()");

	const PaletteColor*		pc = (*gCurrentPalette)[pcid];

	if (pc == NULL)
		return nullColorID;				// Couldn't find id in palette

	const RGB&	rgb = pc->GetColor();
	ColorID		cid;
	
	if (!gColorManager->GetColorID(rgb, cid))
		return nullColorID;				// Couldn't find RGB in CM

	return cid;
}

Palette*	PM_GetPalette(PaletteID pid)
{
	Trace("PM_GetPalette()");

	if (pid == gCorePaletteID)
		return NULL;

	return gPaletteManager->GetPalette(pid);
}

PaletteID	PM_GetCurrentPalette(void)
{
	Trace("PM_GetCurrentPalette()");

	return gCurrentPaletteID;
}

bool		PM_SetCurrentPalette(PaletteID pid)
{
	Trace("PM_SetCurrentPalette()");

	if (pid == gCorePaletteID)
		return false;

	if (pid == gCurrentPaletteID)
		return true;

	Palette*	palette = gPaletteManager->GetPalette(pid);

	if (palette == NULL)
		return false;

	if (!FreePalette(gCurrentPaletteID))
		return false;

	if (!AllocPalette(pid))
	{
		AllocPalette(gCurrentPaletteID);
		return false;
	}

	gCurrentPaletteID	= pid;
	gCurrentPalette		= palette;

	gActivePalette->SetColors(*gCurrentPalette, *gColorManager);

	return true;
}

PaletteID	PM_NewPalette(const string& s)
{
	Trace("PM_NewPalette()");

	PaletteID	pid = gPaletteManager->NewPalette();
	Palette*	palette = gPaletteManager->GetPalette(pid);

	if (pid == nullPaletteID)
		return nullPaletteID;

	if (!palette->FromString(s))
	{
		gPaletteManager->DeletePalette(pid);
		return nullPaletteID;
	}

	return pid;
}

bool		PM_DeletePalette(PaletteID pid)
{
	Trace("PM_DeletePalette()");

	if ((pid == gCorePaletteID) || (pid == gDefaultPaletteID) ||
		(pid == gCurrentPaletteID))
		return false;

	Palette*	palette = gPaletteManager->GetPalette(pid);

	if (palette == NULL)
		return false;

	return gPaletteManager->DeletePalette(pid);
}

bool		AllocPalette(PaletteID pid)
{
	Trace("PM_AllocPalette()");

	Palette*	palette = gPaletteManager->GetPalette(pid);

	if (palette == NULL)
		return false;

	for (int i=0; i<palette->GetSize(); i++)
	{
		RGB		rgb = palette->GetColor(i)->GetColor();

		if (!gColorManager->AllocColor(rgb))
		{
			for (int j=0; j<i; j++)
			{
				rgb = palette->GetColor(j)->GetColor();
				gColorManager->FreeColor(rgb);
			}

			return false;
		}
	}

	return true;
}

bool		FreePalette(PaletteID pid)
{
	Trace("PM_FreePalette()");

	Palette*	palette = gPaletteManager->GetPalette(pid);
	bool		result = true;

	if (palette == NULL)
		return false;

	for (int i=0; i<palette->GetSize(); i++)
	{
		RGB		rgb = palette->GetColor(i)->GetColor();
		
		if (!gColorManager->FreeColor(rgb))
			result = false;
	}

	return result;
}

//******************************************************************************
// Utility Functions (Active Palette)
//******************************************************************************

XColorID	AP_GetXColorID(PColorID pcid)
{
	Trace("AP_GetXColorID()");

	PColorID	rpcid = pcid;

	if (pcid == nullPColorID)
		rpcid = 0;
	else if (pcid < 0)
		rpcid = -pcid;

	return (*gActivePalette)[rpcid % gActivePalette->GetSize()];

//	return (*gActivePalette)[pcid];
}

//******************************************************************************
