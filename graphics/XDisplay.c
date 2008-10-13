/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2008
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
  Revision 1.93  2008/06/11 19:16:56  wenger
  Fixed a problem that sometimes caused DEVise to free the same FontStruct
  twice; more flexible generation of font names (horizontal and vertical
  resolution are no longer specified); added -allowCore command-line flag
  that overrides limits on core dump size, if possible; we now print the
  core dump size limit at startup.

  Revision 1.92  2007/12/26 21:28:12  wenger
  Added the "-defaultFont" command-line argument to allow DEVise to
  at least run when we have a very limited selection of fonts (as
  is now the case after the RHEL5 "upgrade").

  Revision 1.91  2005/12/06 20:03:13  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.90  2003/01/13 19:25:12  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.89.4.3  2005/09/28 17:14:43  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.89.4.2  2003/04/18 17:07:44  wenger
  Merged gcc3_br_0 thru gcc3_br_1 to V1_7b0_br.

  Revision 1.89.4.1.4.1  2003/04/18 15:26:04  wenger
  Committing *some* of the fixes to get things to compile with gcc
  3.2.2; these fixes should be safe for earlier versions of the
  comiler.

  Revision 1.89.4.1  2002/09/17 18:58:27  wenger
  Limited warning about horizontal and vertical points per pixel
  mismatch to one time; DEVise version is now 1.7.11.

  Revision 1.89  2002/02/05 19:32:33  wenger
  Fixed bug 750 (problem with child view/pile combinations with an X
  server with no backing store); added TEST_NO_BACKING_STORE code to
  test this case even if we really do have backing store on the server.

  Revision 1.88  2001/11/30 21:20:30  wenger
  Fixed bug 737 (zero-size visual filter crashes DEVise) and related
  bugs.

  Revision 1.87  2001/08/20 18:20:26  wenger
  Fixes to various font problems: XDisplay calculates point sizes correctly
  and uses screen resolution in specifying font; JS passes *its* screen
  resolution to the devised so that fonts show up correctly in the JS
  (JS protocol version now 7.0); changed DEVise version to 1.7.4.

  Revision 1.86  2001/07/31 15:53:22  wenger
  Added -fontkludge argument to allow bypassing of font families that
  don't work with Xvfb on SPARC/Solaris.

  Revision 1.85  2001/05/08 14:14:49  wenger
  Improved error message if we can't connect to X server.

  Revision 1.84  2001/03/23 18:06:31  wenger
  Color palettes are now associated with sessions; added borders to
  color chooser buttons so they're visible even if they're the same
  color as the background; fixed various color-related bugs.

  Revision 1.83  1999/11/30 22:28:08  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.82  1999/11/19 21:29:16  wenger
  Removed Journal class and related code (no longer works); removed various
  other unused or unnecessary code.

  Revision 1.81  1999/10/08 19:57:47  wenger
  Fixed bugs 470 and 513 (crashes when closing a session while a query
  is running), 510 (disabling actions in piles), and 511 (problem in
  saving sessions); also fixed various problems related to cursors on
  piled views.

  Revision 1.80  1999/10/04 19:37:00  wenger
  Mouse location is displayed in "regular" DEVise.

  Revision 1.79  1999/09/02 17:25:52  wenger
  Took out the ifdefs around the MARGINS code, since DEVise won't compile
  without them; removed all of the TK_WINDOW code, and removed various
  unnecessary includes of tcl.h, etc.

  Revision 1.78  1999/08/05 21:42:38  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.77  1999/07/30 21:27:06  wenger
  Partway to cursor dragging: code to change mouse cursor when on a DEVise
  cursor is in place (but disabled).

  Revision 1.76  1999/07/16 21:35:56  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

  Revision 1.75  1999/05/20 15:18:39  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.74  1999/02/11 19:54:41  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.73  1999/01/04 15:30:25  wenger
  Misc. code cleanups.

  Revision 1.72.2.1  1998/12/29 17:24:51  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.72  1998/12/15 18:47:13  wenger
  New option in fixed text symbol: if size is <=1, it is assumed to be a
  fraction of the screen height, rather than the font size in points.

  Revision 1.71  1998/09/22 17:23:45  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.70  1998/09/10 23:21:10  wenger
  Fixed bug 388 (missing window in JavaScreen) (caused by '/' in window
  name, which was then used as part of temp file name); default for
  JavaScreen is to save selected view when saving a session.

  Revision 1.69  1998/05/05 15:14:55  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.68  1998/04/16 21:50:53  wenger
  Committed Sanjay's text code.

  Revision 1.67  1998/02/26 00:19:09  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.66  1998/02/19 23:24:53  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.65  1998/02/09 17:28:45  wenger
  Added more diagnostic/debug output.

  Revision 1.64  1998/01/30 02:16:43  wenger
  Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.

  Revision 1.63  1997/12/28 16:40:13  beyer
  Can select scalable fonts now.

  Revision 1.62  1997/12/16 17:53:55  zhenhai
  Added OpenGL features to graphics.

  Revision 1.61  1997/11/24 23:14:40  weaver
  Changes for the new ColorManager.

  Revision 1.60.4.1  1998/01/16 23:41:23  wenger
  Fixed some problems that Tony found with the client/server communication
  and GIF generation; fixed problem that session files specified on the
  command line were still opened by the Tcl code.

  Revision 1.60  1997/09/22 15:57:30  wenger
  Printing display to GIF no longer prints window manager decorations.

  Revision 1.59  1997/07/22 19:44:29  wenger
  Removed extra dependencies that broke cslib link.

  Revision 1.58  1997/07/17 21:10:59  wenger
  Fixed bugs 204 and 205 (GIF printing problems).

  Revision 1.57  1997/06/10 19:21:41  wenger
  Removed (some) debug output.

  Revision 1.56  1997/05/21 22:10:01  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.55.2.1  1997/05/21 20:40:08  weaver
  Changes for new ColorManager

  Revision 1.55  1997/05/08 00:18:07  wenger
  Kludge fix for bug 182 (crash when closing multi1.tk session).

  Revision 1.54  1997/05/05 16:53:48  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.53  1997/04/11 18:48:54  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.52  1997/03/25 17:59:01  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.51.4.1  1997/03/15 00:31:10  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.51  1997/01/30 15:57:45  wenger
  Fixed compile warnings on Sun.

  Revision 1.50  1997/01/24 21:18:54  wenger
  Fixed memory leak in X font handling code; noted other minor leaks.

  Revision 1.49  1997/01/24 16:38:25  wenger
  Fixed bugs 078, 103, 125; also improved X font handling (does a better
  job of finding fonts).

  Revision 1.48  1997/01/17 20:31:49  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.47  1996/12/30 23:51:13  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.46  1996/12/15 20:22:35  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.45  1996/12/03 17:00:25  jussi
  Added SetFont() for generic font support. Removed SetSmallFont().

  Revision 1.44  1996/11/26 09:30:38  beyer
  Mucked with debugging some statements

  Revision 1.43  1996/11/23 00:24:12  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.42  1996/11/18 23:11:21  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.41  1996/11/13 16:56:17  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.40  1996/09/26 20:38:31  jussi
  Removed duplicate default argument.

  Revision 1.39  1996/09/19 03:34:02  guangshu
  Minor midification to make it more flexible when saving the mapping file.

  Revision 1.38  1996/09/18 20:14:43  guangshu
  Added function ExportView to save each view in a window to a separate gif
  file. Modified function ExportGIF.

  Revision 1.37  1996/09/13 23:04:27  guangshu
  Added methods to save the map files for www when saving display.

  Revision 1.36  1996/09/10 20:07:13  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.35  1996/09/05 21:30:14  jussi
  Moved user-specified screen size to Display.

  Revision 1.34  1996/09/05 20:03:47  jussi
  Minor fix to get rid of compiler warning.

  Revision 1.33  1996/09/05 20:01:36  jussi
  Added support for user-specified screen size (mainly used
  in batch mode).

  Revision 1.32  1996/09/05 19:11:23  jussi
  Displays containing multiple windows are now properly
  exported as GIF files.

  Revision 1.31  1996/09/04 21:24:50  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.30  1996/08/29 22:10:49  guangshu
  Changed arguments of ConvertAndWriteGIF.

  Revision 1.29  1996/07/23 19:34:08  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.28  1996/07/23 15:34:42  jussi
  Added mechanism for bypassing the Devise internal color map.

  Revision 1.27  1996/07/21 02:15:32  jussi
  Changed value of backing_store from Always to WhenMapped.

  Revision 1.26  1996/07/18 01:21:59  jussi
  Added ExportImage() and ConvertAndWriteGIF() methods. XWindowRep's
  that are actual windows (as opposed to pixmaps) are now inserted
  into a window tree.

  Revision 1.25  1996/07/14 01:36:59  jussi
  Added support for ICCCM (inter-client communication) between
  window manager and the X windows we create. This allows the
  window manager to Close/Delete a window by sending a
  WM_DELETE_WINDOW message to Devise, rather than Destroying
  a window which closes the X connection.

  Revision 1.24  1996/06/24 19:38:35  jussi
  Cleaned up a little and made XDisplay now use the ConnectionNumber()
  as the file descriptor. Also added a Flush() to InternalProcessing()
  in order to synchronize the client and the server; there still seems
  to be a problem in that some keyboard events aren't received
  in a timely manner.

  Revision 1.23  1996/06/21 19:30:10  jussi
  Replaced MinMax calls with MIN() and MAX().

  Revision 1.22  1996/05/20 18:45:02  jussi
  Merged with ClientServer library code.

  Revision 1.21  1996/05/13 21:57:30  jussi
  XDisplay goes into batch mode (pixmaps used instead of screen)
  if control panel is in batch mode.

  Revision 1.20  1996/05/11 02:29:26  jussi
  Ifdef'd out some startup options (whether windows should appear
  iconic or not when a session is restored).

  Revision 1.19  1996/05/08 15:59:36  jussi
  Replaced _display->fd, which is an illegal access, with the
  more portable ConnectionNumber(_display) defined in Xlib.h.

  Revision 1.18  1996/04/20 19:52:24  kmurli
  Changed Viex.c to use a pipe mechanism to call itself if it needs to be
  done again. The view now is not called contiously by the Dispatcher,instead
  only of there is some data in the pipe.
  The pipe mechanism is implemented transparently through static functions
  in the Dispatcher.c (InsertMarker,CreateMarker,CloseMarker,FlushMarker)

  Revision 1.17  1996/04/18 18:16:22  jussi
  This class now creates both X windows and X pixmaps, depending
  on whether the system is in interactive mode or batch mode.

  Revision 1.16  1996/04/11 18:15:36  jussi
  Made XDisplay use the polling mechanism temporarily until the
  occasional display update problem has been resolved.

  Revision 1.15  1996/04/09 18:06:13  jussi
  Added Flush() method.

  Revision 1.14  1996/04/08 16:58:39  jussi
  #define'd XLIB_ILLEGAL_ACCESS so that we have access to Display->fd
  in the X display data structured. Minor other fixes.

  Revision 1.13  1996/04/04 05:18:30  kmurli
  Major modification: The dispatcher now receives the register command
  from the displays directly (i.e. from XDisplay instead of from Display)
  corrected a bug in call to register function. Also now dispatcher
  uses socket number passed from the XDisplay class to select on it and
  call the relevant functions.

  Revision 1.12  1996/02/26 23:46:39  jussi
  XDisplay tries to load 5x8 font if loading 5x7 fails.

  Revision 1.11  1996/02/05 23:55:17  jussi
  Added support for small fonts.

  Revision 1.10  1996/01/13 20:58:38  jussi
  Made system display the name of the base color when another
  color is mapped to it.

  Revision 1.9  1996/01/10 20:59:03  jussi
  Failure to allocate all colors will now cause a base color
  (black) to be substituted.

  Revision 1.8  1995/12/28 18:56:51  jussi
  Small fix to remove compiler warning.

  Revision 1.7  1995/12/14 21:13:40  jussi
  Replaced 0x%x with 0x%p.

  RevisionXDisplay 1.6  1995/12/06 21:22:33  jussi
  Tries to allocate a color by using a close color approximation
  if requested color cannot be allocated.

  Revision 1.5  1995/12/02 21:30:11  jussi
  Tried if letting Tcl/Tk handle all X events and pass a copy
  of each event to us would solve the problem with TK_WINDOW_old.
  This event handling mechanism didn't turn out to work right
  so I renamed it TK_WINDOW_EV2.

  Revision 1.4  1995/11/29 15:30:09  jussi
  Disabled a debugging message.

  Revision 1.3  1995/11/29 15:12:07  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:13:38  jussi
  Added/updated CVS header.
