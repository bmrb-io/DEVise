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

  $Log$*/

#ifndef DerivedDataSource_h
#define DerivedDataSource_h

#include <string.h>

#include "DeviseTypes.h"

class DerivedDataSource {
  public:
    static Boolean IsDerivedDataType(char *type) {
        if (!strcmp(type, "VIEWSTAT"))
            return true;
        return false;
    }

    static char *GetDerivedData(char *type, char *name);

  protected:
    virtual char *GetViewStatistics() = 0;
};

#endif
