/* testPageRange.c */
#include <stdio.h>
#include <libc.h>
#include "PageRange.h"
/* Note: MAXPAGES should be even */
const int MAXPAGES = 1000;

/* inserted[i] is TRUE if page i has been inserted */
int inserted[MAXPAGES];

class MyCb : public PageRangeCallback{
public:
	virtual void PageMerged(int p1, int p2, void *lastInP1, void *firstInP2){
		if (p2 != p1+1){
			printf("merged error 1: %d,%d, %d,%d\n",
				p1,p2,*((int *)lastInP1), *((int *)firstInP2));
			/*
			exit(1);
			*/
			abort();
		}
		int num1 = *((int *)lastInP1);
		int num2 = *((int *)firstInP2);
		if (num1 != p1*2 || num2 != p2*2-1){
			printf("merged error 2: %d,%d, %d,%d\n",
				p1,p2,*((int *)lastInP1), *((int *)firstInP2));
			exit(1);
		}
	}
};

class MyCb myCallBack;

/*******************************************************
Insert and test the values returned by InsertPage. The values of the records
for each page are: pageNum*2-1 and pageNum*2. For example:
page 1: 1,2
	2: 3,4
	3: 5,6
	...
********************************************************/
void InsertAndTest(PageRange *prange, int pageNum, PageRange::PageRangeMergeInfo info=PageRange::MergeBoth){
	int *num1 = new int(pageNum*2-1);
	int *num2 = new int (pageNum*2);
	void *left, *right;
	prange->InsertPage(pageNum, num1, num2, info, &myCallBack);
	int leftNum;
	int rightNum;
}

main(){
	/* simple test:consecutive insertion */
	PageRange *prange = new PageRange(sizeof(int));
	int i;
	int next;
	for (i=1; i <= MAXPAGES; i++){
		if ((next=prange->NextUnprocessed(i))!= i){
			fprintf(stderr,"next unprocessed returns %d != %d\n", next, i);
			exit(1);
		}
		InsertAndTest(prange,i);
	}
	printf("print single range 1,200\n");
	prange->Print();
	printf("printing cleared range\n");
	prange->Print();

	/* More complex test: issert odd numbers first */
	printf("Test forward odd insertion\n");
	prange->Clear();
	for (i=1; i <= MAXPAGES; i += 2){
		InsertAndTest(prange,i);
	}
	printf("print odd numbers\n");
	prange->Print();

	/* get back even numbers */
	for (i=2; i <= MAXPAGES; i += 2){
		if ((next=prange->NextUnprocessed(i-1))!= i){
			fprintf(stderr,"next unprocessed returns %d != %d\n", next, i);
			exit(1);
		}
		InsertAndTest(prange,i);
	}
	printf("print single range 1,200\n");
	prange->Print();

	/* More complex test: issert odd numbers backwards */
	printf("\nTest backward ood insertion\n");
	prange->Clear();
	for (i=MAXPAGES-1; i >= 1; i -= 2){
		InsertAndTest(prange,i);
	}
	printf("print odd numbers\n");
	prange->Print();

	/* get back even numbers */
	for (i=2; i <= MAXPAGES; i += 2){
		if ((next=prange->NextUnprocessed(i-1))!= i){
			fprintf(stderr,"next unprocessed returns %d != %d\n", next, i);
			exit(1);
		}
		InsertAndTest(prange,i);
	}
	printf("print single range 1,200\n");
	prange->Print();

	printf("test random insertion\n");
	prange->Clear();
	for (i=0; i < MAXPAGES; i++)
		inserted[i] = 0;
	
	/* randomly insert 1/2 of pages */
	for (i=0; i < MAXPAGES/2; i++){
		int rnd = (int)(random() % MAXPAGES);
		if (!inserted[rnd]){
			/* insert this page */
			inserted[rnd] = 1;
			InsertAndTest(prange,rnd+1);
		}
	}
	printf("randomly inserted pages\n");
	prange->Print();
	next = 1;
	next=prange->NextUnprocessed(next);
	while (next <= MAXPAGES){
		if (inserted[next-1]){
			/* next unprocessed is actually inserted already */
			fprintf(stderr,"page %d already processed\n", next);
			exit(1);
		}
		InsertAndTest(prange,next);

		next=prange->NextUnprocessed(next);
	}
	printf("pring single range 1,200\n");
	prange->Print();

	/* Test MergeLeft */
	/* More complex test: insert odd numbers first */
	printf("Test MergeLeft: forward odd insertion\n");
	prange->Clear();
	for (i=1; i <= MAXPAGES; i += 2){
		InsertAndTest(prange,i, PageRange::MergeRight);
	}

	for (i=2; i <= MAXPAGES; i += 4){
		if ((next=prange->NextUnprocessed(i+2))!= i+2){
			fprintf(stderr,"next unprocessed returns %d != %d\n", next, i+2);
			exit(1);
		}
		InsertAndTest(prange,i+2, PageRange::MergeLeft);
		if ((next=prange->NextUnprocessed(i-1))!= i){
			fprintf(stderr,"next unprocessed returns %d != %d\n", next, i);
			exit(1);
		}
		InsertAndTest(prange,i, PageRange::MergeLeft);
	}
	printf("print MergeLeft ranges:\n");
	prange->Print();

	/* Test MergeRight */
	/* More complex test: insert odd numbers first */
	printf("Test MergeRight: forward odd insertion\n");
	prange->Clear();
	for (i=1; i <= MAXPAGES; i += 2){
		InsertAndTest(prange,i,PageRange::MergeRight);
	}

	/* get back even numbers */
	for (i=2; i <= MAXPAGES; i += 4){
		if ((next=prange->NextUnprocessed(i+2))!= i+2){
			fprintf(stderr,"next unprocessed returns %d != %d\n", next, i+2);
			exit(1);
		}
		InsertAndTest(prange,i+2, PageRange::MergeRight);

		if ((next=prange->NextUnprocessed(i-1))!= i){
			fprintf(stderr,"next unprocessed returns %d != %d\n", next, i);
			exit(1);
		}
		InsertAndTest(prange,i, PageRange::MergeRight);
	}
	printf("print MergeRight ranges:\n");
	prange->Print();
}
