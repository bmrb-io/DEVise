/* testBufMgrNull.c. Run "testRecFile" to generate records
for into file "testFile" */
#include "TDataRec.h"
#include "BufMgrNull.h"
#include <stdio.h>
#include "TestData.h"

const int NUM_RECS = 100;

main(){
	int i;

	struct Data *buf;
	/* Create a new rec file */
	TDataRec *tdata= new TDataRec("testFile", sizeof(Data));
	if (tdata == NULL){
		printf("can't create existing file\n");
		exit(1);
	}

	BufMgr *mgr = new BufMgrNull();


	int numRecs;
	void **recs;
	void *dataBuf;
	RecId startRid;

	/* Get records in even order */
	printf("geting records in even order\n");
	for (i=0; i < NUM_RECS; i+= 2){
		mgr->GetRecs(tdata,i,i,AnyOrder, startRid, numRecs, dataBuf, recs);
		buf = (struct Data *)dataBuf;
		printf("(%d,%d)\n", buf->x,buf->y);
		mgr->FreeRecs(buf, NoChange);
	}

	/* Get records in odd order */
	printf("getting records in odd order\n");
	for (i=1; i < NUM_RECS; i+= 2){
		mgr->GetRecs(tdata,i,i,AnyOrder, startRid, numRecs, dataBuf, recs);
		buf = (struct Data *)dataBuf;
		printf("(%d,%d)\n", buf->x,buf->y);
		mgr->FreeRecs(buf, NoChange);
	}

	/* Get records */
	printf("getting all records\n");
	RecId headId;
	RecId lastId;
	if (!tdata->HeadID(headId)){
		fprintf(stderr,"empty file\n");
		exit(1);
	}
	if (!tdata->LastID(lastId)){
		fprintf(stderr,"empty file\n");
		exit(1);
	}
	Boolean stat = mgr->GetRecs(tdata,headId, lastId,
		AnyOrder, startRid, numRecs, dataBuf, recs);
	int recSize = tdata->RecSize();
	Boolean done = false;
	do {
		if (recSize > 0){
			void *ptr = dataBuf;
			for (i=0; i < numRecs; i++){
				buf = (struct Data *)ptr;
				ptr += recSize;
				printf("(%d,%d)\n", buf->x,buf->y);
			}
		}
		else {
			for (i=0; i < numRecs; i++){
				buf = (struct Data *)recs[i];
				printf("(%d,%d)\n", buf->x,buf->y);
			}
		}
		mgr->FreeRecs(buf, NoChange);
		if (stat){
			/* get more */
			stat = mgr->GetRecs(startRid, numRecs, dataBuf, recs);
		}
		else
			done = true;
	} while (!done);

}
