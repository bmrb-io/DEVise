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
  Revision 1.3  1996/01/12 15:38:54  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.2  1995/09/05 22:16:32  jussi
  Added CVS header.
*/

/* print statistics on testAms1 runs.

   usage: rstat 
	input: standard input.
	output: standard output.
	flag: -s n : skip every nth line, including 1st. that is,
		skip 1st, 1+n-1, 1+2n-1, etc.
	file format:
		1st column == column width of visual filter
		2nd column == time to complete operation.
	output:
		statistics for each visual filter.
*/

#include <stdio.h>
#include <stdlib.h>
#ifndef SGI
#include <math.h>
#endif


#include "Parse.h"

/* structure used to group records by visual filter width */

struct GroupRec {
  double width;    /* width of visual filter */
  int numOccur;   /* # of occurrences with this width */
  double totalTime; /*total time taken for all filters with this width(secs)*/
  double sumXiSquare; /* = sum(time of each occurrence^2), in seconds */
};

const int MaxGroupRecs = 512;
int _numGroups = 0;	/* # of groups found so far */
GroupRec groups[MaxGroupRecs];

/****************************************************
Init group records
****************************************************/
void InitGroupRecs()
{
  _numGroups = 0;
}

/****************************************************
  Insert a record into a group.
  delta = fudge factor when deciding where to put a group.
  That is, all width within += delta are placed into the same group.
  time is in seconds.
*****************************************************/

void InsertGroup(double width, double time, double delta = 10.0)
{
  int index;
  int found = 0;
  for (index=0; index < _numGroups; index++){
    if (fabs(groups[index].width-width)<= delta){
      found = 1;
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
  groups[index].totalTime += time;
  groups[index].sumXiSquare += time*time;
}

/*************************************************************************
Print the group
**************************************************************************/

void PrintGroup()
{
  printf("There are %d groups\n", _numGroups);
  double avg;
  int i;
  if (_numGroups > 0){
    printf("num\twidth\t\t\tnum\tavg\tstd\n");
    double num = .5;
    double increment = .5;
    for (i=0; i < _numGroups; i++){
      avg = groups[i].totalTime/groups[i].numOccur;
      printf("%f\t%f\t\t%d\t%f\t%f\n", num,
	     groups[i].width,groups[i].numOccur, avg, 
	     groups[i].numOccur <= 1? 0.0:
	     sqrt((groups[i].sumXiSquare*groups[i].numOccur - 
		   groups[i].totalTime*groups[i].totalTime)/
		  (groups[i].numOccur*(groups[i].numOccur-1))));
      num += increment;
    }
  }
}

Usage()
{
  fprintf(stderr,"Usage: rstat [-s n]\n");
  exit(2);
}

main(int argc, char **argv)
{
  char *file = NULL;
  char buf[512];
  int skip = 0;	/* # of lines to skip */
  
  int i;
  for (i=1; i < argc; ){
    if (argv[i][0]== '-' ){
      if ( argv[i][1] == 's'){
	if (i >= argc-1)
	  Usage();
	skip = atoi(argv[i+1]);
	i += 2;
      }
      else Usage();
    }
    else Usage();
    
  }
  
  InitGroupRecs();
  
  int pargc;
  char **pargv;
  int line = 0;
  while (gets(buf) != NULL){
    if (!(skip >0 && (line % skip)==0)){
      /* no need to skip this line */
      Parse(buf, pargc, pargv);
      if (pargc < 2){
	printf("expecting 2 or more columns, but got %d\n", pargc);
	exit(1);
			}
      InsertGroup(atof(pargv[0]), atof(pargv[1]));
    }
    line++;
  }
  PrintGroup();
}
