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
  $Id$

  $Log$
  Revision 1.14  1997/01/28 16:50:37  wenger
  Fixed bugs 122 and 124 (reduced data and X axis area so selection rectangle
  doesn't draw over them); Devise now returns a status of 0 when exiting
  normally; cleaned up some of the exit code.

  Revision 1.13  1996/12/30 23:51:07  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.12  1996/12/20 16:49:27  wenger
  Conditionaled out RTree code for libcs and attrproj.

  Revision 1.11  1996/12/15 06:52:31  donjerko
  Added the initialization and shutdown for RTree code.

  Revision 1.10  1996/11/26 09:29:59  beyer
  The exit code now removes the temp directory for its process.
  This could be more portable -- right now it just calls 'rm -fr'.

  Revision 1.9  1996/09/04 21:24:48  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.8  1996/05/20 18:44:59  jussi
  Merged with ClientServer library code.

  Revision 1.7  1996/05/20 17:47:37  jussi
  Had to undo some previous changes.

  Revision 1.6  1996/05/09 18:12:11  kmurli
  No change to this makefile.

  Revision 1.5  1996/04/22 21:10:42  jussi
  Error message printed to console includes file name and line
  number.

  Revision 1.4  1996/04/16 19:45:35  jussi
  Added DoAbort() method and DOASSERT macro.

  Revision 1.3  1996/01/11 21:51:14  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.2  1995/09/05 21:12:45  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Exit.h"

#if !defined(LIBCS)
#include "Init.h"
#include "Util.h"
#endif

#if !defined(LIBCS) && !defined(ATTRPROJ)
#include "Control.h"
#include "DaliIfc.h"
#include "ETkIfc.h"
#endif

#if !defined(LIBCS) && !defined(ATTRPROJ)
#include "RTreeCommon.h"
#endif

void Exit::DoExit(int code)
{
#if defined(DEBUG)
    printf("Exit::DoExit(%d)\n", code);
#endif

#if !defined(LIBCS) && !defined(ATTRPROJ)
    shutdown_system(VolumeName, RTreeFile, VolumeSize);
#endif

#if !defined(LIBCS)
    /* Clean out temp directory.  This code was moved here from Control.c,
     * replacing inline code.  The cleanup should be here, so that it
     * gets called whenever we exit (as much as is possible). */
    char *tmpDir = Init::TmpDir();
    ClearDir(tmpDir);
    (void)rmdir(tmpDir);
#endif

#if !defined(LIBCS) && !defined(ATTRPROJ)
    if (Init::DaliQuit()) {
	(void) DaliIfc::Quit(Init::DaliServer());
    }

    if (ETkIfc::GetQuitFlag()) {
	(void) ETkIfc::SendSimpleCommand(ETkIfc::GetServer(), "quit");
    }
    
    if (Init::DoAbort()) {
	fflush(stdout);
	fflush(stderr);
	abort();
    }
#endif

    exit(code);
}

void Exit::DoAbort(char *reason, char *file, int line)
{
  char fulltext[256];
  sprintf(fulltext, "%s (%s:%d)", reason, file, line);

  fprintf(stderr, "An internal error has occurred. The reason is:\n");
  fprintf(stderr, "  %s\n", fulltext);

#if !defined(LIBCS) && !defined(ATTRPROJ)
  ControlPanel::Instance()->DoAbort(fulltext);
#endif

  DoExit(2);
}
