//******************************************************************************
// class HSVColor
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: HSVColor.h
// Last modified: Thu Sep 25 16:34:25 1997 by Chris Weaver
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
// HSVColor contains a hue-saturation-value (HSV) representation of color.
//
// DEVise doesn't use this code yet. It will be used for more flexible color
// selection (via the tcl interface) and more advanced mappings->color
// conversions later.
//
//******************************************************************************

#ifndef __CLASS_HSVCOLOR
#define __CLASS_HSVCOLOR

//******************************************************************************
// Libraries
//******************************************************************************

#include <iostream.h>
#include <cmath>
#include <string>

#include "TriColor.h"
#include "RGB.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class HSVColor
//******************************************************************************

class HSVColor : public TriColor
{
	private:

		double	h;		// Hue (angle): [0.0, 360.0)
		double	s;		// Saturation:  [0.0, 1.0]
		double	v;		// Value:       [0.0, 1.0]

	public:

		// Constructors and Destructors
		HSVColor(double hh = 0.0, double ss = 0.0, double vv = 0.0);
		HSVColor(const RGB& rgb)			{ FromRGB(rgb);				}
		HSVColor(const TriColor& color)		{ FromRGB(color.ToRGB());	}
		virtual ~HSVColor(void) {}
	
		// Getters and Setters
		double	GetHue(void) const			{ return h;	}
		double	GetSaturation(void) const	{ return s;	}
		double	GetValue(void) const		{ return v;	}

		void	SetHue(double hh)			{ h = fmod(hh, 360.0);	}
		void	SetSaturation(double ss)	{ s = fmod(ss, 1.0);	}
		void	SetValue(double vv)			{ v = fmod(vv, 1.0);	}
		
		// Utility Functions
		virtual RGB		ToRGB(void) const;
		virtual void	FromRGB(const RGB& rgb);

		virtual void	Invert(void);
		virtual bool	IsGray(void) const;
		
		double	Lightness(void) const;

		// Conversions and Operators
		bool	operator==(const HSVColor& t) const;
		bool	operator<(const HSVColor& t) const;

		// Iostream Operators
		friend ostream&		operator<<(ostream& s, const HSVColor& t);
		friend istream&		operator>>(istream& s, HSVColor& t);

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef HSVColor*	HSVColorPtr;

//******************************************************************************
// Constructors and Destructors (Inline)
//******************************************************************************

inline	HSVColor::HSVColor(double hh, double ss, double vv)
{
	SetHue(hh);
	SetSaturation(ss);
	SetValue(vv);
}

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline bool		HSVColor::operator==(const HSVColor& t) const
{
	return ((h == t.h) && (s == t.s) && (v == t.v));
}

inline bool		HSVColor::operator<(const HSVColor& t) const
{
	return (ToRGB() < t.ToRGB());
}

//******************************************************************************
#endif