*/

#include <math.h>

#ifdef SUN
#include <strings.h>
#endif
#include <X11/Xatom.h>

#include "XDisplay.h"
#include "XWindowRep.h"
#if !defined(LIBCS)
#include "Control.h"
#include "Init.h"
#endif
#include "Version.h"

#include "Color.h"
#include "XColor.h"

#define String XVString
#define black XVblack
extern "C" {
#include "xv.h"
}
#undef black
#undef String

//#define DEBUG
//#define DEBUG_MORE

#include "Util.h"

//#include "Session.h"
// Note: other stuff in Session.h conflicts with X includes.
class Session {
public:
  static void SetDefaultPalette();
};

#ifndef LIBCS
#include "WinClassInfo.h"
#endif

/* The maximum intensity of red, green, and blue. */
const int MaxColorIntensity = 65535;

static const int errBufSize = 1024;
static char errBuf[errBufSize];

/*******************************************************************
Open a new X display
********************************************************************/

XDisplay::XDisplay(char *name, Boolean fontKludge)
{
#if defined(DEBUG)
  printf("XDisplay::XDisplay(%s)\n", name != NULL ? name : "NULL");
#endif
  if (!(_display = XOpenDisplay(name))) {
    reportErrNosys("Cannot open XDisplay");
    Exit::DoExit(1);
  }

#if defined(DEBUG)
  printf("X display has backing store? %d\n",
      XDoesBackingStore(XScreenOfDisplay(_display, DefaultScreen(_display))));
#endif

  GetScreenRes();

  _fontKludge = fontKludge;

  _fontStruct = NULL;
  _normalFontStruct = NULL;
  SetDefaultFont();

  /* init stipples for patterns */
  Window win = XCreateSimpleWindow(_display, DefaultRootWindow(_display),
				   (unsigned)0, (unsigned)0, (unsigned)10,
				   (unsigned)10, 5, 0, 0);
  DOASSERT(win, "Cannot create window");

  for(int i = 0; i < XNumBitmaps; i++) {
    _stipples[i] = XCreateBitmapFromData(_display, win, xBitmaps[i],
					 XBitmapWidth, XBitmapHeight);
  }

  XDestroyWindow(_display, win);

#ifndef LIBCS
  Register();
#endif
  Colormap cmap = DefaultColormap(_display, DefaultScreen(_display));
  if (!InitColor(_display, DefaultDepth(_display, DefaultScreen(_display)),
                1, &cmap)) {
    reportErrNosys("Color initialization failed");
  }
  Session::SetDefaultPalette();
}

