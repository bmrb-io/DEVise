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
   Revision 1.12  1996/12/03 20:36:27  jussi
   Removed unnecessary AsyncFd() ans AsyncIO().

   Revision 1.11  1996/08/06 19:23:02  beyer
   Made functional again after the last changes

   Revision 1.10  1996/08/04 21:23:24  beyer
   DataSource's are now reference counted.
   Added Version() which TData now check to see if the DataSource has changed,
   and if it has, it rebuilds its index and invalidates the cache.
   DataSourceFixedBuf is a DataSourceBuf that allocates and destoyes its
   own buffer.
   DerivedDataSource functionality is now in the TData constructor.
   Added some defaults for virtual methods.

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
    // right now the segment is in control of the original data source.
    // when the data source is registered in a catalog, it will no
    // longer delete it.
    delete _dataSource;
}

//---------------------------------------------------------------------------

char* DataSourceSegment::objectType() 
{
    return "DataSourceSegment";
};


/*------------------------------------------------------------------------------
 * function: DataSourceSegment::Open
 * Open the file or other data source corresponding to this object.
 */
DevStatus DataSourceSegment::Open(char *mode)
{
    DO_DEBUG(printf("DataSourceSegment::Open()\n"));

    DevStatus	result = StatusOk;

    DOASSERT(_dataSource != NULL, "no dataSource");
    if (_dataSource->Open(mode) != StatusOk) {
	result = StatusFailed;
    } else {
	if (_dataSource->Seek(_dataOffset, SEEK_SET) < 0) {
	    result = StatusFailed;
	}
    }

    return result;
}

//-----------------------------------------------------------------------------

char DataSourceSegment::IsOk()
{
    return _dataSource && _dataSource->IsOk();
}

//-----------------------------------------------------------------------------

DevStatus DataSourceSegment::Close()
{
    DOASSERT(_dataSource != NULL, "no dataSource");
    return _dataSource->Close();
}

//---------------------------------------------------------------------------

char* DataSourceSegment::Fgets(char *buffer, int size)
{
#if defined(DEBUG)
    printf("DataSourceSegment::Fgets(%d)\n", size);
#endif

    DOASSERT(_dataSource != NULL, "no dataSource");

    if (_dataLength != DATA_LENGTH_UNDEFINED) {
      long offset = Tell();
      if (offset >= _dataLength) {
        return NULL;
      /* Note: + 1 here for the byte to hold the \0. */
      } else if (offset + size > _dataLength + 1) {
        size = _dataLength - offset + 1;
      }
    }

    return _dataSource->Fgets(buffer, size);
}

//---------------------------------------------------------------------------

size_t DataSourceSegment::Fread(char *buf, size_t size, size_t itemCount)
{
#if defined(DEBUG)
    printf("DataSourceSegment::Fread(%d, %d)\n", size, itemCount);
#endif

    DOASSERT(_dataSource != NULL, "no dataSource");

    if (_dataLength != DATA_LENGTH_UNDEFINED) {
      long offset = Tell();
      if (offset >= _dataLength) {
        return 0;
      } else if (offset + size * itemCount > (unsigned long) _dataLength) {
	itemCount = (_dataLength - offset) / size;
      }
    }

    return _dataSource->Fread(buf, size, itemCount);
}

//---------------------------------------------------------------------------

size_t DataSourceSegment::Read(char *buf, int byteCount)
{
#if defined(DEBUG)
    printf("DataSourceSegment::Read(%d)\n", byteCount);
#endif

    DOASSERT(_dataSource != NULL, "no dataSource");

    if (_dataLength != DATA_LENGTH_UNDEFINED) {
      long offset = Tell();
      if (offset >= _dataLength) {
        return 0;
      } else if (offset + byteCount > _dataLength) {
	byteCount = _dataLength - offset;
      }
    }

    return _dataSource->Read(buf, byteCount);
}

//---------------------------------------------------------------------------

