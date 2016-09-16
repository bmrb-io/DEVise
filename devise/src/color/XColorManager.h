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
// Use only one ColorManager instance in an application.
// XColorManager performs color management when X supplies the RGB values
// (via integer indexes, previously called LocalColors).
 */

/*
  $Id$

  $Log$
  Revision 1.4.22.1  2003/12/19 18:12:50  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.4.40.1  2003/12/17 00:18:40  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.4.38.2  2003/12/16 16:09:00  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.4.38.1  2003/04/18 16:11:01  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.4  1998/05/05 15:16:15  zhenhai
  Corrected make files for sgi uses.

  Revision 1.3  1998/02/19 23:26:12  wenger
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
// 970505 [weaver]: Updated for new Color Manager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#ifndef __CLASS_XCOLORMANAGER
#define __CLASS_XCOLORMANAGER

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

#include "ColorManager.h"
#include "RGB.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class XColorManager
//******************************************************************************

class XColorManager : public ColorManager
{
	private:

		Display*	display;	// X display
		Colormap        *cmap;           // Colormap
		int		ncmap;           // Colormap

	public:

		// Constructors and Destructors
		//XColorManager(Display* d);
		XColorManager(Display * d, int depth, int nmap, Colormap *map);
		virtual ~XColorManager(void);

		// Getters and Setters
		const Display*	GetDisplay(void) const	{ return display;}
		void SetDisplay(Display* d)	{ display = d;		}
		
		// Utility Functions
		virtual bool	GetXRGB(ColorID cid, RGB& rgb) const;
		virtual void	GetXRGBList(RGBList& list) const;
		
		// Conversions and Operators

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);

	protected:

		// Utility Functions
		virtual bool	XAllocColor(const RGB& rgb, XColorID& cid) const;
		virtual bool	XFreeColor(XColorID xcid) const;
};

typedef XColorManager*	XColorManagerPtr;

//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

//******************************************************************************
#endif
