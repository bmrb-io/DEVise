/*
  $Id$

  $Log$*/

#include "Symbol.h"
#include "TDataAppend.h"
#include "GData.h"
#include "TDataMap.h"
#include "PageRange.h"
#include "BufMgr.h"

const int MAX_RECS = 500;

char *tdataName = "tdataFile";
char *gdataName = "gdataFile";

/* tdata record */
struct TDataRec{
	int x;
	char padding[402];
};

struct GDataRec{
	int x;
	char padding[124];
};

class TestMap: public TDataMap{
public:
	TestMap(char *name, TData *tdata, int gRecSize)
		:TDataMap(name, tdata, gRecSize){}

	virtual void MapArray(TData *, RecId, void **, Symbol **, int numRecs,
		int startIndex= 0, int incr= 1){
	}

	/* convert from Tdata to Gdata */
	virtual void ConvertToGData(void **tRecs, int numRecs,void **gdataArray){
		int i;
		for (i=0; i < numRecs; i++){
			/*
			printf("converting x= %d\n", ((TDataRec *)syms[i])->x);
			*/
			((GDataRec *)gdataArray[i])->x = ((TDataRec *)tRecs[i])->x;
		}
	}

	/* return the size of each GData record */
	virtual int GDataRecordSize() {
		return sizeof(GDataRec);
	}


	void DrawGDataArray(WindowRep *win, void **syms, int numSyms){}

};

void PrintGData(GData *gdata){
	int numRanges;
	RangeData *list;
	gdata->GetRangeList(numRanges, list);

	printf("GData: There are %d ranges\n", numRanges);
	RangeData *current;
	for (current = list->next; current != list; current = current->next){
		printf("Range: (%d,%d)\n",current->lowPage, current->highPage);

		int j;
		for (j = current->lowPage; j <= current->highPage; j++){
			printf("page %d: ",j);
			int numRecs;
			void **recs;
			RecId startRid;
			gdata->GetPage(j, numRecs, startRid, recs);
			int k;
			for (k=0; k < numRecs; k++){
				GDataRec *rec = (GDataRec *)recs[k];
				printf("%d ",rec->x);
			}
			gdata->FreePage(j);
			printf("\n");
		}
	}
}

main(){

	unlink(tdataName);
	unlink(gdataName);

	/* make TData */
	BufMgr *mgr = new BufMgr();
	TDataAppend *tdata = new TDataAppend(tdataName,mgr, sizeof(TDataRec));
	int i=i;
	TDataRec rec;
	for (i=0; i < MAX_RECS; i++){
		rec.x = i;
		tdata->InsertRec(&rec);
	}

	/* make GData */
	TDataMap *map = new TestMap("testMap", tdata, sizeof(GDataRec));
	GData *gdata = map->GetGData();

	printf("convert odd pages\n");
	int page;
	for (page = gdata->FirstPage(); page <= gdata->LastPage(); page += 2){
		gdata->ConvertPage(page);
	}

	PrintGData(gdata);

	printf("converting even pages\n");
	for (page = gdata->FirstPage()+1; page <= gdata->LastPage(); page += 2){
		gdata->ConvertPage(page);
	}

	PrintGData(gdata);
}
