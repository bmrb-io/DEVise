/*
   $Id$

   Revision 1.1  1995/09/18 18:30:33  jussi
   Initial revision of archive.
*/

#include <stdio.h>

void comp_create(char **syms, int num);

main(int argc, char *argv[])
{
  /* Find out the command line parameters.
     List of ticker symbols.
  */
  if (argc < 2)
  {
    printf("Usage : %s <list of symbol names>\n", argv[0]);
    exit(1);
  }

  /* Call function to create the data file. */
  comp_create(&argv[1], argc - 1);
}
