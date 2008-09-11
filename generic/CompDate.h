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
  Header for CompDate (compile date) class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1996/07/09 15:59:31  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

 */

#ifndef _CompDate_h_
#define _CompDate_h_


class CompDate
{
public:
    static const char *Get();
};


#endif /* _CompDate_h_ */

/*============================================================================*/
