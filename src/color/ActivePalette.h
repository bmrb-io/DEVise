/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2003
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
  Revision 1.5.10.1  2003/12/19 18:12:49  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.5.28.1  2003/12/17 00:18:40  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.5.26.2  2003/12/16 16:09:00  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.5.26.1  2003/04/18 16:10:59  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.5  2001/06/12 15:29:52  wenger
  Implemented a choice of modulus (default) or truncate color modes.

  Revision 1.4  1998/06/24 14:44:02  beyer
  added destructor

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

#include "Palette.h"
#include "ColorManager.h"
#include "ColorUtil.h"

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
	return colors[PColorID2Index(pcid, colors.size())];
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
