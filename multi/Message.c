/* Message.c */
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "MultiRec.h"
#include "TData.h"
#include "Exit.h"

int binarySearch = false;

/* Get a record from TData */
void GetRec(TData *tdata, RecId id, MultiRec &rec){
/*
printf("GetRec %d\n", id);
*/
	tdata->InitGetRecs(id,id,AnyOrder);
	RecId startRid; int numRecs, dataSize;
	if(!tdata->GetRecs(&rec,sizeof(MultiRec),startRid,numRecs,dataSize, NULL)){
		fprintf(stderr,"GetRec: no record\n");
		Exit::DoExit(1);
	}
	tdata->DoneGetRecs();
}

/* Find the record whose id overlaps with the time. Return the
last id scanned. Here we are using binary search but we
are not guaranteed to return anything in particular. */
void BinSearch(TData *tdata, int time, int node,RecId &id){
/*
printf("BinSearch\n");
*/
	RecId first, last, mid, end;
	if (!tdata->HeadID(first)){
		fprintf(stderr,"binSearch: can't find first\n");
		Exit::DoExit(1);
	}
	if (!tdata->LastID(last)){
		fprintf(stderr,"binSearch: can't find last\n");
		Exit::DoExit(1);
	}
	end = last;
	mid = (first+last)/2;
	while (mid > first){
		MultiRec rec;
		GetRec(tdata,mid,rec);
		if (rec.end > time)
			last = mid;
		else first = mid;

		mid = (first+last)/2;
	}
	id = mid;
}


const int MAX_MSGS = 100;
const int MSG_BUF_SIZE = 4096;
char msgBuf[MSG_BUF_SIZE];
int msgIndex;
char *msgPtr[MAX_MSGS];
int numMsgs;

void CheckParams(){
	if (numMsgs >= MAX_MSGS){
		fprintf(stderr,"MAX_MSGS exceeded\n");
		Exit::DoExit(1);
	}
	if (msgIndex >= MSG_BUF_SIZE){
		fprintf(stderr,"MSG_BUF_SIZE exceeded\n");
		Exit::DoExit(1);
	}
}
void InitPutMessage(int time, int node){
	msgIndex = 0;
	numMsgs = 0;

	/* just print x and y */
	sprintf(&msgBuf[msgIndex],"x    : %d",time);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += (strlen(&msgBuf[msgIndex])+1);
	CheckParams();

	sprintf(&msgBuf[msgIndex],"y    : %d",node);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += (strlen(&msgBuf[msgIndex])+1);
	CheckParams();
}

void PutMessage(MultiRec *rec){
	msgPtr[numMsgs++] = "";

	sprintf(&msgBuf[msgIndex],"begin: %d",rec->begin);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += (strlen(&msgBuf[msgIndex])+1);
	CheckParams();

	sprintf(&msgBuf[msgIndex],"end  : %d",rec->end);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += (strlen(&msgBuf[msgIndex])+1);
	CheckParams();

	sprintf(&msgBuf[msgIndex],"node : %d",rec->node);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += (strlen(&msgBuf[msgIndex])+1);
	CheckParams();

	sprintf(&msgBuf[msgIndex],"job  : %d",rec->job);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += (strlen(&msgBuf[msgIndex])+1);
	CheckParams();

	sprintf(&msgBuf[msgIndex],"cycle: %d", rec->cycles);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += (strlen(&msgBuf[msgIndex])+1);
	CheckParams();

}

void EndPutMessage(int &numMessages, char **&msgs){
	numMessages = numMsgs;
	msgs = msgPtr;
}

void GenMessage(TData *tdata, int time, int node, int &numMsgs, char **&msgs){
/*
printf("GenMessage time=%d, node = %d\n",time, node);
*/


/*
printf("done binary search id = %d\n",id);
*/
	RecId first, last;
	if (!tdata->HeadID(first)){
		fprintf(stderr,"binSearch: can't find first\n");
		Exit::DoExit(1);
	}
	if (!tdata->LastID(last)){
		fprintf(stderr,"binSearch: can't find last\n");
		Exit::DoExit(1);
	}

	InitPutMessage(time, node);

	MultiRec rec;
	if (binarySearch){
		RecId id; 
		BinSearch(tdata, time, node,id);

		/* search backwards */
		RecId cur= id;
		/*
		printf("first= %d,last= %d\n", first,last);
		*/
		while (cur >= first){
			/*
			printf("scan backward %d\n",cur);
			*/
			GetRec(tdata,cur, rec);
			if (rec.begin <= time && rec.end >= time && 
				rec.node == node){
				/* found it */
				PutMessage(&rec);
			}
			/* check cur == first because it's unsigned */
			if (cur == first || (rec.end < time && rec.node == node &&
				rec.job > 0))
				break;
			cur--;
		}

		/* search forward */
		cur = id+1;
		while (cur <= last){
			/*
			printf("scan forward %d\n",cur);
			*/
			GetRec(tdata,cur, rec);
			if (rec.begin <= time && rec.end >= time && 
				rec.node == node){
				/* found it */
				PutMessage(&rec);
			}
			if (rec.begin > time && rec.node == node && rec.job > 0){
					break;
			}
			cur++;
		}
	}
	else {
		RecId id;
		for (id=first; id <= last; id++){
			GetRec(tdata,id, rec);
			if (rec.node == node && rec.begin <= time && rec.end >= time )
				/* found it */
				PutMessage(&rec);
		}
	}

	EndPutMessage(numMsgs, msgs);
}
