/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.1  1997/06/25 21:25:21  wenger
  Added writeDesc (write session description) command needed by Hongyu's
  Java client.

 */

#ifndef _SessionDesc_h_
#define _SessionDesc_h_


#include <sys/types.h>

#include "DeviseTypes.h"
#include "DevStatus.h"


class SessionDesc
{
  public:
    static DevStatus Write(char *filename, Boolean physical = true);
};


#endif /* _SessionDesc_h_ */

/*============================================================================*/
