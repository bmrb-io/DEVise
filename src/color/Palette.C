//******************************************************************************
// class Palette
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: Palette.C
// Last modified: Tue Nov  4 18:17:01 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970328 [weaver]: Original file.
// 970401 [weaver]: Added FromString().
// 970402 [weaver]: Adjusted iterator constructs, removed the list getters.
// 970407 [weaver]: Changed containment from manager to vector.
// 970430 [weaver]: Modified for new ColorManager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971104 [weaver]: Fixed FromString().
//
//******************************************************************************

#include "Palette.h"

//******************************************************************************
// Libraries
//******************************************************************************

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

PColorID	Palette::NewColor(const PaletteColor& color)
{
	colors.push_back(color);

	return PColorID(colors.size() - 1);
}

bool	Palette::DeleteColor(PColorID pcid)
{
	if ((0 > pcid) || (ulong(pcid) >= colors.size()))
		return false;					// Can't delete out-of-range color

	iterator	i = colors.begin();
	PColorID	id = 0;

	while ((i != colors.end()) && (id != pcid))
		i++;

	if (i == colors.end())				// This shouldn't happen...
		return false;

	colors.erase(i);

	return true;
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************

//******************************************************************************
// Iostream Operators
//******************************************************************************

// A palette is output as a sequence of endline-delimited PaletteColors.
ostream&	operator<<(ostream& s, const Palette& t)
{
	Palette::const_iterator	   i = t.colors.begin();

	while (i != t.colors.end())
		s << *i++ << endl;

	return s;
}

// Parses a sequence of return-delimited PaletteColors, adding them to the
// vector until the stream is exhausted. Old colors are removed from the
// Palette first.
istream&	operator>>(istream& s, Palette& t)
{
	PaletteColor	color;
	
	t.colors.erase(t.colors.begin(), t.colors.end());
	
	while (!s.eof())
	{
		s >> color;
		t.colors.push_back(color);
	}

	return s;
}

//******************************************************************************
// Output Functions
//******************************************************************************

string	Palette::ToString(void) const
{
	string			str;
	const_iterator	i = colors.begin();

	while (i != colors.end())
	{
		str += (*i++).ToString();
		str += '\n';
	}

	return str;
}

// Parses a sequence of return-delimited PaletteColors, adding them to the
// vector until the string is exhausted or an error occurs. Old colors are
// removed from the Palette first.
bool	Palette::FromString(const string& s)
{
	unsigned int	old = 0, pos, n;
	PaletteColor	color;

	colors.erase(colors.begin(), colors.end());

	do
	{
		pos = s.find('\n', old);
		n = ((pos != string::npos) ? pos : s.length()) - old;

		if (!color.FromString(s.substr(old, n)))
			return false;

		colors.push_back(color);
		old = pos + 1;
	}
	while ((pos != string::npos) && (old != s.length()));

	return true;
}

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