XDisplay::~XDisplay()
{
  TermColor();
}

/* Register the display with the dispatcher */

#ifndef LIBCS
void XDisplay::Register()
{
  Dispatcher::Current()->Register(this, 10, AllState, true, fd());
}
#endif

void XDisplay::SetNormalFont()
{
#if defined(DEBUG)
  printf("XDisplay::SetNormalFont()\n");
#endif

  if (_fontStruct != _normalFontStruct) {
#if defined(DEBUG)
    printf("Freeing font %p\n", _fontStruct);
#endif
	FreeFont(_fontStruct);
  }

  _fontStruct = _normalFontStruct;
}


/* Set font and point size */

void XDisplay::SetFont(const char *family, const char *weight, const char *slant,
                       const char *width, float pointSize)
{
#if defined(DEBUG)
  printf("XDisplay::SetFont(%s %s %s %s %f)\n", family, weight, slant, width,
    pointSize);
#endif

    if (_fontKludge) {
	    // Helvetica and Times fonts don't work with Xvfb on SPARC/Solaris.
        if (!strcmp(family, "Helvetica") || !strcmp(family, "Times")) {
	        family = "New Century Schoolbook";
	    }
    }

    /* Special case for Courier or Helvetica italic. */
    if (((!strcasecmp(family, "courier")) ||
      (!strcasecmp(family, "helvetica"))) && !strcasecmp(slant, "i")) {
        slant = "o";
    }

    /*
       Attempt to load font as specified. If font cannot be loaded,
       increase point size by one unit and try again.
    */
    for(float p = pointSize; p <= pointSize + 5.0; p += 1.0) {
	    const int bufSize = 256;
	    char fname[bufSize];

	    // Note: fields of font name are as follows:
	    //   foundry
	    //   font family
	    //   weight
	    //   slant
	    //   set width
	    //   additional style
	    //   pixels
	    //   points (in tenths of a point)
	    //   horizontal resolution (dpi)
	    //   vertical resolution (dpi)
	    //   spacing
	    //   average width (in tenths of a pixel)
	    //   character set
        int formatted = snprintf(fname, bufSize,
	      "-*-%s-%s-%s-%s-*-*-%d-*-*-*-*-*-*", family, weight, slant, width,
		  (int) (p * 10.0));
        checkAndTermBuf(fname, bufSize, formatted);

		SetFont(fname);
		if (_fontStruct) return;
    }
    int formatted = snprintf(errBuf, errBufSize,
      "Warning: could not find font %s %f", family, pointSize);
    checkAndTermBuf2(errBuf, formatted);
    reportErrNosys(errBuf);
    _fontStruct = _normalFontStruct;
}

void XDisplay::SetFont(const char *fname)
{
#if 0
    printf("  Trying font %s\n", fname);
#endif

    XFontStruct *oldFont = _fontStruct;

    _fontStruct = XLoadQueryFont(_display, fname);
    if (_fontStruct) {
#if defined(DEBUG)
        printf("Loaded font %s (%p)\n", fname, _fontStruct);
#endif
        if (oldFont && oldFont != _normalFontStruct) {
#if defined(DEBUG)
            printf("Freeing font %p\n", oldFont);
#endif
            FreeFont(oldFont);
        }
	}
}

#ifndef LIBCS
/* Export display image to other graphics formats */

