/*
  $Id$

  $Log$*/

/* tests TDataAppend. Remember to remove "tdatafile" before running .*/

#include <stdio.h>
#include <libc.h>
#include "BufMgr.h"
#include "TDataAppend.h"

const int MAXRECS = 5000;
char *fname1 = "tdatafile1";
char *fname2 = "tdatafile2";

struct testrec {
	char padding[120];
	double x;
	double y;
};


/********************************************************
Insert recordsinto file
**********************************************************/
void InsertRecs(char *name, TData *file){
	testrec rec;
	if (file->NumRecords() != 0){
		fprintf(stderr,"file %s not empty, has %d records\n", name,
				file->NumRecords());
		exit(1);
	}

	/* Insert records */
	printf("inserting %d records into %s\n",MAXRECS,name);
	int i;
	for (i=0; i < MAXRECS; i++){
		rec.x = (double)i;
		rec.y = ((double)i)+1.0;
		file->InsertRec(&rec);
	}
}

/* Read records back from file */
void GetFile(TDataAppend *file){
	printf("Reading one record at a time\n");
	int i;
	testrec *rec;
	void *returnRec;
	int pageNum;
	for (i=0; i < MAXRECS; i++){
		file->GetRecPage(i, pageNum, returnRec);
		rec = (testrec *)returnRec;
		if (rec->x != ((double)i ) || rec->y != ((double)i)+1){
			fprintf(stderr,"record %d: x= %f, y = %f\n", i, rec->x, rec->y);
			exit(1);
		}
		file->FreePage(pageNum, Stay);
	}

	printf("Reading page at a time\n");
	int numPages = file->NumPages();
	void **recArray;
	RecId currentId = 0;
	int numRecs;
	RecId startRid;
	for (i=file->FirstPage(); i <= file->LastPage(); i++){
		file->GetPage(i, numRecs, startRid, recArray);
		if (startRid != currentId){
			fprintf(stderr,"currentId %d != startRid %d\n",
					currentId, startRid);
			exit(1);
		}
		int j;
		for (j = 0; j < numRecs; j++){
			testrec *trec = (testrec *)recArray[j];
			if (trec->x != (double)(j+currentId)
				|| trec->y != (double)(j+currentId+1)){
				fprintf(stderr,"record %d: x= %f, y = %f\n", 
					j+currentId, rec->x, rec->y);
				exit(1);
			}
		}
		file->FreePage(i,Stay);
		currentId += numRecs;
	}
}

main(){

	BufMgr *mgr = new BufMgr(100);
	mgr->InitPolicy(BufMgr::RND);
	TDataAppend *file1 = new TDataAppend(fname1, mgr, sizeof(testrec));
	TDataAppend *file2 = new TDataAppend(fname2, mgr, sizeof(testrec));

	printf("inserting files1\n");
	InsertRecs(fname1, file1);

	printf("insertinf file2\n");
	InsertRecs(fname2, file2);

	printf("\ntesting if records are ok\n");
	GetFile(file1);
	GetFile(file2);
	printf("success\n");

	printf("\ndeleting files from memory\n");
	delete file1;
	delete file2;

	printf("\ngetting file back\n");
	file1 = new TDataAppend(fname1, mgr,sizeof(testrec));
	if (file1->Size() != MAXRECS){
		fprintf(stderr,"file %s has %d records != %d\n", fname1, file1->Size(),
				MAXRECS);
		exit(1);
	}
	file2 = new TDataAppend(fname2, mgr, sizeof(testrec));
	if (file2->Size() != MAXRECS){
		fprintf(stderr,"file %s has %d records != %d\n", fname2, file2->Size(),
				MAXRECS);
		exit(1);
	}

	printf("\ntesting if records are Ok\n");
	GetFile(file1);
	GetFile(file2);
	printf("success\n");

	printf("\nwitching to RND policy\n");

	printf("\nmore stringent test by getting from both files\n");
	int i;
	testrec *rec;
	void *returnRec;
	int pNum;
	for (i= MAXRECS-1; i >= 0 ; i--){
		file1->GetRecPage(i,pNum, returnRec);
		rec = (testrec *)returnRec;
		if (rec->x != ((double)i ) || rec->y != ((double)i)+1){
			fprintf(stderr,"record %d: x= %f, y = %f\n",
					i, rec->x, rec->y);
			exit(1);
		}
		file1->FreePage(pNum, Stay);

		file2->GetRecPage(i,pNum, returnRec);
		rec = (testrec *)returnRec;
		if (rec->x != ((double)i ) || rec->y != ((double)i)+1){
			fprintf(stderr,"record %d: x= %f, y = %f\n",
					i, rec->x, rec->y);
			exit(1);
		}
		file2->FreePage(pNum, Stay);
	}
	printf("success\n");

	mgr->PrintStat();
	delete file1;
	delete file2;
	delete mgr;

}
