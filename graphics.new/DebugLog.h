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
  Revision 1.6  1999/10/05 17:55:47  wenger
  Added debug log level.

  Revision 1.5  1999/09/29 00:56:01  wenger
  Improved handing of session files in JavaScreen support: better error
  checking, devised won't go up from 'base' session directory;
  more flexible debug logging method now available.

  Revision 1.4  1999/07/16 21:36:06  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

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
  enum Level {LevelFatalError = 0, LevelError, LevelWarning, LevelInfo0,
      LevelInfo1, LevelCommand, LevelInfo2, LevelDispatcher};

  void Message(Level level, const char *msg1, const char *msg2 = NULL,
      const char *msg3 = NULL);
  void Message(Level level, const char *msg1, int argc,
      const char * const *argv, const char *msg2 = "\n");

  void SetLogLevel(Level logLevel);

  static DebugLog *DefaultLog();
  static void DeleteAll();

protected:
  // Note: these should be moved to being public once we implement a
  // list of all DebugLog objects.
  DebugLog(Level logLevel, const char *filename = "devise_debug_log",
      long maxSize = (2 * 1024 * 1024));
  ~DebugLog();

private:
  static const char *GetTimeString();

  int _logNum;
  int _fd;
  long _maxSize;
  Level _logLevel;
  char *_filename;
};

#endif /* _DebugLog_h_ */

/*============================================================================*/
