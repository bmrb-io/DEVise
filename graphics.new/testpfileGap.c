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
  Revision 1.2  1995/09/05 22:17:02  jussi
  Added CVS header.
*/

/* test page file with gap */

#include <stdio.h>
#include <stdlib.h>

#include "DeviseTypes.h"
#include "PageFile.h"
#include "BufMgr.h"
#include "PageRange.h"

/* name of test file */
char *fname = "testfile";
char *rangeName = "testfile.Prange";
const int MAXPAGES = 100;   /* max # of pages */
const int BUFPAGE = 10;     /* # of pages to buffer in memory */

/************************************************************************
Get list of page ranges in the current file
************************************************************************/

void PrintRange(PageFile *pfile)
{
  RangeData *head;
  int numRanges;
  pfile->GetRangeList(numRanges, head);
  printf("There are %d ranges\n", numRanges);
  RangeData *current;
  for (current = head->next; current != head; current = current->next){
    printf("(%d,%d)",current->lowPage, current->highPage);
  }
  printf("\n");
}

main()
{
  unlink(fname);
  unlink(rangeName);
  
  int i;
  
  BufMgr *mgr = new BufMgr(10);
  PageFile *pfile = new PageFile(fname,mgr);
  BufPage *page;
  
  printf("creating odd pages\n"); 
  for (i=1; i < MAXPAGES; i += 2){
    page=pfile->CreatePageWithGap(i);
    *((int *)page->PageData())= i;
    pfile->DirtyPage(page);
    pfile->UnfixPage(page, Future);
  }
  
  printf("getting the ranges back\n");
  PrintRange(pfile);
  
  printf("deleting page file \n");
  delete pfile;
  
  printf("getting page file back and print range\n");
  pfile = new PageFile(fname,mgr);
  PrintRange(pfile);
  
  printf("inserting even pages\n");
  for (i=2; i < MAXPAGES; i += 2){
    page = pfile->CreatePageWithGap(i);
    *((int *)page->PageData())= i;
    pfile->DirtyPage(page);
    pfile->UnfixPage(page, Future);
  }
  PrintRange(pfile);
  
  printf("deleting page file \n");
  delete pfile;
  
  printf("getting page file back and print range\n");
  pfile = new PageFile(fname,mgr);
  PrintRange(pfile);
  
  delete pfile;
}

