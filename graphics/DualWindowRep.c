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
  Implementation of the DualWindowRep class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1996/10/28 15:55:36  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.1  1996/10/18 20:34:03  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

 */

#define _DualWindowRep_c_

//#define DEBUG

#include <stdio.h>

#include "DualWindowRep.h"
#include "XWindowRep.h"
#include "PSWindowRep.h"
#include "Util.h"


/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DualWindowRep::DualWindowRep
 * Constructor.
 */
DualWindowRep::DualWindowRep()
{
  DO_DEBUG(printf("DualWindowRep(0x%p)::DualWindowRep()\n", this));

  _windowRep = NULL;
  _screenWinRep = NULL;
  _fileWinRep = NULL;
}

/*------------------------------------------------------------------------------
 * function: DualWindowRep::~DualWindowRep
 * Destructor.
 */
DualWindowRep::~DualWindowRep()
{
  DO_DEBUG(printf("DualWindowRep(0x%p)::~DualWindowRep()\n", this));
}

/*------------------------------------------------------------------------------
 * function: DualWindowRep::SetScreenWinRep
 * Sets the WindowRep for screen output.
 */
void
DualWindowRep::SetScreenWinRep(WindowRep *screenWinRep)
{
  DO_DEBUG(printf("DualWindowRep(0x%p)::SetScreenWinRep(0x%p)\n", this,
    screenWinRep));

  if ((_windowRep != NULL) && (_windowRep == _screenWinRep))
  {
    _windowRep = screenWinRep;
  }

  _screenWinRep = screenWinRep;
}

/*------------------------------------------------------------------------------
 * function: DualWindowRep::SetFileWinRep
 * Sets the WindowRep for file output.
 */
void
DualWindowRep::SetFileWinRep(WindowRep *fileWinRep)
{
  DO_DEBUG(printf("DualWindowRep(0x%p)::SetFileWinRep(0x%p)\n", this,
    fileWinRep));

  if ((_windowRep != NULL) && (_windowRep == _fileWinRep))
  {
    DOASSERT(false, "Can't change file WindowRep during output");
  }

  _fileWinRep = fileWinRep;
}

/*------------------------------------------------------------------------------
 * function: DualWindowRep::SetScreenOutput
 * Sets output to the screen.
 */
void
DualWindowRep::SetScreenOutput()
{
  DO_DEBUG(printf("DualWindowRep(0x%p)::SetScreenOutput()\n", this));

  _windowRep = _screenWinRep;
}

/*------------------------------------------------------------------------------
 * function: DualWindowRep::SetFileOutput
 * Sets output to file.  Copies the transforms and clip masks from the
 * _screenWinRep to the _fileWinRep so that the _fileWinRep is set up
 * correctly.
 */
void
DualWindowRep::SetFileOutput(const Rectangle &viewGeom,
  const Rectangle &parentGeom)
{
  DO_DEBUG(printf("DualWindowRep(0x%p)::SetFileOutput()\n", this));

  /* Set up the "pixel" to point transform. This MUST be done before
   * the WindowRep state is copied so that the "pixel" to point transform
   * is in place for the PushClips. */
  ((PSWindowRep *) _fileWinRep)->SetPPTrans(viewGeom, parentGeom);

  /* Copy state from screen WindowRep. */
  _fileWinRep->CopyState(_screenWinRep);

  /* Set to file output. */
  _windowRep = _fileWinRep;
}

/*============================================================================*/
