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
  Header file for DataSourceWeb class.
 */

/*
  $Id$

  $Log$*/

#ifndef _DataSourceWeb_h_
#define _DataSourceWeb_h_

#include "DataSourceFileDesc.h"

class DataSourceWeb : public DataSourceFileStream
{
public:
    DataSourceWeb(char *url, char *label, char *cache);
    virtual ~DataSourceWeb();

    virtual char *objectType() {return "DataSourceWeb";};
    
    virtual DevStatus Open(char *mode);
    virtual Boolean IsOk();
    virtual DevStatus Close();

    virtual int AsyncFd() { return _fd; }
    virtual void AsyncIO();

    virtual size_t Fwrite(const char *buf, size_t size, size_t itemCount);
    virtual size_t Write(const char *buf, size_t byteCount);

    virtual int append(void *buf, int recSize);
    
protected:
    char * _url;              // URL of data source
    char * _cache;            // name of cache file
    int    _fd;               // file descriptor of network connection
    FILE * _cfile;            // for writing to cache file
};


#endif /* _DataSourceWeb_h_ */

/*============================================================================*/
