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
  Implementation of HSVColor class.
 */

/*
  $Id$

  $Log$
 */

//******************************************************************************
// Modification History:
//
// 970320 [weaver]: Original file.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#include "HSVColor.h"

//******************************************************************************
// Libraries
//******************************************************************************

#include <cstdio>

//******************************************************************************
// Static Member Definitions
//******************************************************************************

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

//******************************************************************************
// Getters and Setters
//******************************************************************************

//******************************************************************************
// Utility Functions
//******************************************************************************

RGB		HSVColor::ToRGB(void) const
{
	int		i = int(floor(h/60));
	double	f = ((i%2) ? (h/60 - i) : (1 - h/60 + i));
	double	m = v * (1 - s);
	double	n = v * (1 - s * f);
	double	r, g, b;
	
	switch (i)
	{
		case 6:
		case 0:	r = v; g = n; b = m;	break;
		case 1:	r = n; g = v; b = m;	break;
		case 2:	r = m; g = v; b = n;	break;
		case 3:	r = m; g = n; b = v;	break;
		case 4:	r = n; g = m; b = v;	break;
		case 5:	r = v; g = m; b = n;	break;
	}

	return RGB(RGBValue(r * maxRGBValue),
			   RGBValue(g * maxRGBValue),
			   RGBValue(b * maxRGBValue));
}

void	HSVColor::FromRGB(const RGB& rgb)
{
	double	r = double(rgb.r) / maxRGBValue;
	double	g = double(rgb.g) / maxRGBValue;
	double	b = double(rgb.b) / maxRGBValue;
	double	i, f, min, max;

	min = (r < g) ? r : g;
	min = (min < b) ? min : b;

	max = (r > g) ? r : g;
	max = (max > b) ? max : b;

	if (max == min)
	{
		h = 0;
		s = (max == 0) ? 0 : (max - min) / max;
		v = max;
	}
	else
	{
		if (r == min)
			f = g - b, i = 3;
		else if (g == min)
			f = b - r, i = 5;
		else
			f = r - g, i = 1;

		h = 60 * (i - f / (max - min));
		s = (max - min) / max;
		v = max;
	}
}

// Special inversion in HSV color spaces changes hue only.
void	HSVColor::Invert(void)
{
	SetHue(h + 180.0);
}

bool	HSVColor::IsGray(void) const
{
	return (s == 0);
}

// An approximate measure of the lightness/brightness of a color (for
// 2-D layout).
double	HSVColor::Lightness(void) const
{
	return sqrt((s*s + v*v) / 2);
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************

//******************************************************************************
// Iostream Operators
//******************************************************************************

ostream&	operator<<(ostream& s, const HSVColor& t)
{
	return s << t.ToString();
}

istream&	operator>>(istream& s, HSVColor& t)
{
	char	str[24];

	s.get(str, 20);
	t.FromString(string(str));

	return s;
}

//******************************************************************************
// Output Functions
//******************************************************************************

// Generates a string in format: HHH.HH S.SSSS V.VVVV
// using doubles for each color channel.
string	HSVColor::ToString(void) const
{
	char	str[24];

	sprintf(str, "%06.2f %06.4f %06.4f", h, s, v);

	return string(str);
}

// Parses a string from format: HHH.HH S.SSSS V.VVVV
// into doubles for each color channel. Changes nothing if the string format
// cannot be converted into three doubles (modulo channel maxima).
bool	HSVColor::FromString(const string& s)
{
	float	hh, ss, vv;

	if (sscanf(s.c_str(), "%f %f %f", &hh, &ss, &vv) != 3)
		return false;

	SetHue(hh);
	SetSaturation(ss);
	SetValue(vv);

	return true;
}
//******************************************************************************
// Static Functions
//******************************************************************************

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
