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
  Header file for DataSourceFileDesc class.
 */

/*
  $Id$

  $Log$*/

#ifndef _DataSourceFileDesc_h_
#define _DataSourceFileDesc_h_

#include "DataSourceFileStream.h"

class DataSourceFileDesc : public DataSourceFileStream
{
public:
    DataSourceFileDesc(int fd, char *label);
    virtual ~DataSourceFileDesc();

    virtual char *objectType() {return "DataSourceFileDesc";};

    virtual DevStatus Open(char *mode);
    virtual Boolean IsOk();
    virtual DevStatus Close();

protected:
    int _fd;
};


#endif /* _DataSourceFileDesc_h_ */

/*============================================================================*/
