/*
  $Id$

  $Log$*/

/*
   Time.h: keeps track of current time in terms of # of milliseconds
   since the beginning of the program
*/

#ifndef Time_h
#define Time_h
#if defined(IBM_POWER_STATION) || defined(ALPHA) || defined(SUN)
#include <sys/time.h>
#elif defined(__GNUC__)
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "Journal.h"

class Time {
public:
	static void Init();

	static long Now();

private:
static struct timeval _beginning; /* time of day at the beginning */
};

#endif
