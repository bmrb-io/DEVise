/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of SessionDesc (session description) class.
 */

/*
  $Id$

  $Log$
 */

#ifndef _SessionDesc_h_
#define _SessionDesc_h_


#include <sys/types.h>

#include "DevStatus.h"


class SessionDesc
{
  public:
    static DevStatus Write(char *filename);
};


#endif /* _SessionDesc_h_ */

/*============================================================================*/
