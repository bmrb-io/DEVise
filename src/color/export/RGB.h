/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1997-2005
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
// A RGB object is simple three integer color channel values. It is the
// fundamental color object which should be passed around during color
// operations.
 */

/*
  $Id$

  $Log$
  Revision 1.1.26.2  2005/09/06 21:20:31  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.1.26.1  2003/12/19 18:12:56  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.1.44.1  2003/12/17 00:18:46  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.1.42.1  2003/04/18 16:11:09  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.1  1998/02/20 20:44:08  wenger
  Changed color and utils libraries to new export directory scheme for
  dealing with include files (email with complete explanation forthcoming).

  Revision 1.4  1998/02/19 23:26:09  wenger
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
// 970320 [weaver]: Original file.
// 970331 [weaver]: Added iostream operators and FromString().
// 970402 [weaver]: Created RGB.cpp
// 970407 [weaver]: Added Distance(), Error(), Invert(), and IsGray().
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971204 [weaver]: Corrected Error().
// 971211 [weaver]: Corrected Error().
//
//******************************************************************************

#ifndef __CLASS_RGB
#define __CLASS_RGB

//******************************************************************************
// Libraries
//******************************************************************************

#include <iostream>
#include <cmath>
#include <string>

using namespace std;

//******************************************************************************
// Types and Constants
//******************************************************************************

typedef unsigned short	RGBValue;

const RGBValue	minRGBValue		=     0;
const RGBValue	maxRGBValue		= 65535;
const double	maxRGBDistance	= sqrt(3.0 * maxRGBValue * maxRGBValue);

//******************************************************************************
// class RGB
//******************************************************************************

class RGB
{
	public:

		static const RGB	black, white;

	public:

		RGBValue	r;		// Red:   [0,65535]
		RGBValue	g;		// Green: [0,65535]
		RGBValue	b;		// Blue:  [0,65535]

	public:

		// Constructors and Destructors
		RGB(RGBValue rr = 0, RGBValue gg = 0, RGBValue bb = 0);
		RGB(double rr, double gg, double bb);
		virtual ~RGB() {}
		
		// Utility Functions
		void	Get(double& rr, double& gg, double& bb) const;
		void	Set(double rr, double gg, double bb);

		double	Distance(const RGB& rgb) const;
		double	Error(const RGB& rgb) const;

		void	Invert(void);
		bool	IsGray(void) const	{ return ((r == g) && (g == b));	}

		// Conversions and Operators
		bool	operator==(const RGB& t) const;
		bool	operator<(const RGB& t) const;
		
		// Iostream Operators
		friend ostream&		operator<<(ostream& s, const RGB& t);
		friend istream&		operator>>(istream& s, RGB& t);

		// Output Functions
		virtual string	ToString(void) const;
		virtual bool	FromString(const string& s);
};

typedef RGB*	RGBPtr;

//******************************************************************************
// Constructors and Destructors (Inline)
//******************************************************************************

inline	RGB::RGB(RGBValue rr, RGBValue gg, RGBValue bb)
	: r(rr), g(gg), b(bb)
{
}

inline	RGB::RGB(double rr, double gg, double bb)
{
	r = RGBValue(floor(rr * maxRGBValue));
	g = RGBValue(floor(gg * maxRGBValue));
	b = RGBValue(floor(bb * maxRGBValue));
}

//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

inline void		RGB::Get(double& rr, double& gg, double& bb) const
{
	rr = double(r) / double(maxRGBValue);
	gg = double(g) / double(maxRGBValue);
	bb = double(b) / double(maxRGBValue);
}

inline void		RGB::Set(double rr, double gg, double bb)
{
	r = RGBValue(floor(rr * maxRGBValue));
	g = RGBValue(floor(gg * maxRGBValue));
	b = RGBValue(floor(bb * maxRGBValue));
}

inline double	RGB::Distance(const RGB& rgb) const
{
	double	dr = double(r) - double(rgb.r);
	double	dg = double(g) - double(rgb.g);
	double	db = double(b) - double(rgb.b);
	
	return sqrt(dr*dr + dg*dg + db*db);
}

// Returns the ratio of distance to maximum distance, taken as the RGB distance
// between black and white.
inline double	RGB::Error(const RGB& rgb) const
{
	double	dr = double(r) - double(rgb.r);
	double	dg = double(g) - double(rgb.g);
	double	db = double(b) - double(rgb.b);

	return sqrt(dr*dr + dg*dg + db*db) / maxRGBDistance;
}

inline void		RGB::Invert(void)
{
	r = maxRGBValue - r;
	g = maxRGBValue - g;
	b = maxRGBValue - b;
}

//******************************************************************************
// Conversions and Operators (Inline)
//******************************************************************************

inline bool		RGB::operator==(const RGB& t) const
{
	return ((r == t.r) && (g == t.g) && (b == t.b));
}

inline bool		RGB::operator<(const RGB& t) const
{
	return ( (r < t.r) || ((g < t.g) && (r == t.r)) ||
		((b < t.b) && (r == t.r) && (g == t.g)) );
}

//******************************************************************************
#endif

