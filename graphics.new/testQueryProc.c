/*
  $Id$

  $Log$*/

#include "BufMgr.h"
#include "TDataAppend.h"
#include "TDataMap.h"
#include "QueryProc.h"

const int BufSize = 10;	/* # of buffer pages */
const int NumRecs = 1000;	/* # of records */

struct Rec {
	double x;
	double y;
	char padding[248];
};

char *fileName = "testQueryData";

class MyMap: public TDataMap{
public:
	void Map(TData *, RecId , void *rec, Symbol *sym){
		Rec *record = (Rec *)rec;
		sym->x = record->x;
		sym->y = record->y;
	}


};

class MyCallback: public QueryCallback {
public:
	void ReturnGData(Symbol **syms, int numSyms, Boolean isBreak, 
			Boolean repeatFirst, Boolean repeatLast){
		printf("QueryCallbck: break %d, repeatFirst %d, repeatLast %d\n",
			isBreak, repeatFirst, repeatLast);
		int i;
		for (i=0;i < numSyms; i++){
			printf("%7.1f ",syms[i]->x);
		}
		printf("\n");
	};

	void QueryDone(){ printf("Query done\n");};
};

main(){

	/* create buffer manager */
	BufMgr *mgr = new BufMgr(BufSize);

	/* create file */
	TDataAppend *tdata = new TDataAppend(fileName,mgr,sizeof(Rec));
	TDataMap *map;
	QueryCallback *callback;
	VisualFilter filter, hint;

	QueryProc *qp = new QueryProc;

	/* make sure file is empty */
	if (tdata->NumRecords() != 0){
		fprintf(stderr,"data file %s not empty. Erase it first\n",
			fileName);
		goto done;
	}

	/* insert records into file */
	int i;
	Rec rec;
	for (i=0;i < NumRecs; i++){
		rec.x = (double)i;
		rec.y = (double)i -.1;
		tdata->InsertRec(&rec);
	}

	/* Create a callback for our query */
	callback = new MyCallback;

	/* queue up a query */
	filter.xLow = (double)(NumRecs/5);
	filter.xHigh = (double)(NumRecs*3/5);
	filter.flag = VISUAL_X;

	/* hint for future */
	hint.xLow = (double)(NumRecs*2/5);
	hint.xHigh = (double)(NumRecs*4/5);
	hint.flag = VISUAL_X;

	/* create a new mapping */
	map = new MyMap();

	qp->BatchQuery(mgr,tdata,map,filter,hint,callback);
	do {
		/* let query processor run until it's ready for next command. */
		qp->ProcessQuery();
	}while (qp->GetState() != QueryProc::WaitCmd);

	printf("buffer now contains:\n");
	mgr->Print();

	/* do next query */
	filter = hint;
	hint.xLow = 0;
	hint.xHigh = (double)( NumRecs/2);

	qp->BatchQuery(mgr,tdata,map,filter,hint,callback);
	do {
		/* let query processor run until it's ready for next command. */
		qp->ProcessQuery();
	}while (qp->GetState() != QueryProc::WaitCmd);
	printf("buffer now contains:\n");
	mgr->Print();


	/* deleting TData forces it to flush header to disk */
	delete tdata;
done:
	mgr->Clean();

}
