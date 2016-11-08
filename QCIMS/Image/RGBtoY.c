#include <stdio.h>

#include "Image.h"


int
main(int argc, char *argv[])
{
  char *outfname = 0; 
  char *dataptr; 
  long nbytes; 
  DataDest ddest; 
  Image Im, * ImPtr = &Im;

  ImInitImg(ImPtr);
  if (argc > 1) {
    if (!ImSetImgFile(ImPtr, argv[1], 0)) {
      fprintf(stderr, "Error opening file\n");
      return(0);
    }
  } else {
    if (!ImSetImgFile(ImPtr, "-", 0)) {
      fprintf(stderr, "Error opening file\n");
      return(0);
    }
  }
  if (argc > 2) outfname = argv[2]; 
  ImSetSilent(ImPtr);

  ImDiscardOriginal(ImPtr); 

  if (!ImPeekImg(ImPtr)) {
    fprintf(stderr, "Unknown format\n");
    return(0);
  }

  if ((ImPlanesOfImg(ImPtr) != 3) || (ImAllSampleBytesEqual(ImPtr) != 1)) { 
    fprintf(stderr,"Not an 8-bit RGB image\n");
    return 0;
  }

  if (!ImReadImg(ImPtr)) { 
    fprintf(stderr,"Error reading image\n"); 
    return 0;
  } 

  DD_Init(&ddest); 
  if (!DD_OpenFile(&ddest, outfname)) { 
    fprintf(stderr, "Error opening destination\n"); 
    return(0);
  }

  if (!ImChangeCspace(ImPtr, ImC_YCbCr)) { 
    fprintf(stderr, "Error converting to YCC\n"); 
    return(0);
  }
  
  DD_printf(&ddest,"P5\n%d %d\n255\n", ImOrigCols(ImPtr),
				       ImOrigRows(ImPtr)); 
  nbytes = ((long) ImOrigCols(ImPtr))*ImOrigRows(ImPtr);
  dataptr = (char *) ImPlane(ImPtr,0); 
  DD_Write(&ddest, dataptr, nbytes);
  DD_Close(&ddest);

  ImFreeImg(ImPtr);

  return(0);
}

