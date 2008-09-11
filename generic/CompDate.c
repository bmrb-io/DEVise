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
  Revision 1.1  1996/07/09 15:59:29  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

 */

#define _CompDate_c_

#include "CompDate.h"


/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static const char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: CompDate::Get
 * Return the compile date.
 */
const char *
CompDate::Get()
{
    return __DATE__;
}

/*============================================================================*/
