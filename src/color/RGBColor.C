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
  Implementation of RGBColor class.
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

#include "RGBColor.h"

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

RGB		RGBColor::ToRGB(void) const
{
	return RGB(r, g, b);
}

void	RGBColor::FromRGB(const RGB& rgb)
{
	r = rgb.r;
	g = rgb.g;
	b = rgb.b;
}

bool	RGBColor::IsGray(void) const
{
	return ((r == g) && (g == b));
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************

//******************************************************************************
// Iostream Operators
//******************************************************************************

ostream&	operator<<(ostream& s, const RGBColor& t)
{
	return s << t.ToString();
}

istream&	operator>>(istream& s, RGBColor& t)
{
	char	str[16];
	
	s.get(str, 16, '\n');
	t.FromString(string(str));

	return s;
}

//******************************************************************************
// Output Functions
//******************************************************************************

// Generates a string in format #RRRRGGGGBBBB using 16-bit hex values for each
// color channel (one of the Tk RGB color formats).
string	RGBColor::ToString(void) const
{
	return ToRGB().ToString();
}

// Parses a string in format #RRRRGGGGBBBB into 16-bit hex values for each
// color channel. Changes nothing if the string format is incorrect.
bool	RGBColor::FromString(const string& s)
{
	RGB		rgb;
	bool	result = rgb.FromString(s);

	if (result)
		FromRGB(rgb);

	return result;
}

//******************************************************************************
// Static Functions
//******************************************************************************

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
