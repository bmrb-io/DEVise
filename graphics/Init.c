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
  Revision 1.5  1995/12/02 20:54:45  jussi
  Substituted DeviseTime for Time.

  Revision 1.4  1995/11/29 17:22:40  jussi
  Changed default value of _iconify parameter to false.

  Revision 1.3  1995/11/07 20:46:27  jussi
  Commented out InitTimer() because timer functions are currently
  not used, and sometimes a SIGALRM gets through unhandled, causing
  image to exit.

  Revision 1.2  1995/09/05 21:12:52  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <libc.h>
#include <string.h>
#include <signal.h>

#include "Dispatcher.h"
#include "Exit.h"
#include "Init.h"
#include "Time.h"
#include "Timer.h"
#include "BufPolicy.h"
#include "Util.h"


/*************************************************************
Create unique temporary file name 
**************************************************************/
static char uniqueFileName[100];
static char *CreateUniqueFileName(char *progname){
	progname = StripPath(progname);
	/* Create a temporary file */
	pid_t pid = getpid();
	int fd;
	char char1,char2;
	for (char1= 'a'; char1 <= 'z'; char1++)
		for (char2='a'; char2 <= 'z'; char2++){
			sprintf(uniqueFileName,"work/%s_%05d%c%c",progname,pid,char1,char2);
			if ((fd=open(uniqueFileName, O_WRONLY,0666))<0){
				return uniqueFileName;
			}
			else close(fd);
		}

	fprintf(stderr,"CreateUniqueFile: can't create temp file name\n");
	Exit::DoExit(1);
	return NULL; /*keep compiler happy */
}

Boolean Init::_savePopup = false; /* TRUE if save pop up window 
						and wait for button event */
Boolean Init::_doPlayback = false; /* TRUE if do journal playback */
char *Init::_playbackFile = "";	/* name of playback file */
Boolean Init::_prefetch = true; /* TRUE if buffer manager does prefetch */
int Init::_bufferSize= 1536;		/* size of buffer */
BufPolicy::policy Init::_policy= BufPolicy::FIFO; /* policy for buffer manager*/
/* TRUE if existing buffers should be checked first in query processing */
Boolean Init::_existing = true;	
Boolean Init::_tdataQuery = false; /* TRUE if file is tdata, else gdata */
Boolean Init::_convertGData = true; /* true if TData is converted into G
									while system is idle */
Boolean Init::_abort = false; /* TRUE if abort instead of exit() on program 
							exit */
Boolean Init::_iconify= false; /* TRUE if windows are iconified when
							restoring a session */
int Init::_gdataPages = -1; /* max # of disk pages for gdata */
char *Init::_progName;	/* name of program */
char *Init::_workDir;	/* name of work directory */
char *Init::_tmpDir;	/* name of temp directory */
char *Init::_sessionName = "session.tk";	/* name of program */
char *Init::_qpName = "default"; /* name of query processor */
Boolean Init::_restore = false; /* TRUE if we need to restore a session file */
long Init::_progModTime;	/* when program was modified */
Boolean Init::_randomize = true; /* true if TData fetches are randomized */
int Init::_pageSize = 16384;	/* size of page */
Boolean Init::_hasXLow=false, Init::_hasYLow=false;
Boolean Init::_hasXHigh=false, Init::_hasYHigh = false;
Coord Init::_xLow, Init::_yLow, Init::_xHigh, Init::_yHigh;
Boolean Init::_simpleInterpreter = true; /* true if interpreted
	mapping should use its own interpreter to process
	simple commands instead of calling tcl */
Boolean Init::_printTDataAttr = false; /* true to print
	TData attribute list when it's created */
Boolean Init::_elimOverlap = true; /* true if overlapping
	GData should be eliminated while drawing */
Boolean Init::_dispGraphics = true; /* true to display graphics */
Boolean Init::_batchRecs = true; /* true if batching records */
Boolean Init::_printViewStat = false;  /* true to print view statistics */


/**************************************************************
Remove positions from index to index+len-1 from argv
Update argc.
***************************************************************/
static void MoveArg(int &argc, char **argv, int index, int len){
	if (index+len > argc){
		fprintf(stderr,"MoveArg:: index+len too big\n");
		Exit::DoExit(2);
	}

	int j;
	for (j=index+len; j < argc; j++)
		argv[j-len] = argv[j];
	argc -= len;
}

