/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2004
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
// A Palette maps PColorID indicies onto PaletteColors.
//
// A. more flexible color addition/removal method will be needed later.
 */

/*
  $Id$

  $Log$
  Revision 1.4.10.2  2004/08/23 22:53:45  wenger
  Fixed bug 890 (problems with drill-down dialog caused by
  some color palettes not having a pure white color).

  Revision 1.4.10.1  2003/12/19 18:12:56  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.4.28.1  2003/12/17 00:18:46  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.4.26.2  2003/12/16 16:09:06  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.4.26.1  2003/04/18 16:11:08  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.4  2001/06/12 15:30:00  wenger
  Implemented a choice of modulus (default) or truncate color modes.

  Revision 1.3  2001/03/23 18:07:05  wenger
  Color palettes are now associated with sessions; added borders to
  color chooser buttons so they're visible even if they're the same
  color as the background; fixed various color-related bugs.

  Revision 1.2  1998/06/24 14:44:36  beyer
  added destructor

  Revision 1.1  1998/02/20 20:44:07  wenger
  Changed color and utils libraries to new export directory scheme for
  dealing with include files (email with complete explanation forthcoming).

  Revision 1.3  1998/02/19 23:26:06  wenger
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
// 970328 [weaver]: Original file.
// 970401 [weaver]: Added FromString().
// 970402 [weaver]: Adjusted iterator constructs, removed the list getters.
// 970407 [weaver]: Changed containment from manager to vector.
// 970505 [weaver]: Modified for new ColorManager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#ifndef __CLASS_PALETTE
#define __CLASS_PALETTE

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>
#include <iostream>
#include <vector>
#include <values.h>

#include "PaletteColor.h"

using namespace std;

//******************************************************************************
// Types and Constants
//******************************************************************************

typedef unsigned long	ulong;

typedef long	PColorID;
const PColorID	nullPColorID = -MAXINT;


//******************************************************************************
// class Palette
//******************************************************************************

class Palette
{
	public:

		typedef vector<PaletteColor>		Vector;
		typedef Vector::iterator			iterator;
		typedef Vector::const_iterator		const_iterator;

	private:

		Vector		colors;

	public:

		// Constructors and Destructors
		Palette(void) {}

		virtual ~Palette() {}

		// Special method to make sure palette has black and white; returns
		// true iff black or white was missing from the palette
		bool EnsureBlackWhite();

		// Getters and Setters
		const Vector&	GetColors(void) const	{ return colors;			}
		int				GetSize(void) const		{ return colors.size();		}
		
		// Utility Functions
		PColorID	NewColor(const PaletteColor& color = PaletteColor());
		bool		DeleteColor(PColorID pcid);

		PaletteColor*			GetColor(PColorID pcid);
		const PaletteColor*		GetColor(PColorID pcid) const;

		bool		GetPColorID(const RGB& rgb, PColorID& pcid);

		// Conversions and Operators
		PaletteColor*			operator[](PColorID pcid);
		const PaletteColor*		operator[](PColorID pcid) const;

		// Iostream Operators
		friend ostream&		operator<<(ostream& s, const Palette& t);
		friend istream&		operator>>(istream& s, Palette& t);

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef Palette*	PalettePtr;

//******************************************************************************
// Getters and Setters (Inline)
//******************************************************************************

//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline PaletteColor*	Palette::operator[](PColorID pcid)
{
	return GetColor(pcid);
}

inline const PaletteColor*	Palette::operator[](PColorID pcid) const
{
	return GetColor(pcid);
}

//******************************************************************************
#endif
