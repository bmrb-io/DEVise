/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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

  $Log$
  Revision 1.1  1996/07/01 19:21:26  jussi
  Initial revision.
*/

#ifndef _DataSourceFileDesc_h_
#define _DataSourceFileDesc_h_

#include "DataSourceFileStream.h"

class DataSourceFileDesc : public DataSourceFileStream
{
public:
    DataSourceFileDesc(int fd, const char *label);
    virtual ~DataSourceFileDesc();

    virtual char *objectType() {return "DataSourceFileDesc";};

    virtual DevStatus Open(const char *mode);
    virtual Boolean IsOk();
    virtual DevStatus Close();

protected:
    int _fd;
};


#endif /* _DataSourceFileDesc_h_ */

/*============================================================================*/
