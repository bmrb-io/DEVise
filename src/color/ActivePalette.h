//******************************************************************************
// class ActivePalette
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: ActivePalette.h
// Last modified: Thu Sep 25 18:32:09 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970408 [weaver]: Original file.
// 970505 [weaver]: Modified for new ColorManager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************
//
// An ActivePalette is a table mapping a PColorID into an XColorID.
//
// In DEVise, this allows faster lookup of an XColor pixel given a color value
// calculated from the mapping.
//
// For active use by a session. Whenever there's a change in the corresponding
// Palette, call SetColors().
//
//******************************************************************************

#ifndef __CLASS_ACTIVEPALETTE
#define __CLASS_ACTIVEPALETTE

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>
#include <stl.h>

#include "Palette.h"
#include "ColorManager.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class ActivePalette
//******************************************************************************

class ActivePalette
{
	private:

		typedef vector<PColorID>			Vector;
		typedef Vector::iterator			iterator;
		typedef Vector::const_iterator		const_iterator;

	private:

		Vector		colors;
		
	public:

		// Constructors and Destructors
		ActivePalette(void) {}
		ActivePalette(const Palette& palette, const ColorManager& cm);

		// Getters and Setters
		int			GetSize(void) const			{ return colors.size();	}

		// Utility Functions
		XColorID	GetXColorID(PColorID pcid) const;
		void		SetColors(const Palette& palette, const ColorManager& cm);

		// Conversions and Operators
		XColorID	operator[](PColorID pcid);
		XColorID	operator[](PColorID pcid) const;

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef ActivePalette*	ActivePalettePtr;

//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

inline XColorID		ActivePalette::GetXColorID(PColorID pcid) const
{
	if ((0 <= pcid) && (ulong(pcid) < colors.size()))
		return colors[pcid];
	else
		return nullXColorID;
}

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline XColorID		ActivePalette::operator[](PColorID pcid)
{
	return GetXColorID(pcid);
}

inline XColorID		ActivePalette::operator[](PColorID pcid) const
{
	return GetXColorID(pcid);
}

//******************************************************************************
#endif