void XDisplay::ExportImage(DisplayExportFormat format, char *filename)
{
#if defined(DEBUG)
  printf("XDisplay::ExportImage(%d, %s)\n", (int) format, filename);
#endif

  if (format == GIF) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
	reportErrSys("open");
        int formatted = snprintf(errBuf, errBufSize,
	  "Cannot open file %s for writing\n", filename);
        checkAndTermBuf2(errBuf, formatted);
        reportErrNosys(errBuf);
        return;
    }
    ExportGIF(fp);
    if (fp != stderr && fp != stdout)
        fclose(fp);
    return;
  } else {
#ifndef LIBCS
    ExportToPS(format, filename);
#else
    reportErrNosys("Cannot export display image in PS/EPS yet\n");
#endif
  }
}

/* Export every view as a window in the display */

void XDisplay::ExportView(DisplayExportFormat format, char *filename)
{
#if defined(DEBUG)
  printf("XDisplay::ExportView(%d, %s)\n", (int) format, filename);
#endif

  if (format == GIF) {
	int i = 0;
	int index1 = _winList.InitIterator();

	while(_winList.More(index1)) {
	   XWindowRep *win = _winList.Next(index1);
	   if (win->_parent)
	      continue;
	   int index2 = win->_children.InitIterator();
	   while(win->_children.More(index2)) {
	      XWindowRep *winc = win->_children.Next(index2);
	      char fn[strlen(filename) + 10];
	      sprintf(fn,"%s.%d.gif", filename, i);
#if defined(DEBUG)
	      printf("Saving the view to file %s\n", fn);
#endif
	      FILE *fp = fopen(fn, "wb");
	      if (!fp) {
	         reportErrSys("open");
		 int formatted = snprintf(errBuf, errBufSize,
		   "Cannot open file %s for writing.\n", fn);
                 checkAndTermBuf2(errBuf, formatted);
                 reportErrNosys(errBuf);
		 return;
	      }
	      winc->ExportGIF(fp, 1);
	      fclose(fp);
	      i++;
	   }
	   win->_children.DoneIterator(index2);
	}
	_winList.DoneIterator(index1);
	return;
  } else {
    reportErrNosys("Cannot export all views in PS/EPS yet\n");
  }
}

/* Export the display and the map file associated with the display */

void XDisplay::ExportImageAndMap(DisplayExportFormat format, char *gifFilename, 
				char *mapFileName, char *url, char *defaultUrl)
{
#if defined(DEBUG)
  printf("XDisplay::ExportImageAndMap(%d, %s)\n", (int) format, gifFilename);
#endif

  if (format == GIF) {
     FILE *fp1 = fopen(gifFilename,"wb");
     if (!fp1) {
	reportErrSys("open");
	int formatted = snprintf(errBuf, errBufSize,
	  "Cannot open file %s for writing\n", gifFilename);
        checkAndTermBuf2(errBuf, formatted);
        reportErrNosys(errBuf);
	return;
      }

      char prename[strlen(gifFilename) + 1];
      strcpy(prename, gifFilename);
      char *p = strstr(prename, ".gif");
      char *p_start = strrchr(prename, '/');
      if (p_start) p_start++;
      else p_start = prename;
      if (p) {
          *p = '\0';
      } else {
          int formatted = snprintf(errBuf, errBufSize,
	    "Warning: gif file name %s is not ended with .gif\n", 
            gifFilename);
          checkAndTermBuf2(errBuf, formatted);
          reportErrNosys(errBuf);
      }
#if defined(DEBUG)
      printf("prename is %s\n", prename);
#endif
     FILE *fp2 = fopen(mapFileName,"wb");
     if (!fp2) {
	reportErrSys("open");
        int formatted = snprintf(errBuf, errBufSize,
	  "Cannot open file %s for writing\n", mapFileName);
        checkAndTermBuf2(errBuf, formatted);
        reportErrNosys(errBuf);
        return;
      }
      int x = 0 ,y = 0;
      unsigned int w = 0, h = 0;

      int index1 = _winList.InitIterator();
      int i = 0;

      while(_winList.More(index1)) {
         XWindowRep *win = _winList.Next(index1); 
	 if (win->_parent)
            continue;   
         int wx, wy;
	 unsigned int ww, wh;
	 win->GetRootGeometry(wx, wy, ww, wh);

#if defined(DEBUG) || 0
            printf("The geometry of this window: wx = %d, wy = %d, "
                        "ww = %u, wh = %u\n", wx, wy, ww, wh);
#endif

	 if (!w && !h) {
      	    /* this is the first window */
      	    x = wx;
            y = wy;
            w = ww;
            h = wh;
#if defined(DEBUG) || 0
    	    printf("In the first window: x = %d, y = %d, "
			"w = %u, h = %u\n", x, y, w, h);
#endif
    	 } else {
      	    /* compute combined area */
      	    int x2 = MAX(x + w - 1, wx + ww - 1);
      	    int y2 = MAX(y + h - 1, wy + wh - 1);
      	    x = MIN(x, wx);
       	    y = MIN(y, wy);
      	    w = x2 - x + 1;
      	    h = y2 - y + 1;
    	 }
      }
      _winList.DoneIterator(index1);
#if defined(DEBUG) || 0
      printf("The display has: x = %d, y = %d, "
			"w = %u, h = %u\n", x, y, w, h);
#endif

      index1 = _winList.InitIterator();
      while(_winList.More(index1)) {
	 XWindowRep *win = _winList.Next(index1);
	 if (win->_parent)
	    continue;
	 int rx =0, ry = 0;
	 unsigned int rw, rh;
	 win->GetRootGeometry(rx, ry, rw, rh);
#if  defined(DEBUG) || 0
	 printf("The parent has: rx = %d, ry = %d, rw = %u, rh = %u\n",
			rx, ry, rw, rh);
#endif

         int index2 = win->_children.InitIterator();
	 while(win->_children.More(index2)) {
	     XWindowRep *winc = win->_children.Next(index2);
	     int sub_x = 0, sub_y = 0;
	     unsigned int sub_w, sub_h;
	     winc->Origin(sub_x, sub_y);
	     winc->Dimensions(sub_w, sub_h);
	     char temp[LINE_SIZE];
  	     sprintf(temp, "rect %s?%s+%d %d,%d %u,%u\n", url, p_start, i, sub_x+(rx-x), 
			sub_y+(ry-y)+23, sub_w+sub_x+(rx-x), sub_h+sub_y+(ry-y)+23); 
	     i++;
	     fprintf(fp2, temp);
	 }
         win->_children.DoneIterator(index2);
      }
      _winList.DoneIterator(index1);
      fprintf(fp2, "default %s\n", defaultUrl);
      fclose(fp2);
      MakeAndWriteGif(fp1, x, y, w, h);
      fclose(fp1);
      *p = '.';
      return;
  } else {
    reportErrNosys("Cannot export display image in PS/EPS yet\n");
  } 
}

