/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2.10.1  1997/05/21 20:39:57  weaver
  Changes for new ColorManager

  Revision 1.2  1996/11/13 16:56:08  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.1  1996/07/10 16:40:38  jussi
  Initial revision.
*/

#include "NullDisplay.h"
#ifndef LIBCS
#include "Control.h"
#include "Journal.h"
#include "Init.h"
#endif

//#define DEBUG

/*******************************************************************
Open a new display
********************************************************************/

NullDisplay::NullDisplay(char *name)
{
  /* do something */
}

/*************************************************************
Create a new window 
***************************************************************/

WindowRep *NullDisplay::CreateWindowRep(char *name, Coord x, Coord y,
                                        Coord width, Coord height, 
                                        WindowRep *parentRep,
                                        Coord min_width, Coord min_height,
                                        Boolean relative, Boolean winBoundary)
{
  /* do something */
  return 0;
}

/**************************************************************
Destroy a window
**************************************************************/

void NullDisplay::DestroyWindowRep(WindowRep *win)
{
  delete win;
}
