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
  Implementation of DataSource, DataSourceFile, and DataSourceBuf classes.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1996/05/07 16:04:17  wenger
  Added final version of code for reading schemas from session files;
  added an error-reporting class to improve error info.
 */

#define _DataSource_c_

//#define DEBUG

#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/param.h"

#include "DataSource.h"
#include "Util.h"
#include "DevError.h"


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DataSource::DataSource
 * DataSource constructor.
 */
DataSource::DataSource(char *name)
{
	DO_DEBUG(printf("DataSource::DataSource(%s)\n", name));

	_name = strdup(name);
}

/*------------------------------------------------------------------------------
 * function: DataSource::~DataSource
 * DataSource destructor.
 */
DataSource::~DataSource()
{
	DO_DEBUG(printf("DataSource::~DataSource()\n"));

	delete [] _name;
}

/*------------------------------------------------------------------------------
 * function: DataSource::getName
 * Get the name of a DataSource object.
 */
char *
DataSource::getName()
{
	DO_DEBUG(printf("DataSource::getName()\n"));

	return _name;
}

/*------------------------------------------------------------------------------
 * function: DataSourceFile::DataSourceFile
 * DataSourceFile constructor.
 */
DataSourceFile::DataSourceFile(char *filename, char *name) : DataSource(name)
{
	DO_DEBUG(printf("DataSourceFile::DataSourceFile(%s, %s)\n", filename,
		name));

	_filename = strdup(filename);
	_file = NULL;
}

/*------------------------------------------------------------------------------
 * function: DataSourceFile::~DataSourceFile
 * DataSourceFile destructor.
 */
DataSourceFile::~DataSourceFile()
{
	DO_DEBUG(printf("DataSourceFile::~DataSourceFile()\n"));

	delete [] _filename;
	if (_file != NULL) ::fclose(_file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFile::fopen
 * Do an fopen() on the file corresponding to the DataSourceFile object.
 */
DevStatus
DataSourceFile::fopen(char *type)
{
	DO_DEBUG(printf("DataSourceFile::fopen()\n"));

	DevStatus	result = StatusOk;

	_file = ::fopen(_filename, type);
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
 * function: DataSourceFile::fclose
 * Do an fclose() on the file corresponding to the DataSourceFile object.
 */
DevStatus
DataSourceFile::fclose()
{
	DO_DEBUG(printf("DataSourceFile::fclose()\n"));

	DevStatus	result = StatusOk;

	if (::fclose(_file) != 0)
	{
		reportError("error closing file", errno);
		result = StatusFailed;
	}
	_file = NULL;	// So destructor doesn't try to close it again.

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceFile::fgets
 * Do an fgets() on the file corresponding to the DataSourceFile object.
 */
char *
DataSourceFile::fgets(char *buffer, int bufSize)
{
	DO_DEBUG(printf("DataSourceFile::fgets()\n"));

	return ::fgets(buffer, bufSize, _file);
}

/*------------------------------------------------------------------------------
 * function: DataSourceFile::getName
 * Get the name of a DataSourceFile object.
 */
char *
DataSourceFile::getName()
{
	DO_DEBUG(printf("DataSourceFile::getName()\n"));

	return _filename;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::DataSourceBuf
 * DataSourceBuf constructor.
 */
DataSourceBuf::DataSourceBuf(char *buffer, char *name) : DataSource(name)
{
	DO_DEBUG(printf("DataSourceBuf::DataSourceBuf(%s)\n", name));

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
 * function: DataSourceBuf::fopen
 * Simulate an fopen() on the DataSourceBuf object.
 */
DevStatus
DataSourceBuf::fopen(char *)
{
	DO_DEBUG(printf("DataSourceBuf::fopen()\n"));

	DevStatus	result = StatusOk;

	_currentLoc = _sourceBuf;

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::fclose
 * Simulate an fclose() on the DataSourceBuf object.
 */
DevStatus
DataSourceBuf::fclose()
{
	DO_DEBUG(printf("DataSourceBuf::fclose()\n"));

	DevStatus	result = StatusOk;

	_currentLoc = NULL;

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceBuf::fgets
 * Simulate an fgets() on the DataSourceBuf object.
 */
char *
DataSourceBuf::fgets(char *buffer, int bufSize)
{
	DO_DEBUG(printf("DataSourceBuf::fgets()\n"));

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

/*============================================================================*/
