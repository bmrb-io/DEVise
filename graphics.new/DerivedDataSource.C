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
  Revision 1.1  1996/07/12 03:52:28  jussi
  Initial revision.
*/

#include <stdio.h>

#include "DerivedDataSource.h"
#include "Control.h"
#include "Exit.h"

char *DerivedDataSource::GetDerivedData(char *type, char *name)
{
    DerivedDataSource *src = (DerivedDataSource *)
                             ControlPanel::FindInstance(name);
    if (!src) {
        fprintf(stderr, "Cannot find source of derived data: %s\n", name);
        DOASSERT(0, "Invalid source of derived data");
    }

    if (!strcmp(type, "VIEWSTAT")) {
        return src->GetViewStatistics();
    }

    fprintf(stderr, "Invalid derived data type: %s\n", type);
    return 0;
}
