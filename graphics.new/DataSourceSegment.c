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
  Revision 1.9  1996/07/13 02:31:58  jussi
  Moved template instantiations to end of source file. Older
  compilers seem to want it that way.

  Revision 1.8  1996/07/12 18:24:45  wenger
  Fixed bugs with handling file headers in schemas; added DataSourceBuf
  to TDataAscii.

  Revision 1.7  1996/07/11 23:48:43  jussi
  Had to undo previous "bug fix" -- it was not a bug after all,
  I just had messed with the template file so the data segment
  offset was incorrect.

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
#include "DataSourceBuf.h"
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

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::DataSourceSegment
 * DataSourceSegment constructor.
 */
DataSourceSegment::DataSourceSegment(DataSource* dataSource,
				     long dataOffset,
				     long dataLength)
: DataSource("segment"),
  _dataSource(dataSource),
  _dataOffset(dataOffset),
  _dataLength(dataLength)
{
	DO_DEBUG(printf("DataSourceSegment::DataSourceSegment()\n"));
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::~DataSourceSegment
 * DataSourceSegment destructor.
 */
DataSourceSegment::~DataSourceSegment()
{
	DO_DEBUG(printf("DataSourceSegment::~DataSourceSegment()\n"));
	// right now the segment is in control of the origina data source.
	// when the data source is registered in a catalog, it will no
	// longer delete it.
	delete _dataSource;
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::Open
 * Open the file or other data source corresponding to this object.
 */
DevStatus
DataSourceSegment::Open(char *mode)
{
	DO_DEBUG(printf("DataSourceSegment::Open()\n"));

	DevStatus	result = StatusOk;

	if (_dataSource->Open(mode) != StatusOk)
	{
		result = StatusFailed;
	}
	else
	{
		if (_dataSource->Seek(_dataOffset, SEEK_SET) < 0)
		{
			result = StatusFailed;
		}
	}

	return result;
}

//-----------------------------------------------------------------------------

DevStatus
DataSourceSegment::Close()
{
    return _dataSource->Close();
}


//-----------------------------------------------------------------------------

char
DataSourceSegment::IsOk()
{
    return _dataSource && _dataSource->IsOk();
}


/*-----------------------------------------------------------------------------
 * function: DataSourceSegment::Seek
 * Do a seek on the data source corresponding to this object.
 */
int
DataSourceSegment::Seek(long offset, int from)
{
	DO_DEBUG(printf("DataSourceSegment::Seek()\n"));

        /*
         * The return value from Seek() should not be offset by substracting
         * _dataOffset from it because Seek() returns zero (not file offset)
         * on successful return, -1 otherwise.
         */

	return _dataSource->Seek(offset + _dataOffset, from);
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::Tell
 * Do a tell on the data source corresponding to this object.
 */
long
DataSourceSegment::Tell()
{
	DO_DEBUG(printf("DataSourceSegment::Tell()\n"));

	return _dataSource->Tell() - _dataOffset;
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::gotoEnd
 * Go to the end of the data source corresponding to this object.
 */
int
DataSourceSegment::gotoEnd()
{
	DO_DEBUG(printf("DataSourceSegment::gotoEnd()\n"));

	int		result = 0;

	if (_dataLength == DATA_LENGTH_UNDEFINED)
	{
		if (_dataSource->gotoEnd() < 0)
		{
			result = -1;
		}
	}
	else
	{
		if (_dataSource->Seek(_dataOffset + _dataLength, SEEK_SET) < 0)
		{
			reportError("Cannot seek to end of file", devNoSyserr);
			result = -1;
			DOASSERT(false, "");
		}
	}

	if (result != -1)
	{
		result = _dataSource->Tell() - _dataOffset;
	}

	return result;
}

/*============================================================================*/

