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
