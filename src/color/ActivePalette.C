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
  Implementation of the ActivePalette class.
 */

/*
  $Id$

  $Log$
 */

//******************************************************************************
// Modification History:
//
// 970409 [weaver]: Original file.
// 970505 [weaver]: Modified for new ColorManager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#include "ActivePalette.h"

//******************************************************************************
// Libraries
//******************************************************************************

//******************************************************************************
// Static Member Definitions
//******************************************************************************

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ActivePalette::ActivePalette(const Palette& palette, const ColorManager& cm)
{
	SetColors(palette, cm);
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

//******************************************************************************
// Utility Functions
//******************************************************************************

void	ActivePalette::SetColors(const Palette& palette, const ColorManager& cm)
{
	colors.erase(colors.begin(), colors.end());

	Palette::const_iterator		i = palette.GetColors().begin();	

	while (i != palette.GetColors().end())
	{
		RGB			rgb = (*i++).GetColor();
		ColorID		cid;
		XColorID	xcid;
		
		if (cm.GetColorID(rgb, cid) && cm.GetXColorID(cid, xcid))
			colors.push_back(xcid);
		else
			colors.push_back(nullXColorID);
	}
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************

//******************************************************************************
// Iostream Operators
//******************************************************************************

//******************************************************************************
// Output Functions
//******************************************************************************

string	ActivePalette::ToString(void) const
{
	return string();
}

bool	ActivePalette::FromString(const string& /*s*/)
{
	return true;
}

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
