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
  Implementation of Version class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1996/07/09 16:00:24  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

 */

#define _Version_c_

#include <stdio.h>

#include "Version.h"
#include "CompDate.h"


/*
 * Static global variables.
 */

// Master DEVise version number.
static const char *	version = "1.1.2";

// Master DEVise copyright dates.
static const char *	copyright = "Copyright (c) 1992-1996";


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: Version::Get
 * Get the current DEVise version.
 */
const char *
Version::Get()
{
	return version;
}

/*------------------------------------------------------------------------------
 * function: Version::GetCopyright
 * Get the DEVise copyright message.
 */
const char *
Version::GetCopyright()
{
	return copyright;
}

/*------------------------------------------------------------------------------
 * function: Version::PrintInfo
 * Print version and copyright information.
 */
void
Version::PrintInfo()
{
	printf("DEVise Data Visualization Software\n");
	printf("%s\n", copyright);
	printf("By the DEVise Development Group\n");
	printf("All Rights Reserved.\n");
	printf("Version %s\n", version);
	printf("Compile date: %s\n", CompDate::Get());
}

/*============================================================================*/
