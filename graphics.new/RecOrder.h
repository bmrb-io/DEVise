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
  Revision 1.2  1995/09/05 22:15:31  jussi
  Added CVS header.
*/

#ifndef RecOrder_h
#define RecOrder_h

/* order in which to return records */
enum RecordOrder { Randomize,   /* randomize the records if possible */
		   RecIdOrder,  /* arrange according to recId order */
		   AnyOrder     /* any order is acceptable */
};

#endif
