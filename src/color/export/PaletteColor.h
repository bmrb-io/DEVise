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
// A PaletteColor is simply an RGB value and a name (string). Palettes contain
// this special class (instead of the RGB values alone) for future addition
// of palette statistics, special PaletteColor values, etc.
//
// DEVise currently eschews color naming.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1998/02/19 23:26:07  wenger
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
// 970401 [weaver]: Changed FromString() to use new class String tokenization.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
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



