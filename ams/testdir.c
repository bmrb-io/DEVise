/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.3  1995/09/05 20:23:49  jussi
  Updated CVS header.

  Revision 1.2  1995/09/05 20:19:23  jussi
  Added CVS header
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>

main(int argc, char **argv)
{
  DIR *dirp;
  struct dirent *dp;
  char *dir;

  if (argc != 2) {
    printf("usage: testdir dir\n");
    exit(1);
  }
  dir = argv[1];
  printf("directory %s\n",dir);
  dirp = opendir(dir);
  if (dirp != NULL) {
    for(dp = readdir(dirp); dp != (struct dirent *)NULL; dp = readdir(dirp)) {
      printf("file %s\n",dp->d_name);
    }
    closedir(dirp);
  }
}
