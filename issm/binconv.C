/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.4  1995/12/28 18:31:51  jussi
  Small fixes to remove compiler warnings.

  Revision 1.3  1995/11/02 16:52:40  jussi
  Updated copyright message.

  Revision 1.2  1995/09/22 15:49:41  jussi
  Added copyright message.

  Revision 1.1  1995/09/06 15:28:54  jussi
  Initial revision of archive.
*/

#include <math.h>

#include "binconv.h"
#include "Exit.h"

/********************************************************************
** vaxtofloat() - convert IEEE floating point to the local machine's
**		  floating point floating point  
**
** floating point format:
**  sccccccccmmmmmmmmmmmmmmmmmmmmmm
**
**  s is one bit sign indicator - if set, number is negative
**  c is 8 bit characteristic (exponent). It is in excess 128 format
**  m is 23 bit mantissa. The mantissa is actually a fractional
**    number - it begins with a radix point.
**
**				     s   (c-127)
** value is calculated by:  val = (-1 )(2      )(m)
**
**			         s   (24-e-127)       24 
** (actually, it's more like: (-1 )(2          )(m * 2  )
**
********************************************************************/

float BinaryConversion::convreal(unsigned long num)
{
  long exp;	/* high order bit will always be 0, so signed is */
                /*  safe, and we need the sign in the pow() call */
  unsigned long mantissa;
  union {
    unsigned long uflt;
    float fflt;
  } flt;
  
  exp = num & 0x7f800000;	/* get exponent */
  exp >>= 23;			/* shift all the way to the right */
  exp -= 127;			/* take out of excess-127 notation */

  exp = 23 - exp;		/* effectivly multiply mantissa by  */
                                /*  2**23 to make it a whole number */

  mantissa = num & 0x007FFFFF;	/* get mantissa */
  mantissa = mantissa | 0x0800000;	/* leading bit is always 1 */
  if(mantissa == 0)			/* if mantissa == 0, number == 0 */
    return(0.0);

  flt.fflt = (float) mantissa / pow(2.0, (float) exp);
  
  if(num & 0x80000000)	/* if sign bit set, then number is negative */
    flt.fflt = -flt.fflt;

  return(flt.fflt);
}

#define LEAPYEAR(y) ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)

void BinaryConversion::CRSPDate(int index, int &day, int &month, int &year)
{
  // CRSP date indexing starts from July 2, 1962 which corresponds
  // to date value 1. Every trading day since has increased the
  // index by 1.

  // Apply a simple conversion until we have a more accurate
  // computation of dates. Current scheme does not take holidays
  // into account when they fall on Monday through Friday.

  int baseidx = 1;                      // July 2, 1962
  int baseyear = 1962;
  int basemonth = 7;
  int baseday = 2;
  int basewday = 1;                     // Monday

  // a couple of known index points for recalibration
  if (index >= 7422) {
    baseidx = 7422;                     // January 2, 1992
    baseyear = 1992;
    basemonth = 1;
    baseday = 2;
    basewday = 4;                       // Thursday
  } else if (index >= 6916) {
    baseidx = 6916;                     // January 2, 1990
    baseyear = 1990;
    basemonth = 1;
    baseday = 2;
    basewday = 2;                       // Tuesday
  }

  int mdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  int since = index - baseidx;          // distance from last known point
  since += basewday - 1;                // add day-of-week offset
  int weeks = since / 5;                // assume 5 day trading weeks
  int days = weeks * 7 + since % 5;     // convert to 7 day weeks
  days -= basewday - 1;                 // remove day-of-week offset

  if (LEAPYEAR(baseyear))
    mdays[1] = 29;                      // add February 29 for leap year

  int i;
  for(i = 0; i < basemonth - 1; i++)
    days += mdays[i];                   // add base month
  days += baseday - 1;                  // add base day

  year = baseyear;
  while(1) {
    int yeardays = (LEAPYEAR(year) ? 366 : 365);
    if (days < yeardays)
      break;
    year++;
    days -= yeardays;
  }

  mdays[1] = 28;
  if (LEAPYEAR(year))
    mdays[1] = 29;                      // add February 29 for leap years

  for(i = 0; i < (int)(sizeof mdays / sizeof mdays[0]); i++) {
    if (days < mdays[i])                // found current month?
      break;
    days -= mdays[i];
  }

  month = i + 1;
  day = days + 1;

  DOASSERT(month >= 1 && month <= 12, "Invalid month number");
  DOASSERT(day >= 1 && day <= mdays[month - 1], "Invalid day number");
}
