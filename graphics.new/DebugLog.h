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
  Declaration of DebugLog class (creates a limited-size debug log file
  by writing to the file in a circular fashion).
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  1998/03/05 16:10:53  wenger
  Added DebugLog class for use in extensive logging of debug information.

 */

#ifndef _DebugLog_h_
#define _DebugLog_h_

class DebugLog {
public:
  void Message(char *msg);

  static DebugLog *DefaultLog();
  static void DeleteAll();

protected:
  // Note: these should be moved to being public once we implement a
  // list of all DebugLog objects.
  DebugLog(char *filename = "devise_debug_log", long maxSize = 1048576L);
  ~DebugLog();

private:
  FILE *_stream;
  long _maxSize;
};

#endif /* _DebugLog_h_ */

/*============================================================================*/
