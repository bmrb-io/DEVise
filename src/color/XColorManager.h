//******************************************************************************
// class XColorManager
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: XColorManager.h
// Last modified: Thu Sep 25 16:35:56 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970331 [weaver]: Original file.
// 970505 [weaver]: Updated for new Color Manager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************
//
// Use only one ColorManager instance in an application.
// XColorManager performs color management when X supplies the RGB values
// (via integer indexes, previously called LocalColors).
//
//******************************************************************************

#ifndef __CLASS_XCOLORMANAGER
#define __CLASS_XCOLORMANAGER

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>
#include <stl.h>

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

	public:

		// Constructors and Destructors
		XColorManager(Display* d);
		virtual ~XColorManager(void);

		// Getters and Setters
		const Display*	GetDisplay(void) const	{ return display;	}
		void			SetDisplay(Display* d)	{ display = d;		}
		
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
