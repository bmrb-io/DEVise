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
  Header file for DevError class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1996/06/26 23:55:44  jussi
  Added method to turn on/off error reporting.

  Revision 1.1  1996/05/07 16:03:08  wenger
  Added final version of code for reading schemas from session files;
  added an error-reporting class to improve error info.

 */

#ifndef _DevError_h_
#define _DevError_h_


#include <stdio.h>
#include "DeviseTypes.h"


const int	devNoSyserr = -9999;

#define		reportError(message, errno) DevError::ReportError((message), \
	__FILE__, __LINE__, (errno))


class DevError
{
public:
    static void ReportError(char *message, char *file, int line, int errno);
    static Boolean SetEnabled(Boolean enabled) {
        Boolean old = _enabled;
        _enabled = enabled;
        return old;
    }

protected:
    static Boolean _enabled;
};


#endif /* _DevError_h_ */

/*============================================================================*/
