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
  Implementation of DataSourceSegment classes (template).
 */

/*
  $Id$

  $Log$
  Revision 1.6  1996/07/11 23:36:43  jussi
  Fixed off-by-one error in gotoEnd().

  Revision 1.5  1996/07/11 17:25:30  wenger
  Devise now writes headers to some of the files it writes;
  DataSourceSegment class allows non-fixed data length with non-zero
  offset; GUI for editing schema files can deal with comment lines;
  added targets to top-level makefiles to allow more flexibility.

  Revision 1.4  1996/07/01 20:23:13  jussi
  Added #ifdef conditionals to exclude the Web data source from
  being compiled into the Attribute Projection executable.

  Revision 1.3  1996/07/01 19:31:34  jussi
  Added an asynchronous I/O interface to the data source classes.
  Added a third parameter (char *param) to data sources because
  the DataSegment template requires that all data sources have the
  same constructor (DataSourceWeb requires the third parameter).

  Revision 1.2  1996/06/04 19:58:45  wenger
  Added the data segment option to TDataBinary; various minor cleanups.

  Revision 1.1  1996/06/04 14:21:41  wenger
  Ascii data can now be read from session files (or other files
  where the data is only part of the file); added some assertions
  to check for pointer alignment in functions that rely on this;
  Makefile changes to make compiling with debugging easier.

 */

#define _DataSourceSegment_c_

#ifdef __GNUG__
#pragma implementation "DataSourceSegment.h"
#endif

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <unistd.h>

#include "DataSourceFileStream.h"
#include "DataSourceTape.h"
#include "DataSourceSegment.h"
#include "Util.h"
#include "DevError.h"

#ifndef ATTRPROJ
#   include "DataSourceWeb.h"
#endif

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

template class DataSourceSegment<DataSourceFileStream>;
template class DataSourceSegment<DataSourceTape>;
#ifndef ATTRPROJ
template class DataSourceSegment<DataSourceWeb>;
#endif

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::DataSourceSegment
 * DataSourceSegment constructor.
 */
template<class TYPE>
DataSourceSegment<TYPE>::DataSourceSegment(char *name, char *label,
                                           char *param, long dataOffset,
                                           long dataLength) :
     TYPE(name, label, param)
{
	DO_DEBUG(printf("DataSourceSegment::DataSourceSegment()\n"));

	_dataOffset = dataOffset;
	_dataLength = dataLength;
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::~DataSourceSegment
 * DataSourceSegment destructor.
 */
template<class TYPE>
DataSourceSegment<TYPE>::~DataSourceSegment()
{
	DO_DEBUG(printf("DataSourceSegment::~DataSourceSegment()\n"));
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::Open
 * Open the file or other data source corresponding to this object.
 */
template<class TYPE>
DevStatus
DataSourceSegment<TYPE>::Open(char *mode)
{
	DO_DEBUG(printf("DataSourceSegment::Open()\n"));

	DevStatus	result = StatusOk;

	if (TYPE::Open(mode) != StatusOk)
	{
		result = StatusFailed;
	}
	else
	{
		if (TYPE::Seek(_dataOffset, SEEK_SET) < 0)
		{
			result = StatusFailed;
		}
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::Seek
 * Do a seek on the data source corresponding to this object.
 */
template<class TYPE>
int
DataSourceSegment<TYPE>::Seek(long offset, int from)
{
	DO_DEBUG(printf("DataSourceSegment::Seek()\n"));

        /*
         * The return value from Seek() should not be offset by substracting
         * _dataOffset from it because Seek() returns zero (not file offset)
         * on successful return, -1 otherwise.
         */

	return TYPE::Seek(offset + _dataOffset, from);
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::Tell
 * Do a tell on the data source corresponding to this object.
 */
template<class TYPE>
long
DataSourceSegment<TYPE>::Tell()
{
	DO_DEBUG(printf("DataSourceSegment::Tell()\n"));

	return TYPE::Tell() - _dataOffset;
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::gotoEnd
 * Go to the end of the data source corresponding to this object.
 */
template<class TYPE>
int
DataSourceSegment<TYPE>::gotoEnd()
{
	DO_DEBUG(printf("DataSourceSegment::gotoEnd()\n"));

	int		result = 0;

	if (_dataLength == DATA_LENGTH_UNDEFINED)
	{
		if (TYPE::gotoEnd() < 0)
		{
			result = -1;
		}
	}
	else
	{
		if (TYPE::Seek(_dataOffset + _dataLength, SEEK_SET) < 0)
		{
			reportError("Cannot seek to end of file", devNoSyserr);
			result = -1;
			DOASSERT(false, "");
		}
	}

	if (result != -1)
	{
		result = TYPE::Tell() - _dataOffset;
	}

	return result;
}

/*============================================================================*/
