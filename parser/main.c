/*
  $Id$

  $Log$*/

/*
   main.c
   usage: parser [-o file]
*/

#include "parse.h"

main(argc,argv)
     int argc;
     char **argv;
{
  char *fname= "mapfile.h";
  FILE *mapFile;
  if (argc > 1){
    if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'o')
      fname = argv[2];
    else {
      fprintf(stderr,"usage: parser [-o file] < infile\n");
      exit(1);
    }
  }
  mapFile =fopen(fname,"w+");
  if (mapFile == NULL){
    fprintf(stderr,"can't open mapfile.h\n");
    exit(2);
  }
  Parse(mapFile);
}