size_t DataSourceSegment::Fwrite(const char *buf, size_t size, size_t itemCount)
{
#if defined(DEBUG)
    printf("DataSourceSegment::Fwrite(%d, %d)\n", size, itemCount);
#endif

    DOASSERT(_dataSource != NULL, "no dataSource");

    if (_dataLength != DATA_LENGTH_UNDEFINED) {
      long offset = Tell();
      if (offset >= _dataLength) {
        return 0;
      } else if (offset + size * itemCount > (unsigned long) _dataLength) {
	itemCount = (_dataLength - offset) / size;
      }
    }

    return _dataSource->Fwrite(buf, size, itemCount);
}

//---------------------------------------------------------------------------

size_t DataSourceSegment::Write(const char *buf, size_t byteCount)
{
#if defined(DEBUG)
    printf("DataSourceSegment::Write(%d)\n", byteCount);
#endif

    DOASSERT(_dataSource != NULL, "no dataSource");

    if (_dataLength != DATA_LENGTH_UNDEFINED) {
      long offset = Tell();
      if (offset >= _dataLength) {
        return 0;
      } else if (offset + byteCount > (unsigned long) _dataLength) {
	byteCount = _dataLength - offset;
      }
    }

    return _dataSource->Write(buf, byteCount);
}

/*-----------------------------------------------------------------------------
 * function: DataSourceSegment::Seek
 * Do a seek on the data source corresponding to this object.
 */
int DataSourceSegment::Seek(long offset, int from)
{
    DO_DEBUG(printf("DataSourceSegment::Seek(%ld)\n", offset));
    DOASSERT(_dataSource != NULL, "no dataSource");

    /*
     * The return value from Seek() should not be offset by substracting
     * _dataOffset from it because Seek() returns zero (not file offset)
     * on successful return, -1 otherwise.
     */
    int result = _dataSource->Seek(offset + _dataOffset, from);

    if (_dataLength != DATA_LENGTH_UNDEFINED) {
      if (Tell() > _dataLength) {
	result = gotoEnd();
      }
    }

    return result;
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::Tell
 * Do a tell on the data source corresponding to this object.
 */
long DataSourceSegment::Tell()
{
    DO_DEBUG(printf("DataSourceSegment::Tell()\n"));
    DOASSERT(_dataSource != NULL, "no dataSource");

    return _dataSource->Tell() - _dataOffset;
}

/*------------------------------------------------------------------------------
 * function: DataSourceSegment::gotoEnd
 * Go to the end of the data source corresponding to this object.
 */
int DataSourceSegment::gotoEnd()
{
    DO_DEBUG(printf("DataSourceSegment::gotoEnd()\n"));
    DOASSERT(_dataSource != NULL, "no dataSource");

    int		result = 0;

    if (_dataLength == DATA_LENGTH_UNDEFINED) {
	if (_dataSource->gotoEnd() < 0) {
	    result = -1;
	}
    } else {
	if (_dataSource->Seek(_dataOffset + _dataLength, SEEK_SET) < 0) {
	    reportError("Cannot seek to end of file", devNoSyserr);
	    result = -1;
	    DOASSERT(false, "");
	}
    }

    if (result != -1) {
	result = _dataSource->Tell() - _dataOffset;
    }

    return result;
}

//---------------------------------------------------------------------------

int DataSourceSegment::append(void *buf, int recSize)
{
    DOASSERT(_dataSource != NULL, "no dataSource");
    if (_dataLength != DATA_LENGTH_UNDEFINED) {
      reportErrNosys("Cannot append to a fixed-length data segment");
      return -1;
    } else {
      return _dataSource->append(buf, recSize);
    }
}

//---------------------------------------------------------------------------

int DataSourceSegment::GetModTime()
{
    DOASSERT(_dataSource != NULL, "no dataSource");
    return _dataSource->GetModTime();
}

//---------------------------------------------------------------------------

Boolean DataSourceSegment::isFile()
{
    DOASSERT(_dataSource != NULL, "no dataSource");
    return _dataSource->isFile();
}

//---------------------------------------------------------------------------

Boolean DataSourceSegment::isBuf()
{
    DOASSERT(_dataSource != NULL, "no dataSource");
    return _dataSource->isBuf();
}

//---------------------------------------------------------------------------

Boolean DataSourceSegment::isTape()
{
    DOASSERT(_dataSource != NULL, "no dataSource");
    return _dataSource->isTape();
}

/*============================================================================*/
