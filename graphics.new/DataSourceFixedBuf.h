/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2008
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1997/03/20 22:05:35  guangshu
  Added function Resize.

  Revision 1.1  1996/08/04 21:23:24  beyer
  DataSource's are now reference counted.
  Added Version() which TData now check to see if the DataSource has changed,
    and if it has, it rebuilds its index and invalidates the cache.
  DataSourceFixedBuf is a DataSourceBuf that allocates and destoyes its
    own buffer.
  DerivedDataSource functionality is now in the TData constructor.
  Added some defaults for virtual methods.

*/

#ifndef _DATASOURCEFIXEDBUF_H_
#define _DATASOURCEFIXEDBUF_H_

#include "DataSourceBuf.h"


class DataSourceFixedBuf
: public DataSourceBuf
{
  public:

    DataSourceFixedBuf(int buffer_size, const char* label);

    ~DataSourceFixedBuf();

    void Resize(int buffer_size); // will clear data!!

  protected:

  private:

    // N/A
    DataSourceFixedBuf(const DataSourceFixedBuf& other);
    DataSourceFixedBuf& operator=(const DataSourceFixedBuf& other);
};



inline
DataSourceFixedBuf::DataSourceFixedBuf(int buffer_size, const char* label)
: DataSourceBuf(new char[buffer_size], buffer_size, 0, label)
{
}


inline
DataSourceFixedBuf::~DataSourceFixedBuf()
{
    delete [] _sourceBuf;
}


inline
void DataSourceFixedBuf::Resize(int buffer_size)
{
    char* newbuf = new char[buffer_size];
    delete [] _sourceBuf;
    _sourceBuf = newbuf;
    Clear();
    _end_buffer = newbuf + buffer_size -1;
}


#endif // _DATASOURCEFIXEDBUF_H_
