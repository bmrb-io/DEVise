/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  DataReader utilities (for DEVise commands).  This is pulled out of
  the DataReader class itself because DeviseCommand.C cannot include
  DataReader.h without running into problems.
 */

/*
  $Id$

  $Log$
 */

#ifndef _DRUtils_h_
#define _DRUtils_h_


#include <sys/types.h>

   // Convert a Datareader schema to a Tcl-style list.
   char* DRSchema2TclList(const char *schemaFile);

#endif // _DRUtils_h_

/*============================================================================*/
