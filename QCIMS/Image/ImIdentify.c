#include <stdio.h>

#include "Image.h"

Image Im;

int
main(int argc, char *argv[])
{
  DataDest ddest;

  ImInitImg(&Im);
  if (argc > 1) {
    if (!ImSetImgFile(&Im, argv[1], 0)) {
      printf("Error opening file\n");
      return(0);
    }
  } else {
    if (!ImSetImgFile(&Im, "-", 0)) {
      printf("Error opening file\n");
      return(0);
    }
  }
  ImSetSilent(&Im);

  if (!ImPeekImg(&Im)) {
    printf("Unknown format\n");
    return(0);
  }


  DD_Init(&ddest); 
  if (!DD_OpenStream(&ddest, stdout) || !ImPrintImgChars(&Im,&ddest)) {
    printf("Unknown format\n");
    return(0);
  }
  
  ImFreeImg(&Im);

  return(0);
}

