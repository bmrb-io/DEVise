/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 21:13:48  jussi
  Added/update CVS header.
*/

/* print statistics on journal file

   usage: jstat [flags] file
	flags:
	-pn = print listing, where n==
	 0: print a dump of the file.
	 1: print visual filter width and completion time only, in seconds.
	 2: print visual filter width and buffer statistics only,
			including # miss, and # accessed
	-s = print statistics
*/

#include <math.h>
#include "Journal.h"

void Usage(char *name){
	fprintf(stderr,"Usage: %s [-p] file\n",name);
	fprintf(stderr,"\t-p: print listing\n");
	exit(1);
}

/*****************************************************
print time in hr:min:sc:msc format
time = in milliseconds
*******************************************************/
const int MILLI_PER_HOUR= 3600000; /* milliseconds per hour */
const int MILLI_PER_MIN = 60000; /* milliseconds per min */
const int MILLI_PER_SEC = 1000; /* milliseconds per second */
void PrintTime(long time){
	long hour = time / MILLI_PER_HOUR;
	time -= hour * MILLI_PER_HOUR;
	long min = time / MILLI_PER_MIN;
	time -= min* MILLI_PER_MIN;
	long sec = time / MILLI_PER_SEC;
	long msec = time - sec* MILLI_PER_SEC;
	printf("%03d:%02d:%02d:%03d",hour,min,sec,msec);
}

/* structure used to group records by visual filter width */
struct GroupRec {
	Coord width;	/* width of visual filter */
	int numOccur;	/* # of occurrences with this width */
	double totalTime; /*total time taken for all filters with this width(secs)*/
	double sumXiSquare; /* = sum(time of each occurrence^2), in seconds */
};

const int MaxGroupRecs = 512;
int _numGroups = 0;	/* # of groups found so far */
GroupRec groups[MaxGroupRecs];

/****************************************************
Init group records
****************************************************/
void InitGroupRecs(){
	
		_numGroups = 0;
}

/****************************************************
Insert a record into a group.
delta = fudge factor when deciding where to put a group.
That is, all width within += delta are placed into the same group.
*****************************************************/
void InsertGroup(Coord width, Coord time, Coord delta = 10.0){
	int index;
	Boolean found = false;
	for (index=0; index < _numGroups; index++){
		if (fabs(groups[index].width-width)<= delta){
			found = true;
			break;
		}
	}
	if (!found){
		/* allocate a new group */
		if (_numGroups >= MaxGroupRecs){
			fprintf(stderr,"no more groups\n");
			exit(2);
		}
		index = _numGroups++;
		groups[index].width = width;
		groups[index].numOccur = 0;
		groups[index].totalTime = 0.0;
		groups[index].sumXiSquare = 0.0;
	}

	/* update record in the index */
	groups[index].numOccur++;
	double secs = time/MILLI_PER_SEC;
	groups[index].totalTime += secs;
	groups[index].sumXiSquare += secs*secs;
}

/*************************************************************************
Print the group
**************************************************************************/
void PrintGroup(){
	printf("There are %d groups\n", _numGroups);
	double avg;
	int i;
	if (_numGroups > 0){
		printf("width\t\t\tnum\tavg\tstd\n");
		for (i=0; i < _numGroups; i++){
			avg = groups[i].totalTime/groups[i].numOccur;
			printf("%f\t\t%d\t%f\t%f\n", groups[i].width,groups[i].numOccur,
				avg, 
				groups[i].numOccur <= 1? 0.0:
					sqrt((groups[i].sumXiSquare*groups[i].numOccur - 
						groups[i].totalTime*groups[i].totalTime)/
					(groups[i].numOccur*(groups[i].numOccur-1))));
		}
	}
}


