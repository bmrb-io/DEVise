/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.13  1996/07/09 15:59:55  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

  Revision 1.12  1996/04/18 18:12:45  jussi
  Replaced 'postScript' member variable and method with more appropriate
  name 'batchFile.'

  Revision 1.11  1996/04/16 20:06:58  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.10  1996/02/05 23:56:54  jussi
  Added DEVise logo display.

  Revision 1.9  1996/01/27 00:20:51  jussi
  Added -postscript command option.

  Revision 1.8  1996/01/11 21:53:06  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.7  1995/12/28 18:47:59  jussi
  Small fixes to remove compiler warnings.

  Revision 1.6  1995/12/14 17:10:33  jussi
  Small fixes.

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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#include "Dispatcher.h"
#include "Exit.h"
#include "Init.h"
#include "Time.h"
#include "BufPolicy.h"
#include "Util.h"
#include "Version.h"


/*************************************************************
Create unique temporary file name 
**************************************************************/

static char uniqueFileName[100];
static char *CreateUniqueFileName(char *progname)
{
  progname = StripPath(progname);
  pid_t pid = getpid();
  for(char char1 = 'a'; char1 <= 'z'; char1++) {
    for(char char2 = 'a'; char2 <= 'z'; char2++) {
      sprintf(uniqueFileName, "work/%s_%05ld%c%c", progname,
	      (long)pid, char1, char2);
      int fd = open(uniqueFileName, O_WRONLY, 0666);
      if (fd < 0)
	return uniqueFileName;
      close(fd);
    }
  }

  DOASSERT(0, "Cannot create unique temporary file name");

  return NULL; /* keep compiler happy */
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
char *Init::_progName = 0; /* name of program */
char *Init::_workDir = 0; /* name of work directory */
char *Init::_tmpDir = 0;/* name of temp directory */
char *Init::_sessionName = "session.tk";	/* name of program */
Boolean Init::_dispLogo = true;
char *Init::_batchFile = 0;
char *Init::_qpName = "default"; /* name of query processor */
Boolean Init::_restore = false; /* TRUE if we need to restore a session file */
long Init::_progModTime;	/* when program was modified */
Boolean Init::_randomize = true; /* true if TData fetches are randomized */
int Init::_pageSize = 16384;	/* size of page */
Boolean Init::_hasXLow=false, Init::_hasYLow = false;
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

static void MoveArg(int &argc, char **argv, int index, int len)
{
  DOASSERT(index + len <= argc, "Argument too long");

  for(int j = index + len; j < argc; j++)
    argv[j - len] = argv[j];

  argc -= len;
}

static void Usage(char *prog)
{
  fprintf(stderr, "Usage: %s [options]\n", prog);
  fprintf(stderr, "\nOptions are:\n");
  fprintf(stderr, "\t-journal <file>: name of journal file\n");
  fprintf(stderr, "\t-playback <file>: journal file to play back\n");
  fprintf(stderr, "\t-buffer <size>: buffer size in pages\n");
  fprintf(stderr, "\t-prefetch 0|1: do prefetch or not\n");
  fprintf(stderr, "\t-policy <policy>: buffer replacement policy, one of:\n");
  fprintf(stderr, "\t                  lru, fifo, lifo, focal, or rnd\n");
  fprintf(stderr, "\t-existing 0|1: use existing buffers first or not\n");
  fprintf(stderr, "\t-norandom: don't randomize record retrieval\n");
  fprintf(stderr, "\t-batch <file>: batch file to execute\n");
  fprintf(stderr, "\t-version: print version number and compile date\n");
  fprintf(stderr, "\t-usage: print this message\n");
  fprintf(stderr, "\t-queryProc <name>: name of query procedure\n");
  fprintf(stderr, "\t-session <file>: name of session file to open\n");
  fprintf(stderr, "\t-pagesize <size>: size of page to use (must be a\n");
  fprintf(stderr, "\t                  multiple of 4096)\n");
  fprintf(stderr, "\t-printViewStat 0|1: print view statistics or not\n");
  fprintf(stderr, "\t-batchRecs 0|1: batch records or not\n");
  fprintf(stderr, "\t-dispGraphics 0|1: display graphics or not\n");
  fprintf(stderr, "\t-elimOverlap 0|1: eliminate overlapping GData or not\n");
  fprintf(stderr, "\t-gdata 0|1: data is GData (vs. TData) or not\n");
  fprintf(stderr, "\t-gdatapages <number>: max. number of GData disk pages\n");
  fprintf(stderr, "\t-convert 0|1: convert TData to GData while idle or not\n");
  fprintf(stderr, "\t-iconify 0|1: iconify windows when restoring a\n");
  fprintf(stderr, "\t              session or not\n");
  fprintf(stderr, "\t-printTDataAttr 0|1: print TData attribute list or not\n");
  fprintf(stderr, "\t-simpleInterpreter 0|1: use simple interpreter or not\n");
  fprintf(stderr, "\t-nologo: don't display logo\n");
  fprintf(stderr, "\t-abort: abort instead of exit when program quits\n");
  fprintf(stderr, "\t-savePopup: save popup window and wait for button event\n");
  fprintf(stderr, "\t-xlow <value>: not yet implemented\n");
  fprintf(stderr, "\t-ylow <value>: not yet implemented\n");
  fprintf(stderr, "\t-xhigh <value>: not yet implemented\n");
  fprintf(stderr, "\t-yhigh <value>: not yet implemented\n");

  Exit::DoExit(1);
}

static void CatchInt(int)
{
  Dispatcher::QuitNotify();
}

void Init::DoInit(int &argc, char **argv)
{
  /* set up interrupt handling for INTR */
  (void)signal(SIGINT, CatchInt);

  /* Create work directory, if needed */
  char *workDir = getenv("DEVISE_WORK");
  if (!workDir)
    workDir = "work";
  CheckAndMakeDirectory(workDir);
  _workDir = CopyString(workDir);

  char *journalName = NULL;
#define MAXARGS 512
  char *args[512];

  DOASSERT(argc <= MAXARGS, "Too many arguments");

  for(int j = 0; j < argc; j++)
    args[j] = argv[j];
  
  /* init current time */
  DeviseTime::Init();

  _progName = CopyString(argv[0]);
  _progModTime = ModTime(argv[0]);

  char *tmpDir =  getenv("DEVISE_TMP");
  if (!tmpDir)
    tmpDir = "tmp";

  CheckAndMakeDirectory(tmpDir);
  pid_t pid = getpid();
  char buf[512];
  DOASSERT(strlen(tmpDir) + 20 <= 512, "String space too small");
  sprintf(buf, "%s/DEVise_%ld", tmpDir, (long)pid);
  CheckAndMakeDirectory(buf, true);
  _tmpDir = CopyString(buf);

  /* parse parameters */
  int i = 1;
  while (i < argc) {
    if (argv[i][0] == '-') {

      if (strcmp(&argv[i][1], "queryProc") == 0) {
	if (i >= argc-1)
	  Usage(argv[0]);
	_qpName = CopyString(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "batch") == 0) {
	if (i >= argc - 1)
	  Usage(argv[0]);
	_batchFile = CopyString(argv[i + 1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "session") == 0) {
	if (i >= argc-1)
	  Usage(argv[0]);
	_sessionName = CopyString(argv[i + 1]);
	_restore = true;
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "playback") == 0) {
	if (i >= argc-1)
	  Usage(argv[0]);
	_playbackFile = CopyString(argv[i+1]);
	_doPlayback = true;
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "journal") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	journalName = CopyString(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "buffer") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_bufferSize = atoi(argv[i+1]);
	if (_bufferSize <= 0) {
	  fprintf(stderr, "invalid buffer size %d\n", _bufferSize);
	  Usage(argv[0]);
	}
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "pagesize") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_pageSize = atoi(argv[i+1]);
	if (_pageSize <= 0) {
	  fprintf(stderr, "invalid buffer size %d\n", _pageSize);
	  Usage(argv[0]);
	}
	if ((_pageSize % 4096) != 0) {
	  fprintf(stderr, "page %d must be multiple of 4096\n", _pageSize);
	  Usage(argv[0]);
	}
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "policy") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	if (strcmp(argv[i+1], "lru") == 0)
	  _policy = BufPolicy::LRU;
	else if (strcmp(argv[i+1], "fifo") == 0)
	  _policy = BufPolicy::FIFO;
	else if (strcmp(argv[i+1], "lifo") == 0)
	  _policy = BufPolicy::LIFO;
	else if (strcmp(argv[i+1], "rnd") == 0)
	  _policy = BufPolicy::RND;
	else if (strcmp(argv[i+1], "focal") == 0)
	  _policy = BufPolicy::FOCAL;
	else {
	  fprintf(stderr, "unknown policy %s\n", argv[i+1]);
	  Usage(argv[0]);
	}
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "prefetch") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_prefetch = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "printViewStat") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_printViewStat = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "batchRecs") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_batchRecs = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "dispGraphics") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_dispGraphics = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "elimOverlap") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_elimOverlap = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "existing") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_existing = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "gdata") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_tdataQuery = (atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "gdatapages") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_gdataPages = atoi(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "convert") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_convertGData = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "iconify") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_iconify = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "printTDataAttr") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_printTDataAttr = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "simpleInterpreter") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_simpleInterpreter = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "nologo") == 0) {
	_dispLogo = false;
	MoveArg(argc,argv,i,1);
      }

      else if (strcmp(&argv[i][1], "abort") == 0) {
	_abort = true;
	MoveArg(argc,argv,i,1);
      }

      else if (strcmp(&argv[i][1], "savePopup") == 0) {
	_savePopup = true;
	MoveArg(argc,argv,i,1);
      }

      else if (strcmp(&argv[i][1], "norandom") == 0) {
	_randomize = false;
	MoveArg(argc,argv,i,1);
      }

      else if (strcmp(&argv[i][1], "version") == 0) {
	Version::PrintInfo();
  	Exit::DoExit(1);
      }

      else if (strcmp(&argv[i][1], "usage") == 0) {
	Usage(argv[0]);
  	Exit::DoExit(1);
      }

      else if (strcmp(&argv[i][1], "xlow") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_xLow = atof(argv[i+1]);
	_hasXLow = true;
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "ylow") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_yLow = atof(argv[i+1]);
	_hasYLow = true;
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "xhigh") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_xHigh = atof(argv[i+1]);
	_hasXHigh = true;
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "yhigh") == 0) {
	if (i >= argc -1)
	  Usage(argv[0]);
	_yHigh = atof(argv[i+1]);
	_hasYHigh = true;
	MoveArg(argc,argv,i,2);
      }
      else i++;
    }
    else i++;
  }

  if (!journalName)
    journalName = CreateUniqueFileName(argv[0]);

#if 0
  Journal::Init(journalName, argc, args);
#endif
}

void Init::BufPolicies(int &bufSize, Boolean &prefetch,
		       BufPolicy::policy &policy, Boolean &existing)
{
  bufSize = _bufferSize;
  prefetch = _prefetch;
  policy = _policy;
  existing = _existing;
}

long Init::ProgModTime()
{
  return _progModTime;
}

int Init::PageSize()
{
  return _pageSize;
}

BufPolicy::policy Init::Policy()
{
  return _policy;
}

Boolean Init::GetXLow(Coord &xLow)
{ 
  xLow = _xLow;
  return _hasXLow;
}

Boolean Init::GetYLow(Coord &yLow)
{
  yLow = _yLow;
  return _hasYLow;
}

Boolean Init::GetXHigh(Coord &xHigh)
{
  xHigh = _xHigh;
  return _hasXHigh;
}

Boolean Init::GetYHigh(Coord &yHigh)
{
  yHigh = _yHigh;
  return _hasYHigh;
}
