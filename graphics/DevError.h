/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Header file for DevError class.
 */

/*
  $Id$

  $Log$
  Revision 1.6  1999/03/24 17:26:03  wenger
  Non-DTE data source code prevents adding duplicate data source names;
  added "nice axis" feature (sets axis limits to multiples of powers of
  10 if enabled); improved the propagation of DEVise errors back to the
  GUI; fixed bug 474 (problem with view home).

  Revision 1.5  1998/07/14 15:39:36  wenger
  Committed Matt's fix for egcs.

  Revision 1.4  1996/10/02 15:23:35  wenger
  Improved error handling (modified a number of places in the code to use
  the DevError class).

  Revision 1.3  1996/07/23 20:12:42  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

  Revision 1.2  1996/06/26 23:55:44  jussi
  Added method to turn on/off error reporting.

  Revision 1.1  1996/05/07 16:03:08  wenger
  Added final version of code for reading schemas from session files;
  added an error-reporting class to improve error info.

 */

#ifndef _DevError_h_
#define _DevError_h_


#include <stdio.h>
#include <errno.h>
#include <sys/param.h>

#include "DeviseTypes.h"


const int	devNoSyserr = -9999;

#define		reportError(message, syserr) DevError::ReportError((message), \
	__FILE__, __LINE__, (syserr))
#define		reportErrSys(message) DevError::ReportError((message), \
	__FILE__, __LINE__, errno)
#define		reportErrNosys(message) DevError::ReportError((message), \
	__FILE__, __LINE__, devNoSyserr)


class DevError
{
public:
    static void ReportError(const char *message, char *file, int line,
      int errnum);
    static Boolean SetEnabled(Boolean enabled) {
        Boolean old = _enabled;
        _enabled = enabled;
        return old;
    }

    static void ResetError() { _errBuf[0] = '\0'; }
    static const char *GetLatestError() { return _errBuf; }

protected:
    static Boolean _enabled;
    static char _errBuf[MAXPATHLEN * 2];
};


#endif /* _DevError_h_ */

/*============================================================================*/
