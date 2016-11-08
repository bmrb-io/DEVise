/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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

  $Log$*/

#include <stdio.h>

int main(int argc, char **argv)
{
  int d,m,y,h,m2,s;
  char stock;
  float price;
  int num;
  char code;

  while(scanf("%d,%d,%d,%d,%d,%d,%c,%f,%d,%c", &d, &m, &y,
	      &h, &m2, &s, &stock, &price, &num, &code) == 10) {
    fwrite(&d, sizeof d, 1, stdout);
    fwrite(&m, sizeof m, 1, stdout);
    fwrite(&y, sizeof y, 1, stdout);
    fwrite(&h, sizeof h, 1, stdout);
    fwrite(&m2, sizeof m2, 1, stdout);
    fwrite(&s, sizeof s, 1, stdout);
    fwrite(&stock, sizeof stock, 1, stdout);
    fwrite(&price, sizeof price, 1, stdout);
    fwrite(&num, sizeof num, 1, stdout);
    fwrite(&code, sizeof code, 1, stdout);
  }

  return 1;
}
