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
  Declaration of DebugLog class (creates a limited-size debug log file
  by writing to the file in a circular fashion).
 */

/*
  $Id$

  $Log$
  Revision 1.3  1999/06/25 15:58:21  wenger
  Improved debug logging, especially for JavaScreen support: JavaScreenCmd.C
  now uses DebugLog facility instead of printf; dispatcher logging is turned
  on by default, and commands and queries are logged; added -debugLog command
  line flag to turn logging on and off.

  Revision 1.2  1998/03/08 01:10:54  wenger
  Merged cleanup_1_4_7_br_9 through cleanup_1_4_7_br_10 (fix to idle
  CPU usage bug (308)).

  Revision 1.1.2.1  1998/03/05 16:10:53  wenger
  Added DebugLog class for use in extensive logging of debug information.

 */

#ifndef _DebugLog_h_
#define _DebugLog_h_

class DebugLog {
public:
  void Message(const char *msg);
  void Message(const char *msg1, int argc, const char * const *argv,
      const char *msg2 = "\n");

  static DebugLog *DefaultLog();
  static void DeleteAll();

protected:
  // Note: these should be moved to being public once we implement a
  // list of all DebugLog objects.
  DebugLog(const char *filename = "devise_debug_log",
      long maxSize = (2 * 1024 * 1024));
  ~DebugLog();

private:
  static const char *GetTimeString();

  int _logNum;
  int _fd;
  long _maxSize;
};

#endif /* _DebugLog_h_ */

/*============================================================================*/
