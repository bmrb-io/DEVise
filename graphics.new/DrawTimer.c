/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DrawTimer class.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.3  1997/08/20 19:32:58  wenger
  Removed/disabled debug output for interruptible drawing.

  Revision 1.1.2.2  1997/08/15 23:06:24  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.1.2.1  1997/08/15 16:06:42  wenger
  Oops!  Forgot to cvs add these files earlier.

 */


#include <stdio.h>

#include "DrawTimer.h"
#include "Init.h"

//#define DEBUG

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

Boolean DrawTimer::_active = false;
double DrawTimer::_expTime;

/*------------------------------------------------------------------------------
 * function: DrawTimer::Set()
 * Set the timer to expire a user-defined number of seconds from the current
 * time.
 */
void
DrawTimer::Set()
{
#if defined(DEBUG)
  printf("DrawTimer::Set()\n");
#endif

  _active = true;

  double timeout = Init::DrawTimeout();
  struct timeval time;
  gettimeofday(&time, NULL);
  _expTime = timeout + time.tv_sec + time.tv_usec / 1.0e6;
}

/*------------------------------------------------------------------------------
 * function: DrawTimer::Cancel()
 * Cancel the timer.
 */
void
DrawTimer::Cancel()
{
#if defined(DEBUG)
  printf("DrawTimer::Cancel()\n");
#endif

  _active = false;
}

/*------------------------------------------------------------------------------
 * function: DrawTimer::Expired()
 * Find out whether the timer has expired.
 */
Boolean
DrawTimer::Expired()
{
  if (_active) {
    struct timeval time;
    gettimeofday(&time, NULL);
    double currentTime = time.tv_sec + time.tv_usec / 1.0e6;
    return currentTime >= _expTime;
  } else {
    return false;
  }
}

/*============================================================================*/
