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
// RGBColor contains a red-green-blue (RGB) representation of color.
//
// DEVise doesn't use this code yet. It will be used for more flexible color
// selection (via the tcl interface) and more advanced mappings->color
// conversions later.
//
// An RGBColor object has the same structure as an RGB object, but is used for
// different purposes (and is more flexible).
 */

/*
  $Id$

  $Log$
  Revision 1.3.26.1  2003/12/19 18:12:50  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.3.44.1  2003/12/17 00:18:40  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.3.42.1  2003/04/18 16:11:01  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.3  1998/02/19 23:26:10  wenger
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
// 970320 [weaver]: Original file.
// 970331 [weaver]: Added iostream operators and FromString().
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#ifndef __CLASS_RGBCOLOR
#define __CLASS_RGBCOLOR

//******************************************************************************
// Libraries
//******************************************************************************

#include <iostream>
#include <string>
#include <cmath>

#include "TriColor.h"
#include "RGB.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class RGBColor
//******************************************************************************

class RGBColor : public TriColor
{
	private:

		RGBValue	r;		// Red:   [0,65535]
		RGBValue	g;		// Green: [0,65535]
		RGBValue	b;		// Blue:  [0,65535]

	public:

		// Constructors and Destructors
		RGBColor(RGBValue rr = 0, RGBValue gg = 0, RGBValue bb = 0);
		RGBColor(const RGB& rgb)		{ FromRGB(rgb);				}
		RGBColor(const TriColor& color)	{ FromRGB(color.ToRGB());	}
		virtual ~RGBColor(void) {}

		// Getters and Setters
		RGBValue	GetRed(void) const		{ return r;	}
		RGBValue	GetGreen(void) const	{ return g;	}
		RGBValue	GetBlue(void) const		{ return b;	}

		void	SetRed(RGBValue rr)		{ r = rr;		}
		void	SetGreen(RGBValue gg)	{ g = gg;		}
		void	SetBlue(RGBValue bb)	{ b = bb;		}

		// Utility Functions
		virtual RGB		ToRGB(void) const;
		virtual void	FromRGB(const RGB& rgb);

		virtual bool	IsGray(void) const;

		// Conversions and Operators
		bool	operator==(const RGBColor& t) const;
		bool	operator<(const RGBColor& t) const;

		// Iostream Operators
		friend ostream&		operator<<(ostream& s, const RGBColor& t);
		friend istream&		operator>>(istream& s, RGBColor& t);

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef RGBColor*	RGBColorPtr;

//******************************************************************************
// Constructors and Destructors (Inline)
//******************************************************************************

inline	RGBColor::RGBColor(RGBValue rr, RGBValue gg, RGBValue bb)
	: r(rr), g(gg), b(bb)
{
}

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline bool		RGBColor::operator==(const RGBColor& t) const
{
	return ((r == t.r) && (g == t.g) && (b == t.b));
}

inline bool		RGBColor::operator<(const RGBColor& t) const
{
	return ( (r < t.r) || ((g < t.g) && (r == t.r)) ||
		((b < t.b) && (r == t.r) && (g == t.g)) );
}

//******************************************************************************
#endif

