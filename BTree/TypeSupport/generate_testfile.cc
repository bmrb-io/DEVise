#include<stdlib.h>
#include<stdio.h>

main()
{
  printf("2 0 0 if\n");
  int a;
  float b;
  for(int i=0; i<10000; i++)
    {
      a=i;
      b=0.1*i;
      fwrite((void *)&a, 4, 1, stdout);
      fwrite((void *)&b, 4, 1, stdout);
      fwrite((void *)&a, 4, 1, stdout);
      fwrite((void *)&b, 4, 1, stdout);
    }
