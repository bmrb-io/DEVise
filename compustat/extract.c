/*
   $Id$

   $Log$*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "compustat.h"

main(int argc, char *argv[])
{
  /* Find out the command line parameters.
     1. first = name of the specification field
     2. second = value of the specification field
  */
  if (argc != 3)
  {
    printf("Usage : %s <field name> <field value>\n", argv[0]);
    exit(1);
  }

  /* Call function to create the data file. */
  create_comp_dat(argv[1], argv[2]);
}
