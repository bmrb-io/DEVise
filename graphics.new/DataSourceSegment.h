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
  Header file for DataSourceSegment classes (template).
 */

/*
  $Id$

  $Log$
  Revision 1.3  1996/07/01 19:31:34  jussi
  Added an asynchronous I/O interface to the data source classes.
  Added a third parameter (char *param) to data sources because
  the DataSegment template requires that all data sources have the
  same constructor (DataSourceWeb requires the third parameter).

  Revision 1.2  1996/06/27 15:50:59  jussi
  Added IsOk() method which is used by TDataAscii and TDataBinary
  to determine if a file is still accessible. Also moved GetModTime()
  functionality from TDataAscii/TDataBinary to the DataSource
  classes.

  Revision 1.1  1996/06/04 14:21:41  wenger
  Ascii data can now be read from session files (or other files
  where the data is only part of the file); added some assertions
  to check for pointer alignment in functions that rely on this;
  Makefile changes to make compiling with debugging easier.

 */

#ifndef _DataSourceSegment_
#define _DataSourceSegment_


#ifdef __GNUG__
#pragma interface
#endif


#include "DeviseTypes.h"

#define DATA_LENGTH_UNDEFINED (-1)


template<class TYPE>
class DataSourceSegment : public TYPE
{
public:
    DataSourceSegment(char *name, char *label, char *param,
                      long dataOffset, long dataLength);
    ~DataSourceSegment();

    virtual char *objectType() {return "DataSourceSegment";};

    virtual DevStatus Open(char *mode);

    virtual int Seek(long offset, int from);
    virtual long Tell();

    virtual int gotoEnd();

private:
    long _dataOffset;
    long _dataLength;
};


#endif /* _DataSourceSegment_ */

/*============================================================================*/