// Whether or not to print window manager decorations to GIF output.
#define PRINT_WM_DECORATIONS 0

void XDisplay::ExportGIF(FILE *fp, int isView)
{
#if defined(DEBUG)
  printf("XDisplay::ExportGIF()\n");
#endif

  /* compute the bounding rectangle of all windows */

  int x = 0, y = 0;
  unsigned int w = 0, h = 0;

  ClassInfo *windowInfo;
  ViewWin *windowP;
  int index = DevWindow::InitIterator();
  while (DevWindow::More(index)) {
    windowInfo = DevWindow::Next(index);
    windowP = (ViewWin *) windowInfo->GetInstance();
    if ((windowP != NULL) && !windowP->Iconified() &&
        !windowP->GetPrintExclude()) {
#if defined(DEBUG)
    printf("Getting bounding box for window %s\n", windowP->GetName());
#endif
      XWindowRep *win = (XWindowRep *) windowP->GetWindowRep();
      int wx, wy;
      unsigned int ww, wh;
#if PRINT_WM_DECORATIONS
      win->GetRootGeometry(wx, wy, ww, wh);
#else
      win->Dimensions(ww, wh);
      win->AbsoluteOrigin(wx, wy);
#endif
      if (!w && !h) {
        /* this is the first window */
        x = wx;
        y = wy;
        w = ww;
        h = wh;
      } else {
        /* compute combined area */
        int x2 = MAX(x + w - 1, wx + ww - 1);
        int y2 = MAX(y + h - 1, wy + wh - 1);
        x = MIN(x, wx);
        y = MIN(y, wy);
        w = x2 - x + 1;
        h = y2 - y + 1;
      }
    }
  }
  DevWindow::DoneIterator(index);

#ifdef DEBUG
  printf("Bounding rectangle of windows is %d,%d,%u,%u\n", x, y, w, h);
#endif
  if ((w != 0) && (h != 0)) {
    MakeAndWriteGif(fp, x, y, w, h);
  }
}

void XDisplay::MakeAndWriteGif(FILE *fp, int x, int y, int w, int h)
{
#if defined(DEBUG)
    printf("XDisplay::MakeAndWriteGif()\n");
#endif
  /* Allocate a pixmap large enough to hold all windows */

  unsigned int depth = DefaultDepth(_display, DefaultScreen(_display));
  Window parent = DefaultRootWindow(_display);
  Pixmap pixmap = XCreatePixmap(_display, parent, w, h, depth);
  if (!pixmap) {
    int formatted = snprintf(errBuf, errBufSize,
      "Cannot create %ux%d pixmap\n", w, h);
    checkAndTermBuf2(errBuf, formatted);
    reportErrNosys(errBuf);
    return;
  }

  /* Create and set up graphics context */

  GC gc = XCreateGC(_display, DefaultRootWindow(_display), 0, NULL);

  XSetState(_display, gc, CM_GetXColorID(GetColorID(gifWhiteColor)),
            CM_GetXColorID(GetColorID(gifBlackColor)), GXcopy, AllPlanes);

  XFillRectangle(_display, pixmap, gc, 0, 0, w, h);

  /* Copy windows to pixmap */

  ClassInfo *windowInfo;
  ViewWin *windowP;
  int index = DevWindow::InitIterator();
  while (DevWindow::More(index)) {
    windowInfo = DevWindow::Next(index);
    windowP = (ViewWin *) windowInfo->GetInstance();
    if ((windowP != NULL) && !windowP->Iconified() &&
        !windowP->GetPrintExclude()) {
#if defined(DEBUG)
    printf("Printing window %s\n", windowP->GetName());
#endif
      XWindowRep *win = (XWindowRep *) windowP->GetWindowRep();
      int wx, wy;
      unsigned int ww, wh;
#if PRINT_WM_DECORATIONS
      win->GetRootGeometry(wx, wy, ww, wh);
#else
      win->Dimensions(ww, wh);
      win->AbsoluteOrigin(wx, wy);
#endif
      int dx = wx - x;
      int dy = wy - y;
#ifdef DEBUG
      printf("Copying from window 0x%lx to %d,%d,%u,%u\n",
             win->GetWinId(), dx, dy, ww, wh);
#endif
      DOASSERT(dx >= 0 && dy >= 0, "Invalid window coordinates");
      Drawable src = win->GetPixmapId();
      if (src) {
        XWindowRep::CoalescePixmaps(win);
      } else {
#if PRINT_WM_DECORATIONS
        src = win->FindTopWindow(win->GetWinId());
#else
	src = win->GetWinId();
#endif
        XRaiseWindow(_display, src);
      }
      XImage *image = XGetImage(_display, src, 0, 0, ww, wh, AllPlanes,
	ZPixmap);
      if (!image || !image->data) {
        reportErrNosys("Cannot get image of window or pixmap\n");
        continue;
      }
      XPutImage(_display, pixmap, gc, image, 0, 0, dx, dy, ww, wh);
      image->data = NULL;
      XDestroyImage(image);
    } else {
      if(windowP != NULL) printf(
	  "Excluding window %s from printing\n", windowP->GetName());
    }
  }
  DevWindow::DoneIterator(index);

  /* Cannot get attributes of pixmap using XGetWindowAttributes
     so let's get the attributes from the root window and then
     patch xwa with the pixmap's information */

  XWindowAttributes xwa;
  if (!XGetWindowAttributes(_display, DefaultRootWindow(_display), &xwa)) {
    reportErrNosys("Cannot get window attributes\n");
    return;
  }
  xwa.x = x;
  xwa.y = y;
  xwa.width = w;
  xwa.height = h;
  
  /* Convert pixmap to GIF and write to file */

  ConvertAndWriteGIF(pixmap, xwa, fp);

  /* Free gc and pixmap area */

  XFreeGC(_display, gc);
  XFreePixmap(_display, pixmap);
}
#endif

