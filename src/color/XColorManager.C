//******************************************************************************
// class XColorManager
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: XColorManager.C
// Last modified: Thu Dec 11 19:40:39 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970331 [weaver]: Original file.
// 970513 [weaver]: Updated to standard C++ class String.
// 970520 [weaver]: Added trace points.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#include "XColorManager.h"

//******************************************************************************
// Libraries
//******************************************************************************

#include <cstdio>

//#define INLINE_TRACE
#include "debug.h"

//******************************************************************************
// Special Purpose Code for XColorManager::XAllocColor
//******************************************************************************

static const double		gMaxColorErr = 0.25;
static RGB				gRefRGB;

static bool		Compare_RGBDistance(const RGB& rgb1, const RGB& rgb2);

bool	Compare_RGBDistance(const RGB& rgb1, const RGB& rgb2)
{
	return (gRefRGB.Distance(rgb1) < gRefRGB.Distance(rgb2));
}

//******************************************************************************
// Static Member Definitions
//******************************************************************************

//******************************************************************************
// Constructors & Destructors
//******************************************************************************

XColorManager::XColorManager(Display* d)
{
	Trace("XColorManager::XColorManager()");

	SetDisplay(d);
}

XColorManager::~XColorManager(void)
{
	Trace("XColorManager::~XColorManager()");

	lmap_iterator		i = lmap.begin();

	while (i != lmap.end())
		XFreeColor((*i).second.xcid);			// Physical free
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

//******************************************************************************
// Utility Functions
//******************************************************************************

// Gets the physical (not necessarily the requested) color corresponding to the
// supplied ColorID.
bool	XColorManager::GetXRGB(ColorID cid, RGB& rgb) const
{
	Trace("XColorManager::GetXRGB()");

	if (display == NULL)
		return false;

	Colormap	map = DefaultColormap(display, DefaultScreen(display));
	XColor		xcolor;
	
	if (!GetXColorID(cid, xcolor.pixel))
		return false;

	::XQueryColor(display, map, &xcolor);

	rgb.r = xcolor.red;
	rgb.g = xcolor.green;
	rgb.b = xcolor.blue;

	return true;
}

// Builds a list of RGBs physically allocated by the ColorManager. The RGBs are
// not necessarily the ones originally requested.
void	XColorManager::GetXRGBList(RGBList& list) const
{
	Trace("XColorManager::GetXRGBList()");

	list.erase(list.begin(), list.end());

	if (display == NULL)
		return;

	Colormap		map = DefaultColormap(display, DefaultScreen(display));
	const ulong		mapSize = 256;
	XColor			xc[mapSize];

	for (ulong i=0; i<mapSize; i++)
		xc[i].pixel = i;

	::XQueryColors(display, map, xc, mapSize);

	for (ulong i=0; i<mapSize; i++)
		list.push_back(RGB(xc[i].red, xc[i].green, xc[i].blue));
}

// Grab color, grab closest color, or remap to auto choice (later).
bool	XColorManager::XAllocColor(const RGB& rgb, XColorID& xcid) const
{
	Trace("XColorManager::XAllocColor()");

	if (display == NULL)
		return false;

	int			scr_num = DefaultScreen(display);
	Visual*		vis = DefaultVisual(display, scr_num);

    // Haven't thought through / don't have machines to try / don't want
    // to do this case.
	if (vis->c_class != PseudoColor || DefaultDepth(display, scr_num) != 8)
		return false;

	Colormap	map = DefaultColormap(display, DefaultScreen(display));
	XColor		xcolor;
	float		error;

	xcolor.red		= rgb.r;
	xcolor.green	= rgb.g;
	xcolor.blue		= rgb.b;

	if (::XAllocColor(display, map, &xcolor))
	{
		xcid = xcolor.pixel;
		error = 0.0;
//		printf("Allocating RGB %s: no error\n", rgb.ToString().c_str());
		
		return true;
	}

	RGBList		list(pmap.size());		// Argument is an allocation hint
	bool		isGray = rgb.IsGray();
	
	gRefRGB = rgb;
	GetXRGBList(list);
	sort(list.begin(), list.end(), Compare_RGBDistance);

	RGBList::const_iterator		i = list.begin();

	while (i != list.end())
	{
		error = (*i).Error(rgb);
//		printf("Trying RGB %s: %g error\n", (*i).ToString().c_str(),
//			   error);

		if (error > gMaxColorErr)
			break;

		if (!(!isGray ^ (*i).IsGray()))
		{
			i++;
			continue;
		}

		xcolor.red		= (*i).r;
		xcolor.green	= (*i).g;
		xcolor.blue		= (*i).b;

		if (::XAllocColor(display, map, &xcolor))
		{
			xcid = xcolor.pixel;
			error = (*i).Error(rgb);
//			printf("Allocating RGB %s: %g error\n", rgb.ToString().c_str(),
//				   error);

			return true;
		}

		i++;
	}

//	printf("Allocating RGB %s: failed\n", rgb.ToString().c_str());

	return false;
}

bool	XColorManager::XFreeColor(XColorID xcid) const
{
	Trace("XColorManager::XFreeColor()");

	if (display == NULL)
		return false;

	Colormap	map = DefaultColormap(display, DefaultScreen(display));
	XColor		xcolor;

	xcolor.pixel = xcid;
	
	::XFreeColors(display, map, &xcolor.pixel, 1, 0);

	return true;
}

//******************************************************************************
// Output Functions
//******************************************************************************

string	XColorManager::ToString(void) const
{
	return string();
}

bool	XColorManager::FromString(const string& /*s*/)
{
	return false;
}

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
