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
 */

#define _DualWindowRep_c_

//#define DEBUG

#include <stdio.h>

#include "DualWindowRep.h"
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
  DO_DEBUG(printf("DualWindowRep::DualWindowRep()\n"));

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
  DO_DEBUG(printf("DualWindowRep::~DualWindowRep()\n"));
}

/*------------------------------------------------------------------------------
 * function: DualWindowRep::SetScreenWinRep
 * Sets the WindowRep for screen output.
 */
void
DualWindowRep::SetScreenWinRep(WindowRep *screenWinRep)
{
  DO_DEBUG(printf("DualWindowRep::SetScreenWinRep()\n"));

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
  DO_DEBUG(printf("DualWindowRep::SetScreenWinRep()\n"));

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
  DO_DEBUG(printf("DualWindowRep::SetScreenOutput()\n"));

  _windowRep = _screenWinRep;
}

/*------------------------------------------------------------------------------
 * function: DualWindowRep::SetFileOutput
 * Sets output to file.  Copies the transforms and clip masks from the
 * _screenWinRep to the _fileWinRep so that the _fileWinRep is set up
 * correctly.
 */
void
DualWindowRep::SetFileOutput()
{
  DO_DEBUG(printf("DualWindowRep::SetFileOutput()\n"));

  int count;

// Copy 2D transforms.
  _fileWinRep->ClearTransformStack();
  for (count = 0; count <= _screenWinRep->_current; count++)
  {
    _fileWinRep->_transforms[count].Copy(_screenWinRep->_transforms[count]);
  }
  _fileWinRep->_current = _screenWinRep->_current;

// Copy 3D transforms.
  _fileWinRep->ClearTransformStack3();
  for (count = 0; count <= _screenWinRep->_current3; count++)
  {
    _fileWinRep->_transforms3[count].Copy(_screenWinRep->_transforms3[count]);
  }
  _fileWinRep->_current3 = _screenWinRep->_current3;

// Copy clip masks.
  ClipRect clip;
  for (count = 0; count <= _screenWinRep->_clipCurrent; count++)
  {
    clip = _screenWinRep->_clippings[count];
    _fileWinRep->PushClip(clip.x, clip.y, clip.width, clip.height);
  }

// Set to file output.
  _windowRep = _fileWinRep;
}

/*============================================================================*/
