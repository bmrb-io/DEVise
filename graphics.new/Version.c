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
  Implementation of Version class.
 */

/*
  $Id$

  $Log$
  Revision 1.35  1997/08/20 22:11:13  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.34.2.1  1997/08/14 16:16:06  wenger
  Statistics, etc., now work correctly for timed-out draw in ViewScatter-
  type views; bumped up version because of improved stop capability.

  Revision 1.34  1997/07/16 17:50:30  wenger
  Oops!  I didn't realize that I had already bumped the version for
  UniData!

  Revision 1.33  1997/07/16 16:15:55  wenger
  Incremented version because of UniData in DTE.

  Revision 1.32  1997/06/18 15:33:17  wenger
  Fixed bug 177; improved workaround of bug 137; incremented version
  number (because of Unidata being added).

  Revision 1.31  1997/04/30 21:45:39  wenger
  Fixed non-constant strings in complex mappings bug; TDataAsciiInterp
  no longer gives warning message on blank data lines; added makefile
  targets to make a Purify'd version of multi; fixed uninitialized memory
  read in the DList code; fixed bug that caused 1.4 version of multi to
  always crash; better error messages in DTE command parser; version is
  now 1.4.4.

  Revision 1.30  1997/04/21 16:39:57  wenger
  Removed lex.yy.c from CVS to fix compile problems; fixed some compile
  warnings in the DTE; default is now to compile with neither -O nor
  -gstabs -- set DEV_DEBUG env. var. to -1 for -O, anything else for
  -gstabs; bumped version number up to 1.4.3.

  Revision 1.29  1997/03/25 17:59:28  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.28  1997/02/26 16:31:49  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.27  1997/02/14 16:47:48  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.26  1997/01/31 19:04:18  wenger
  Changed Devise version to 1.4.0.

  Revision 1.25.4.4  1997/02/27 22:46:25  wenger
  Most of the way to having Tasvir images work in PostScript output;
  various WindowRep-related fixes; version now 1.3.4.

  Revision 1.25.4.3  1997/02/20 20:50:45  wenger
  Changed mapping to use ShapeAttr4 for line width.

  Revision 1.25.4.2  1997/02/14 19:45:20  wenger
  Fixed bug 158; bumped version up to 1.3.2.

  Revision 1.25.4.1  1997/02/07 15:21:33  wenger
  Updated Devise version to 1.3.1; fixed bug 148 (GUI now forces unique
  window names); added axis toggling and color selections to Window menu;
  other minor fixes to GUI; show command to Tasvir now requests image to
  be shown all at once.

  Revision 1.25  1997/01/23 17:02:43  wenger
  Bumped version up to 1.3.0.

  Revision 1.24  1997/01/14 20:05:48  wenger
  Fixed some compile warnings; fixed relative positions of OK/Cancel
  buttons in link GUI; removed some debug code I accidentally left
  in place.

  Revision 1.23  1997/01/09 18:38:40  wenger
  Updated copyright dates to 1997.

  Revision 1.22  1997/01/08 19:01:52  wenger
  Fixed bug 064 and various other problems with drawing piled views;
  added related debug code.

  Revision 1.21  1996/12/27 17:00:13  wenger
  Got latest code to compile for sun, solsparc, and hp; updated dependencies;
  changed 'Import' to 'New Table' in main window GUI to reflect change of
  function.

  Revision 1.20  1996/12/15 20:23:08  wenger
  Incremented revision; prints current architecture at startup.

  Revision 1.19  1996/12/13 18:20:58  wenger
  Added release.linux; incremented DEVise revision.

  Revision 1.18  1996/12/05 15:31:13  wenger
  Bumped up revision.

  Revision 1.17  1996/11/26 15:44:21  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.16  1996/11/20 20:35:22  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.15  1996/11/18 23:11:32  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.14  1996/10/23 14:57:13  wenger
  Renamed LandsendDateDiffComposite composite parser to
  MailorderDateDiffComposite as part of getting rid of the "Land's
  End" name in visible places; schema types changed from "LANDSEND..."
  to "MAILORDER...".

  Revision 1.13  1996/10/08 21:49:10  wenger
  ClassDir now checks for duplicate instance names; fixed bug 047
  (problem with FileIndex class); fixed various other bugs.

  Revision 1.12  1996/10/04 19:58:50  wenger
  Temporarily (?) removed threads from Devise (removed -lpthread from
  some Makefiles so Purify works); other minor cleanups.

  Revision 1.11  1996/09/27 21:09:38  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.10  1996/09/19 19:32:52  wenger
  Devise now complains about illegal command-line flags (fixes bug 042).

  Revision 1.9  1996/09/10 20:07:24  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

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
#include "machdep.h"


/*
 * Static global variables.
 */

// Master DEVise version number.
static const char *	version = "1.4.7";

// Master DEVise copyright dates.
static const char *	copyright = "Copyright (c) 1992-1997";

// Trademark logo for each DEVise window.
static const char *	winLogo = "Visualization by DEVise (tm) 1997";


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
  printf("Version %s (%s)\n", version, ARCH_NAME);
  printf("Compile date: %s\n", CompDate::Get());
}

/*============================================================================*/
