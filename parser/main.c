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
  Revision 1.4  1995/09/23  00:03:52  ravim
  Parser changed to ANSI style - g++ now used uniformly
 
  Revision 1.3  1995/09/22 15:56:21  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:35:17  jussi
  Added CVS header.
*/

/*
   main.c
   usage: parser [-o file]
*/

#include "parse.h"

main(int argc, char **argv)
{
  char *fname = "mapfile.h";

  if (argc > 1) {
    if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'o')
      fname = argv[2];
    else {
      fprintf(stderr, "usage: parser [-o file] < infile\n");
      exit(1);
    }
  }

  FILE *mapFile;

  if (!(mapFile = fopen(fname,"w+"))) {
    fprintf(stderr, "can't open mapfile.h\n");
    exit(2);
  }

  Parse(mapFile);

  fclose(mapFile);
}
