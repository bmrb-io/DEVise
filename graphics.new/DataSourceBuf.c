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
  Implementation of the DataSourceBuf class.
 */

/*
  $Id$

  $Log$
 */

#define _DataSourceBuf_c_

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <unistd.h>

#include "DataSourceBuf.h"
#include "Util.h"
#include "DevError.h"
#include "tapedrive.h"


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::DataSourceBuf
 * DataSourceBuf constructor.
 */
DataSourceBuf::DataSourceBuf(char *buffer, char *label) : DataSource(label)
{
	DO_DEBUG(printf("DataSourceBuf::DataSourceBuf(%s)\n", label));

	_sourceBuf = buffer;
	_currentLoc = NULL;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::~DataSourceBuf
 * DataSourceBuf destructor.
 */
DataSourceBuf::~DataSourceBuf()
{
	DO_DEBUG(printf("DataSourceBuf::~DataSourceBuf()\n"));
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::Open
 * Simulate an fopen() on the DataSourceBuf object.
 */
DevStatus
DataSourceBuf::Open(char *)
{
	DO_DEBUG(printf("DataSourceBuf::Open()\n"));

	DevStatus	result = StatusOk;

	_currentLoc = _sourceBuf;

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::Close
 * Simulate an fclose() on the DataSourceBuf object.
 */
DevStatus
DataSourceBuf::Close()
{
	DO_DEBUG(printf("DataSourceBuf::Close()\n"));

	DevStatus	result = StatusOk;

	_currentLoc = NULL;

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::Fgets
 * Simulate an fgets() on the DataSourceBuf object.
 */
char *
DataSourceBuf::Fgets(char *buffer, int bufSize)
{
	DO_DEBUG(printf("DataSourceBuf::Fgets()\n"));

	char *		result = buffer;
	char *		endOfBuf = buffer + bufSize - 1;
	Boolean		endOfLine = false;
	char *		outputP = buffer;

	if (_currentLoc == NULL)
	{
		reportError("DataSourceBuf: not open", devNoSyserr);
	}
	else
	{
		while ((outputP < endOfBuf) && !endOfLine)
		{
			*outputP = *_currentLoc;

			// End of string in the buffer is equivalent to EOF in real
			// fgets().
			if (*_currentLoc == '\0')
			{
				if (outputP == buffer) result = NULL;	// Signal "EOF".
				break;
			}

			if (*_currentLoc == '\n') endOfLine = true;

			_currentLoc++;
			outputP++;
		}

		// Terminate the output string.
		if (outputP < endOfBuf)
		{
			*outputP = '\0';
		}
		else
		{
			*endOfBuf = '\0';
		}
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::Fread
 * Simulate fread() on the buffer associated with this object.
 */
size_t
DataSourceBuf::Fread(char *buf, size_t size, size_t itemCount)
{
	DO_DEBUG(printf("DataSourceBuf::Fread()\n"));

	DOASSERT(0, "DataSourceBuf::Fread not yet implemented");

	return 0;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::Seek
 * Simulate seek() on the buffer associated with this object.
 * Note: no checking for going past end of buffer.
 */
int
DataSourceBuf::Seek(long offset, int from)
{
	DO_DEBUG(printf("DataSourceBuf::Seek()\n"));

	int		result = 0;

	switch (from)
	{
	case SEEK_SET:
		_currentLoc = _sourceBuf + offset;
		break;

	case SEEK_CUR:
		_currentLoc += offset;
		break;

	case SEEK_END:
		(void) gotoEnd();
		break;

	default:
		reportError("Illegal 'seek from' value", devNoSyserr);
		result = -1;
		break;
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::Tell
 * Simulate tell() on the buffer associated with this object.
 */
long
DataSourceBuf::Tell()
{
	DO_DEBUG(printf("DataSourceBuf::Tell()\n"));

	return _currentLoc - _sourceBuf;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::gotoEnd
 * Go to the end of the buffer associated with this object, and return the
 * offset of the end of the buffer.
 */
int
DataSourceBuf::gotoEnd()
{
	DO_DEBUG(printf("DataSourceBuf::gotoEnd()\n"));

	int result = strlen(_sourceBuf);

	_currentLoc = _sourceBuf + result;

	return result;
}

/*============================================================================*/
