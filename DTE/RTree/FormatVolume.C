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

#include<stdio.h>
#include "RTreeCommon.h"

void Format(const char *FileName,
         int  VolumeSize)
{
  FILE *in_file = fopen(FileName, "w");
  fprintf(in_file, "%d   ", ((int) 1));
  for (int i=1; i<=((VolumeSize-4)/4); i++)
    fprintf(in_file, "    ");
  fclose(in_file);
}

main(int argc, char *argv[]){
	Format(VolumeName, VolumeSize); //erases everything on the volume
}
