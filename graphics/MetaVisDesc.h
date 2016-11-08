/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of MetaVisDesc class (session description for
  metavisualization).
 */

/*
  $Id$

  $Log$
 */

#ifndef _MetaVisDesc_h_
#define _MetaVisDesc_h_

#include <sys/types.h>

#include "DevStatus.h"

class View;
class DeviseLink;
class DeviseCursor;

class MetaVisDesc
{
public:
    static DevStatus Write(const char *filename, Boolean writeHeader = true,
      Boolean writeComments = true);

private:
    static DevStatus WriteDesc(FILE *file, Boolean writeHeader,
      Boolean writeComments);

    static DevStatus WriteHeader(FILE *file);

    static DevStatus WriteAllViews(FILE *file, Boolean writeComments);
    static DevStatus WriteView(FILE *file, View *view, Boolean writeComments);

    static DevStatus WriteAllLinks(FILE *file, Boolean writeComments);
    static DevStatus WriteLink(FILE *file, DeviseLink *link,
      Boolean writeComments);

    static DevStatus WriteAllCursors(FILE *file, Boolean writeComments);
    static DevStatus WriteCursor(FILE *file, DeviseCursor *cursor,
      Boolean writeComments);
};

#endif // _MetaVisDesc_h_

/*============================================================================*/
