//******************************************************************************
// class RGB
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: RGB.C
// Last modified: Thu Sep 25 16:35:19 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970402 [weaver]: Original file.
// 970407 [weaver]: Added Distance(), Invert(), and IsGray().
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#include "RGB.h"

//******************************************************************************
// Libraries
//******************************************************************************

#include <cctype>
#include <cstdio>

//******************************************************************************
// Static Member Definitions
//******************************************************************************

const RGB	RGB::black(minRGBValue, minRGBValue, minRGBValue);
const RGB	RGB::white(maxRGBValue, maxRGBValue, maxRGBValue);

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

//******************************************************************************
// Getters and Setters
//******************************************************************************

//******************************************************************************
// Utility Functions
//******************************************************************************

//******************************************************************************
// Conversions and Operators
//******************************************************************************

//******************************************************************************
// Iostream Operators
//******************************************************************************

ostream&	operator<<(ostream& s, const RGB& t)
{
	return s << t.ToString();
}

istream&	operator>>(istream& s, RGB& t)
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
string	RGB::ToString(void) const
{
	char	s[16];

	sprintf(s, "#%04x%04x%04x", r, g, b);

	return string(s);
}

// Parses a string in format #RRRRGGGGBBBB into 16-bit hex values for each
// color channel. Changes nothing if the string format is incorrect.
// (More careful than using sscanf.)
bool		RGB::FromString(const string& s)
{
	if (s.length() != 13)
		return false;

	string	str(s);

	for (int i=1; i<13; i++)
	{
		if (!isxdigit(str[i]))
			return false;
		else if (isdigit(str[i]))
			str[i] -= '0';
		else if (isupper(str[i]))
			str[i] = str[i] - 'A' + 10;
		else
			str[i] = str[i] - 'a' + 10;
	}

	r  = str[ 1]; r <<= 4;
	r += str[ 2]; r <<= 4;
	r += str[ 3]; r <<= 4;
	r += str[ 4];

	g  = str[ 5]; g <<= 4;
	g += str[ 6]; g <<= 4;
	g += str[ 7]; g <<= 4;
	g += str[ 8];

	b  = str[ 9]; b <<= 4;
	b += str[10]; b <<= 4;
	b += str[11]; b <<= 4;
	b += str[12];

	return true;
}

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
