#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

main(int argc, char** argv)
{
  if(argc < 4)
    {
      printf("Usage: make_data size dims output-file [seed]\n");
      exit(1);
    }
  int size = atoi(argv[1]);
  assert(size > 0);
  int dims = atoi(argv[2]);
  assert(dims > 0);
  FILE *fd = fopen(argv[3],"w");
  assert(fd != NULL);
  
  int T = 1;
  if(argc > 4)
    T = atoi(argv[4]);
  srand48(T);
  for(int j = 0; j < size; j++)
    {
      fprintf(fd,"i\n");
      for(int i = 0; i < dims; i++)
	fprintf(fd,"%d ",(int)(drand48() * 1000));
      fprintf(fd,"\n%g\n",drand48()*100.0);
    }
  fprintf(fd,"o after %d insertions\n", size);
  fclose(fd);
} 


