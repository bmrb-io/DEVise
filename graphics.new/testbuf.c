/*
  $Id$

  $Log$*/

#include "BufPolicy.h"
#include "BufMgr.h"
#include "UnixDiskFile.h"
#include <stdio.h>
#include <libc.h>

/* name of test file */
char *fname = "testfile";
const int MAXPAGES = 1024;	/* max # of pages */
int BUFPAGE = 256;		/* # of pages to buffer in memory */

DiskFile *pfile;
BufMgr *mgr;

/****************************************************
get pages in memory
*****************************************************/
void GetPagesInMem(){
	int num;
	int *pages;
	mgr->PagesInMem(pfile, num, pages);
	printf("%d pages in mem:\n",num);
	int i;
	for (i=0; i < num; i++){
		printf("%d ", pages[i]);
	}
	printf("\n");
}

/******************************************
get all pages
********************************************/
void GetPages(int trace=false){
	/* get the data back */
	int i;
	BufPage *page;
	printf("Getting all the pages back \n");
	for (i=0; i < MAXPAGES; i++){
		page = mgr->GetAndFixPage(pfile, i);
		printf("page %d, %c\n", page->PageNum(), *((char *)page->PageData()));
		mgr->UnfixPage(page,Future);
		if (trace)
			mgr->Print();
	}
}


main(int argc, char **argv){
	if (argc == 2){
		BUFPAGE = atoi(argv[1]);
	}
	printf("using %d buffer pages\n",BUFPAGE);

	pfile = DiskFile::CreateFile(fname);
	if (pfile == NULL){
		fprintf(stderr,"can't create file %s\n", fname);
		exit(1);
	}

	/* create buffer manager with 10 free pages */
	mgr = new BufMgr(BUFPAGE);

	/* Do LIFO policy */
	mgr->InitPolicy(BufPolicy::LIFO);
	BufPage *page;
	printf("LIFO policy: inserting %d pages into manager\n",MAXPAGES);
	int i;
	for (i=0; i < MAXPAGES; i++){
		page = mgr->CreateAndFixPage(pfile,i);
		*((char *)page->PageData())= i+'A';
		mgr->DirtyPage(page);
		mgr->UnfixPage(page,Future);
	}

	/* print the buffer */
	printf("buf mgr now contains: \n");
	mgr->Print();
	GetPagesInMem();

	/* get pages back */
	GetPages();

	/* print the buffer */
	printf("Now buffer mgr contains\n");
	mgr->Print();
	GetPagesInMem();

	printf("Reinitialize buffer\n");
	mgr->InitPolicy(BufPolicy::LIFO);
	GetPages();
	mgr->Print();
	GetPagesInMem();

	/* Reinit buffer for RND replacement */
	printf("reinitialize to RND policy\n");
	mgr->InitPolicy(BufPolicy::RND);
	GetPages();
	mgr->Print();
	GetPagesInMem();

	/* Reinit buffer for LRU replacement */
	printf("reinitialize to LRU policy\n");
	mgr->InitPolicy(BufPolicy::LRU);
	GetPages();
	mgr->Print();
	GetPagesInMem();

	/* reinit to LIFO policy */
	printf("Reinitialize buffer\n");
	mgr->InitPolicy(BufPolicy::LIFO);
	GetPages();
	mgr->Print();
	GetPagesInMem();
}
