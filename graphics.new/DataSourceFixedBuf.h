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
  $Id$

  $Log$
*/

#ifndef _DATASOURCEFIXEDBUF_H_
#define _DATASOURCEFIXEDBUF_H_

#include "DataSourceBuf.h"


class DataSourceFixedBuf
: public DataSourceBuf
{
  public:

    DataSourceFixedBuf(int buffer_size, char* label);

    ~DataSourceFixedBuf();

  protected:

  private:

    // N/A
    DataSourceFixedBuf(const DataSourceFixedBuf& other);
    DataSourceFixedBuf& operator=(const DataSourceFixedBuf& other);
};



inline
DataSourceFixedBuf::DataSourceFixedBuf(int buffer_size, char* label)
: DataSourceBuf(new char[buffer_size], buffer_size, 0, label)
{
}


inline
DataSourceFixedBuf::~DataSourceFixedBuf()
{
    delete [] _sourceBuf;
}


#endif // _DATASOURCEFIXEDBUF_H_
