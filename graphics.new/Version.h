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
  Description of module.
 */

/*
  $Id$

  $Log$
 */

#ifndef _Version_h_
#define _Version_h_


class Version
{
public:
    static const char *Get();
    static const char *GetCopyright();
    static void PrintInfo();
};


#endif /* _Version_h_ */

/*============================================================================*/
