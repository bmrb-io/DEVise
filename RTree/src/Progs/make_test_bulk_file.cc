#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include"../setup.h"

void main()
{
  FILE *outfile=fopen("test_bulk_file","w");
  char line[160];

  int size=sprintf(line, "%d %d 0", 2, 0);
  strncat(line, "          ", (8-((size+1)%8))%8);
  strncat(line, "\n", 1);
  fwrite(line, size+1+(8-((size+1)%8))%8, 1, outfile);
  dbword_t data[4];
  for(int i=0;i<10000; i++)
    {
      data[0]=i;
      data[1]=i;
      data[2]=i;
      data[3]=i;
      fwrite(data, 4*sizeof(dbword_t), 1, outfile);
    }
}
