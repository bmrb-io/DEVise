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
  Revision 1.2  1996/07/12 21:53:31  jussi
  Rewrote once more.

  Revision 1.1  1996/07/12 03:52:29  jussi
  Initial revision.
*/

#ifndef DerivedDataSource_h
#define DerivedDataSource_h

#include <string.h>

#include "DeviseTypes.h"

class DerivedDataSource {
  public:
    static Boolean IsDerivedDataType(char *type) {
        if ((!strcmp(type, "BASICSTAT")) || (!strcmp(type, "HISTOGRAM")))
            return true;
        return false;
    }


    static char *GetDerivedData(char *type, char *name);

  protected:
    virtual char *GetViewStatistics() = 0;
    virtual char *GetViewHistogram() = 0;
};

#endif