/**********************************************************
Usage
***************************************************************/
static void Usage(char *prog){
	fprintf(stderr,"usage %s [-journal file] [-play file] [-buffer size] [-prefetch yes_no] [-policy policy] [-existing yes_no]\n",prog);
	fprintf(stderr,"\t-journal file: name of journal file\n");
	fprintf(stderr,"\t-play file: name of journal file for playback\n");
	fprintf(stderr,"\t-buffer size: # of buffer blocks\n");
	fprintf(stderr,"\t-prefetch yes_no: whether or not to do prefetch\n");
	fprintf(stderr,"\t-policy policy: buffer replacement policy, one of:\n");
	fprintf(stderr,"\t\tlru, fifo, lifo, focal, or rnd\n");
	fprintf(stderr,"\t-existing yes_no: whether to use existing buffers first\n");
	fprintf(stderr,"\t-norandom: don't randomize record retrieval\n");
			
	Exit::DoExit(2);
}

static void CatchInt(int){
	printf("caught INT\n");

	Dispatcher::QuitNotify();
}


/****************************************************************
Init parameters
*****************************************************************/
void Init::DoInit(int &argc, char **argv){
	/* set up interrupt handling for INTR */
	(void)signal(SIGINT, CatchInt);

	/* Create work directory, if needed */
	char *workDir= getenv("DEVISE_WORK");
	if (workDir == NULL)
		workDir = "work";
	CheckAndMakeDirectory(workDir);
	_workDir = CopyString(workDir);

char *journalName = NULL;
#define MAXARGS 512
char *args[512];

	/* make a copy of args */
	if (argc > MAXARGS){
		fprintf(stderr,"Init::DoInit argc of %d too big. max = %d\n",
				argc, MAXARGS);
		Exit::DoExit(2);
	}
	int j;
	for (j=0; j < argc; j++)
		args[j] = argv[j];

	/* init current time */
	DeviseTime::Init();

	/* init timer */
#if 0
	Timer::InitTimer();
#endif

	_progName = (char *)malloc(strlen(argv[0])+1);
	strcpy(_progName, argv[0]);
	_progModTime = ModTime(argv[0]);

	char *tmpDir =  getenv("DEVISE_TMP");
	if (tmpDir == NULL){
		tmpDir = "tmp";
	} 
	CheckAndMakeDirectory(tmpDir);
	pid_t pid = getpid();
	char buf[512];
	if (strlen(tmpDir)+20 > 512) {
		fprintf(stderr,"Init: buf too small\n");
		Exit::DoExit(1);
	}
	sprintf(buf,"%s/DEVise_%d",tmpDir,pid);
	CheckAndMakeDirectory(buf,true);
	_tmpDir = CopyString(buf);


	/* parse parameters */
	int i=1;
	while (i < argc){
		if (argv[i][0] == '-'){
			if (strcmp(&argv[i][1],"queryProc")==0){
				if (i >= argc-1)
					Usage(argv[0]);
				_qpName = CopyString(argv[i+1]);
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"session")==0){
				if (i >= argc-1)
					Usage(argv[0]);
				_sessionName = CopyString(argv[i+1]);
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"restore") == 0){
				_restore = true;
				MoveArg(argc,argv,i,1);
			}
			else if (strcmp(&argv[i][1],"playback")==0){
				if (i >= argc-1)
					Usage(argv[0]);

				if ((_playbackFile = (char *)malloc(strlen(argv[i+1])+1))==NULL)
						goto error;
				strcpy(_playbackFile,argv[i+1]);
				_doPlayback = true;

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"journal")==0){
				if (i >= argc -1)
					Usage(argv[0]);
	
				journalName = argv[i+1];
	
				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"buffer")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				
				_bufferSize = atoi(argv[i+1]);
				if (_bufferSize <= 0){
					fprintf(stderr,"invalid buffer size %d\n", _bufferSize);
					Usage(argv[0]);
				}

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"pagesize")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				
				_pageSize = atoi(argv[i+1]);
				if (_pageSize <= 0){
					fprintf(stderr,"invalid buffer size %d\n", _pageSize);
					Usage(argv[0]);
				}

				if ((_pageSize % 4096) != 0){
					fprintf(stderr,"page %d must be multiple of 4096\n", 
						_pageSize);
					Usage(argv[0]);
				}

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"policy")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				if (strcmp(argv[i+1],"lru")== 0)
					_policy = BufPolicy::LRU;
				else if (strcmp(argv[i+1],"fifo")== 0)
					_policy = BufPolicy::FIFO;
				else if (strcmp(argv[i+1],"lifo")== 0)
					_policy = BufPolicy::LIFO;
				else if (strcmp(argv[i+1],"rnd")== 0)
					_policy = BufPolicy::RND;
				else if (strcmp(argv[i+1],"focal")== 0)
					_policy = BufPolicy::FOCAL;
				else {
					fprintf(stderr,"unknown policy %s\n",argv[i+1]);
					Usage(argv[0]);
				}

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);

			}
			else if (strcmp(&argv[i][1],"prefetch")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				_prefetch = !(atoi(argv[i+1])==0);

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"printViewStat")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				_printViewStat = !(atoi(argv[i+1])==0);

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"batchRecs")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				_batchRecs = !(atoi(argv[i+1])==0);

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"dispGraphics")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				_dispGraphics = !(atoi(argv[i+1])==0);

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"elimOverlap")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				_elimOverlap = !(atoi(argv[i+1])==0);

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"existing")==0){
				if (i >= argc -1)
					Usage(argv[0]);
				_existing = !(atoi(argv[i+1])==0);

				/* shift args in argv forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"gdata")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_tdataQuery = (atoi(argv[i+1])==0);

				/* shift args forward by 1 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"gdatapages")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_gdataPages = atoi(argv[i+1]);

				/* shift args forward by 1 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"convert")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_convertGData = !(atoi(argv[i+1])==0);

				/* shift args forward by 1 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"iconify")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_iconify = !(atoi(argv[i+1])==0);

				/* shift args forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"printTDataAttr")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_printTDataAttr = !(atoi(argv[i+1])==0);

				/* shift args forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"simpleInterpreter")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_simpleInterpreter = !(atoi(argv[i+1])==0);

				/* shift args forward by 2 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"abort")== 0){
				_abort = true;

				/* shift args forward by 1 */
				MoveArg(argc,argv,i,1);
			}
			else if (strcmp(&argv[i][1],"savePopup")== 0){
				_savePopup = true;

				/* shift args forward by 1 */
				MoveArg(argc,argv,i,1);
			}
			else if (strcmp(&argv[i][1],"norandom")== 0){
				_randomize = false;

				/* shift args forward by 1 */
				MoveArg(argc,argv,i,1);
			}
			else if (strcmp(&argv[i][1],"xlow")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_xLow = atof(argv[i+1]);
				_hasXLow = true;
				/* shift args forward by 1 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"ylow")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_yLow = atof(argv[i+1]);
				_hasYLow = true;
				/* shift args forward by 1 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"xhigh")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_xHigh = atof(argv[i+1]);
				_hasXHigh = true;
				/* shift args forward by 1 */
				MoveArg(argc,argv,i,2);
			}
			else if (strcmp(&argv[i][1],"yhigh")== 0){
				if (i >= argc -1)
					Usage(argv[0]);
				_yHigh = atof(argv[i+1]);
				_hasYHigh = true;
				/* shift args forward by 1 */
				MoveArg(argc,argv,i,2);
			}
			else i++;
		}
		else i++;
	}

	/* Init journal */
	if (journalName == NULL)
		journalName= CreateUniqueFileName(argv[0]);
	/*
	Journal::Init(journalName, argc, args);
	*/

	return;
error:
	fprintf(stderr,"Init::DoInit(): out of memory\n");
	Exit::DoExit(1);
}

/********************************************************************
Get current buffer policies
*********************************************************************/
void Init::BufPolicies(int &bufSize, Boolean &prefetch, BufPolicy::policy &policy, Boolean &existing){
	bufSize = _bufferSize;
	prefetch = _prefetch;
	policy = _policy;
	existing = _existing;
}

long Init::ProgModTime() { return _progModTime; }

int Init::PageSize() { return _pageSize; }
BufPolicy::policy Init::Policy() { return _policy; }


Boolean Init::GetXLow(Coord &xLow){ 
	xLow = _xLow;
	return _hasXLow;
}
Boolean Init::GetYLow(Coord &yLow){
	yLow = _yLow;
	return _hasYLow;
}

Boolean Init::GetXHigh(Coord &xHigh){
	xHigh = _xHigh;
	return _hasXHigh;
}
Boolean Init::GetYHigh(Coord &yHigh){
	yHigh = _yHigh;
	return _hasYHigh;
}

char *Init::ProgName(){ return _progName; }
