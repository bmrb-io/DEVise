/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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

#ifndef GRAPHICSDEBUG_H
#define GRAPHICSDEBUG_H
#ifdef DEBUG
#define DPRINTF printf("%s:%d ",__FILE__,__LINE__),printf
#define DEBUGE(a) printf("%s:%d %s\n",__FILE__,__LINE__,#a),a
#else
#define DPRINTF void
#define DEBUGE(a) a
#endif
#endif
