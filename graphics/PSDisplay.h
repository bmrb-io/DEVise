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
  Revision 1.1  1996/07/10 16:23:01  jussi
  Initial revision.
*/

#ifndef PSDisplay_h
#define PSDisplay_h

#include "Display.h"
#include "WindowRep.h"

class PSDisplay: public DeviseDisplay {
public:
    PSDisplay(char *name = 0);

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
				       Color fgnd = ForegroundColor,
				       Color bgnd = BackgroundColor,
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

    /* Export display image to other graphics formats */
    virtual void ExportImage(DisplayExportFormat format, char *filename) {}
    virtual void ExportGIF(FILE *fp) {}

protected:
#ifndef LIBCS
    /* Register with the dispatcher */
    virtual void Register() {}
#endif

    virtual void AllocColor(char *name, Color globalColor);
    virtual void AllocColor(float r, float g, float b, Color globalColor);
};

#endif
