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
  Implementation of XColorManager class.
 */

/*
  $Id$

  $Log$
  Revision 1.5  1998/02/19 23:26:11  wenger
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

//#define DEBUG

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

XColorManager::XColorManager(Display * d, int depth, int nmap, Colormap *map) :
  ColorManager(depth),ncmap(nmap)
{
	cmap=new Colormap[nmap];
	for (int i=0; i<nmap; i++) {
	  cmap[i]=map[i];
	}
	Trace("XColorManager::XColorManager()");

	SetDisplay(d);
}

XColorManager::~XColorManager(void)
{
	Trace("XColorManager::~XColorManager()");

	lmap_iterator		i = lmap.begin();

	while (i != lmap.end())
		XFreeColor((*i).second.xcid);			// Physical free
	delete [] cmap;
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

//	Colormap	map = DefaultColormap(display, DefaultScreen(display));
	Colormap	map = cmap[0];
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

//	Colormap		map = DefaultColormap(display, DefaultScreen(display));
	Colormap	map = cmap[0];
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

	if (display == NULL) {
		cerr << "NULL Display at " << __FILE__ << ": " << __LINE__ << "\n";
		return false;
	}

//	int			scr_num = DefaultScreen(display);
//	Visual*		vis = DefaultVisual(display, scr_num);

    for (int mcount =0 ; mcount < ncmap ; mcount++) {
	Colormap	map = cmap[mcount];
//	Colormap	map = DefaultColormap(display, DefaultScreen(display));
	XColor		xcolor;
	float		error;

	xcolor.red		= rgb.r;
	xcolor.green	= rgb.g;
	xcolor.blue		= rgb.b;

	if (::XAllocColor(display, map, &xcolor))
	{
		xcid = xcolor.pixel;
		error = 0.0;
#if defined(DEBUG)
		cout << "Allocating RGB " << rgb.ToString().c_str() << ": no error\n";
#endif
		
		return true;
	}
#if defined(DEBUG)
	else
	{
		cout << "Allocating RGB " << rgb.ToString().c_str() << " failed\n";
	}
#endif

	RGBList		list(pmap.size());		// Argument is an allocation hint
	bool		isGray = rgb.IsGray();
	
	gRefRGB = rgb;
	GetXRGBList(list);
	sort(list.begin(), list.end(), Compare_RGBDistance);

	RGBList::const_iterator		i = list.begin();

	while (i != list.end())
	{
		error = (*i).Error(rgb);
#if defined(DEBUG)
		cout << "Trying RGB " << (*i).ToString().c_str() << ": " << error <<
		  " error\n";
#endif

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
#if defined(DEBUG)
			cout << "Allocating RGB " << (*i).ToString().c_str() << ": " <<
			  error << " error\n";
#endif

			break;
		}
#if defined(DEBUG)
		else
		{
		    cout << "Allocating RGB " << rgb.ToString().c_str() << " failed\n";
		}
#endif

		i++;
	}
	if (i==list.end())
	  return false;
    }

#if defined(DEBUG)
    cout << "Allocating RGB " << rgb.ToString().c_str() << " failed\n";
#endif
    return true;

}

bool	XColorManager::XFreeColor(XColorID xcid) const
{
	Trace("XColorManager::XFreeColor()");

	if (display == NULL)
		return false;
    for (int mcount =0 ; mcount < ncmap ; mcount++) {
	Colormap	map = cmap[mcount];
//	Colormap	map = DefaultColormap(display, DefaultScreen(display));
	XColor		xcolor;

	xcolor.pixel = xcid;
	
	::XFreeColors(display, map, &xcolor.pixel, 1, 0);
    }

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
