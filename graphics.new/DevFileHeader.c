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
  Implementation of DevFileHeader class.
 */

/*
  $Id$

  $Log$
  Revision 1.5  1998/03/27 15:08:53  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

  Revision 1.4  1997/07/22 15:36:35  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.3  1997/06/25 21:25:28  wenger
  Added writeDesc (write session description) command needed by Hongyu's
  Java client.

  Revision 1.2  1996/11/03 02:41:36  kmurli
  Modified to include the query schema level. Also modified to include DQL
  processing

  Revision 1.1  1996/07/11 17:25:36  wenger
  Devise now writes headers to some of the files it writes;
  DataSourceSegment class allows non-fixed data length with non-zero
  offset; GUI for editing schema files can deal with comment lines;
  added targets to top-level makefiles to allow more flexibility.

 */

#define _DevFileHeader_c_

#include <stdio.h>

#include "DevFileHeader.h"
#include "Version.h"
#include "CompDate.h"
#include "Util.h"
#include "DevError.h"


/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DevFileHeader::Get
 * Get the DEVise file header for the given file type.
 */
char *
DevFileHeader::Get(char *fileType)
{
	static char		headerBuf[1024];

	// Make sure the file type is legal.
	if (!strcmp(fileType, FILE_TYPE_SESSION) ||
		!strcmp(fileType, FILE_TYPE_TDATA) ||
		!strcmp(fileType, FILE_TYPE_DATACAT) ||
		!strcmp(fileType, FILE_TYPE_PSCHEMA) ||
		!strcmp(fileType, FILE_TYPE_LSCHEMA) ||
		!strcmp(fileType, FILE_TYPE_QSCHEMA) ||
		!strcmp(fileType, FILE_TYPE_TEMP) ||
		!strcmp(fileType, FILE_TYPE_WORK) ||
		!strcmp(fileType, FILE_TYPE_CACHE) ||
		!strcmp(fileType, FILE_TYPE_CORAL) ||
		!strcmp(fileType, FILE_TYPE_PIXMAP) ||
		!strcmp(fileType, FILE_TYPE_SCHEMACAT) ||
		!strcmp(fileType, FILE_TYPE_SESSIONDESCL) ||
		!strcmp(fileType, FILE_TYPE_SESSIONDESCP) ||
		!strcmp(fileType, FILE_TYPE_LINKDESC) ||
		!strcmp(fileType, FILE_TYPE_DEBUGLOG) ||
		!strcmp(fileType, FILE_TYPE_CMDLOG))
	{
		// File type is legal.
	}
	else
	{
		// File type is illegal.
		char errBuf[256];
		sprintf(errBuf, "Illegal file type (%s)", fileType);
		reportError(errBuf, devNoSyserr);
		fileType = "unknown";
	}

	// Get the DEVise version.
	const char *	version = Version::Get();

	// Get the DEVise compile date.
	const char *	devDate = CompDate::Get();

	// Get the current date.
	time_t tm = time(NULL);
	const char *	date = DateString(tm);

	sprintf(headerBuf,
		"# DEVise startHeader\n"
		"# file type: %s\n"
		"# file date: %s\n"
		"# DEVise version: %s\n"
		"# DEVise date: %s\n"
		"# DEVise endHeader\n",
		fileType, date, version, devDate);

	return headerBuf;
}

/*============================================================================*/
