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
  Revision 1.3  1995/12/02 21:07:33  jussi
  Substituted DeviseTime for Time and added copyright notice.

  Revision 1.2  1995/09/05 22:14:33  jussi
  Added CVS header.
*/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#ifdef IBM_POWER_STATION
#include <sys/select.h>
#endif
#include "Exit.h"
#include "Time.h"
#include "View.h"
#include "Command.h"
#include "Parse.h"
#include "BufMgr.h"
#include "QueryProc.h"
#include "View.h"
#include "TData.h"
#include "Control.h"
#include "Init.h"

/****************************************************
Constuctor, input from stdin
*****************************************************/

Command::Command(QueryProc *qp)
{
  _input = stdin;
  _queryProc = qp;
  Init();
}

/****************************************************
Constuctor
****************************************************/

Command::Command(char *fname, QueryProc *qp)
{
  _queryProc = qp;
  if (!(_input = fopen(fname, "r"))) {
    fprintf(stderr, "Command: can't open file %s\n", fname);
    Exit::DoExit(1);
  }
  Init();
}

/***********************************************************
Initialization
***********************************************************/

void Command::Init()
{
  /*
     _state = ReadInput;
  */
  _state = WaitCmd;
  Dispatcher::Current()->Register(this, 21);
}

/*******************************************************
timer is up
*********************************************************/

void Command::TimerWake(int)
{
  /* change state to read input */
  /*
     printf("wake \n");
  */
  _state = ReadInput;
}

/*********************************************************************
Create a new visual filter based on old filter and command.
index = index to start looking for new command.
Return 
	index = last index scanned.
	true if a new valid filter is constructed
	false = no filter is found.
***********************************************************************/

Boolean Command::UpdateVisualFilter(const VisualFilter oldFilter, 
				    VisualFilter &newFilter, int &index,
				    int argc, char **argv)
{
  if (argc-index == 0) {
    fprintf(stderr, "warning: command has no arg\n");
    return false;
  }

  newFilter.flag = VISUAL_LOC;
  double ratio;
  if (strcmp(argv[index], "move") == 0) {
    if (argc-index < 5) {
      fprintf(stderr, "move needs at args: x y width height\n");
      return false;
    }
    /* get the x, y, width, height */
    newFilter.xLow = atof(argv[++index]);
    newFilter.yLow = atof(argv[++index]);
    double width = atof(argv[++index]);
    double height = atof(argv[++index]);
    if (width <0 || height < 0) {
      fprintf(stderr, "Command: move width/height must be > 0\n");
      return false;
    }
    newFilter.xHigh = newFilter.xLow + width;
    newFilter.yHigh = newFilter.yLow + height;
  } else if (strcmp(argv[index], "movex")== 0) {
    if (argc-index < 1) {
      fprintf(stderr, "movex must have at least 1 arg\n");
      return false;
    }
    newFilter.xLow = atof(argv[++index]);
    newFilter.yLow = oldFilter.yLow;
    if (index < argc)
      newFilter.xHigh = atof(argv[++index]);
    else
      newFilter.xHigh = newFilter.xLow + 
	(oldFilter.xHigh-oldFilter.xLow);
    newFilter.yHigh = oldFilter.yHigh;
  } else if (strcmp(argv[index], "scrollx") == 0) {
    if (argc-index < 2) {
      fprintf(stderr, "scrollx must have at least 2 args\n");
      return false;
    }
    double width = oldFilter.xHigh - oldFilter.xLow;
    ratio = atof(argv[++index]);
    newFilter.xLow  = oldFilter.xLow + width * ratio;
    newFilter.yLow = oldFilter.yLow;
    newFilter.xHigh = newFilter.xLow + width;
    newFilter.yHigh = oldFilter.yHigh;
  } else if (strcmp(argv[index], "zoomx") == 0) {
    if (argc < 2) {
      fprintf(stderr, "zoomx must have at least 2 args\n");
      return false;
    }
    ratio = atof(argv[++index]);
    if (ratio <= 0.0) {
      fprintf(stderr, "zoom: zoom ratio %f must be > 0.0\n",ratio);
      return false;
    }
    double width = oldFilter.xHigh - oldFilter.xLow;
    double mid = (oldFilter.xLow + oldFilter.xHigh) / 2;
    newFilter.xLow = mid - width * ratio / 2.0;
    newFilter.yLow = oldFilter.yLow;
    newFilter.xHigh = newFilter.xLow + width * ratio;
    newFilter.yHigh = oldFilter.yHigh;
  } else {
    fprintf(stderr, "command %s not found\n",argv[index]);
    return false;
  }
  
  return true;
}