/* Convert drawable to GIF and write to file */
// Color code is independent of ColorManager for now. Ignore.
void XDisplay::ConvertAndWriteGIF(Drawable drawable,
                                  XWindowAttributes xwa,
                                  FILE *fp)
{
  XImage *image = XGetImage(_display, drawable, 0, 0, xwa.width, xwa.height,
			    AllPlanes, ZPixmap);
  if (!image || !image->data) {
    reportErrNosys("Cannot get image of window or pixmap\n");
    return;
  }

  XColor *colors = 0;
  int ncolors = getxcolors(&xwa, &colors, _display);//TEMP -- xv function
  int code = convertImage(image, colors, ncolors, &xwa);//TEMP -- xv function

  XDestroyImage(image);
  if (colors)
    free(colors);

  if (code != 1 || !grabPic) {
    reportErrNosys("Cannot convert image\n");
    return;
  }

  int ptype = (gbits == 24 ? PIC24 : PIC8);
  int colorstyle = -1;                  // use 1 for grayscale
  const char *comment = Version::GetWinLogo();

  int status = WriteGIF(fp, grabPic, ptype, gWIDE, gHIGH,
			grabmapR, grabmapG, grabmapB, ncolors,
			colorstyle, (char *) comment);
  if (status)
    reportErrNosys("Cannot write GIF image\n");

  free(grabPic);
  grabPic = 0;
}

void XDisplay::SetTasvirServer(const char *server) {
  int index = _winList.InitIterator();
  while(_winList.More(index)) {
    XWindowRep *win = _winList.Next(index);
    win->SetDaliServer(server);
  }
  _winList.DoneIterator(index);
}

void XDisplay::SetETkServer(const char *server) {
  int index = _winList.InitIterator();
  while(_winList.More(index)) {
    XWindowRep *win = _winList.Next(index);
    win->SetETkServer(server);
  }
  _winList.DoneIterator(index);
}

void XDisplay::Dimensions(Coord &width, Coord &height)
{
  int screen = DefaultScreen(_display);
  width = (Coord)DisplayWidth(_display, screen);
  height = (Coord)DisplayHeight(_display, screen);
#ifndef LIBCS
  if (_desiredScreenWidth > 0)
    width = _desiredScreenWidth;
  if (_desiredScreenHeight > 0)
    height = _desiredScreenHeight;
#endif
}

void XDisplay::WinDimensions(Window win, Coord &winWidth, Coord &winHeight)
{
  if (win == DefaultRootWindow(_display)) {
    Dimensions(winWidth, winHeight);
    return;
  }

  Window root;
  int x, y;
  unsigned int width, height;
  unsigned int border_width, depth;
  XGetGeometry(_display, win, &root, &x, &y, &width, &height,
               &border_width, &depth);
  winWidth = (Coord)width;
  winHeight = (Coord)height;
}

/*
 * This structure forms the WMHINTS property of the window,
 * letting the window manager know how to handle this window.
 * See Section 9.1 of the Xlib manual.
 */

static XWMHints xwmh = {
  (InputHint | StateHint | IconPositionHint),	/* flags */
  False,					/* input */
  NormalState,					/* initialstate */
  0,						/* icon pixmap */
  0,						/* icon window */
  0, 0,						/* icon location */
  0,						/* icon mask */
  0						/* Window group */
};

/*************************************************************
Create a new window 
***************************************************************/

