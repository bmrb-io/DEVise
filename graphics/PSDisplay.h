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
  Revision 1.8  1996/11/13 16:56:09  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.7  1996/10/28 15:55:41  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.6  1996/09/26 20:25:00  jussi
  Added missing methods for LIBCS environment.

  Revision 1.5  1996/09/19 20:11:51  wenger
  More PostScript output code (still disabled); some code for drawing
  view borders (disabled).

  Revision 1.4  1996/09/18 20:13:30  guangshu
  Added function ExportView and modified function ExportGIF.

  Revision 1.3  1996/09/16 21:30:18  wenger
  Added ExportImageAndMap() member function (needed for compatibility with
  Guangshun's latest update).

  Revision 1.2  1996/09/10 20:07:09  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

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
    //TEMPTEMP -- what about destructor?
    // destructor should close _printFile if it's open

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
				       GlobalColor fgnd = ForegroundColor,
				       GlobalColor bgnd = BackgroundColor,
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
    virtual void ExportImageAndMap(DisplayExportFormat format,
      char *gifFilename, char *mapFilename, char *url, char *defaultUrl) {}
    virtual void ExportGIF(FILE *fp, int isView = 0) {}
    virtual void ExportView(DisplayExportFormat format, char *f) {}


    virtual DevStatus OpenPrintFile(char *filename);
    virtual DevStatus ClosePrintFile();
    virtual FILE *GetPrintFile() { return _printFile; }

    virtual void PrintPSHeader(char *title);
    virtual void PrintPSTrailer();

    /* Set page geometry in inches. */
    virtual void SetUserPageGeom(Coord width, Coord height, Coord xMargin,
      Coord yMargin);

    /* Get page geometry in points. */
    virtual void GetPageGeom(Coord &width, Coord &height, Coord &xMargin,
      Coord &yMargin);

#ifdef LIBCS
    /* Translate RGB colors to pixel values and back */
    virtual LocalColor FindLocalColor(float r, float g, float b) {
        return 0;
    }
    virtual void FindLocalColor(GlobalColor c, float &r, float &g, float &b) {
        r = g = b = 0;
    }
#endif

protected:
#ifndef LIBCS
    /* Register with the dispatcher */
    virtual void Register() {}
#endif

    virtual void AllocColor(char *name, GlobalColor globalColor, RgbVals &rgb);
    virtual void AllocColor(RgbVals &rgb, GlobalColor globalColor);

private:
    FILE *_printFile;

    Coord _userWidth, _userHeight;
    Coord _userXMargin, _userYMargin;

    Coord _outputWidth, _outputHeight;
    Coord _outputXMargin, _outputYMargin;
};

#endif
