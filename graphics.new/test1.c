/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <libc.h>

struct rec {
	double a;
	double b;
};

main(int argc, char **argv)
{
  int size;
  if (argc > 1)
    size = atoi(argv[1]);
  else size = 1024;
  int* ar = new (int[size]);
  rec **rr = new (rec *[size]);
  for (;;);
}
