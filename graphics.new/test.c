/*
  $Id$

  $Log$*/

#include "Parse.h"

main(){
  double val;
  ParseFloatDate("Nov  2 1989 16:00:00", val);
  printf("%f\n",val);
}
