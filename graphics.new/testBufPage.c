/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.2  1995/09/05 22:16:38  jussi
  Added CVS header.
*/

#include "BufPage.h"

const int NUM_PAGES = 100;
main(){
  int i;

 BufPage::SetMaxPages(NUM_PAGES);
  for (i=0;i < NUM_PAGES; i++){
    BufPage *bp = new BufPage();
    printf("buffer addr = %d, data addr = %d\n",
	   bp, bp->PageData());
  }
}
