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

#include <stdio.h>

#include "DerivedDataSource.h"
#include "ClassDir.h"
#include "ViewGraph.h"
#include "Exit.h"

char *DerivedDataSource::GetDerivedData(char *type, char *name)
{
    char *objectName = strchr(name, ':');
    if (!objectName || !strlen(objectName + 1)) {
        fprintf(stderr, "Invalid derived data name: %s\n", name);
        DOASSERT(0, "Invalid derived data name");
    }

    /* replace colon with NULL */
    *objectName = 0;
    objectName++;

    char *dataBuf = 0;

    if (!strcmp(name, "View")) {
        ViewGraph *v = (ViewGraph *)ControlPanel::FindInstance(objectName);
        if (!v) {
            fprintf(stderr, "Cannot find view %s\n", objectName);
            DOASSERT(0, "Invalid view name");
        }
        dataBuf = v->GetViewStatistics();
    } else {
        fprintf(stderr, "Invalid derived data object: %s\n", name);
        DOASSERT(0, "Invalid derived data object");
    }

    /* restore colon */
    objectName--;
    *objectName = ':';

    return dataBuf;
}
