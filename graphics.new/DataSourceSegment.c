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


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

template class DataSourceSegment<DataSourceFileStream>;
template class DataSourceSegment<DataSourceTape>;

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::DataSourceSegment
 * DataSourceSegment constructor.
 */
template<class TYPE>
DataSourceSegment<TYPE>::DataSourceSegment(char *filename, char *label,
	long dataOffset, long dataLength) : TYPE(filename, label)
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
	//DO_DEBUG(printf("DataSourceSegment::gotoEnd()\n"));

	int		result = 0;

	if (TYPE::Seek(_dataOffset + _dataLength, SEEK_SET) < 0)
	{
		reportError("Cannot seek to end of file", devNoSyserr);
		result = -1;
		DOASSERT(false, "");
	}
	else
	{
		result = TYPE::Tell() - _dataOffset;
	}

	return result;
}

/*============================================================================*/
