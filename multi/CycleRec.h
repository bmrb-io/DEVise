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
 */

/* CycleRec.h */
#ifndef CycleRec_h
#define CycleRec_h

struct CycleRec {
	int time;
	int last_time;
	int length;
	int occ;
	int working;
	int cpu_time;
	int suspended;
	double eff;
};

#endif
