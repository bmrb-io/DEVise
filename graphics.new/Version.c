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
  Revision 1.8  1996/09/04 21:25:03  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.7  1996/08/30 15:56:11  wenger
  Modified View and QueryProcessor code to work correctly with current
  dispatcher semantics (call back forever unless cancelled).

  Revision 1.6  1996/08/28 00:19:51  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.5  1996/08/14 21:22:56  wenger
  Minor dispatcher cleanups, etc.  Fixed release script to release
  statically-linked executables for HP and Sun.

  Revision 1.4  1996/08/05 19:48:59  wenger
  Fixed compile errors caused by some of Kevin's recent changes; changed
  the attrproj stuff to make a .a file instead of a .o; added some more
  TData file writing stuff; misc. cleanup.

  Revision 1.3  1996/07/29 21:45:08  wenger
  Fixed various compile errors and warnings.

  Revision 1.2  1996/07/23 20:13:08  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

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
static const char *	version = "1.1.8";

// Master DEVise copyright dates.
static const char *	copyright = "Copyright (c) 1992-1996";

// Trademark logo for each DEVise window.
static const char *	winLogo = "Visualization by DEVise (tm) 1996";


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
 * function: Version::GetWinLogo
 * Get the DEVise window trademark logo message.
 */
const char *
Version::GetWinLogo()
{
	return winLogo;
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
