//******************************************************************************
// class Coloring
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: Coloring.C
// Last modified: Tue Nov 18 18:05:57 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970429 [weaver]: Original file.
// 970505 [weaver]: Updated to new Color Manager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971118 [weaver]: Changed from ColorID to PColorID for palette operations.
//
//******************************************************************************

#include "Coloring.h"

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

void	Coloring::SetColors(PColorID fgid, PColorID bgid)
{
	fore = fgid;
	back = bgid;
}

//******************************************************************************
// Utility Functions
//******************************************************************************

void	Coloring::AddDataColor(PColorID pcid)
{
	data.push_back(pcid);
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************
// Total ordering including data PColorIDs should be added eventually.

// Two colorings are the same if the fore and back PColorIDs are the same).
bool	Coloring::operator==(const Coloring& t) const
{
	return ((fore == t.fore) && (back == t.back));
}

// Total ordering on PColorIDs, in precedence fore > back.
bool	Coloring::operator<(const Coloring& t) const
{
	return ((fore < t.fore) || ((fore == t.fore) && (back < t.back)));
}

//******************************************************************************
// Output Functions
//******************************************************************************

string	Coloring::ToString(void) const
{
	return string();
}

bool	Coloring::FromString(const string& /*s*/)
{
	return true;
}

//******************************************************************************
// Static Functions
//******************************************************************************

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
