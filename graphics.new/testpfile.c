/*
  $Id$

  $Log$*/

/* test page files */

#include <stdio.h>
#include <libc.h>
#include "PageFile.h"
#include "BufMgr.h"

/* name of test file */
char *fname = "testfile";
const int MAXPAGES = 100;	/* max # of pages */
const int BUFPAGE = 10;		/* # of pages to buffer in memory */


/******************************************
get all pages
********************************************/
void GetPages(PageFile *pfile){
	/* get the data back */
	int size = pfile->NumPages();
	int header;
	pfile->ReadHeader(&header, sizeof(int));
	printf("size = %d, header = %d\n", size, header);
	int i;
	BufPage *page;
	printf("Getting all the pages back \n");
	for (i=1; i <= size; i++){
		page = pfile->GetPage(i);
		printf("page %d, %d\n", i, *((int *)page->PageData()));
		pfile->UnfixPage(page, Future);
	}

	int num;
	int *pages;
	pfile->PagesInMem(num, pages);
	printf("%d pages in mem:",num);
	for (i=0; i < num; i++)
		printf("%d ", pages[i]);
	printf("\n");
}

main(){
	/* create buffer manager with 10 pages */
	BufMgr *mgr = new BufMgr(10);

	PageFile *pfile = new PageFile(fname, mgr);

	/* writer header */
	int hdr = -999;
	pfile->WriteHeader(&hdr, sizeof(hdr));

	/* Do LIFO policy */
	mgr->InitPolicy(BufMgr::LIFO);
	BufPage *page;
	printf("LIFO policy: inserting %d pages into manager\n",MAXPAGES);
	int i;
	for (i=1; i <= MAXPAGES; i++){
		int temp;
		page = pfile->CreatePage(temp);
		*((int *)page->PageData())= i;
		pfile->DirtyPage(page);
		pfile->UnfixPage(page, Future);
	}

	/* print the buffer */
	GetPages(pfile);

	printf("Reinitialize buffer\n");
	mgr->InitPolicy(BufMgr::LIFO);
	GetPages(pfile);

	/* Reinit buffer for RND replacement */
	printf("reinitialize to RND policy\n");
	mgr->InitPolicy(BufMgr::RND);
	GetPages(pfile);
	mgr->Print();

	printf("deleting pfile\n");
	delete pfile;
	mgr->Print();

	printf("opening pfile again\n");
	pfile = new PageFile(fname, mgr);

	/* reinit to RND policy */
	printf("Reinitialize buffer\n");
	mgr->InitPolicy(BufMgr::RND);
	GetPages(pfile);
	mgr->Print();
}
