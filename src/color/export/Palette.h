/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2001
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
#include <stl.h>
#include <iostream.h>
#include <values.h>

#include "PaletteColor.h"

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

		// Getters and Setters
		const Vector&	GetColors(void) const	{ return colors;			}
		int				GetSize(void) const		{ return colors.size();		}
		
		// Utility Functions
		PColorID	NewColor(const PaletteColor& color = PaletteColor());
		bool		DeleteColor(PColorID pcid);

		PaletteColor*			GetColor(PColorID pcid);
		const PaletteColor*		GetColor(PColorID pcid) const;

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
