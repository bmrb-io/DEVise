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
// An ActivePalette is a table mapping a PColorID into an XColorID.
//
// In DEVise, this allows faster lookup of an XColor pixel given a color value
// calculated from the mapping.
//
// For active use by a session. Whenever there's a change in the corresponding
// Palette, call SetColors().
 */

/*
  $Id$

  $Log$
  Revision 1.3  1998/02/19 23:26:01  wenger
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
// 970408 [weaver]: Original file.
// 970505 [weaver]: Modified for new ColorManager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
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
		ActivePalette() {}
		ActivePalette(const Palette& palette, const ColorManager& cm);

		virtual ~ActivePalette() {}

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
