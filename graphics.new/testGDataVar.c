/* testGdataVar.c */
#include "Symbol.h"
#include "TDataAppend.h"
#include "GDataVar.h"
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
		:TDataMap(name, tdata, gRecSize, 0, 0, -1){}

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

	void UpdateBoundingBox(int, void **, int){}

	void DrawGDataArray(WindowRep *win, void **syms, int numSyms,
		double, double){}

};

class PrintRecClass: public GDataVarPrintCallback {
	virtual PrintRec(int pageNum, void **gdata, int numRecs){
		int i;
		for (i=0; i < numRecs; i++){
			printf("%5d %5d\n", pageNum, ((GDataRec *)gdata[i])->x);
		}
	}
};

PrintRecClass *printRec= NULL;

void PrintGData(GDataVar *gdata){
	if (printRec == NULL)
		printRec = new PrintRecClass;

	gdata->Print( printRec);
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
	GDataVar *gdata = map->GetGData();

	/* convert 2nd half of pages */
	int page;
	int mid = (tdata->FirstPage()+tdata->LastPage())/2;
	for (page = mid; page <= tdata->LastPage(); page++){
		int tmp= gdata->NextNotConverted(page);
		if (tmp != page){
			fprintf(stderr,"NextUnprocessed(%d) gets %d\n",
				page, tmp);
			exit(2);
		}
			
		gdata->ConvertPage(page, Stay);
	}

	for (page = tdata->FirstPage(); page < mid; page++){
		int tmp= gdata->NextNotConverted(page);
		if (tmp != page){
			fprintf(stderr,"NextUnprocessed(%d) gets %d\n",
				page, tmp);
			exit(2);
		}
		gdata->ConvertPage(page, Stay);
	}
	printf("printing \n");
	PrintGData(gdata);

	delete gdata;
}
