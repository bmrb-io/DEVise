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
  Revision 1.3  1997/03/28 16:09:30  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 21:13:50  jussi
  Added/update CVS header.
*/

#include "Util.h"

#define ITERATIONS 10000000

double
getTime()
{
  struct timeval tv;
  int timeResult = gettimeofday(&tv, NULL);
  if (timeResult < 0) {
    perror("gettimeofday");
    return -1.0;
  } else {
    return tv.tv_sec + tv.tv_usec * 1.0e-6;
  }
}

main(){
  char *number = "32.54";
  float f;
  int i;	

  double startTime = getTime();
  for (i=0; i < ITERATIONS; i++){
    f = UtilAtof(number);
  }
  double stopTime = getTime();
  printf("%d iterations of UtilAtof() took %f seconds\n", ITERATIONS,
    stopTime - startTime);


  startTime = getTime();
  char buf[128];
  for (i=0; i < ITERATIONS; i++){
    strcpy(buf, number);
  }
  stopTime = getTime();
  printf("%d iterations of strcpy() took %f seconds\n", ITERATIONS,
    stopTime - startTime);


  startTime = getTime();
  int length = strlen(number) + 1;
  for (i=0; i < ITERATIONS; i++){
    memcpy(buf, number, length);
  }
  stopTime = getTime();
  printf("%d iterations of memcpy() took %f seconds\n", ITERATIONS,
    stopTime - startTime);


  startTime = getTime();
  for (i=0; i < ITERATIONS; i++){
    f = atof(number);
  }
  stopTime = getTime();
  printf("%d iterations of atof() took %f seconds\n", ITERATIONS,
    stopTime - startTime);


  startTime = getTime();
  for (i=0; i < ITERATIONS; i++){
    sscanf(number, "%g", &f);
  }
  stopTime = getTime();
  printf("%d iterations of sscanf() took %f seconds\n", ITERATIONS,
    stopTime - startTime);
}
