//******************************************************************************
// class PaletteColor
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: PaletteColor.C
// Last modified: Tue Nov  4 18:10:45 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970403 [weaver]: Original file.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#include "PaletteColor.h"

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

void	PaletteColor::GenerateName(void)
{
	char	s[24];

	sprintf(s, "R%04x_G%04x_B%04x", color.r, color.g, color.b);
	name = string(s);
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************

//******************************************************************************
// Iostream Operators
//******************************************************************************

ostream&	operator<<(ostream& s, const PaletteColor& t)
{
	return s << t.ToString();
}

istream&	operator>>(istream& s, PaletteColor& t)
{
	char	str[80];

	s.get(str, 80, '\n');
	t.FromString(string(str));

	return s;
}

//******************************************************************************
// Output Functions
//******************************************************************************

// Generates a string in format: #RRRRGGGGBBBB MyColorName
string	PaletteColor::ToString(void) const
{
	return (color.ToString() + ' ' + name);
}

// Parses a string in format: #RRRRGGGGBBBB MyColorName
// Changes nothing if the RGBColor format is incorrect.
// This needs to be made robust (changed for new C++ standard library strings).
bool	PaletteColor::FromString(const string& s)
{
	unsigned int	pos = s.find(' ');
	bool			result = true;

	if (pos == string::npos)
	{
		result = color.FromString(s);
	}
	else
	{
		result = color.FromString(s.substr(0, pos));
		name = s.substr(pos, s.length());
	}
	
	return result;
}

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
