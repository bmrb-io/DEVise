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
  Implementation of DataSourceFileStream class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1996/05/22 17:52:03  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

 */

#define _DataSourceFileStream_c_

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <unistd.h>

#include "DataSourceFileStream.h"
#include "Util.h"
#include "DevError.h"


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::DataSourceFileStream
 * DataSourceFileStream constructor.
 */
DataSourceFileStream::DataSourceFileStream(char *filename, char *label) : DataSource(label)
{
	DO_DEBUG(printf("DataSourceFileStream::DataSourceFileStream(%s, %s)\n", filename,
		label));

	_filename = strdup(filename);
	_file = NULL;
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::~DataSourceFileStream
 * DataSourceFileStream destructor.
 */
DataSourceFileStream::~DataSourceFileStream()
{
	DO_DEBUG(printf("DataSourceFileStream::~DataSourceFileStream()\n"));

	delete [] _filename;
	if (_file != NULL) fclose(_file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Open
 * Do an fopen() on the file corresponding to the DataSourceFileStream object.
 */
DevStatus
DataSourceFileStream::Open(char *mode)
{
	DO_DEBUG(printf("DataSourceFileStream::Open()\n"));

	DevStatus	result = StatusOk;

	_file = fopen(_filename, mode);
	if (_file == NULL)
	{
		char	errBuf[MAXPATHLEN+100];
		sprintf(errBuf, "unable to open file %s", _filename);
		reportError(errBuf, errno);
		result = StatusFailed;
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Close
 * Do an fclose() on the file corresponding to the DataSourceFileStream object.
 */
DevStatus
DataSourceFileStream::Close()
{
	DO_DEBUG(printf("DataSourceFileStream::Close()\n"));

	DevStatus	result = StatusOk;

	if (fclose(_file) != 0)
	{
		reportError("error closing file", errno);
		result = StatusFailed;
	}
	_file = NULL;	// So destructor doesn't try to close it again.

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Fgets
 * Do an fgets() on the file corresponding to the DataSourceFileStream object.
 */
char *
DataSourceFileStream::Fgets(char *buffer, int bufSize)
{
	DO_DEBUG(printf("DataSourceFileStream::Fgets()\n"));

	return fgets(buffer, bufSize, _file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Fileno
 * Returns the the file descriptor associated with this object.
 */
int
DataSourceFileStream::Fileno()
{
	DO_DEBUG(printf("DataSourceFileStream::Fileno()\n"));

	return fileno(_file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Fread
 * Do fread() on the stream associated with this object.
 */
size_t
DataSourceFileStream::Fread(char *buf, size_t size, size_t itemCount)
{
	DO_DEBUG(printf("DataSourceFileStream::Fread()\n"));

	return fread(buf, size, itemCount, _file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Read
 * Do read() on the stream associated with this object.
 */
size_t
DataSourceFileStream::Read(char *buf, int byteCount)
{
	DO_DEBUG(printf("DataSourceFileStream::Read()\n"));

	return read(fileno(_file), buf, byteCount);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Fwrite
 * Do fwrite() on the stream associated with this object.
 */
size_t
DataSourceFileStream::Fwrite(const char *buf, size_t size, size_t itemCount)
{
	DO_DEBUG(printf("DataSourceFileStream::Fwrite()\n"));

	return fwrite(buf, size, itemCount, _file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Write
 * Do write() on the stream associated with this object.
 */
size_t
DataSourceFileStream::Write(const char *buf, size_t byteCount)
{
	DO_DEBUG(printf("DataSourceFileStream::Write()\n"));

	return write(fileno(_file), buf, byteCount);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Seek
 * Do fseek() on the stream associated with this object.
 */
int
DataSourceFileStream::Seek(long offset, int from)
{
	DO_DEBUG(printf("DataSourceFileStream::Seek()\n"));

	return fseek(_file, offset, from);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::Tell
 * Do ftell() on the stream associated with this object.
 */
long
DataSourceFileStream::Tell()
{
	DO_DEBUG(printf("DataSourceFileStream::Tell()\n"));

	return ftell(_file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::gotoEnd
 * Go to the end of the file associated with this object, and return the
 * offset of the end of the file.
 */
int
DataSourceFileStream::gotoEnd()
{
	DO_DEBUG(printf("DataSourceFileStream::gotoEnd()\n"));

	int		result = 0;

	if (fseek(_file, 0, SEEK_END) < 0)
	{
		reportError("Cannot seek to end of file", errno);
		result = -1;
		DOASSERT(false, "");
	}
	else
	{
		result = ftell(_file);
		if (result < 0)
		{
			reportError("Cannot get current file position", errno);
			result = -1;
			DOASSERT(false, "");
		}
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceFileStream::append
 * Append the given record to the end of the file associated with this
 * object.
 */
int
DataSourceFileStream::append(void *buf, int recSize)
{
	DO_DEBUG(printf("DataSourceFileStream::append()\n"));
	int		result = 0;

	if (gotoEnd() < 0)
	{
		reportError("Cannot go to end of file", devNoSyserr);
		result = -1;
	}
	else
	{
		result = write(fileno(_file), buf, recSize);
	}

	return result;
}

/*============================================================================*/
