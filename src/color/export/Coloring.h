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
// Colored objects (shapes, windows, views, etc.) are derived from this class.
//
// Add a more flexible data color addition/removal mechanism when it becomes
// necessary.
 */

/*
  $Id$

  $Log$
  Revision 1.1.26.2  2003/12/19 18:12:56  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.1.26.1.2.1  2003/12/17 00:18:46  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.1.26.1  2003/11/05 17:02:04  wenger
  First part of display modes for printing is implemented (view foreground
  and background colors work, haven't done anything for symbol colors yet).

  Revision 1.1.42.2  2003/12/16 16:09:06  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.1.42.1  2003/04/18 16:11:08  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.1  1998/02/20 20:44:06  wenger
  Changed color and utils libraries to new export directory scheme for
  dealing with include files (email with complete explanation forthcoming).

  Revision 1.4  1998/02/19 23:26:04  wenger
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

#include "Color.h"

using namespace std;

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
		virtual PColorID	GetForeground(void) const	{ return fore;	}
		virtual PColorID	GetBackground(void) const	{ return back;	}
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

