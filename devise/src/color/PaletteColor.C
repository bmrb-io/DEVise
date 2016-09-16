/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2002
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of PaletteColor class.
 */

/*
  $Id$

  $Log$
  Revision 1.3.26.1  2002/08/22 20:49:23  wenger
  Fixed problem with extra spaces getting into color names (fixes
  bugs 677 and 692).

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
	unsigned int	firstBlank = s.find(' ');
	unsigned int	lastBlank = s.rfind(' ');
	bool			result = true;

	if (firstBlank == string::npos)
	{
		result = color.FromString(s);
	}
	else
	{
		result = color.FromString(s.substr(0, firstBlank));
		name = s.substr(lastBlank + 1, s.length());
	}
	
	return result;
}

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
