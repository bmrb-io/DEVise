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

  $Log$
  Revision 1.2  1995/09/05 22:16:58  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Bitmap.h"

#define NUMBITS 1000

main()
{
  /* Create bitmap */
  Bitmap *bitmap = new Bitmap(NUMBITS);
  
  /* Set even bits */
  int i;
  printf("setting even bits\n");
  for (i=0; i < NUMBITS; i += 2)
    bitmap->SetBit(i);
  
  printf("Make sure even bits are set\n");
  for (i=0; i < NUMBITS; i += 2){
    if (!bitmap->TestBit(i)){
      fprintf(stderr,"error: bit %d not set\n", i);
      exit(1);
    }
  }
  
  printf("Make sure odd bits are not set\n");
  for (i=1; i < NUMBITS; i += 2){
    if (bitmap->TestBit(i)){
      fprintf(stderr,"error: bit %d set\n", i);
      exit(1);
    }
  }
  
  printf("clearing bitmap\n");
  bitmap->ClearBitmap();
  
  /* Set odd bits */
  printf("setting odd bits\n");
  for (i=1; i < NUMBITS; i += 2)
    bitmap->SetBit(i);
  
  printf("Make sure ood bits are set\n");
  for (i=1; i < NUMBITS; i += 2){
    if (!bitmap->TestBit(i)){
      fprintf(stderr,"error: bit %d not set\n", i);
      exit(1);
    }
  }
  
  printf("Make sure even bits are not set\n");
  for (i=0; i < NUMBITS; i += 2){
    if (bitmap->TestBit(i)){
      fprintf(stderr,"error: bit %d set\n", i);
      exit(1);
    }
  }

  printf("success\n");
}

