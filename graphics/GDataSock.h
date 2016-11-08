/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.2  1998/12/10 21:53:17  wenger
  Devised now sends GIFs to JavaScreen on a per-view rather than per-window
  basis; GIF "dirty" flags are now also referenced by view rather than by
  window.

  Revision 1.1  1997/11/18 23:26:36  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

 */

#ifndef _GDataSock_h_
#define _GDataSock_h_


#include <sys/types.h>

#include "DevStatus.h"

class ViewGraph;
class TDataMap;
class AttrVals;
class AttrInfo;
class StringStorage;
class RecordVals;

class GDataSock {
public:
  struct Params {
    int portNum; // port number for socket
    char *file; // send to socket if non-NULL
    Boolean sendText; // vs. binary
    char separator; // for text only
    Boolean rgbColor; // convert color to RGB value
  };

  GDataSock(Params &params);
  ~GDataSock();

  DevStatus GetStatus() { return _status; }

  DevStatus Send(ViewGraph *view, void **gdataArray, TDataMap *tdMap,
		 int recCount);

private:
  DevStatus GetRecordVals(AttrInfo **attrInfos, TDataMap *tdMap,
      char *gdata, StringStorage *stringTable, RecordVals &vals);
  DevStatus PrintRecordVals(const RecordVals &vals);
  DevStatus GetShapeAttr(int attrNum, const AttrInfo *attrInfo,
      TDataMap *tdMap, const char *gdata, StringStorage *stringTable,
      AttrVals &attrVal);
  DevStatus PrintShapeAttr(const AttrVals &attrVal, char buf[], int &offset,
      int bufSize);

  static int _objectCount;
  static int _sockOutputCount;

  DevStatus _status;
  Params _params;
  int _fd;
};

#endif /* _GDataSock_h_ */

/*============================================================================*/
