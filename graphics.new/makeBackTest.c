/*
  $Id$

  $Log$*/

/**************************************************************************
MakeBackTest.c: make backwards test file
	to compile: CC -o makeTest makeTest.c

 usage: makeTest tdata tdataSize gdataLow gdataHigh bufSize cmd waitTime [wrong]
 where: tdata == name of tdata file
		tdataSize == size of tdata file (in bytes)
		gdataLow = left X-axis boundary of gdata
		gdataHigh = right X-axis boundary of gdata
		bufSize = size of buffer (in bytes)
		cmd: prefix name of command file to generate.
		wait == time to wait after each event, in seconds.
		wrong == set to generate wrong guesses.
*************************************************************************/
#include <stdio.h>
#include <libc.h>

void Usage(){
	fprintf(stderr, "usage: makeTest tdata tdataSize gdataLow gdataHigh bufSize cmd waitTime\n");
	exit(1);
}

int wrong = 0;

/**************************************************************
Make output to file
****************************************************************/
void MakeOutput(int argc, char **argv, int bufSize, double gdataHigh,
	FILE *file,double minWinSize, double maxWinSize, int waitTime){

	fprintf(file,"\# command file generated with ");
	int i;
	for (i=0; i < argc; i++){
		fprintf(file," %s",argv[i]);
	}
	fprintf(file,"\n");

	int pages = bufSize/4096;
	fprintf(file,"\# run this file with: runTest %s %d times\n", argv[6], pages);

int multiple = 1;
double curWinSize = minWinSize;
double scroll = 0.5;
if (wrong)
	scroll = 1.0;

while (curWinSize  <= maxWinSize){
		if (multiple == 1)
			fprintf(file,"wait 5\n");
		else if (waitTime > 0){
			fprintf(file,"wait %d\n",waitTime);
		}
		double bufMultiple = multiple* 0.5;
		fprintf(file,"# window size = %f buffer size\n",bufMultiple);
		fprintf(file,"s1 move %f -120 %f 1200 scrollx %.2f\n", 
			gdataHigh-curWinSize, curWinSize, scroll);
		if (waitTime > 0)
			fprintf(file,"wait %d\n",waitTime);
		fprintf(file,"s1 scrollx -0.5 scrollx %.2f\n",scroll);
		if (waitTime > 0)
			fprintf(file,"wait %d\n",waitTime);
		fprintf(file,"s1 scrollx -0.5 scrollx %.2f\n",scroll);
		if (waitTime > 0)
			fprintf(file,"wait %d\n",waitTime);
		fprintf(file,"s1 scrollx -0.5 scrollx %.2f\n",scroll);

		multiple++;
		curWinSize = multiple * minWinSize;
	}
	fprintf(file,"stat\n");
	fprintf(file,"quit\n");
}

main(int argc, char **argv){
	if ( argc != 8 && argc != 9) 
		Usage();
	
	char *tdata = argv[1];
	double tdataSize = atof(argv[2]);
	double gdataLow = atof(argv[3]);
	double gdataHigh = atof(argv[4]);
	int bufSize = atoi(argv[5]);
	char cmdFile[128];
	int waitTime = atoi(argv[7]);
	if (argc == 9)
		wrong = 1;

	/* max window size for 3 scrolls */
	double maxWinSize =  (gdataHigh - gdataLow)*2/5 ;
	printf("maxWinSiz = %f\n", maxWinSize);

	/* window size for .5 buffer size*/
	double minWinSize = (bufSize / 2/ tdataSize )* ( gdataHigh - gdataLow);
	printf("minWinSiz = %f\n", minWinSize);


	sprintf(cmdFile,"%s.bk.cmd.np",argv[6]);
	FILE *file = fopen(cmdFile,"w");
	if (file == NULL){
		fprintf(stderr,"can't open file %s\n",cmdFile);
		exit(1);
	}
	MakeOutput(argc,argv,bufSize, gdataHigh,file,minWinSize, maxWinSize, 0);
	fclose(file);

	sprintf(cmdFile,"%s.bk.cmd.yp",argv[6]);
	file = fopen(cmdFile,"w");
	if (file == NULL){
		fprintf(stderr,"can't open file %s\n",cmdFile);
		exit(1);
	}
	MakeOutput(argc,argv,bufSize, gdataHigh,file,minWinSize, maxWinSize, waitTime);
	fclose(file);
}
