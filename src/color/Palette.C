/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2004
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of Palette class.
 */

/*
  $Id$

  $Log$
  Revision 1.4.10.1  2004/08/23 22:53:39  wenger
  Fixed bug 890 (problems with drill-down dialog caused by
  some color palettes not having a pure white color).

  Revision 1.4  2001/06/12 15:29:52  wenger
  Implemented a choice of modulus (default) or truncate color modes.

  Revision 1.3  1998/02/19 23:26:06  wenger
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
#include "ColorUtil.h"

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

bool	Palette::EnsureBlackWhite()
{
	bool	missing = false;

	PColorID pcid;

	PaletteColor black(RGB::black, "black");
	if (!GetPColorID(black.GetColor(), pcid)) {
		NewColor(black);
		missing = true;
	}

	PaletteColor white(RGB::white, "white");
	if (!GetPColorID(white.GetColor(), pcid)) {
		NewColor(white);
		missing = true;
	}

	return missing;
}

PaletteColor*	Palette::GetColor(PColorID pcid)
{
	return &(colors[PColorID2Index(pcid, colors.size())]);
}

const PaletteColor*	Palette::GetColor(PColorID pcid) const
{
    return (const PaletteColor*)GetColor(pcid);
}

bool	Palette::GetPColorID(const RGB& rgb, PColorID& pcid)
{
	for (PColorID id = 0; id < GetSize(); ++id) {
		if (GetColor(id)->GetColor() == rgb) {
			pcid = id;
			return true;
		}
	}

	cerr << "Palette::GetPColorID unable to find requested color " <<
	  rgb << endl;

	return false;
}

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
