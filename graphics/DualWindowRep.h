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
  Declaration of DualWindowRep class (used to hold the WindowReps
  corresponding to screen and file output for a given window, and
  to switch between them).
 */

/*
  $Id$

  $Log$
  Revision 1.1  1996/10/18 20:34:04  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

 */

#ifndef _DualWindowRep_h_
#define _DualWindowRep_h_


#include <sys/types.h>

#include "WindowRep.h"


class DualWindowRep
{
public:
  DualWindowRep();
  ~DualWindowRep();

  WindowRep *GetWindowRep() { return _windowRep; }

  void SetScreenWinRep(WindowRep *screenWinRep);
  void SetFileWinRep(WindowRep *fileWinRep);

  WindowRep *GetScreenWinRep() { return _screenWinRep; }
  WindowRep *GetFileWinRep() { return _fileWinRep; }

  void SetScreenOutput();
  void SetFileOutput(const Rectangle &viewGeom, const Rectangle &parentGeom);

  Boolean IsScreenOutput() { return _windowRep == _screenWinRep; }
  Boolean IsFileOutput() { return _windowRep == _fileWinRep; }

private:
  WindowRep *_windowRep;/* current WindowRep */
  WindowRep *_screenWinRep; /* WindowRep for output to screen */
  WindowRep *_fileWinRep; /* WindowRep for output to file */

};

#endif /* _DualWindowRep_h_ */

/*============================================================================*/
