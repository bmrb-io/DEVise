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
  Revision 1.17  1997/11/24 23:14:28  weaver
  Changes for the new ColorManager.

  Revision 1.16  1997/07/22 19:44:28  wenger
  Removed extra dependencies that broke cslib link.

  Revision 1.15  1997/06/13 18:02:39  wenger
  Got client/server library to compile with latest changes.

  Revision 1.14  1997/06/04 15:50:27  wenger
  Printing windows to PostScript as pixmaps is now implemented, including
  doing so when printing the entire display.

  Revision 1.13  1997/05/21 22:09:55  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.12.6.1  1997/05/21 20:39:59  weaver
  Changes for new ColorManager

  Revision 1.12  1997/05/05 16:53:45  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.11  1997/03/25 17:58:54  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.10.4.1  1997/03/15 00:31:04  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.10  1996/12/03 23:29:10  wenger
  Fixed PostScript bounding box to closely surround the image (fixed
  bug 089).

  Revision 1.9  1996/11/26 15:44:06  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

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
#include "PSWindowRep.h"

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

#ifndef LIBCS
    /* Export display image to other graphics formats */
    virtual void ExportImage(DisplayExportFormat format, char *filename) {}
    virtual void ExportImageAndMap(DisplayExportFormat format,
      char *gifFilename, char *mapFilename, char *url, char *defaultUrl) {}
    virtual void ExportGIF(FILE *fp, int isView = 0) {}
    virtual void ExportView(DisplayExportFormat format, char *f) {}
    virtual void ExportToPS(DisplayExportFormat format, char *filename) {}
#endif

    virtual DevStatus OpenPrintFile(const char *filename);
    virtual DevStatus ClosePrintFile();
    virtual FILE *GetPrintFile() { return _printFile; }
#ifndef LIBCS
    virtual DevStatus ImportWindow(ViewWin *window,
      DisplayExportFormat format);
#endif
    virtual DevStatus ImportPSImage(char *filename, Rectangle *location = NULL);

    virtual void PrintPSHeader(char *title, const Rectangle &screenPrintRegion,
      Boolean maintainAspect);
    virtual void PrintPSTrailer();
    virtual void GetBoundingBox(Rectangle &boundingBox) {
      boundingBox = _boundingBox; }
    virtual void GetScreenPrintRegion(Rectangle &screenPrintRegion) {
      screenPrintRegion = _screenPrintRegion; }

    /* Set page geometry in inches. */
    virtual void SetUserPageGeom(Coord width, Coord height, Coord xMargin,
      Coord yMargin);

    /* Get page geometry in points. */
    virtual void GetPageGeom(Coord &width, Coord &height, Coord &xMargin,
      Coord &yMargin);

    virtual void SetTasvirServer(const char *server);

protected:
#ifndef LIBCS
    /* Register with the dispatcher */
    virtual void Register() {}
#endif

private:
    FILE *_printFile;
    PSWindowRepList _winList;

    Coord _userWidth, _userHeight;
    Coord _userXMargin, _userYMargin;

    Coord _outputWidth, _outputHeight;
    Coord _outputXMargin, _outputYMargin;

    Rectangle _boundingBox;	// x, y are lower left corner, y positive up
    Rectangle _screenPrintRegion; // x, y are upper left corner, y positive down
};

#endif
