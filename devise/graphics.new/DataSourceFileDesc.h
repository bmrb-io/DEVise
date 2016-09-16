/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.2.14.1  2005/09/06 21:20:16  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.2  2000/01/13 23:07:03  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

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

    virtual const char *objectType() {return "DataSourceFileDesc";};

    virtual DevStatus Open(const char *mode);
    virtual Boolean IsOk();
    virtual DevStatus Close();

protected:
    int _fd;
};


#endif /* _DataSourceFileDesc_h_ */

/*============================================================================*/
