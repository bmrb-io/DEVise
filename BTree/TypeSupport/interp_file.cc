#include<stdlib.h>
#include<stdio.h>
#include"typed_key.h"
#include"int_key.h"

main()
{
  char line[160];
  int pt_key;
  int dim, data_sz;

  char read_ret=0;
  gets(line);
  sscanf(line, "%d %d %d", &dim, &data_sz, &pt_key);
  for(int i=1; i<10000; i++)
    {
      long long bounds[2*dim];
      if (pt_key)
	fread(bounds, 8, dim, stdin);
      else
	fread(bounds, 8, 2*dim, stdin);
      char data[data_sz];
      fread(data, 1, data_sz, stdin);
      int_key_t my_int_key(bounds, dim, pt_key);
      typed_key_t my_typed_key(my_int_key, data, "ifif");
      my_typed_key.debug(); printf("\n");
    }
