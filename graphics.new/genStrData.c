/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <libc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include "StrData.h"

int NumRecords= 1000; /* number of records to generate */
const char *fileName = "strData"; /* name of output file */
const int numProcs = 100; /* # of processes */

struct ProcRec{
	char name[StrDataNameSize];
	double meanUser; 
	double meanCpu;
	double meanIo;
} procRecs[numProcs];

main(int argc, char **argv){
	if (argc >= 2){
		NumRecords = atoi(argv[1]);
		if (NumRecords <= 0){
			printf("can't generated %d records\n", NumRecords);
			exit(2);
		}
	}

	int fd;
	if ((fd=open(fileName, O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR))<0){
	   perror("can't create file");
	   fprintf(stderr,"file name is %s\n",fileName);
	   exit(2);
	}

	/* Generate records for process names and mean times*/
	int i;
	for (i=0; i < numProcs; i++){
		sprintf(procRecs[i].name,"proc%d",i);
		procRecs[i].meanUser = (double)(random() % 100);
		procRecs[i].meanCpu = (double)(random() % 100);
		procRecs[i].meanIo =  (double)(random() % 100);
	}

	/* generate records */
	for (i=0; i < NumRecords; i++){
		int proc = random() % numProcs;
		struct StrData rec;
		strcpy(rec.name,procRecs[proc].name);
		rec.user = procRecs[proc].meanUser + (random() % 10)-5;
		rec.cpu= procRecs[proc].meanCpu+ (random() % 10)-5;
		rec.io= procRecs[proc].meanIo+ (random() % 10)-5;

		if (write(fd,(char *)&rec, sizeof(rec))!= sizeof(rec)){
			perror("write");
			exit(2);
		}
	}

	close(fd);

}
