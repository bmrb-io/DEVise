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
  Revision 1.2  1995/09/05 21:12:25  jussi
  Added/update CVS header.
*/

#ifndef BufPolicy_h
#define BufPolicy_h

class BufPolicy {
public:
  enum policy { FIFO, RND, LIFO, LRU, FOCAL };
};

#endif
