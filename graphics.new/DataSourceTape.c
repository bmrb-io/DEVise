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
  Implementation of DataSourceTape class.
 */

/*
  $Id$

  $Log$
 */

#define _DataSourceTape_c_

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <unistd.h>

#include "DataSourceTape.h"
#include "Util.h"
#include "DevError.h"
#include "tapedrive.h"


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DataSourceTape::DataSourceTape
 * DataSourceTape constructor.
 */
DataSourceTape::DataSourceTape(char *filename, char *label) : DataSource(label)
{
	DO_DEBUG(printf("DataSourceTape::DataSourceTape(%s, %s)\n", filename,
		label));

	_filename = strdup(filename);
	_tapeP = NULL;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::~DataSourceTape
 * DataSourceTape destructor.
 */
DataSourceTape::~DataSourceTape()
{
	DO_DEBUG(printf("DataSourceTape::~DataSourceTape()\n"));

	delete [] _filename;
	if (_tapeP != NULL) delete _tapeP;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::Open
 * Create a TapeDrive object corresponding to the file named in this object.
 */
DevStatus
DataSourceTape::Open(char *mode)
{
	DO_DEBUG(printf("DataSourceTape::Open()\n"));

	DevStatus	result = StatusOk;

	_tapeP = new TapeDrive(_filename, mode);
	if (_tapeP == NULL)
	{
		reportError("Error instatiating TapeDrive object", devNoSyserr);
		result = StatusFailed;
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::Close
 * Delete the TapeDrive object associated with this object.
 */
DevStatus
DataSourceTape::Close()
{
	DO_DEBUG(printf("DataSourceTape::Close()\n"));

	if (_tapeP != NULL) delete _tapeP;

	return StatusOk;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::Fgets
 * Simulate an fgets() on the TapeDrive object associated with this object.
 */
char *
DataSourceTape::Fgets(char *buffer, int size)
{
	DO_DEBUG(printf("DataSourceTape::Fgets()\n"));

	char *		result = buffer;

	if (_tapeP->gets(buffer, size) != size) result = NULL;

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::Fread
 * Simulate an fread() on the TapeDrive object associated with this object.
 */
size_t
DataSourceTape::Fread(char *buf, size_t size, size_t itemCount)
{
	DO_DEBUG(printf("DataSourceTape::Fread()\n"));

	return _tapeP->read((void *) buf, size * itemCount) / size;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::Read
 * Simulate a read() on the TapeDrive object associated with this object.
 */
size_t
DataSourceTape::Read(char *buf, int byteCount)
{
	DO_DEBUG(printf("DataSourceTape::Read()\n"));

	return _tapeP->read((void *) buf, byteCount);
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::Seek
 * Simulate a seek() on the TapeDrive object associated with this object.
 */
int
DataSourceTape::Seek(long offset, int from)
{
	DO_DEBUG(printf("DataSourceTape::Seek()\n"));
	int		result = 0;

	if (from != SEEK_SET)
	{
		reportError(
			"'Seek from' value must be SEEK_SET for DataSourceTape object",
			devNoSyserr);
		result = -1;
	}
	else
	{
		result = _tapeP->seek(offset);
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::Tell
 * Simulate a tell() on the TapeDrive object associated with this object.
 */
long
DataSourceTape::Tell()
{
	DO_DEBUG(printf("DataSourceTape::Tell()\n"));

	return _tapeP->tell();
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::gotoEnd
 * Go to the end of the tape file associated with this object, and return the
 * offset of the end of the file.
 */
int
DataSourceTape::gotoEnd()
{
	DO_DEBUG(printf("DataSourceTape::gotoEnd()\n"));
	int		result = 0;

	long end = 1024 * 1024 * 1024;
	if (_tapeP->seek(end) >= end)
	{
		reportError("Could not seek to end of tape file", devNoSyserr);
		result = -1;
	}
	else
	{
		result = _tapeP->tell();
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::append
 * Append the given record to the end of the tape file associated with this
 * object.
 */
int
DataSourceTape::append(void *buf, int recSize)
{
	DO_DEBUG(printf("DataSourceTape::append()\n"));

	DOASSERT(0, "DataSourceTape::append not yet implemented");

	return _tapeP->append(buf, recSize);
}

/*------------------------------------------------------------------------------
 * function: DataSourceTape::printStats
 * Print usage statistics.
 */
void
DataSourceTape::printStats()
{
	DO_DEBUG(printf("DataSourceTape::printStats()\n"));

	_tapeP->printStats();

	return;
}

/*============================================================================*/
