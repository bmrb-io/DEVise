/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:19:21  jussi
  Added VCS header
*/

/*
   tdataStat.c: Print statistics of ams file
   Usage: tdataStat infile 
*/

#include <stdio.h>
#include <libc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/uio.h>
#include <unistd.h>
#include "TDataAppend.h"
#include "ams.h"

void Usage(char *name){
	printf("Usage: %s infile\n",name);
	exit(1);
}

main(int argc, char **argv){
int numTimes = 1;
char *infile, *outfile;

	if (argc < 2)
		Usage(argv[0]);
	
	infile = argv[1];


	/* Open output file */
	TDataAppend *tdata = new TDataAppend(infile,sizeof(AmsData));
	printf("There are %d pages\n", tdata->NumPages());
	printf("There are %d records\n", tdata->NumRecords());
	if (tdata->NumRecords() > 0){
		/* get 1st record */
		int numRecs;
		RecId startRid;
		void **recs;
		tdata->GetPage(tdata->FirstPage(),numRecs,startRid,recs);
		AmsData *data = (AmsData *)recs[0];
		printf("1st record, time = %f\n",data->time);
		tdata->FreePage(tdata->FirstPage());

		/* get last record */
		tdata->GetPage(tdata->LastPage(),numRecs,startRid,recs);
		data = (AmsData *)recs[numRecs-1];
		printf("1st record, time = %f\n",data->time);
		tdata->FreePage(tdata->LastPage());
	}
	delete tdata;
}
