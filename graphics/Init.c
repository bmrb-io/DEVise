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
  Revision 1.23  1996/11/13 16:56:07  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.22  1996/10/07 22:53:49  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.21  1996/09/19 19:32:46  wenger
  Devise now complains about illegal command-line flags (fixes bug 042).

  Revision 1.20  1996/09/12 18:49:40  wenger
  Oops!  I guess I should make sure this works before I commit it.

  Revision 1.19  1996/09/12 18:42:31  wenger
  Make sure imageDelay is non-negative.

  Revision 1.18  1996/09/12 18:37:43  wenger
  Added optional delay before drawing images.

  Revision 1.17  1996/09/05 20:00:05  jussi
  Added screenWidth and screenHeight command line arguments.

  Revision 1.16  1996/09/04 21:24:49  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.15  1996/08/23 16:55:34  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.14  1996/07/12 15:21:00  jussi
  Removed references to Timer.

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

Boolean Init::_savePopup = false;

Boolean Init::_doPlayback = false;
char *Init::_playbackFile = "";

Boolean Init::_prefetch = true;
int Init::_bufferSize = 1536;
BufPolicy::policy Init::_policy = BufPolicy::FIFO;
Boolean Init::_existing = true;	

Boolean Init::_tdataQuery = false;
Boolean Init::_convertGData = true;
int Init::_gdataPages = -1;
Boolean Init::_randomize = true;

char *Init::_progName = 0;
long Init::_progModTime;
Boolean Init::_dispLogo = true;
Boolean Init::_abort = false;

char *Init::_workDir = 0;
char *Init::_tmpDir = 0;
char *Init::_cacheDir = 0;

char *Init::_sessionName = "session.tk";
Boolean Init::_restore = false;
Boolean Init::_iconify= false;

int Init::_pageSize = 16384;
Boolean Init::_elimOverlap = true;

Boolean Init::_simpleInterpreter = true;
Boolean Init::_printTDataAttr = false;
Boolean Init::_dispGraphics = true;
Boolean Init::_batchRecs = true;
Boolean Init::_printViewStat = false;

char *Init::_batchFile = 0;

char * Init::_daliServer = NULL;
Boolean Init::_daliQuit = false;
int Init::_imageDelay = 0;

int Init::_screenWidth = -1;
int Init::_screenHeight = -1;

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
  fprintf(stderr, "\t-tasvir <name>: specify name of tasvir server\n");
  fprintf(stderr, "\t-tasvirquit: kills tasvir server when Devise exits\n");
  fprintf(stderr, "\t-screenWidth <value>: sets screen width for batch mode\n");
  fprintf(stderr, "\t-screenHeight <value>: sets screen height for batch mode\n");
  fprintf(stderr, "\t-imageDelay <value>: sets delay before drawing images\n");

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
  CheckDirSpace(workDir, "DEVISE_WORK", 1024 * 1024, 0);

  /* Get name of cache directory. */
  char *cacheDir = getenv("DEVISE_CACHE");
  if (!cacheDir) cacheDir = ".";
  _cacheDir = CopyString(cacheDir);
  CheckDirSpace(cacheDir, "DEVISE_CACHE", 1024 * 1024, 0);

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
  CheckDirSpace(tmpDir, "DEVISE_TMP", 1024 * 1024, 0);

  /* parse parameters */
  int i = 1;
  while (i < argc) {
    if (argv[i][0] == '-') {

      if (strcmp(&argv[i][1], "batch") == 0) {
	if (i >= argc - 1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_batchFile = CopyString(argv[i + 1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "session") == 0) {
	if (i >= argc-1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_sessionName = CopyString(argv[i + 1]);
	_restore = true;
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "playback") == 0) {
	if (i >= argc-1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_playbackFile = CopyString(argv[i+1]);
	_doPlayback = true;
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "journal") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	journalName = CopyString(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "buffer") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_bufferSize = atoi(argv[i+1]);
	if (_bufferSize <= 0) {
	  fprintf(stderr, "invalid buffer size %d\n", _bufferSize);
	  Usage(argv[0]);
	}
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "pagesize") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
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
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
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
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_prefetch = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "printViewStat") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_printViewStat = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "batchRecs") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_batchRecs = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "dispGraphics") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_dispGraphics = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "elimOverlap") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_elimOverlap = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "existing") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_existing = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "gdata") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_tdataQuery = (atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "gdatapages") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_gdataPages = atoi(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "convert") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_convertGData = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "iconify") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_iconify = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "printTDataAttr") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_printTDataAttr = !(atoi(argv[i+1]) == 0);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "simpleInterpreter") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
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
      }

      else if (strcmp(&argv[i][1], "tasvir") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_daliServer = CopyString(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "tasvirquit") == 0) {
	_daliQuit = true;
	MoveArg(argc,argv,i,1);
      }

      else if (strcmp(&argv[i][1], "screenWidth") == 0) {
	if (i >= argc - 1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_screenWidth = atoi(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "screenHeight") == 0) {
	if (i >= argc - 1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_screenHeight = atoi(argv[i+1]);
	MoveArg(argc,argv,i,2);
      }

      else if (strcmp(&argv[i][1], "imageDelay") == 0) {
	if (i >= argc -1) {
	  fprintf(stderr, "Value needed for argument %s\n", argv[i]);
	  Usage(argv[0]);
	}
	_imageDelay = atoi(argv[i+1]);
	// Make sure delay value is non-negative.
	_imageDelay = MAX(0, _imageDelay);
	MoveArg(argc,argv,i,2);
      }

      else {
        fprintf(stderr, "Unrecognized argument '%s'\n", argv[i]);
	Usage(argv[0]);
	i++;
      }
    }
    else {
      printf("Argument '%s' doesn't begin with '-'\n", argv[i]);
      Usage(argv[0]);
      i++;
    }
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
