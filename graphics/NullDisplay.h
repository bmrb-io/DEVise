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
  $Id$

  $Log$
  Revision 1.2.10.1  1997/05/21 20:39:58  weaver
  Changes for new ColorManager

  Revision 1.2  1996/11/13 16:56:09  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.1  1996/07/10 16:40:39  jussi
  Initial revision.
*/

#ifndef NullDisplay_h
#define NullDisplay_h

#include "Display.h"
#include "WindowRep.h"

class NullDisplay: public DeviseDisplay {
public:
    NullDisplay(char *name = 0);

    /* get file descriptor for connection */
    virtual int fd() { return -1; }

    virtual int NumPlanes() { return 8; }

    virtual void Dimensions(Coord &width, Coord &height) {
      width = 100;
      height = 100;
      /* do something */
    }

    /* Create a new window representative. Dimensions are in normalized
       coord from (0,0) to (1,1) */
    virtual WindowRep *CreateWindowRep(char *name, Coord x, Coord y,
				       Coord width, Coord height, 
				       WindowRep *parentRep = NULL,
                                       Coord min_width = 0.05,
				       Coord min_height = 0.05,
                                       Boolean relative = true,
				       Boolean winBoundary = true);

    /* Destroy a window rep*/
    virtual void DestroyWindowRep(WindowRep *win);

    /* Do internal event processing, but do not block*/
    virtual void InternalProcessing() {}

    /* Flush buffered window operations to screen */
    virtual void Flush() {}

protected:
#ifndef LIBCS
    /* Register with the dispatcher */
    virtual void Register() {}
#endif
};

#endif
