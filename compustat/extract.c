/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.4  1995/09/22 15:52:15  jussi
  Added copyright message.

  Revision 1.3  1995/09/19 14:59:43  jussi
  Added log message.

  Revision 1.1  1995/09/18 18:30:33  jussi
  Initial revision of archive.
*/

#include <stdio.h>

void comp_create(char **syms, int num);

main(int argc, char *argv[])
{
  /* Find out the command line parameters.
     List of ticker symbols.
  */
  if (argc < 2)
  {
    printf("Usage : %s <list of symbol names>\n", argv[0]);
    exit(1);
  }

  /* Call function to create the data file. */
  comp_create(&argv[1], argc - 1);
}
