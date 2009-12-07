/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2009
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
  Revision 1.83  2009/12/03 21:52:22  wenger
  Merged devise_1_10_br_0 thru devise_1_10_br_1 to trunk.

  Revision 1.82  2009/12/03 16:15:14  wenger
  Changed DEVise version to 1.11.0x1 and JS version to 5.13.0; added
  appropriate version history sections.

  Revision 1.81.2.3  2009/12/07 20:55:18  wenger
  Changed DEVise version to 1.10.1x1 and JavaScreen version to 5.12.1x1;
  added corresponding version history sections.

  Revision 1.81.2.2  2009/12/03 17:21:58  wenger
  Cleaned up version numbers & version history for 1.10.0 release.

  Revision 1.81.2.1  2009/12/03 15:55:46  wenger
  Changed DEVise version to 1.10.0x1 and JS version to 5.12.0 for
  1.10 branch; added appropriate version history sections.

  Revision 1.81  2009/11/23 21:11:19  wenger
  Changed version to 1.9.2x1, added 1.9.2 section in version history.

  Revision 1.80  2009/11/20 21:42:26  wenger
  Changed version to 1.9.1 for local release.

  Revision 1.79  2009/11/16 21:48:46  wenger
  Added new 'isAngle' field to the mapping for HighLow symbols -- if
  this is turned on, the HighLow symbols treat the data as angles
  from -180 to 180 degrees, with special provision for angles that
  "wrap around" the -180 to 180 boundary (low > high in the data).

  Revision 1.78  2009/05/15 20:29:47  wenger
  Implemented to-do 04.001 (be able to exclude views from drill-down;
  this is needed to fix Peptide-CGI bug 071); also fixed some dangerous
  code (strcpy, strcat) in Session.c; added GUI for setting drill-down
  exclusion and copying it when copying a view.

  Revision 1.77  2009/05/13 22:43:56  wenger
  Changed version to 1.9.1x3.

  Revision 1.76  2009/05/13 22:41:30  wenger
  Merged x86_64_centos5_br_0 thru x86_64_centos5_br_1/dist_1_9_1x2 to
  the trunk.

  Revision 1.75.6.1  2009/05/01 22:26:41  wenger
  Debug code (and a few actual changes) trying to get DEVise to work
  on the x86_64/Centos 5 machines at BMRB (currently, opening
  histogram2.ds causes a core dump).

  Revision 1.75  2006/12/08 16:24:50  wenger
  Merged V1_8b0_br_1 thru V1_8b0_br_2 to the trunk (took some manual
  changes to merge the DEViseCanvas.java stuff correctly).

  Revision 1.74  2006/07/21 16:26:40  wenger
  Updated versions to reflect 1.9.0 release.

  Revision 1.73  2006/07/11 18:32:59  wenger
  Minor cleanups for DEVise 1.9.0 -- changed versions, etc.

  Revision 1.72  2006/05/26 16:23:05  wenger
  Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.

  Revision 1.71  2006/05/25 20:15:32  wenger
  Merged V1_8b0_br_0 thru V1_8b0_br_1 to the trunk.

  Revision 1.70.2.1  2006/05/02 16:51:40  wenger
  Added "J" to versions so we know it's the Jmol branch.

  Revision 1.70  2005/12/06 22:53:05  wenger
  Updated DEVise and JavaScreen versions for the trunk (now 1.9).

  Revision 1.69.2.2  2006/07/13 21:48:21  wenger
  Changed DEVise and JavaScreen versions to 1.8.1x1 and 5.6.1x1
  so that all 1.8.0 and 5.6.0 stuff is included in 1.9.0 and
  5.7.0.

  Revision 1.69.2.1  2005/12/06 22:49:41  wenger
  Updated DEVise and JavaScreen versions for the 1.8 branch.

  Revision 1.69  2005/12/06 20:04:15  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.68  2003/01/13 19:25:28  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.67  2002/07/19 17:07:26  wenger
  Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.

  Revision 1.66  2002/06/17 19:41:08  wenger
  Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.

  Revision 1.65  2002/05/01 21:30:13  wenger
  Merged V1_7b0_br thru V1_7b0_br_1 to trunk.

  Revision 1.64.2.20  2005/11/01 18:53:59  wenger
  Changed DEVise version to 1.7.20x1, JavaScreen version to 5.5.3x1.

  Revision 1.64.2.19  2004/05/28 15:53:51  wenger
  Changed version back to 1.7.19.

  Revision 1.64.2.18  2004/05/24 18:31:42  wenger
  Argh -- changed version back to 1.7.18 for new distribution that
  works for RH 7.2.

  Revision 1.64.2.17  2004/05/24 14:40:24  wenger
  Changed version to 1.7.19.

  Revision 1.64.2.16  2004/01/05 20:41:56  wenger
  Changed overall copyright message to include 2004.

  Revision 1.64.2.15  2003/10/27 16:52:47  wenger
  DEVise version is now 1.7.18.

  Revision 1.64.2.14  2003/07/29 18:22:20  wenger
  Version is now 1.7.17, since we've released 1.7.16 to the outside world.

  Revision 1.64.2.13  2003/07/02 15:22:46  wenger
  Updated version to 1.7.16, because 1.7.15 is now going out.

  Revision 1.64.2.12  2003/04/16 17:44:29  wenger
  Changed version to 1.7.15.

  Revision 1.64.2.11  2003/02/07 17:44:17  wenger
  Changed copyrights to 2003.

  Revision 1.64.2.10  2003/01/09 22:21:59  wenger
  Added "link multiplication factor" feature; changed version to 1.7.14.

  Revision 1.64.2.9  2002/11/15 22:44:35  wenger
  Views with no TData records don't contribute to filter values on 'home'
  (this helps to fix some problems with the Condor user visualizations);
  added cursorHome command; changed version to 1.7.13.

  Revision 1.64.2.8  2002/11/05 20:02:14  wenger
  Fixed bug 831 (JSPoP can't respond if stuck sending data); incremented
  DEVise and JavaScreen versions.

  Revision 1.64.2.7  2002/09/17 18:58:19  wenger
  Limited warning about horizontal and vertical points per pixel
  mismatch to one time; DEVise version is now 1.7.11.

  Revision 1.64.2.6  2002/09/05 19:14:03  wenger
  Implemented GData attribute value links (but not GUI for creating
  them).

  Revision 1.64.2.5  2002/07/25 19:08:02  wenger
  Incremented version to 1.7.9x.

  Revision 1.64.2.4  2002/06/26 18:25:37  wenger
  This should be the "official" 1.7.8 version.

  Revision 1.64.2.3  2002/05/16 17:11:30  wenger
  Version is now 1.7.8x.

  Revision 1.64.2.2  2002/05/13 17:39:59  wenger
  Final 1.7.7 version.

  Revision 1.64.2.1  2002/04/18 17:25:45  wenger
  Merged js_tmpdir_fix_br_2 to V1_7b0_br (this fixes the problems with
  temporary session files when the JSPoP and DEViseds are on different
  machines).  Note: JS protocol version is now 11.0.

  Revision 1.64.6.1  2002/04/17 17:46:28  wenger
  DEVised, not JSPoP, now does the actual work of creating or clearing
  the temporary session directory (new command from client to DEVised
  means that communication protocol version is now 11.0).  (Client
  switching is not working yet with this code because I need to change
  how temporary sessions are saved and loaded.)

  Revision 1.64  2002/02/21 17:02:13  wenger
  Turned off axis labeling for JS sessions (now done in the JS itself);
  changed version to 1.7.6.

  Revision 1.63  2001/12/17 18:36:59  wenger
  Changed version to 1.7.5.

  Revision 1.62  2001/08/20 18:20:33  wenger
  Fixes to various font problems: XDisplay calculates point sizes correctly
  and uses screen resolution in specifying font; JS passes *its* screen
  resolution to the devised so that fonts show up correctly in the JS
  (JS protocol version now 7.0); changed DEVise version to 1.7.4.

  Revision 1.61  2001/05/18 19:25:35  wenger
  Implemented the DEVise end of 3D drill-down; changed DEVise version to
  1.7.3.

  Revision 1.60  2001/02/20 20:02:54  wenger
  Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
  to the trunk.

  Revision 1.59.4.1  2001/02/16 21:37:59  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

  Revision 1.59  2000/07/12 20:49:27  wenger
  Added first version of metavisualization session description; changed
  DEVise version to 1.7.1.

  Revision 1.58  2000/02/16 14:44:06  wenger
  Changed master copyright string to 2000.

  Revision 1.57  1999/12/06 18:41:06  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

  Revision 1.56  1999/11/16 20:13:01  wenger
  Improvements to debug logging: the beginning of the log isn't overwritten
  when the log wraps around; the values of important environment variables
  are logged at startup.

  Revision 1.55  1999/11/16 17:02:09  wenger
  Removed all DTE-related conditional compiles; changed version number to
  1.7.0 because of removing DTE; removed DTE-related schema editing and
  data source creation GUI.

  Revision 1.54  1999/09/08 20:56:28  wenger
  Removed all Tcl dependencies from the devised (main changes are in the
  Session class); changed version to 1.6.5.

  Revision 1.53  1999/07/13 17:34:10  wenger
  Changed version to 1.6.4 because of new view symbol features.

  Revision 1.52  1999/06/25 15:58:25  wenger
  Improved debug logging, especially for JavaScreen support: JavaScreenCmd.C
  now uses DebugLog facility instead of printf; dispatcher logging is turned
  on by default, and commands and queries are logged; added -debugLog command
  line flag to turn logging on and off.

  Revision 1.51  1999/05/04 17:17:08  wenger
  Merged js_viewsyms_br thru js_viewsyms_br_1 (code for new JavaScreen
  protocol that deals better with view symbols).

  Revision 1.50.2.1  1999/04/29 14:00:06  wenger
  Changed version to 1.6.3 because of new JavaScreen protocol.

  Revision 1.50  1999/02/19 16:05:40  wenger
  Changed version to 1.6.2 because of eliminating UniData.

  Revision 1.49  1999/02/12 20:15:55  wenger
  Extended copyright to 1999.

  Revision 1.48  1999/02/11 19:54:59  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.47.2.1  1999/02/11 18:24:22  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.47  1998/11/19 21:12:51  wenger
  Implemented non-DTE version of DEVise (new code handles data source catalog
  functions; Tables, SQLViews, etc., are not implemented); changed version to
  1.6.0.

  Revision 1.46  1998/08/17 18:51:55  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.45  1998/06/03 17:09:41  wenger
  Rubberband line in JavaScreen now sends updates of all changed windows
  using the "dirty GIF" flag; updated DEVise version to 1.5.3.

  Revision 1.44  1998/03/04 18:40:11  wenger
  Added <arch>/Makefile.config files that now hold stuff like the
  PERL_CORE definitions so those don't have to be repeated in all
  of the lower level makefiles; various other compile fixes; changed
  version to 1.5.2.

  Revision 1.43  1998/02/12 17:17:10  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.42  1998/01/30 21:53:56  wenger
  Updated screen trademark message to 1998.

  Revision 1.41  1998/01/30 02:17:09  wenger
  Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.

  Revision 1.40  1998/01/14 16:39:22  wenger
  Merged cleanup_1_4_7_br_6 thru cleanup_1_4_7_br_7.

  Revision 1.39  1998/01/09 20:45:39  wenger
  Merged cleanup_1_4_7_br_5 thru cleanup_1_4_7_br_6; fixed error in
  previous merge.

  Revision 1.38  1997/12/02 19:35:14  wenger
  Changed DEVise version to 1.5.0.

  Revision 1.37  1997/11/06 20:30:29  wenger
  Changed version to 1.4.8.

  Revision 1.36.2.3  1998/01/16 23:41:47  wenger
  Fixed some problems that Tony found with the client/server communication
  and GIF generation; fixed problem that session files specified on the
  command line were still opened by the Tcl code.

  Revision 1.36.2.2  1998/01/12 20:35:59  wenger
  Incremented version to 1.4.7.2.

  Revision 1.36.2.1  1998/01/09 16:34:03  wenger
  Updated copyright date and version number; minor mods to compile for
  hp and sun; fixed problem with _batchMode flag getting improperly
  reset in the ControlPanel class (prevented using pixmaps instead of
  X windows).

  Revision 1.36  1997/10/03 16:01:37  wenger
  Enabled session opening and saving from back end; incremented version; a
  few more minor fixes to session-related code.

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
#include <stdlib.h>

#include "Version.h"
#include "CompDate.h"
#include "machdep.h"


/*
 * Static global variables.
 */

// Master DEVise version number.
static const char *	version = "1.11.0x1"/*TEMP*/;

// Master DEVise copyright dates.
static const char *	copyright = "Copyright (c) 1992-2009";

// Trademark logo for each DEVise window.
static const char *	winLogo = "Visualization by DEVise (tm) 2006";


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
 * Print and log version and copyright information.
 *
 * Note: we pass in a pointer to the log function, because if this code
 * refers directly to the DebugLog class, the devisec won't link.
 * RKW 1999-06-24.
 */
void
Version::PrintInfo(LogFunc logFunc)
{
  char *msg = "DEVise Data Visualization Software\n";
  printf("%s", msg);
  // if (logFunc) logFunc(msg);

  char buf[256];
  sprintf(buf, "%s\n", copyright);
  printf("%s", buf);
  // if (logFunc) logFunc(buf);

  msg = "By the DEVise Development Group\n";
  printf("%s", msg);
  // if (logFunc) logFunc(msg);

  msg = "All Rights Reserved.\n";
  printf("%s", msg);
  // if (logFunc) logFunc(msg);

  sprintf(buf, "Version %s (%s)\n", version, ARCH_NAME);
  printf("%s", buf);
  // if (logFunc) logFunc(buf);

  sprintf(buf, "Compile date: %s\n", CompDate::Get());
  printf("%s", buf);
  // if (logFunc) logFunc(buf);

  if (logFunc) {
    char *envVars[] = { "DEVISE_DAT", "DEVISE_HOME_TABLE", "DEVISE_CACHE",
        "DEVISE_SCHEMA", "DEVISE_SESSION", "DEVISE_TMP_SESSION",
	"DEVISE_PALETTE", "DEVISE_LIB", "DEVISE_TMP", "DEVISE_WORK",
	"DEVISE_LOG_DIR"};
    int varCount = sizeof(envVars) / sizeof(envVars[0]);
    for (int index = 0; index < varCount; index++) {
      char *value = getenv(envVars[index]);
      if (!value) {
        value = "null";
      }
      char buf[MAXPATHLEN * 2];
      sprintf(buf, "$%s = <%s>", envVars[index], value);
      logFunc(buf);
    }
  }
}

/*============================================================================*/
