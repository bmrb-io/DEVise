/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DevFont (Devise font) class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1999/04/20 14:13:31  wenger
  Improved debug output.

  Revision 1.1  1996/12/20 16:50:14  wenger
  Fonts for view label, x axis, and y axis can now be changed.

 */

#define _DevFont_c_

//#define DEBUG

#include <stdio.h>

#include "DevFont.h"
#include "WindowRep.h"

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

/*------------------------------------------------------------------------------
 * function: DevFont::DevFont
 * DevFont constructor.
 */
DevFont::DevFont()
{
#if defined(DEBUG)
  printf("DevFont(0x%p)::DevFont()\n", this);
#endif

  _family = 0;
  _pointSize = 12.0;
  _bold = false;
  _italic = false;
}

/*------------------------------------------------------------------------------
 * function: DevFont::~DevFont
 * DevFont destructor.
 */
DevFont::~DevFont()
{
#if defined(DEBUG)
  printf("DevFont(0x%p)::~DevFont()\n", this);
#endif
}

/*------------------------------------------------------------------------------
 * function: DevFont::Set
 * Set the font values.
 */
void
DevFont::Set(int family, float pointSize, Boolean bold, Boolean italic)
{
#if defined(DEBUG)
  printf("DevFont(0x%p)::Set(%d, %f, %d, %d)\n", this, family, pointSize,
      bold, italic);
#endif

  _family = family;
  _pointSize = pointSize;
  _bold = bold;
  _italic = italic;
}

/*------------------------------------------------------------------------------
 * function: DevFont::Get
 * Get the font values.
 */
void
DevFont::Get(int &family, float &pointSize, Boolean &bold, Boolean &italic)
{
#if defined(DEBUG)
  printf("DevFont(0x%p)::Get()\n", this);
#endif

  family = _family;
  pointSize = _pointSize;
  bold = _bold;
  italic = _italic;
}

/*------------------------------------------------------------------------------
 * function: DevFont::SetWinFont
 * Set up the font in the given WindowRep to correspond to the state of this
 * DevFont.
 */
void
DevFont::SetWinFont(WindowRep *win)
{
#if defined(DEBUG)
  printf("DevFont(0x%p)::SetWinFont()\n", this);
#endif

  static char *familyTable[] = { "Courier", "Times", "Helvetica",
    "Lucida", "New Century Schoolbook" };
  static const int familyCount = sizeof(familyTable) / sizeof(familyTable[0]);

  char *familyName = familyTable[_family % familyCount];
  const char *weight = _bold ? "Bold" : "Medium";
  const char *slant = _italic ? "i" : "r";
  const char *width = "Normal";

  win->SetFont(familyName, weight, slant, width, _pointSize);
}

/*============================================================================*/
