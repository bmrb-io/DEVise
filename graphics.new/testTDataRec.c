/*
  $Id$

  $Log$*/

/* Run "testRecFile" to generate records for into file "testFile" */

#include "TDataRec.h"
#include <stdio.h>
#include "TestData.h"

const int NUM_RECS = 100;

main(){
	int i;

	struct Data buf;
	/* Create a new rec file */
	TDataRec *tdata= new TDataRec("testFile", sizeof(Data));
	if (tdata == NULL){
		printf("can't create existing file\n");
		exit(1);
	}

	/* Get records */
	printf("getting records\n");
	int numRecs;
	int dataSize;
	RecId startRid;
	tdata->InitGetRecs(0,NUM_RECS-1,AnyOrder);
	while(tdata->GetRecs(&buf, sizeof(buf), startRid,
			numRecs, dataSize,NULL)){
		printf("(%d,%d)\n", buf.x,buf.y);
	}
	tdata->DoneGetRecs();

	/* Get records in even order */
	printf("geting records in even order\n");
	for (i=0; i < NUM_RECS; i+= 2){
		tdata->InitGetRecs(i,i,AnyOrder);
		tdata->GetRecs(&buf, sizeof(buf), startRid, numRecs, dataSize,NULL);
		tdata->DoneGetRecs();
		printf("(%d,%d)\n", buf.x,buf.y);
	}

	/* Get records in odd order */
	printf("getting records in odd order\n");
	for (i=1; i < NUM_RECS; i+= 2){
		tdata->InitGetRecs(i,i,AnyOrder);
		tdata->GetRecs(&buf, sizeof(buf), startRid, numRecs, dataSize,NULL);
		tdata->DoneGetRecs();
		printf("(%d,%d)\n", buf.x,buf.y);
	}
}
