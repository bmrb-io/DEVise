/* --------------------------------------------------------------- */
/* -- Copyright (c) 1994, 1995 Computer Sciences Department,    -- */
/* -- University of Wisconsin-Madison, subject to the terms     -- */
/* -- and conditions given in the file COPYRIGHT.  All Rights   -- */
/* -- Reserved.                                                 -- */
/* --------------------------------------------------------------- */

/*
  $Id$

  $Log$
 */

#ifndef __STATS_H__
#define __STATS_H__
/*
 *   $RCSfile$  
 *   $Revision$  
 *   $Date$      
 */ 
/*
 * stats.h
 *
 * Class definition for the stats class.
 * Member functions are defined in stats.c
 */
#ifndef  _stats
#define  _stats

#ifdef __GNUG__
#pragma interface
#endif

#include <stream.h>
#include <iostream.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/procfs.h>

/*
 * unix_stats objects are meant to be used as follows:
 *
 *        s.start();
 *        section of code for which stats are being gathered
 *        s.Stop();
 *        examine gathered unix_stats
 */

#ifdef __GNUC__
#pragma interface
#endif


/*
 * Stats for Unix processes.
 */
class unix_stats {
    struct timeval	time1;		/* gettimeofday() buffer */
    struct timeval	time2;		/* gettimeofday() buffer */
    struct prusage	usage1;		/* getrusage() buffer */
    struct prusage	usage2;		/* getrusage() buffer */
	int		iterations;

public:
	unix_stats(){};

    void   start();			/* start gathering stats  */
	// you can start and then stop multiple times-- don't need to continue
    void   stop(int iter=1);			/* stop gathering stats  */
    int    clocktime() const;		/* elapsed real time in micro-seconds */
    int    usertime() const;		/* elapsed user time in micro-seconds */
    int    systime() const;	/* elapsed system time in micro-seconds */
    /* variants */
    int    s_clocktime() const;		/* diff of seconds only */
    int    s_usertime() const;		
    int    s_systime() const;	
    int    us_clocktime() const;	/* diff of microseconds only */
    int    us_usertime() const;		
    int    us_systime() const;	

    int    page_reclaims() const;	/* page reclaims */
    int    page_faults() const;	/* page faults */
    int    swaps() const;			/* swaps */
    int    inblock() const;		/* page-ins */
    int    oublock() const;		/* page-outs */
    int    syscalls() const;		/* system calls */
    int    iochars() const;		/* characters read and written */
    int    vcsw() const;			/* voluntary context swtch */
    int    invcsw() const;			/* involuntary context swtch */
    int    msgsent() const;			/* socket messages sent */
    int    msgrecv() const;			/* socket messages recvd */
    int    signals() const;			/* signals dispatched */

    friend ostream& operator<<(ostream&, const unix_stats &s);
};

float compute_time(struct timeval *start_time, struct timeval *end_time);

#endif /*_stats*/
#endif 
