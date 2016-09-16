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
  Revision 1.2  1995/09/05 22:14:08  jussi
  Added CVS header.
*/

/* specification of buffer manager hints */

#ifndef BufHint_h
#define BufHint_h

/* Current == Move page to current list, making page reclaimable.
	Future == Move page into future list, making page unreclaimable.
	NoChange == Page stays at whichever list it's in.
	Toss == toss the page from memory. Place page at beginning of free list.
*/
enum BufHint { Current, Future, NoChange, Toss};

#endif
