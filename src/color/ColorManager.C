//******************************************************************************
// class ColorManager
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: ColorManager.C
// Last modified: Thu Dec 11 18:49:18 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970331 [weaver]: Original file.
// 970505 [weaver]: New version.
// 970520 [weaver]: Added trace points.
//
//******************************************************************************

#include "ColorManager.h"

//******************************************************************************
// Libraries
//******************************************************************************

#if defined(SUN)
#include <values.h>
#  define DBL_MAX MAXDOUBLE
#endif

//#define INLINE_TRACE
#include "debug.h"

//#define DEBUG

//******************************************************************************
// Static Member Definitions
//******************************************************************************

//******************************************************************************
// Constructors & Destructors
//******************************************************************************

ColorManager::ColorManager(int depth)
	: bits(depth)
{
	Trace("ColorManager::ColorManager()");
}

ColorManager::~ColorManager(void)
{
	Trace("ColorManager::~ColorManager()");
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

//******************************************************************************
// Utility Functions
//******************************************************************************

bool	ColorManager::AllocColor(const RGB& rgb)
{
	Trace("ColorManager::AllocColor()");

	ColorID		cid;
	XColorID	xcid;
	Color*		color;

	if (GetColorID(rgb, cid))					// Look up rgb in logical map
	{
		color = lmap.Find(cid);					// Get logical entry

		if (color == NULL)
		{
			cerr << "Internal inconsistency at " << __FILE__ << ": " <<
			  __LINE__ << "\n";
			return false;						// Bad things happening...
		}

		color->refs++;							// Incr references		

		return true;
	}

	if (!XAllocColor(rgb, xcid))
	{
#if defined(DEBUG)
		cerr << "Physical color allocation failed for color " <<
		  rgb.ToString() << "\n";
#endif
		return false;
    }

	cid = lmap.GetID();							// Create new logical entry
	color = lmap.Find(cid);						// Get new entry

	color->rgb	= rgb;							// Set entry field values
	color->xcid	= xcid;
	color->refs	= 1;

	pmap[rgb] = cid;							// Create new physical entry

	return true;
}

bool	ColorManager::FreeColor(const RGB& rgb)
{
	Trace("ColorManager::FreeColor()");

	ColorID		cid;
	Color*		color;

	if (!GetColorID(rgb, cid))					// Look up rgb in physical map
		return false;							// Color not allocated

	color = lmap.Find(cid);						// Get logical entry

	if (color == NULL)
		return false;							// Bad things happening...

	if (color->refs == 1)						// Last reference, free color
		if (!XFreeColor(color->xcid))			// Physical free
			return false;						// Free failed

	if (--color->refs == 0)						// Decr references. None left?
	{
		lmap.PutID(cid);						// Remove logical entry
		pmap.erase(rgb);						// Remove physical entry
	}
	
	return true;
}

// Finds the id of the color closest to the supplied color, using the RGB
// distance function.
ColorID		ColorManager::MatchColor(const RGB& rgb) const
{
	Trace("ColorManager::MatchColor()");

	lmap_const_iterator		i = lmap.begin();
	double					d, dmin = DBL_MAX;
	ColorID					cid = 0, cidmin = 0;

	while (i != lmap.end())
	{
		d = rgb.Distance((*i).second.rgb);

		if (d < dmin)
			dmin = d, cidmin = cid;

		i++, cid++;
	}

	return cidmin;
}

// As MatchColor(), but matching grays to grays and colors to colors if
// possible. (Similar to the original XDisplay::ClosestColor(), without
// communication with X.)
ColorID		ColorManager::ClosestColor(const RGB& rgb) const
{
	Trace("ColorManager::ClosestColor()");

	lmap_const_iterator		i = lmap.begin();
	double					d, dmin = DBL_MAX;
	ColorID					cid = 0, cidmin = 0;
	bool					isGray = rgb.IsGray(), found = false;
	
	while (i != lmap.end())
	{
		if (!isGray ^ (*i).second.rgb.IsGray())
		{
			d = rgb.Distance((*i).second.rgb);

			if (d < dmin)
				dmin = d, cidmin = cid;

			found = true;
		}

		i++, cid++;
	}

	// If gray-gray or color-color match failed, do full match.
	return (found ? cidmin : MatchColor(rgb));
}

// Builds a list of RGBs requested of the ColorManager. The RGBs are not
// necessarily the ones physically allocated.
void	ColorManager::GetRGBList(RGBList& list) const
{
	Trace("ColorManager::GetRGBList()");

	list.erase(list.begin(), list.end());

	pmap_const_iterator		i = pmap.begin();

	while (i != pmap.end())
		list.push_back((*i++).first);
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************

//******************************************************************************
// Iostream Operators
//******************************************************************************

//******************************************************************************
// Output Functions
//******************************************************************************

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