WindowRep *XDisplay::CreateWindowRep(const char *name, Coord x, Coord y,
				     Coord width, Coord height, 
				     WindowRep *parentRep,
				     Coord min_width, Coord min_height,
				     Boolean relative, Boolean winBoundary)
{
  DO_DEBUG(printf("XDisplay::CreateWindowRep(%s)\n", name));
  Window parent = DefaultRootWindow(_display);

  Coord realX, realY, realWidth, realHeight;
  Coord real_min_width, real_min_height;
  if (relative) {
    RealWindowDimensions(parent, x, y, width, height, realX, realY,
			 realWidth, realHeight);
    RealWindowWidthHeight(parent, min_width, min_height,
			  real_min_width, real_min_height);
  } else {
    realX = x;
    realY = y;
    realWidth = width;
    realHeight = height;
    real_min_width = min_width;
    real_min_height = min_height;
  }

  if ((realWidth <= 0) || (realHeight <= 0)) {
    reportErrNosys("Zero-size window!");

    // Avoid crashing...
    realWidth = 10;
    realHeight = 10;
  }

#ifndef LIBCS
  if (ControlPanel::Instance()->GetBatchMode()) {
    // we're in batch mode -- create a pixmap instead of a window

    unsigned int depth = DefaultDepth(_display, DefaultScreen(_display));
    Pixmap pixmap = XCreatePixmap(_display, parent, (unsigned)realWidth,
				  (unsigned)realHeight, depth);
    DOASSERT(pixmap, "Cannot create pixmap");

#ifdef DEBUG
    printf("XDisplay: Created X pixmap 0x%lx at %u,%u, size %ux%u\n",
	   pixmap, (unsigned)realX, (unsigned)realY,
	   (unsigned)realWidth, (unsigned)realHeight);
#endif

    XWindowRep *xwin = new XWindowRep(_display, pixmap, this,
				      (XWindowRep *)parentRep, (int)realX, (int)realY);
    DOASSERT(xwin, "Cannot create XWindowRep");

    _winList.Insert(xwin);
  
    return xwin;
  }
#endif

  /* Define event mask. */

  unsigned long int mask = ExposureMask | ButtonPressMask 
	                   | ButtonReleaseMask | Button1MotionMask
                           | Button2MotionMask | Button3MotionMask
                           | StructureNotifyMask | KeyPressMask
                           | VisibilityChangeMask;
  mask |= PointerMotionMask; // For cursor dragging.
  mask |= LeaveWindowMask; // For mouse position display.

#ifdef RAWMOUSEEVENTS
  mask |= PointerMotionMask;
#endif
  
  /* Define window attributes. */

  XSetWindowAttributes attr;
  attr.background_pixmap 	= None;
  attr.background_pixel  	= CM_GetXColorID(GetColorID(xWinBackColor));
  attr.border_pixmap  		= CopyFromParent;
  attr.border_pixel  		= CM_GetXColorID(GetColorID(xWinForeColor));
  attr.bit_gravity  		= ForgetGravity;   /* CenterGravity */
  attr.win_gravity  		= NorthWestGravity;
#if TEST_NO_BACKING_STORE
  attr.backing_store  		= NotUseful;
#else
  attr.backing_store  		= Always;          /* WhenMapped/NotUseful */
#endif
  attr.backing_planes  		= AllPlanes;
  attr.backing_pixel  		= 0;
  attr.save_under  		= False;
  attr.event_mask  		= mask;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect  	= False;
  attr.colormap	= DefaultColormap(_display, DefaultScreen(_display));
  attr.cursor  			= None;

  /* Create the window. */

  if (parentRep) {
    // Using _myWin here instead of GetWinId() fixes bug 490.  RKW 1999-05-19.
    parent = ((XWindowRep *)parentRep)->_myWin;
  }

  unsigned int border_width;
  if (winBoundary)
    border_width = (!parent ? 5 : 1);
  else
    border_width = (!parent ? 5 : 0);

  Window w = XCreateWindow(_display, parent, (unsigned)realX, (unsigned)realY, 
			   (unsigned)realWidth, (unsigned)realHeight,
			   border_width, 0, InputOutput, CopyFromParent,
			   AllPlanes, &attr);
  DOASSERT(w, "Cannot create window");

#ifdef DEBUG
  printf("XDisplay: Created X window 0x%lx to parent 0x%lx at %u,%u,\n",
	 w, parent, (unsigned)realX, (unsigned)realY);
  printf("          size %u,%u, borderwidth %d\n", (unsigned)realWidth,
	 (unsigned)realHeight, border_width);
#endif

  /*
   * Deal with providing the window with an initial position & size.
   * Fill out the XSizeHints struct to inform the window manager. See
   * Sections 9.1.6 & 10.3.
   */

  XSizeHints xsh;
  xsh.flags 	= PPosition | PSize | PMinSize;
  xsh.height 	= (int)realHeight;
  xsh.width 	= (int)realWidth;
  xsh.x 	= (int)realX;
  xsh.y 	= (int)realY;
  xsh.min_width = (int)real_min_width;
  xsh.min_height = (int)real_min_height;

  XSetStandardProperties(_display, w, name, name, None, 0, 0, &xsh);

  /* Set window manager hints for iconification. */

#if 0
  if (ControlPanel::Instance()->Restoring() && Init::Iconify()) {
    xwmh.flags = InputHint | StateHint | IconPositionHint;
    xwmh.input = true;
    xwmh.initial_state = IconicState;
    xwmh.icon_x = (unsigned)realX;
    xwmh.icon_y = (unsigned)realY;
  }
#endif

  xwmh.flags = InputHint | StateHint;
  xwmh.input = true;
  xwmh.initial_state = NormalState;
  XSetWMHints(_display, w, &xwmh);
  
  /* Allow window manager to send WM_DELETE_WINDOW messages. */

  Atom deleteWindowAtom = XInternAtom(_display, "WM_DELETE_WINDOW", False);
  Atom protocolAtom = XInternAtom(_display, "WM_PROTOCOLS", False);
  XChangeProperty(_display, w, protocolAtom, XA_ATOM, 32,
                  PropModeReplace, (unsigned char *)&deleteWindowAtom, 1);

  /* Map the window so that it appears on the display. */

  if (XMapWindow(_display, w) < 0)
    return NULL;

  /* Do a sync to force the window to appear immediately. */

  XSync(_display, false);

#if 0
  /* Wait for MapNotify event to come back from server. */

  XEvent e;
  e.event = 0;
  while (e.event != MapNotify) {
    XNextEvent(_display, &e);
  }
#endif

  /* Return the XWindowRep structure. */

  XWindowRep *xwin = new XWindowRep(_display, w, this,
                                    (XWindowRep *)parentRep, false);
  DOASSERT(xwin, "Cannot create XWindowRep");
  _winList.Insert(xwin);
  
  return xwin;
}

/**************************************************************
Destroy a window. Parameter "win" better be of type XwindowRep *.
**************************************************************/

void XDisplay::DestroyWindowRep(WindowRep *win)
{
  DO_DEBUG(printf("XDisplay::DestroyWindowRep(%p)\n", win));

  XWindowRep *xwin = (XWindowRep *)win;
  if (!_winList.Delete(xwin)) {
    reportErrNosys("XDisplay:Window to be deleted not found\n");
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(1);
  }

  // Reset any XWindowRep that's outputting to the X window or pixmap we're
  // about to destroy.
  int index = _winList.InitIterator();
  while (_winList.More(index)) {
    XWindowRep *wr = _winList.Next(index);
    if ((xwin->_myWin && (xwin->_myWin == wr->GetWinId())) ||
      (xwin->_myPixmap && (xwin->_myPixmap == wr->GetPixmapId()))) {
        wr->ResetOutput();
      }
  }
  _winList.DoneIterator(index);

  // Free the Dali images and sleep before destroying the X window so
  // Dali doesn't get 'bad window' errors.
  if (xwin->DaliImageCount() > 0)
  {
    (void) xwin->DaliFreeImages();
    sleep(1);
  }

//#if !defined(LIBCS) || defined(NEW_LIBCS)
  // Free embedded Tk windows and sleep before destroying the X window
  if (xwin->ETk_WindowCount() > 0)
  {
    (void) xwin->ETk_FreeWindows();
    sleep(1);
  }
//#endif

  // Make sure we destroy the window or pixmap that this XWindowRep "owns".
  xwin->ResetOutput();

  if (xwin->GetWinId()) {
#ifdef DEBUG
    printf("XDisplay::DestroyWindowRep %p, window 0x%lx\n",
	   xwin, xwin->GetWinId());
#endif
    XDestroyWindow(_display, xwin->GetWinId());
    xwin->_win = 0;
    xwin->_myWin = 0;
  } else {
#ifdef DEBUG
    printf("XDisplay::DestroyWindowRep %p, pixmap 0x%lx\n",
	   xwin, xwin->GetPixmapId());
#endif
    XFreePixmap(_display, xwin->GetPixmapId());
    xwin->_pixmap = 0;
    xwin->_myPixmap = 0;
  }

#if 0 // Note: taking this back out is part of the fix for bugs 470 and
      // 513.  If this code is enabled, we can get goofed up by getting
      // an event when a view is partially destroyed.  RKW 1999-10-08.
  /* KLUDGE WARNING.  This somehow fixes bug 182.  I think we ended up
   * getting goofed up by the destroy event on the XWindow we're destroying
   * here, and somehow processing the events at this point fixes it. */
  Run();
#endif

  delete xwin;

  XSync(_display, false);
}

