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
 */

#ifndef _DevError_h_
#define _DevError_h_


#include <stdio.h>


const int	devNoSyserr = -9999;

#define		reportError(message, errno) DevError::ReportError((message), \
	srcFile, __LINE__, (errno))


class DevError
{
public:
	static void ReportError(char *message, char *file, int line, int errno);
};


#endif /* _DevError_h_ */

/*============================================================================*/
