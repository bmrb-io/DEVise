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
// Exceptionally stupid right now. Just a layer over template manager.
// But, DEVise will allow palette editing/multiple palettes later.
 */

/*
  $Id$

  $Log$
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
#include <stl.h>

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

		Manager			manager;

	public:

		// Constructors and Destructors
		PaletteManager(void)	{}
		virtual ~PaletteManager(void)	{}

		// Getters and Setters
		Palette*		GetPalette(PaletteID pid);
		const Palette*	GetPalette(PaletteID pid) const;

		// Utility Functions
		PaletteID		NewPalette(void) { return manager.GetID(); }
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
	return manager[pid];
}

inline const Palette*	PaletteManager::GetPalette(PaletteID pid) const
{
	return manager[pid];
}

//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

inline bool		PaletteManager::DeletePalette(PaletteID pid)
{
	return manager.PutID(pid);
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