/*******************************************************************
Internal non-blocking event processing
********************************************************************/

void XDisplay::InternalProcessing()
{
#if defined(DEBUG_MORE)
  printf("XDisplay:: Received callback\n");
#endif

  XFlush(_display);

  while (XPending(_display) > 0) {
    /* There are events in the queue */
    XEvent event;
    XNextEvent(_display, &event);

    /* dispatch event to appropriate window */
    Boolean found = false;
    int index;
    for(index = _winList.InitIterator(); _winList.More(index);) {
      XWindowRep *win = _winList.Next(index);

      // Look at _myWin instead of GetWinId() so we pass the event to the
      // XWindowRep that *owns* the window in which the event occurred.
      if (win->_myWin && win->_myWin == event.xany.window) {
	/* Note: got to be careful here. We need to
	   call DoneIterator() before informing the WindowRep
	   because it might trigger a call to DestroyWindowRep()
	   to delete the window. Delete() can't be called
	   when the iterator is active. */
	_winList.DoneIterator(index);
	found = true;
#if defined(DEBUG)
	printf("XDisplay::Dispatching event %d to XWindowRep %p\n",
	       event.type, win);
#endif
	win->HandleEvent(event);
	break;
      }
    }
    if (!found) {
      _winList.DoneIterator(index);
#ifdef DEBUG
      printf("XDisplay::InternalProcessing: window for event %d not found\n",
	     event.type);
#endif
    }
  }

#ifdef DEBUG_MORE
  printf("X event queue length = %d\n", XQLength(_display));
#endif
}

Coord XDisplay::PointsPerPixel()
{
  const int pointsPerInch = 72;

  Coord pointsPerPixelHor = (float)pointsPerInch / (float)_desiredScreenXRes;
  Coord pointsPerPixelVert = (float)pointsPerInch / (float)_desiredScreenYRes;

  static Boolean gavePixelWarning = false;
  if (fabs(pointsPerPixelHor - pointsPerPixelVert) > 0.01 &&
      !gavePixelWarning) {
    int formatted = snprintf(errBuf, errBufSize,
      "Warning: horizontal (%f) and vertical (%f) points per pixel differ!\n",
        pointsPerPixelHor, pointsPerPixelVert);
    checkAndTermBuf2(errBuf, formatted);
    reportErrNosys(errBuf);
    gavePixelWarning = true;
  }

#if defined (DEBUG)
  printf("%f points per pixel\n", pointsPerPixelHor);
#endif

  return pointsPerPixelHor;
}

void
XDisplay::GetScreenRes()
{
#if defined(DEBUG)
  printf("XDisplay::GetScreenRes()\n");
#endif

  int screen_number  = XDefaultScreen(_display);

  int dpyWdPixel = XDisplayWidth(_display,screen_number);
  int dpyWdMm = XDisplayWidthMM(_display,screen_number);

  int dpyHtPixel = XDisplayHeight(_display,screen_number);
  int dpyHtMm = XDisplayHeightMM(_display,screen_number);

#if defined (DEBUG)
  cout << "  width (pixels) = " << dpyWdPixel << endl;
  cout << "  width (mm) = " << dpyWdMm << endl;

  cout << "  height (pixels) = " << dpyHtPixel << endl;
  cout << "  height (mm) = " << dpyHtMm << endl;
#endif

  const double mmPerInch = 25.4;

  double dpyHtIn = dpyHtMm / mmPerInch;
  double dpyWdIn = dpyWdMm / mmPerInch;

  _desiredScreenXRes = (int)(dpyWdPixel / dpyWdIn + 0.5);
  _desiredScreenYRes = (int)(dpyHtPixel / dpyHtIn + 0.5);

#if defined(DEBUG)
  printf("  screen resolution: %d %d\n", _desiredScreenXRes,
      _desiredScreenYRes);
#endif
}

void
XDisplay::SetDesiredScreenXRes(int resolution)
{
  DeviseDisplay::SetDesiredScreenXRes(resolution);
  SetDefaultFont();
}

void
XDisplay::SetDesiredScreenYRes(int resolution)
{
  DeviseDisplay::SetDesiredScreenYRes(resolution);
  SetDefaultFont();
}

void
XDisplay::SetDefaultFont()
{
#if defined(DEBUG)
  printf("XDisplay::SetDefaultFont()\n");
#endif

  if (_normalFontStruct != NULL) {
#if defined(DEBUG)
    printf("Freeing font %p\n", _normalFontStruct);
#endif
    FreeFont(_normalFontStruct);
  }

  const char *_defaultFName = Init::DefaultFont();

  if (_defaultFName) {
    SetFont(_defaultFName);
  } else {
    SetFont("Courier", "Medium", "r", "Normal", 12.0);
  }
  _normalFontStruct = _fontStruct;
  if (!_normalFontStruct) {
	  reportErrNosys("Fatal error: cannot load default font");
      Exit::DoExit(1);
  }
}

void
XDisplay::FreeFont(XFontStruct *fontStruct)
{
#if defined(DEBUG)
    printf("XDisplay::FreeFont(%p)\n", fontStruct);
#endif

  if (fontStruct != NULL) {
    XFreeFont(_display, fontStruct);
	if (_fontStruct == fontStruct) _fontStruct = NULL;
	if (_normalFontStruct == fontStruct) _normalFontStruct = NULL;
  }
}
