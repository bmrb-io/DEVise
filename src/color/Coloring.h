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
// Colored objects (shapes, windows, views, etc.) are derived from this class.
//
// Add a more flexible data color addition/removal mechanism when it becomes
// necessary.
 */

/*
  $Id$

  $Log$
 */

//******************************************************************************
// Modification History:
//
// 970429 [weaver]: Original file.
// 970505 [weaver]: Updated to new Color Manager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971118 [weaver]: Changed from ColorID to PColorID for palette operations.
// 971203 [weaver]: Added RMSDistance() methods.
//
//******************************************************************************

#ifndef __CLASS_COLORING
#define __CLASS_COLORING

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>
#include <stl.h>

#include "Color.h"

//******************************************************************************
// Types and Constants
//******************************************************************************

typedef unsigned long	ulong;

typedef vector<double>	DoubleVector;
typedef vector<ulong>	IntVector;

//******************************************************************************
// class Coloring
//******************************************************************************

class Coloring
{
	protected:

		typedef vector<PColorID>			Vector;
		typedef Vector::iterator			iterator;
		typedef Vector::const_iterator		const_iterator;

	protected:

		PColorID	fore;		// Foreground color
		PColorID	back;		// Background color
		Vector		data;		// Data colors

	public:

		// Constructors and Destructors
		Coloring(PColorID fgid = GetPColorID(defForeColor),
				 PColorID bgid = GetPColorID(defBackColor));
		virtual ~Coloring(void) {}

		// Getters and Setters
		PColorID		GetForeground(void) const		{ return fore;	}
		PColorID		GetBackground(void) const		{ return back;	}
		virtual void	SetForeground(PColorID fgid)	{ fore = fgid;	}
		virtual void	SetBackground(PColorID bgid)	{ back = bgid;	}

		virtual void	SetColors(PColorID fgid = GetPColorID(defForeColor),
								  PColorID bgid = GetPColorID(defBackColor));

		PColorID		GetDataColor(ulong n) const;

		// Utility Functions (Data Colors)
		void		AddDataColor(PColorID pcid);

		// Utility Functions (Color Heuristics)
		double		ForeBackRMSDistance(void);
		double		DataRMSDistance(void);
		double		RMSDistance(void);
		double		Entropy(IntVector count);

		// Conversions and Operators
		bool		operator==(const Coloring& t) const;
		bool		operator<(const Coloring& t) const;

		PColorID	operator[](ulong n);
		PColorID	operator[](ulong n) const;

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef Coloring*	ColoringPtr;

//******************************************************************************
// Constructors and Destructors (Inline)
//******************************************************************************

inline	Coloring::Coloring(PColorID fgid, PColorID bgid)
{
	SetColors(fgid, bgid);
}

//******************************************************************************
// Getters and Setters (Inline)
//******************************************************************************

inline PColorID	Coloring::GetDataColor(ulong n) const
{
	return ((n < data.size()) ? data[n] : nullPColorID);
}

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline PColorID		Coloring::operator[](ulong n)
{
	return GetDataColor(n);
}

inline PColorID		Coloring::operator[](ulong n) const
{
	return GetDataColor(n);
}

//******************************************************************************
#endif