main(int argc, char **argv){
Boolean printFlag = false;
char printType;
Boolean statFlag = false;
char *file = NULL;
	
	InitGroupRecs();

	int i;
	for (i=1; i < argc; i++){
		if (argv[i][0]== '-' ){
			if (argv[i][1] == 'p'){
				printFlag = true;
				printType = argv[i][2];
			}
			else if (argv[i][1] == 's'){
				statFlag = true;
			}
			else Usage(argv[0]);
		}
		else file = argv[i];
	}

	if (file == NULL)
		Usage(argv[0]);

	/* init playback */
	Journal::InitPlayback(file,true);

	Journal::EventType event;
	int selectionId,viewId;
	VisualFilter filter, hint;
	long time ;

	/* # of times for each event */
	int numStart = 0;
	int numStop = 0;
	int numStep = 0;
	int numPush = 0;
	int numPop = 0;
	int numChange =0;
	int numPause = 0;

	/* total time to execute the event */
	long pushTime = 0;
	long popTime = 0;
	long changeTime = 0;
	long pauseTime = 0;	/* amount of time between completion of one and
						start of next event */
	
	int PushPush = 0;	/*# of times pushed followed by push */
	int PushPop = 0;
	int PushChange = 0;
	int PopPush = 0;
	int PopPop = 0;
	int PopChange = 0;
	int ChangePush = 0;
	int ChangePop = 0;
	int ChangeChange = 0;
	/* The next two variables are used to keep track of whether
	we got PushPush/PushPop, etc */
	Journal::EventType lastAction = Journal::Stop; 
	Journal::EventType lastLastAction = Journal::Stop; 


	long now = 0;
	Journal::EventType lastEvent = Journal::Stop;
	if (printFlag ){
		if (printType == '0')
			printf("time\t\tinternval\tevent\tselection\tview\n");
		else if (printType == '1')
			printf("width\ttime\n");
		else if (printType == '2')
			printf("width\tmissed\tacc\tpmiss\tp_acc\n");
	}
	VisualFilter lastFilter;

	int numGetPage, numHits, numPrefetch, numPrefetchHits;
	while (!Journal::NextEvent(time, event, selectionId, viewId, filter,
		hint, numGetPage, numHits, numPrefetch, numPrefetchHits)){
		now += time;
		switch(event){
			case Journal::Start:
				numPause++;
				pauseTime += time;
				numStart++;
				break;
			case Journal::Stop:
				numPause++;
				pauseTime += time;
				numStop++;
				break;
			case Journal::Step:
				numPause++;
				pauseTime += time;
				numStep++;
				break;
			case Journal::PushSelection:
			case Journal::PopSelection:
			case Journal::ChangeSelection:
				lastLastAction = lastAction;
				lastAction = event;
				numPause++;
				pauseTime += time;
				break;
			case Journal::Completion:
				InsertGroup(lastFilter.xHigh-lastFilter.xLow, time);

				if (lastLastAction == Journal::PushSelection 
					&& lastAction == Journal::PushSelection)
					PushPush++;
				else if (lastLastAction == Journal::PushSelection && 
					lastAction == Journal::PopSelection)
					PushPop++;
				else if (lastLastAction == Journal::PushSelection 
					&& lastAction == Journal::ChangeSelection)
					PushChange++;
				else if (lastLastAction == Journal::PopSelection 
					&& lastAction == Journal::PushSelection)
					PopPush++;
				else if (lastLastAction == Journal::PopSelection 
					&& lastAction == Journal::PopSelection)
					PopPop++;
				else if (lastLastAction == Journal::PopSelection 
					&& lastAction == Journal::ChangeSelection)
					PopChange++;
				else if (lastLastAction == Journal::ChangeSelection 
					&& lastAction == Journal::PushSelection)
					ChangePush++;
				else if (lastLastAction == Journal::ChangeSelection 
					&& lastAction == Journal::PopSelection)
					ChangePop++;
				else if (lastLastAction == Journal::ChangeSelection 
					&& lastAction == Journal::ChangeSelection)
					ChangeChange++;

				if (lastEvent == Journal::PushSelection){
					numPush++;
					pushTime += time;
				}
				else if (lastEvent == Journal::PopSelection){
					numPop++;
					popTime += time;
				}
				else if (lastEvent == Journal::ChangeSelection){
					numChange++;
					changeTime += time;
				}
				else {
					fprintf(stderr,"inconsistent file: completion does not follow push/pop/change\n");
					exit(1);
				}
				break;
		}


		if (printFlag){
			if (printType == '0'){
				PrintTime(now);
				printf("\t");
				PrintTime(time);
				printf("\t");
				switch(event){
					case Journal::Start:
						printf("Strt");
						break;
					case Journal::Stop:
						printf("Stop");
						break;
					case Journal::Step:
						printf("Step");
						break;
					case Journal::PushSelection:
						printf("Push");
						break;
					case Journal::PopSelection:
						printf("Pop ");
						break;
					case Journal::ChangeSelection:
						printf("Chng");
						break;
					case Journal::Completion:
						printf("Comp");
						break;
				}
				printf("\t%d\t%d\n", selectionId, viewId);
			}
			else if (printType == '1'){
				if (event == Journal::Completion)
					printf("%f\t%f\n",lastFilter.xHigh-lastFilter.xLow, 
						((double)time)/MILLI_PER_SEC);
			}
			else if (printType == '2'){
				int regularGet = numGetPage - numPrefetch;
				int regularHit = numHits - numPrefetchHits;
				if (event == Journal::Completion)
					printf("%f\t%d\t%d\t%d\t%d\n",
						lastFilter.xHigh-lastFilter.xLow, 
						regularGet-regularHit, regularGet,
						numPrefetch-numPrefetchHits, numPrefetch);
			}
		}
		lastEvent = event;
		lastFilter = filter;
	}

	if (statFlag){
		printf("start = %d, stop = %d, step = %d\n", numStart, numStop,numStep);
		printf("push   = %d, total = ", numPush);
		PrintTime(pushTime);
		printf("\n");
		printf("pop    = %d, total = ", numPop);
		PrintTime(popTime);
		printf("\n");
		printf("change = %d, total = ", numChange);
		PrintTime(changeTime);
		printf("\n");
		printf("pause  = %d, total = ", numPause);
		PrintTime(pauseTime);
		printf("\n");

		printf("PushPush= %d, PushPop= %d, PushChange = %d\n",
			PushPush, PushPop, PushChange);
		printf("PopPush= %d, PopPop = %d, PopChange = %d\n",
			PopPush, PopPop, PopChange);
		printf("ChangePush= %d, ChangePop = %d, ChangeChange= %d\n",
			ChangePush, ChangePop, ChangeChange);
		PrintGroup();
	}
}
