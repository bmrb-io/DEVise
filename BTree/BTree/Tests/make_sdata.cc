#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

main(int argc, char** argv)
{
  if(argc < 3)
    {
      printf("Usage: make_data size output-file [seed]\n");
      exit(1);
    }
  int size = atoi(argv[1]);
  assert(size > 0);
  FILE *fd = fopen(argv[2],"w");
  assert(fd != NULL);
  
  int T = 1;
  if(argc > 3)
    T = atoi(argv[3]);
  srand48(T);
  for(int j = 0; j < size; j++)
    fprintf(fd,"i%g\n",drand48() * 100.0);
  fprintf(fd,"o after %d insertions\n", size);
  fprintf(fd,"s%g\n",drand48() * 100.0);
  fclose(fd);
} 


