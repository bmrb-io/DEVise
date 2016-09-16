/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2008
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
// Exceptionally stupid right now. Just a layer over template manager.
// But, DEVise will allow palette editing/multiple palettes later.
 */

/*
  $Id$

  $Log$
  Revision 1.4  2005/12/06 20:11:23  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.3.26.1  2003/12/19 18:12:50  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.3.44.1  2003/12/17 00:18:40  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.3.42.2  2003/12/16 16:09:00  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.3.42.1  2003/04/18 16:11:00  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.3  1998/02/19 23:26:08  wenger
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
// 970505 [weaver]: Original file.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971104 [weaver]: Added DeletePalette().
//
//******************************************************************************
//
// Exceptionally stupid right now. Just a layer over template manager.
// But, DEVise will allow palette editing/multiple palettes later.
//
//******************************************************************************

#ifndef __CLASS_PALETTEMANAGER
#define __CLASS_PALETTEMANAGER

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>

#include "manager.h"

#include "Color.h"
#include "Palette.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class PaletteManager
//******************************************************************************

class PaletteManager
{
	private:

		typedef manager<PaletteID, Palette, less<PaletteID> >	Manager;
		typedef Manager::iterator								iterator;
		typedef Manager::const_iterator							const_iterator;

	private:

		Manager			managerObject;

	public:

		// Constructors and Destructors
		PaletteManager(void)	{}
		virtual ~PaletteManager(void)	{}

		// Getters and Setters
		Palette*		GetPalette(PaletteID pid);
		const Palette*	GetPalette(PaletteID pid) const;

		// Utility Functions
		PaletteID		NewPalette(void) { return managerObject.GetID(); }
		bool			DeletePalette(PaletteID pid);

		// Conversions and Operators
		Palette*		operator[](PaletteID pid);
		const Palette*	operator[](PaletteID pid) const;

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef PaletteManager*		PaletteManagerPtr;

//******************************************************************************
// Getters and Setters (Inline)
//******************************************************************************

inline Palette*		PaletteManager::GetPalette(PaletteID pid)
{
	return managerObject[pid];
}

inline const Palette*	PaletteManager::GetPalette(PaletteID pid) const
{
	return managerObject[pid];
}

//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

inline bool		PaletteManager::DeletePalette(PaletteID pid)
{
	return managerObject.PutID(pid);
}

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline Palette*		PaletteManager::operator[](PaletteID pid)
{
	return GetPalette(pid);
}

inline const Palette*	PaletteManager::operator[](PaletteID pid) const
{
	return GetPalette(pid);
}

//******************************************************************************
#endif
