/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2003
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
// An abstract class for color management. Provides a way to map logical RGB
// values into physical RGB values.
//
// Four possibilities:
//
//     1) direct color, direct map (24-bit using logical RGB)
//            supplied RGB value
//
//     2) direct color, indexed map (24-bit using ColorID)
//            lookup the physical RGB in the manager table
//
//     3) indexed color, direct map (8-bit using logical RGB)
//            map the logical RGB onto a ColorID onto a physical RGB
//            (if the logical RGB doesn't exist, match it to a physical color
//                and add a manager entry)?
//
//     4) indexed color, indexed map (8-bit using ColorID)
//            lookup the physical RGB in the manager table
//
// Use only one ColorManager instance in an application.
//
// N.B.: The X in XColorIDs means external, not X windows. That is, the
// physical color mechanism is indexed.
 */

/*
  $Id$

  $Log$
  Revision 1.3.26.1  2003/12/19 18:12:49  wenger
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

  Revision 1.3  1998/02/19 23:26:03  wenger
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
// 970331 [weaver]: Original file.
// 970505 [weaver]: New version.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#ifndef __CLASS_COLORMANAGER
#define __CLASS_COLORMANAGER

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>

#include "manager.h"
#include "Color.h"
#include "XColor.h"
#include "RGB.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class ColorManager (abstract)
//******************************************************************************

class ColorManager
{
	protected:

		struct Color
		{
				RGB			rgb;	// RGB requested from device
				XColorID	xcid;	// Device index for color
				ulong		refs;	// Number of uses of this color
		};

		typedef manager<ColorID, Color, less<ColorID> >		LogicalMap;
		typedef LogicalMap::iterator						lmap_iterator;
		typedef LogicalMap::const_iterator					lmap_const_iterator;

		typedef map<RGB, ColorID, less<RGB> >				PhysicalMap;
		typedef PhysicalMap::iterator						pmap_iterator;
		typedef PhysicalMap::const_iterator					pmap_const_iterator;

	protected:

		LogicalMap		lmap;	// Map from ColorID to Color
		PhysicalMap		pmap;	// Map from RGB to ColorID (inverse table)

		int				bits;	// Device bit depth (8, 24, etc.)

	public:

		// Constructors and Destructors
		ColorManager(int depth = 8);
		virtual ~ColorManager(void);

		// Getters and Setters
		bool	GetRGB(ColorID cid, RGB& rgb) const;
		bool	GetXColorID(ColorID cid, XColorID& xcid) const;
		bool	GetColorID(const RGB& rgb, ColorID& cid) const;

		// Utility Functions
		bool	AllocColor(const RGB& rgb);
		bool	FreeColor(const RGB& rgb);

		ColorID		MatchColor(const RGB& rgb) const;
		ColorID		ClosestColor(const RGB& rgb) const;

		void			GetRGBList(RGBList& list) const;

		virtual bool	GetXRGB(ColorID cid, RGB& rgb) const = 0;
		virtual void	GetXRGBList(RGBList& list) const = 0;
		
		// Conversions and Operators

		// Output Functions
		virtual string	ToString(void) const = 0;
		virtual bool	FromString(const string& s) = 0;

	protected:

		// Getters and Setters
		bool	GetRefs(ColorID cid, ulong& refs) const;

		// Utility Functions
		virtual bool	XAllocColor(const RGB& rgb, XColorID& cid) const = 0;
		virtual bool	XFreeColor(XColorID xcid) const = 0;
};

typedef ColorManager*	ColorManagerPtr;

//******************************************************************************
// Getters and Setters (Inline)
//******************************************************************************

inline bool		ColorManager::GetRGB(ColorID cid, RGB& rgb) const
{
	const Color*	color = lmap.Find(cid);

	if (color != NULL)
		rgb = color->rgb;

	return (color != NULL);
}

inline bool		ColorManager::GetXColorID(ColorID cid, XColorID& xcid) const
{
	const Color*	color = lmap.Find(cid);

	if (color != NULL)
		xcid = color->xcid;

	return (color != NULL);
}

inline bool		ColorManager::GetRefs(ColorID cid, ulong& refs) const
{
	const Color*	color = lmap.Find(cid);

	if (color != NULL)
		refs = color->refs;

	return (color != NULL);
}

inline bool		ColorManager::GetColorID(const RGB& rgb, ColorID& cid) const
{
	pmap_const_iterator		i = pmap.find(rgb);

	if (i != pmap.end())
		cid = (*i).second;

	return (i != pmap.end());
}

//******************************************************************************
#endif
