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

/* MultiRec.h */
#ifndef MultiRec_h
#define MultiRec_h

struct MultiRec {
	int node; /* node number */
	int begin; /* beginning time */
	int end;	/* end time */
	int job;
	int cycles;
};

#endif
