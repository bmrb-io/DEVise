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
  Declaration of GDataSock (send GData to socket) class.
 */

/*
  $Id$

  $Log$
 */

#ifndef _GDataSock_h_
#define _GDataSock_h_


#include <sys/types.h>

#include "DevStatus.h"

class ViewGraph;
class TDataMap;

class GDataSock {
public:
  struct Params {
    int portNum; // port number for socket
    char *file; // send to socket if non-NULL
    Boolean sendText; // vs. binary
    char separator; // for text only
  };

  GDataSock(Params &params);
  ~GDataSock();

  DevStatus GetStatus() { return _status; }

  DevStatus Send(ViewGraph *view, void **gdataArray, TDataMap *map,
		 int recCount);

private:
  static int _objectCount;

  DevStatus _status;
  Params _params;
  int _fd;
};

#endif /* _GDataSock_h_ */

/*============================================================================*/
