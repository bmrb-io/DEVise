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
  Implementation of CompDate (compile date) class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1997/05/21 20:27:07  andyt
  ETk server now prints compile date when it starts. SampleClient can now send
  "eval" commands to the ETk server.

  Revision 1.1  1996/07/09 15:59:29  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

 */

#include "ETkCompDate.h"

const char *
ETkCompDate::Get()
{
    return __DATE__;
}

