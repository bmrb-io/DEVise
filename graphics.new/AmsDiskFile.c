/* AmsDiskFile.c */
#include <libc.h>
#include "AmsDiskFile.h"
#include "TDataAppend.h"
#include "ams.h"

const int NumPages = 16721;
const int NumRecs  = 2140160;

/* don't write any pages */
void AmsDiskFile::WritePage(int pageNum, void *buf){
}

/********************************************************
Read a page
**********************************************************/
void AmsDiskFile::ReadPage(int pageNum, void *buf){
	struct FileHeader header;
	if (pageNum == 0){
		/* return header */
		header.numRecs = NumRecs;
		header.recSize = sizeof(struct AmsData);
		(*(int *)buf) = NumPages;
		bcopy(&header,((char *)buf)+sizeof(int),sizeof(header));
	}
	else {
		/* fill page with data */
		struct AmsData *ptr = (AmsData *)buf;
		int recs = 4096/sizeof(AmsData);
		double time = (pageNum-1)*recs*30.0+ 15.0;
		double timeIncr = 30.0;
		int i;
		for (i=0; i < recs; i++){
			ptr->time = time;
			ptr->solDn = 500.0;
			time += timeIncr;
			ptr++;
		}
	}
}

