//******************************************************************************
// class PaletteColor
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: PaletteColor.h
// Last modified: Thu Sep 25 16:38:42 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970320 [weaver]: Original file.
// 970331 [weaver]: Added iostream operators and FromString().
// 970401 [weaver]: Changed FromString() to use new class String tokenization.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************
//
// A PaletteColor is simply an RGB value and a name (string). Palettes contain
// this special class (instead of the RGB values alone) for future addition
// of palette statistics, special PaletteColor values, etc.
//
// DEVise currently eschews color naming.
//
//******************************************************************************

#ifndef __CLASS_PALETTECOLOR
#define __CLASS_PALETTECOLOR

//******************************************************************************
// Libraries
//******************************************************************************

#include <iostream.h>
#include <string>

#include "RGB.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class PaletteColor
//******************************************************************************

class PaletteColor
{
	private:

		RGB		color;		// Logical color value
		string	name;		// Color name

	public:

		// Constructors and Destructors
		PaletteColor(const RGB& rgb = RGB(), const string& s = string());

		// Getters and Setters
		const RGB&		GetColor(void) const			{ return color;	}
		const string&	GetName(void) const				{ return name;	}

		void	SetColor(const RGB& rgb = RGB())		{ color = rgb;	}
		void	SetName(const string& s = string())		{ name = s;		}
		
		// Utility Functions
		void	GenerateName(void);

		// Conversions and Operators
		bool	operator==(const PaletteColor& t) const;
		bool	operator<(const PaletteColor& t) const;

		// Iostream Operators
		friend ostream&		operator<<(ostream& s, const PaletteColor& t);
		friend istream&		operator>>(istream& s, PaletteColor& t);

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef PaletteColor*	PaletteColorPtr;

//******************************************************************************
// Constructors and Destructors (Inline)
//******************************************************************************

inline	PaletteColor::PaletteColor(const RGB& rgb, const string& s)
	: color(rgb), name(s)
{
}

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline bool		PaletteColor::operator==(const PaletteColor& t) const
{
	return (color == t.color);
}

inline bool		PaletteColor::operator<(const PaletteColor& t) const
{
	return (color < t.color);
}

//******************************************************************************
#endif



