/*
  $Id$

  $Log$*/

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
