//******************************************************************************
// class ColorManager
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: ColorManager.h
// Last modified: Thu Sep 25 18:32:37 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970331 [weaver]: Original file.
// 970505 [weaver]: New version.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************
//
// An abstract class for color management. Provides a way to map logical RGB
// values into physical RGB values.
//
// Four possibilities:
//
//     1) direct color, direct map (24-bit using logical RGB)
//            supplied RGB value
//
//     2) direct color, indexed map (24-bit using ColorID)
//            lookup the physical RGB in the manager table
//
//     3) indexed color, direct map (8-bit using logical RGB)
//            map the logical RGB onto a ColorID onto a physical RGB
//            (if the logical RGB doesn't exist, match it to a physical color
//                and add a manager entry)?
//
//     4) indexed color, indexed map (8-bit using ColorID)
//            lookup the physical RGB in the manager table
//
// Use only one ColorManager instance in an application.
//
// N.B.: The X in XColorIDs means external, not X windows. That is, the
// physical color mechanism is indexed.
//
//******************************************************************************

#ifndef __CLASS_COLORMANAGER
#define __CLASS_COLORMANAGER

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>
#include <stl.h>

#include "manager.h"
#include "RGB.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

typedef unsigned long	ulong;

typedef ulong	ColorID;
const ColorID	nullColorID = 0;

typedef ulong	XColorID;
const XColorID	nullXColorID = 0;

typedef vector<RGB>		RGBList;

//******************************************************************************
// class ColorManager (abstract)
//******************************************************************************

class ColorManager
{
	protected:

		struct Color
		{
				RGB			rgb;	// RGB requested from device
				XColorID	xcid;	// Device index for color
				ulong		refs;	// Number of uses of this color
		};

		typedef manager<ColorID, Color, less<ColorID> >		LogicalMap;
		typedef LogicalMap::iterator						lmap_iterator;
		typedef LogicalMap::const_iterator					lmap_const_iterator;

		typedef map<RGB, ColorID, less<RGB> >				PhysicalMap;
		typedef PhysicalMap::iterator						pmap_iterator;
		typedef PhysicalMap::const_iterator					pmap_const_iterator;

	protected:

		LogicalMap		lmap;	// Map from ColorID to Color
		PhysicalMap		pmap;	// Map from RGB to ColorID (inverse table)

		int				bits;	// Device bit depth (8, 24, etc.)

	public:

		// Constructors and Destructors
		ColorManager(int depth = 8);
		virtual ~ColorManager(void);

		// Getters and Setters
		bool	GetRGB(ColorID cid, RGB& rgb) const;
		bool	GetXColorID(ColorID cid, XColorID& xcid) const;
		bool	GetColorID(const RGB& rgb, ColorID& cid) const;

		// Utility Functions
		bool	AllocColor(const RGB& rgb);
		bool	FreeColor(const RGB& rgb);

		ColorID		MatchColor(const RGB& rgb) const;
		ColorID		ClosestColor(const RGB& rgb) const;

		void			GetRGBList(RGBList& list) const;

		virtual bool	GetXRGB(ColorID cid, RGB& rgb) const = 0;
		virtual void	GetXRGBList(RGBList& list) const = 0;
		
		// Conversions and Operators

		// Output Functions
		virtual string	ToString(void) const = 0;
		virtual bool	FromString(const string& s) = 0;

	protected:

		// Getters and Setters
		bool	GetRefs(ColorID cid, ulong& refs) const;

		// Utility Functions
		virtual bool	XAllocColor(const RGB& rgb, XColorID& cid) const = 0;
		virtual bool	XFreeColor(XColorID xcid) const = 0;
};

typedef ColorManager*	ColorManagerPtr;

//******************************************************************************
// Getters and Setters (Inline)
//******************************************************************************

inline bool		ColorManager::GetRGB(ColorID cid, RGB& rgb) const
{
	const Color*	color = lmap.Find(cid);

	if (color != NULL)
		rgb = color->rgb;

	return (color != NULL);
}

inline bool		ColorManager::GetXColorID(ColorID cid, XColorID& xcid) const
{
	const Color*	color = lmap.Find(cid);

	if (color != NULL)
		xcid = color->xcid;

	return (color != NULL);
}

inline bool		ColorManager::GetRefs(ColorID cid, ulong& refs) const
{
	const Color*	color = lmap.Find(cid);

	if (color != NULL)
		refs = color->refs;

	return (color != NULL);
}

inline bool		ColorManager::GetColorID(const RGB& rgb, ColorID& cid) const
{
	pmap_const_iterator		i = pmap.find(rgb);

	if (i != pmap.end())
		cid = (*i).second;

	return (i != pmap.end());
}

//******************************************************************************
#endif
