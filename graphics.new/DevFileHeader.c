/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.7  2000/01/13 23:07:04  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.6  1999/12/06 18:41:04  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

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

//#define DEBUG

/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static const char *_startHeaderPref = "# DEVise startHeader";
static const char *_fileTypePref = "# file type: ";
static const char *_fileDatePref = "# file date: ";
static const char *_deviseVersionPref = "# DEVise version: ";
static const char *_deviseDatePref = "# DEVise date: ";
static const char *_endHeaderPref = "# DEVise endHeader";

/*----------------------------------------------------------------------------
 * function: DevFileHeader::Get
 * Get the DEVise file header for the given file type.
 */
char *
DevFileHeader::Get(const char *fileType)
{
	//TEMP -- not reentrant
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
		!strcmp(fileType, FILE_TYPE_CMDLOG) ||
		!strcmp(fileType, FILE_TYPE_JSCMDCACHE))
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
		"%s\n" // start header
		"%s%s\n" // file type
		"%s%s\n" // file date
		"%s%s\n" // devise version
		"%s%s\n" // devise date
		"%s\n", // end header
		_startHeaderPref, _fileTypePref, fileType, _fileDatePref, date,
		_deviseVersionPref, version, _deviseDatePref, devDate,
		_endHeaderPref);

	return headerBuf;
}

/*----------------------------------------------------------------------------
 * function: DevFileHeader::Read
 * Read the header info from the given file.
 */
DevStatus
DevFileHeader::Read(FILE *file, Info &info)
{
    DevStatus result(StatusOk);

    info.fileType[0] = '\0';
    info.fileDate[0] = '\0';
    info.deviseVersion[0] = '\0';
    info.deviseDate[0] = '\0';

    if (result.IsComplete()) result += GetInfo(file, _startHeaderPref, NULL,
	  0);
    if (result.IsComplete()) result += GetInfo(file, _fileTypePref,
	  info.fileType, info.bufSize);
    if (result.IsComplete()) result += GetInfo(file, _fileDatePref,
	  info.fileDate, info.bufSize);
    if (result.IsComplete()) result += GetInfo(file, _deviseVersionPref,
	  info.deviseVersion, info.bufSize);
    if (result.IsComplete()) result += GetInfo(file, _deviseDatePref,
	  info.deviseDate, info.bufSize);
    if (result.IsComplete()) result += GetInfo(file, _endHeaderPref, NULL, 0);

    if (!result.IsComplete()) {
	    rewind(file);
	}

	return result;
}

/*----------------------------------------------------------------------------
 * function: DevFileHeader::GetInfo
 * Get the info with the given prefix from the given file.
 */
DevStatus
DevFileHeader::GetInfo(FILE *file, const char *prefix, char *infoBuf, 
  int infoBufSize)
{
#if defined(DEBUG)
    printf("DevFileHeader::GetInfo(%s, 0x%p, %d)\n", prefix, infoBuf,
	  infoBufSize);
#endif

    DevStatus result(StatusOk);

	int prefixLen = strlen(prefix);
	int lineBufSize = 1024;
	char lineBuf[lineBufSize];
	Boolean foundInfo = false;
	while (!foundInfo &&
	  (nice_fgets(lineBuf, lineBufSize, file) == fgets_ok)) {
	    if (!strncmp(prefix, lineBuf, prefixLen)) {
			if (infoBuf && infoBufSize > 0) {
			    result += nice_strncpy(infoBuf, &lineBuf[prefixLen],
				  infoBufSize);
			}
		    foundInfo = true;
		}
	}

	if (!foundInfo) {
		char errBuf[1024];
		sprintf(errBuf, "Field %s not found in file", prefix);
		reportErrNosys(errBuf);
		result += StatusFailed;
	}

	return result;
}

/*============================================================================*/
