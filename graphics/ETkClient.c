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
  $Id$

  $Log$
 */

#include "Dali.h"
#include "ETk.h"

int
ETkClient(const char *servName, char *errmsg, int port)
{
    return DaliPatron((char *) servName, errmsg, port);
}

