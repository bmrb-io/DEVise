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
  Implementation of DevError class.
 */

/*
  $Id$

  $Log$
 */

#define _DevError_c_

#include "DevError.h"
#include "Init.h"

/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DevError::ReportError
 * Reports an error.  Right now, just prints info to stderr; we might want
 * to do something more sophisticated eventually.
 */
void
DevError::ReportError(char *message, char *file, int line, int errno)
{
	char *	progName = "DEVise";

	fprintf(stderr, "\n%s error: %s\n", progName, message);
	fprintf(stderr, "  at %s: %d\n", file, line);
	if (errno != devNoSyserr)
	{
		fprintf(stderr, "  syserr: %d: ", errno);
		perror(NULL);
	}
	fprintf(stderr, "\n");
}

/*============================================================================*/
