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
  Revision 1.4  1996/07/19 18:00:28  guangshu
  Added support for histograms.

  Revision 1.3  1996/07/12 21:53:30  jussi
  Rewrote once more.

  Revision 1.2  1996/07/12 19:14:36  jussi
  Rewrote GetDerivedData() method.

  Revision 1.1  1996/07/12 03:52:28  jussi
  Initial revision.
*/

#include <stdio.h>

#include "DerivedDataSource.h"
#include "ViewGraph.h"
#include "Control.h"
#include "Exit.h"

char *DerivedDataSource::GetDerivedData(char *type, char *name)
{
    char *objectName = strchr(name, ':');
    if (!objectName || !strlen(objectName + 1)) {
        fprintf(stderr, "Invalid derived data name: %s\n", name);
        return 0;
    }

    /* replace colon with NULL */
    *objectName = 0;
    objectName++;

    DerivedDataSource *src = 0;

    if (!strcmp(name, "View") || !strcmp(name, "Hist") || !strcmp(name, "Gstat")) {
        ViewGraph *v = (ViewGraph *)ControlPanel::FindInstance(objectName);
        if (!v) {
            fprintf(stderr, "Cannot find view %s\n", objectName);
            return 0;
        }

        src = v;
    } else {
        fprintf(stderr, "Invalid derived data object: %s\n", name);
        return 0;
    }

    /* restore colon */
    objectName--;
    *objectName = ':';

    if (!src) {
        fprintf(stderr, "Cannot find source of derived data: %s\n", name);
        return 0;
    }

    char *dataBuf = 0;

    if (!strcmp(type, "BASICSTAT")) {
        dataBuf = src->GetViewStatistics();
    }
    if (!strcmp(type, "HISTOGRAM")) {
	dataBuf = src->GetViewHistogram();
    }
    if(!strcmp(type, "GDATASTAT")) {
	dataBuf = src->GetGdataStatistics();
    }

    return dataBuf;
}
