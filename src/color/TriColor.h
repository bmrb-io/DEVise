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
// TriColor is the base class for color representations using three values.
//
// DEVise doesn't use this code yet. It will be used for more flexible color
// selection (via the tcl interface) and more advanced mappings->color
// conversions later.
 */

/*
  $Id$

  $Log$
 */

//******************************************************************************
// Modification History:
//
// 970320 [weaver]: Original file.
// 970410 [weaver]: Changed name from Color to TriColor.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************

#ifndef __CLASS_TRICOLOR
#define __CLASS_TRICOLOR

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>

#include "RGB.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

//******************************************************************************
// class TriColor (abstract)
//******************************************************************************

class TriColor
{
	private:
		
	public:

		// Constructors and Destructors
		virtual ~TriColor(void) {}
		
		// Getters and Setters

		// Utility Functions
		virtual RGB		ToRGB(void) const = 0;
		virtual void	FromRGB(const RGB& rgb) = 0;

		virtual double	Distance(const TriColor& color) const;
		virtual double	Error(const TriColor& color) const;
		virtual void	Invert(void);
		virtual bool	IsGray(void) const;
		
		// Conversions and Operators

		// Iostream Operators

		// Output Functions
		virtual string	ToString(void) const = 0;
		virtual bool	FromString(const string& s) = 0;
};

typedef TriColor*	TriColorPtr;

//******************************************************************************
// Inline Functions
//******************************************************************************

//******************************************************************************
#endif

