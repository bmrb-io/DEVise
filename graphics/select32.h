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

#ifndef _SELECT32_H_
#define _SELECT32_H_

#if !defined(FD_SET)

typedef int fd_set;
#define FD_SETSIZE 32
inline void FD_SET(int fd, fd_set* fdset) { (*fdset) |= 1 << fd; }
inline void FD_CLR(int fd, fd_set* fdset) { (*fdset) &= ~(1 << fd); }
inline void FD_ZERO(fd_set* fdset) { *fdset = 0; }
inline bool FD_ISSET(int fd, fd_set* fdset) { return (*fdset) & (1 << fd); }

#endif
#endif // _SELECT32_H_
