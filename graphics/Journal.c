/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 21:12:54  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h> 
#if defined(SUN) || defined(PENTIUM)
#include <string.h>
#else
#include <strings.h>
#endif
#include <time.h>
#include "Journal.h"
#include "Init.h"
#include "Selection.h"
#include "Time.h"



/* static class variables */
int Journal::_fd;
int Journal::_playFd;
long Journal::_time;
char *Journal::_magic = "VISD";
Journal::EventType Journal::_lastEvent = Journal::Stop;
Boolean Journal::_opened = false;


struct JournalRec {
	long time;  /* # of milliseconds from previous record */
	Journal::EventType event; /* type of event */
	int selectionId;    /* id of selection */
	int viewId; /* id of view */
	VisualFilter filter; /* actual filter */
	VisualFilter hint; /* hint used for visual filter */

	/* buffer manager statistics for the query */
	int numGetPage;
	int numHits;
	int numPrefetch;
	int numPrefetchHits;
};

/***************************************************************
Journal::Init
********************************************************************/
void Journal::Init(char *journalName, int argc, char **argv){
	/* find current time */
char buf[80];
#if defined(HPUX) || defined(IBM_POWER_STATION) || defined(SUN) || defined(PENTIUM)
	time_t curTime = (int)time((time_t *)NULL);
	struct tm *timeStruct = localtime(&curTime);
#else
	int curTime = time((int *)0);
	struct tm *timeStruct = localtime(&curTime);
#endif
	Boolean newFile = false;

	_time = DeviseTime::Now();

	/* create a file name for the journal */
	if ((_fd=open(journalName, O_WRONLY,0666))>=0){
		printf("Journal to existing file %s\n", journalName);
		gets(buf);
		/* seek to the end of the file */
		lseek(_fd,0,SEEK_END);
	}
	else {
	   if ((_fd=open(journalName, O_CREAT|O_WRONLY, 0666))<0){
			perror("Journal: create file");
			Exit::DoExit(1);
		}
		printf("journal to new file %s\n", journalName);
		newFile = true;
	}


	if (newFile){
		/* write out header */
		if (write(_fd,_magic, JournalMagicLength) != JournalMagicLength)
			goto error;
	
		/* write out current time */
		if (write(_fd,(char *)timeStruct,sizeof(struct tm)) != sizeof(struct tm))
			goto error;


		/* write out the args */
		if (write(_fd,(char *)&argc,sizeof(argc)) != sizeof(argc))
			goto error;
		int i;
		for (i=0; i < argc; i++){
			int len = strlen(argv[i]);
			if (write(_fd,(char *)&len,sizeof(len)) != sizeof(len))
				goto error;
			if (write(_fd,argv[i], len)!=len)
				goto error;
		}
	}
	_opened = true;
	return;
error:
	fprintf(stderr,"Journal::Init: error writing args\n");
	Exit::DoExit(1);
}

/************************************************************************
Stop recording of the journal
*************************************************************************/
void Journal::StopRecording(){
	if (_opened){
		(void)close(_fd);
		_opened = false;
	}
}


/**********************************************************************
Record an event into the journal
************************************************************************/
void Journal::RecordEvent(EventType type, Selection *selection,
			View *v, VisualFilter *filter, VisualFilter *hint,
			int numGetPage, int numHits, int numPrefetch, int numPrefetchHits){
		if (!_opened)
			return;

		_lastEvent = type;

		long now = DeviseTime::Now();
		long diff = now - _time;
		_time = now;

		JournalRec rec;
		rec.time = diff;
		rec.event = type;
		rec.selectionId = Selection::FindSelectionId(selection);
		rec.viewId = View::FindViewId(v);
		if (filter != NULL){
			rec.filter = *filter;
			if (hint == NULL){
				printf("Journal::RecordEvent: need hint\n");
				exit(1);
			}
			rec.hint = *hint;
		}
		rec.numGetPage = numGetPage;
		rec.numHits = numHits;
		rec.numPrefetch= numPrefetch;
		rec.numPrefetchHits = numPrefetchHits;
		if (write(_fd,(char *)&rec,sizeof(rec))!=sizeof(rec)){
			perror("Journal::RecordEvent: write Push/change");
			Exit::DoExit(1);
		}
}

/******************************************************************
Initiiaze playback of the file 
********************************************************************/
void Journal::InitPlayback(char *fname, Boolean printHeader){
	if ((_playFd=open(fname, O_RDONLY, 0666))<0){
		perror("Journal: Open");
		Exit::DoExit(1);
	}

	/* read header */
	char header[JournalMagicLength];
	if (read(_playFd,header, JournalMagicLength) != JournalMagicLength)
		goto error;
	if (strncmp(header,_magic,JournalMagicLength)!= 0){
		fprintf(stderr,"wrong header\n");
		goto error;
	}

	/* read creation time of structure */
	struct tm timeStruct;
	if (read(_playFd,(char *)&timeStruct,sizeof(struct tm)) 
			!= sizeof(struct tm))
		goto error;
	if (printHeader)
		printf("Created %02d/%02d/%04d %02d:%02d:%02d\n",
			timeStruct.tm_mon+1,timeStruct.tm_mday, 
			timeStruct.tm_year+1900,timeStruct.tm_hour,
			timeStruct.tm_min, timeStruct.tm_sec);

	/* skip the args stored in the header of the file */
	int argc;
	/* read the args back*/
	if (read(_playFd,(char *)&argc,sizeof(argc)) != sizeof(argc))
		goto error;
	int i;
	for (i=0; i < argc; i++){
		int len;
		char buf[100];
		if (read(_playFd,(char *)&len,sizeof(len)) != sizeof(len))
			goto error;
		if (len >= 100)
			goto error;
		if (read(_playFd,buf, len)!=len)
			goto error;
		if (printHeader){
			buf[len] = '\0';
			printf("%s ",buf);
		}
	}
	if (printHeader)
		printf("\n");
	return;
error:
	fprintf(stderr,"Journal::InitPlayback: read error\n");
	Exit::DoExit(1);
}

/*****************************************************************
get next event, but returning only the ids of selection and view 
*******************************************************************/
Boolean Journal::NextEvent(long &time,
		EventType &type, int &selectionId, int &viewId, VisualFilter &filter,
		VisualFilter &hint,int &numGetPage, int &numHits,
		int &numPrefetch, int &numPrefetchHits){
	JournalRec rec;
	int stat;
	if ((stat=read(_playFd,(char *)&rec,sizeof(rec))) != sizeof(rec)){
		if (stat == 0){
			close(_playFd);
			return true;
		}
		perror("Journal::RecordEvent: write Push/change");
		Exit::DoExit(1);
	}

	time = rec.time;
	type = rec.event;
	selectionId = rec.selectionId;
	viewId = rec.viewId;
	filter = rec.filter;
	hint = rec.hint;
	numGetPage = rec.numGetPage;
	numHits = rec.numHits;
	numPrefetch = rec.numPrefetch;
	numPrefetchHits = rec.numPrefetchHits;

	return false;
}

/*************************************************************************
Read next event.
**************************************************************************/
Boolean Journal::NextEvent(long &time,
	EventType &type, Selection *&selection, View *&view, VisualFilter &filter,
	VisualFilter &hint,int &numGetPage, int &numHits,
	int &numPrefetch, int &numPrefetchHits){

	int selectionId, viewId;
	Boolean stat = NextEvent(time,type,selectionId, viewId, filter,hint,
		numGetPage, numHits, numPrefetch, numPrefetchHits);
	if (!stat){
		selection = Selection::FindSelectionById(selectionId);
		view = View::FindViewById(viewId);
	}
	return stat;
}


