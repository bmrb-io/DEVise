/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of CompDate (compile date) class.
 */

/*
  $Id$

  $Log$
 */

#define _CompDate_c_

#include "CompDate.h"


/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: CompDate::Get
 * Return the compile date.
 */
char *
CompDate::Get()
{
    return __DATE__;
}

/*============================================================================*/
