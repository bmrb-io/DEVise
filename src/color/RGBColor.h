//******************************************************************************
// class RGBColor
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: RGBColor.h
// Last modified: Thu Sep 25 16:35:35 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970320 [weaver]: Original file.
// 970331 [weaver]: Added iostream operators and FromString().
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************
//
// RGBColor contains a red-green-blue (RGB) representation of color.
//
// DEVise doesn't use this code yet. It will be used for more flexible color
// selection (via the tcl interface) and more advanced mappings->color
// conversions later.
//
// An RGBColor object has the same structure as an RGB object, but is used for
// different purposes (and is more flexible).
//
//******************************************************************************

#ifndef __CLASS_RGBCOLOR
#define __CLASS_RGBCOLOR

//******************************************************************************
// Libraries
//******************************************************************************

#include <iostream.h>
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

