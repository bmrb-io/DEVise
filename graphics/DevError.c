/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DevError class.
 */

/*
  $Id$

  $Log$
  Revision 1.11.10.1  2002/09/17 23:34:12  wenger
  Fixed a bunch of memory leaks -- especially in DevError::ReportError()!

  Revision 1.11  2001/10/03 19:09:49  wenger
  Various improvements to error logging; fixed problem with return value
  from JavaScreenCmd::Run().

  Revision 1.10  2001/05/27 18:51:07  wenger
  Improved buffer checking with snprintfs.

  Revision 1.9  1999/10/05 17:55:36  wenger
  Added debug log level.

  Revision 1.8  1999/09/08 20:56:21  wenger
  Removed all Tcl dependencies from the devised (main changes are in the
  Session class); changed version to 1.6.5.

  Revision 1.7  1999/07/16 21:35:49  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

  Revision 1.6  1999/04/02 16:48:46  wenger
  Added timestamp to error messages.

  Revision 1.5  1999/03/24 17:26:03  wenger
  Non-DTE data source code prevents adding duplicate data source names;
  added "nice axis" feature (sets axis limits to multiples of powers of
  10 if enabled); improved the propagation of DEVise errors back to the
  GUI; fixed bug 474 (problem with view home).

  Revision 1.4  1998/07/14 15:39:20  wenger
  Committed Matt's fix for egcs.

  Revision 1.3  1996/09/09 14:31:39  jussi
  Added #ifdef LIBCS statements to make code compile in the
  ClientServer library target.

  Revision 1.2  1996/06/26 23:55:44  jussi
  Added method to turn on/off error reporting.

  Revision 1.1  1996/05/07 16:03:05  wenger
  Added final version of code for reading schemas from session files;
  added an error-reporting class to improve error info.

 */

#define _DevError_c_

#include <sys/time.h>
#include <string.h>
#include <strstream.h>

#include "DevError.h"
#include "Util.h"
#ifndef LIBCS
#include "Init.h"
#endif
#include "DebugLog.h"

#define DEBUG_LOG

/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

Boolean DevError::_enabled = true;
char DevError::_errBuf[MAXPATHLEN * 2];

/*------------------------------------------------------------------------------
 * function: DevError::ReportError
 * Reports an error.  Right now, just prints info to stderr; we might want
 * to do something more sophisticated eventually.
 */
void
DevError::ReportError(const char *message, const char *file, int line,
  int errnum)
{
    ReportError(message, 0, NULL, file, line, errnum);
}

/*------------------------------------------------------------------------------
 * function: DevError::ReportError
 * Reports an error.  Right now, just prints info to stderr; we might want
 * to do something more sophisticated eventually.
 */
void
DevError::ReportError(const char *message, int argc, const char * const *argv,
  const char *file, int line, int errnum)
{
    char * progName = "DEVise";

    if (!_enabled) {
        return;
    }

    // Note: if we don't supply our own buffer here, strstream leaks
    // its buffer.  RKW 2002-09-17.
    const int bufSize = MAXPATHLEN * 2;
    char buf[bufSize];
    ostrstream errStr(buf, bufSize);

    errStr << endl << progName << " error: " << message;
    for (int count = 0; count < argc; count++) {
        errStr << argv[count] << " ";
    }
    errStr << endl;
    if (errnum != devNoSyserr) {
        char *sysStr = strerror(errnum);
	errStr << "  syserr: " << errnum;
	if (sysStr != NULL) {
	    errStr << ": " << sysStr;
	}
	errStr << endl;
    }
    errStr << "  at " << file << ": " << line;

#if 0 // Timestamp is already added by DebugLog class.
    struct timeval errTime;
    if (gettimeofday(&errTime, NULL) >= 0) {
        errStr << " (" << DateString(errTime.tv_sec) << ")\n";
    }
#else
    errStr << endl;
#endif

    errStr << ends;

    fprintf(stderr, "%s", errStr.str());
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelError, errStr.str());
#endif
    strncpy(_errBuf, errStr.str(), MAXPATHLEN * 2);
    _errBuf[MAXPATHLEN * 2 - 1] = '\0'; // ensure termination
}

/*============================================================================*/