/**********************************************************
Process a line of command
***********************************************************/

void Command::ProcessLine(char *line)
{
  int argc;
  char **argv;
  
  Parse(line,argc,argv);
  
  if (argc == 0)
    /* skip blank line */
    return;

  if (argv[0][0] == '#')
    /* skip comment line */
    return;
  
  if (strcmp(argv[0], "stat")== 0) {
    /* print statistics */
    printf("%f (%f)seconds have passed\n", Timer::Now() / 1000.0,
	   DeviseTime::Now() / 1000.0);
    /*
       _mgr->PrintStat();
    */
    _queryProc->PrintStat();
    /* Print view statistics */
    if (Init::PrintViewStat()) {
      printf("View Stat\n");
      for(int index = View::InitViewIterator();View::MoreView(index);) {
	View *v = View::NextView(index);
	printf("View %s\n", v->GetName());
	v->PrintStat();
      }
      View::DoneViewIterator(index);
    }
    return;
  }
  if (strcmp(argv[0], "quit")== 0) {
    /* quit program */
    ControlPanel::Instance()->DoQuit();
    /*
       Dispatcher::Current()->Cleanup();
    */
    Exit::DoExit(0);
  }
  if (strcmp(argv[0], "wait")== 0) {
    /* process wait command */
    if (argc < 2) {
      fprintf(stderr, "wait needs time arg\n");
      return;
    }
    double interval = atoi(argv[1])*1000.0; /* # of milliseconds to wait */
    if (interval < 0.0) {
      fprintf(stderr, "wait interval must be > 0\n");
      return;
    }
    _state = WaitTime;
    Timer::Queue((long)interval, this, 0);
    /*
       printf("queue timer %f\n", interval);
    */
    return;
  }
  
  /* process scroll/zoom commands */
	
  /* now parse buf */
  VisualFilter oldFilter;	/* old filter for view*/
  VisualFilter newFilter;		/* new filter for view*/
  /*
     cmdFilter.flag = VISUAL_LOC;
     hintFilter.flag = VISUAL_LOC;
  */
  View *view = View::FindViewByName(argv[0]);
  if (view == NULL) {
    printf("no view %s\n",argv[0]);
    return;
  }
  view->GetVisualFilter(oldFilter);
  
  int index = 1;
  if (!UpdateVisualFilter(oldFilter, newFilter, index, argc, argv))
    /* no command found */
    return;
  view->SetVisualFilter(newFilter);
  
  _state = WaitCmd;
}

/********************************************************
Process Command 
********************************************************/

void Command::Run()
{
#define BUFSIZE 256
  char buf[BUFSIZE];
  struct timeval timeout;
#ifdef IBM_POWER_STATION
  fd_set readfds;
#else
  int readfds;
#endif
  int fd, ret;

  /*
     PrintState();
  */

  switch(_state) {
  case WaitTime:
    /* doing time wait .
       Need to wait for TimerWake() to be called;*/
    break;
  case WaitCmd:
    /* waiting for a command to finish */
    if (_queryProc->Idle())
      _state = ReadInput;
    break;
  case ReadInput:
    /* read next input */
    fd = fileno(_input);
#ifdef IBM_POWER_STATION
    FD_SET(fd, &readfds);
#else
    readfds = 1 << fd;
#endif
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
#if defined(IBM_POWER_STATION) || defined(HPUX)
    ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
#else
    ret = select(fd + 1, (fd_set *)&readfds, NULL, NULL, &timeout);
#endif
    if (ret > 0) {
      /* ready to read */
      if (fgets(buf,BUFSIZE,_input)==NULL) {
	/* no more to read */
	_state = Eof;
      } else {
	/* get rid of '\n' from input */
	int len = strlen(buf);
	if (len > 0) {
	  if (buf[len-1] == '\n')
	    buf[len-1] = '\0';
	  ProcessLine(buf);
	}
      }
    }
    break;
    
  case Eof:
    /* nothing for us to do anymore */
    break;
  }
}

void Command::PrintState()
{
  printf("Command state: ");
  switch (_state) {
  case WaitTime:
    printf("WaitTime\n");
    break;
  case WaitCmd:
    printf("WaitCmd\n");
    break;
  case ReadInput:
    printf("ReadInput\n");
    break;   
  case Eof:
    printf("Eof\n");
    break;
  default:
    printf("unknown\n");
  }
}
