/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.120  1999/01/05 20:53:52  wenger
  Fixed bugs 447 and 448 (problems with symbol patterns); cleaned up some
  of the text symbol code.

  Revision 1.119.2.1  1998/12/29 17:24:53  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.119  1998/12/22 19:39:17  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.118  1998/12/15 14:54:59  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.117  1998/11/16 15:27:57  wenger
  Fixed bug 437 (drill-down on black background).

  Revision 1.116  1998/09/10 23:21:22  wenger
  Fixed bug 388 (missing window in JavaScreen) (caused by '/' in window
  name, which was then used as part of temp file name); default for
  JavaScreen is to save selected view when saving a session.

  Revision 1.115  1998/09/04 17:26:13  wenger
  Got Tasvir images to work in pixmaps (when running the JavaScreen, for
  example) -- fixes bug 385.

  Revision 1.114  1998/08/24 14:57:32  wenger
  Added misc. debug output.

  Revision 1.113  1998/08/20 18:55:37  wenger
  Fixed bug 252 (problem with color of drill-down text).

  Revision 1.112  1998/06/03 17:09:31  wenger
  Rubberband line in JavaScreen now sends updates of all changed windows
  using the "dirty GIF" flag; updated DEVise version to 1.5.3.

  Revision 1.111  1998/05/21 18:18:35  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.110  1998/05/14 18:21:17  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.109  1998/04/16 21:50:57  wenger
  Committed Sanjay's text code.

  Revision 1.108  1998/04/13 22:25:02  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.107  1998/03/13 18:10:42  wenger
  Fixed bug 327 (gaps in view background colors).

  Revision 1.106  1998/03/12 22:58:44  wenger
  Fixed bug 324 (view sometimes not fully redrawing); turned off some
  OpenGL debug code.

  Revision 1.105  1998/02/19 23:24:56  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.104  1998/02/03 18:31:29  zhenhai
  Fully implemented functionalities of XWindowRep with GLWindowRep. Fixed bugs in
  postscript printing.

  Revision 1.103  1998/01/31 20:02:04  wenger
  Fixed bugs 277, 278, and 279; GData sent on socket now has <ctl-D>
  written at the end.

  Revision 1.102  1998/01/30 02:16:48  wenger
  Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.

  Revision 1.101  1998/01/23 20:37:57  zhenhai
  Fixed a bug on transformation which was caused by inconsistency between origins
  or XWindows (Upper left) and OpenGL (Lower left). Also fixed a bug for
  incorrect labelling of axis.

  Revision 1.100  1998/01/14 16:38:59  wenger
  Merged cleanup_1_4_7_br_6 thru cleanup_1_4_7_br_7.

  Revision 1.99  1997/12/20 00:01:42  wenger
  Fixed bug 262 (text not drawn), found some others.

  Revision 1.98  1997/12/16 17:53:57  zhenhai
  Added OpenGL features to graphics.

  Revision 1.97  1997/11/24 23:14:42  weaver
  Changes for the new ColorManager.

  Revision 1.96.6.2  1998/01/16 23:41:27  wenger
  Fixed some problems that Tony found with the client/server communication
  and GIF generation; fixed problem that session files specified on the
  command line were still opened by the Tcl code.

  Revision 1.96.6.1  1998/01/13 18:27:47  wenger
  Printing display now works in batch mode (pixmaps);  cleaned up
  WindowRep classes slightly; interrupted system calls in server code
  don't cause server to exit.

  Revision 1.96  1997/07/18 20:25:08  wenger
  Orientation now works on Rect and RectX symbols; code also includes
  some provisions for locating symbols other than at their centers.

  Revision 1.95  1997/06/13 18:07:30  wenger
  Orientation is now working for text labels and fixed text labels.

  Revision 1.94  1997/06/04 15:50:34  wenger
  Printing windows to PostScript as pixmaps is now implemented, including
  doing so when printing the entire display.

  Revision 1.93  1997/05/21 22:10:04  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.92.6.1  1997/05/21 20:40:10  weaver
  Changes for new ColorManager

  Revision 1.92  1997/05/05 16:53:51  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.91  1997/04/25 16:54:02  wenger
  Text labels are now sized in the same way as Rects; fixed font bug in
  middle button query popup; removed the dialog that warns you about your
  table name getting changed when you open an old data source.

  Revision 1.90  1997/04/11 18:48:56  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.89  1997/03/25 17:59:03  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.88  1997/03/19 19:41:10  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.87  1997/01/28 19:46:35  wenger
  Fixed bug 139; better testing of ScaledText() in client/server example;
  fixes to Exit class for client/server library.

  Revision 1.86  1997/01/17 20:31:51  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.85  1997/01/13 18:07:51  wenger
  Fixed bugs 043, 083, 084, 091, 114.

  Revision 1.84  1997/01/09 18:41:20  wenger
  Added workarounds for some Tasvir image bugs, added debug code.

  Revision 1.83  1997/01/08 19:01:45  wenger
  Fixed bug 064 and various other problems with drawing piled views;
  added related debug code.

  Revision 1.82  1996/12/30 23:51:15  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.81  1996/12/27 17:36:28  wenger
  Fixed some problems with XWindowRep::ScaledText(); added some more stuff
  to the cslib example server.

  Revision 1.80  1996/12/15 20:22:31  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.79  1996/12/03 17:00:21  jussi
  Added SetFont() for generic font support. Removed SetSmallFont().

  Revision 1.78  1996/11/26 16:47:49  ssl
  Added support for Stacked Opaque and Transparent views

  Revision 1.77  1996/11/26 09:31:30  beyer
  If control or alt key used, key is always sent as upper case.

  Revision 1.76  1996/11/23 00:24:14  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.75  1996/11/20 20:34:57  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.74  1996/11/18 23:11:22  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.73  1996/11/13 16:56:21  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.72  1996/10/28 15:55:49  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.71  1996/10/18 20:34:13  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.70  1996/09/26 20:39:24  jussi
  Removed duplicate default argument.

  Revision 1.69  1996/09/18 20:16:56  guangshu
  Modified function ExportGIF.

  Revision 1.68  1996/09/10 20:07:14  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.67  1996/09/09 14:31:42  jussi
  Added #ifdef LIBCS statements to make code compile in the
  ClientServer library target.

  Revision 1.66  1996/09/06 06:59:47  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.65  1996/09/05 19:11:44  jussi
  Added handling of GraphicsExpose and NoExpose events.

  Revision 1.64  1996/09/04 21:24:51  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.63  1996/08/29 22:13:24  guangshu
  Changed arguments of ConvertAndWriteGIF and ExportGIF.

  Revision 1.62  1996/08/29 18:24:32  wenger
  A number of Dali-related improvements: ShapeAttr1 now specifies image
  type when shape is 'image'; added new '-bytes' flag to Dali commands
  when sending images; TDataBinaryInterp now uses StringStorage so GData
  can access strings; fixed hash function for StringStorage so having the
  high bit set in a byte in the string doesn't crash the hash table;
  improved the error checking in some of the Dali code.

  Revision 1.61  1996/08/28 00:19:39  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.60  1996/08/23 16:56:00  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.59  1996/08/08 20:59:55  beyer
  fixed some debugging problems

  Revision 1.58  1996/08/07 20:11:26  wenger
  Fixed various key event-related bugs (see Bugs file); changed
  direction of camera movement for 3D to be more in agreement
  with how panning works for 2D views.

  Revision 1.57  1996/08/05 19:51:41  wenger
  Fixed compile errors caused by some of Kevin's recent changes; changed
  the attrproj stuff to make a .a file instead of a .o; added some more
  TData file writing stuff; misc. cleanup.

  Revision 1.56  1996/08/05 02:03:03  beyer
  The query record popup wasn't display anything.

  Revision 1.55  1996/08/04 23:34:01  beyer
  Devise keys now support both X11R5 and X11R6

  Revision 1.54  1996/08/04 22:20:01  jussi
  Moved variable declarations in HandleEvent out of the switch
  statement. Newer compiler issued warnings for these.

  Revision 1.53  1996/08/04 21:06:39  beyer
  Added support for portable devise keys

  Revision 1.52  1996/08/03 16:43:41  jussi
  Changed backing_store flag.

  Revision 1.51  1996/07/23 15:34:45  jussi
  Added mechanism for bypassing the Devise internal color map.

  Revision 1.50  1996/07/22 19:32:04  jussi
  Requesting a pixmap of a window that is partially off the screen
  no longer causes Devise to crash.

  Revision 1.49  1996/07/18 01:24:10  jussi
  Added GetRootGeometry() and FindTopWindow() methods.

  Revision 1.48  1996/07/15 21:02:19  jussi
  Fixed Arc() computation.

  Revision 1.47  1996/07/15 00:30:21  jussi
  Added handling of ButtonRelease events when not in RAWMOUSEEVENTS
  mode.

  Revision 1.46  1996/07/14 16:50:49  jussi
  Added handling of DestroyNotify events.

  Revision 1.45  1996/07/14 04:08:50  jussi
  Added handling of ReparentNotify events and WM_DELETE_WINDOW
  messages from the window manager.

  Revision 1.44  1996/07/10 16:21:12  jussi
  Improvements and simplifications to the code.

  Revision 1.43  1996/06/21 21:33:43  jussi
  Changed xpr parameters so that a grayscale Postscript image is
  produced instead of black and white.

  Revision 1.42  1996/06/21 19:31:07  jussi
  Moved all 3D-related code to Map3D.C and Map3D.h.
  Replaced MinMax calls with calls to MIN() and MAX().

  Revision 1.41  1996/06/16 01:52:54  jussi
  Minor improvements here and there, mostly debugging
  statements.

  Revision 1.40  1996/06/15 13:49:56  jussi
  Added SetWindowBgColor() which allows Devise to change
  the view background color at runtime.

  Revision 1.39  1996/06/15 06:59:43  yuc
  Add "clipping" to MapAllXpoints, move the code related to perspective
  view to CompProjectionOnViewingPlane, add aim axis (or focus axis) to
  DrawRefAxis.

  Revision 1.38  1996/05/31 20:54:24  jussi
  Fixed computation in Arc().

  Revision 1.37  1996/05/31 15:32:12  jussi
  Added 'state' variable to HandleButton(). This tells the callee
  whether the shift/control keys were pressed in conjunction with
  the mouse button.

  Revision 1.36  1996/05/22 21:02:47  jussi
  Added ImportImage() method.

  Revision 1.35  1996/05/20 18:45:04  jussi
  Merged with ClientServer library code.

  Revision 1.34  1996/05/20 17:27:36  jussi
  Removed platform dependency in ExportImage().

  Revision 1.33  1996/04/20 19:52:29  kmurli
  Changed Viex.c to use a pipe mechanism to call itself if it needs to be
  done again. The view now is not called contiously by the Dispatcher,instead
  only of there is some data in the pipe.
  The pipe mechanism is implemented transparently through static functions
  in the Dispatcher.c (InsertMarker,CreateMarker,CloseMarker,FlushMarker)

  Revision 1.32  1996/04/18 18:17:01  jussi
  Added support for drawing into pixmaps instead of windows.

  Revision 1.31  1996/04/17 20:33:41  jussi
  Added ExportGIF() method.

  Revision 1.30  1996/04/16 19:49:41  jussi
  Replaced assert() calls with DOASSERT().

  Revision 1.29  1996/04/11 17:54:28  jussi
  Added Raise() and Lower().

  Revision 1.28  1996/03/29 18:14:05  wenger
  Got testWindowRep to compile and run, added drawing in
  windows; fixed a few more compile warnings, etc.

  Revision 1.27  1996/03/27 17:55:02  wenger
  Changes to get DEVise to compile and run on Linux.

  Revision 1.26  1996/03/26 15:34:41  wenger
  Fixed various compile warnings and errors; added 'clean' and
  'mostlyclean' targets to makefiles; changed makefiles so that
  object lists are derived from source lists.

  Revision 1.25  1996/02/28 17:37:34  yuc
  Added CompLocationOnViewingSpace, CompProjectionOnViewingPlane,
  DrawXSegments, MapAllPoints, MapAllSegments, and DrawRefAxis.  These
  functions are for 3D display.

  Revision 1.24  1996/02/26 23:46:08  jussi
  Added GetSmallFontHeight().

  Revision 1.23  1996/02/26 16:42:54  jussi
  Makes 10 attempts to export an image.

  Revision 1.22  1996/02/06 19:24:25  jussi
  Restore line attributes (line width and join style).

  Revision 1.21  1996/02/05 23:55:20  jussi
  Added support for small fonts.

  Revision 1.20  1996/01/31 21:35:31  jussi
  Refined Xor stuff.

  Revision 1.19  1996/01/30 21:13:51  jussi
  Removed references to specific colors.

  Revision 1.18  1996/01/30 00:04:17  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.17  1996/01/17 22:40:22  jussi
  One more refinement.

  Revision 1.16  1996/01/17 22:18:32  jussi
  Refined image export support.

  Revision 1.15  1996/01/17 21:05:14  jussi
  Minor fix for SGI EPS output.

  Revision 1.14  1996/01/17 20:53:34  jussi
  Added support for additional image export formats.

  Revision 1.13  1995/12/28 18:59:23  jussi
  Small fixes to remove compiler warnings.

  Revision 1.12  1995/12/18 03:13:13  ravim
  Inverted forms of drawing lines and rectangles.

  Revision 1.11  1995/12/14 21:14:02  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.10  1995/12/14 17:22:01  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.9  1995/12/06 21:24:09  jussi
  Minor bug fixes in computing exact pixel values (removed some one-off
  errors).

  Revision 1.8  1995/12/05 22:02:01  jussi
  DoPopup now uses AbsoluteOrigin to compute the absolute pixel
  position of a popup window rather than duplicate the code.
  Minor other improvements in debugging output.

  Revision 1.7  1995/12/02 21:28:01  jussi
  Added support for TK_WINDOW but the first implementation is
  obsoleted and renamed TK_WINDOW_old. Added various debugging
  output statements.

  Revision 1.6  1995/11/29 15:11:15  jussi
  Minor fix to really #ifdef out WritePostscript debugging output.

  Revision 1.5  1995/11/28 00:00:43  jussi
  Added WritePostscript() method.

  Revision 1.4  1995/11/25 01:16:18  jussi
  Replaced calls to bcopy() with more portable memcpy()'s.

  Revision 1.3  1995/11/24 21:29:33  jussi
  Added copyright notice and cleaned up code. Added debugging
  statements.

  Revision 1.2  1995/09/05 21:13:41  jussi
  Added/updated CVS header.
*/

#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <X11/keysym.h>

#include "XWindowRep.h"
#include "XDisplay.h"
#include "Compress.h"
#include "DevError.h"
#include "ETkIfc.h"
#include "DaliIfc.h"
#include "../xvertext/rotated.h"
#include "XColor.h"

#if !defined(LIBCS)
#include "Init.h"
#endif

// Defines to get around namespace conflicts with xv code. CEW 5/13/97.
#define String XVString
#define black XVblack
extern "C" {
 #include "xv.h"
}
#undef black
#undef String

// xv.h has a #define DELETE that conflicts with DeviseKey::DELETE
// that is not used in this module, so undef it before including
// DeviseKey.h
#undef DELETE
#include "MapIntToInt.h"
#include "DeviseKey.h"

// xv.h has a conflict DEBUG - it has a variable called int DEBUG
// so this define must come after xv.h.
//#define DEBUG

// Define this to produce some extra drawing to X to help debug stuff...
//#define X_DEBUG

// Uncomment this line to print some debugging info relevant to 
// creation and deletion of embedded Tk windows.
//#define DEBUG_ETK

#include "Util.h"

#define MAXPIXELDUMP 0

#define ROUND(type, value) ((type)(value + 0.5))
#define DRAWABLE           (_win ? _win : _pixmap)

#ifndef IMPLEMENTDLIST_DALIIMAGELIST
#define IMPLEMENTDLIST_DALIIMAGELIST
ImplementDList(DaliImageList, int);
#endif

// key translations
// Removed 'num lock' from AltMask (rkw 8/7/96).
static const int AltMask = Mod1Mask | Mod2Mask | Mod3Mask | Mod5Mask;
static MapIntToInt devise_keymap(103);

/**********************************************************************
Module Initializer
***********************************************************************/

class XWindowRepInit
{
  public:

    struct Xlate {
	int xkey;
	int dkey;
    };

    XWindowRepInit();
};

XWindowRepInit::XWindowRepInit()
{
    static Xlate xlate[] = {
	{ XK_BackSpace,   DeviseKey::BACKSPACE },
	{ XK_Tab,         DeviseKey::TAB },
	{ XK_Linefeed,    DeviseKey::LINEFEED },
	{ XK_Clear,       DeviseKey::CLEAR },
	{ XK_Return,      DeviseKey::RETURN },
	{ XK_Pause,       DeviseKey::PAUSE },
	{ XK_Scroll_Lock, DeviseKey::SCROLL_LOCK },
	{ XK_Escape,      DeviseKey::ESCAPE },
	{ XK_Delete,      DeviseKey::DELETE },

	{ XK_Home,        DeviseKey::HOME },
	{ XK_Left,        DeviseKey::LEFT },
	{ XK_Up,          DeviseKey::UP },
	{ XK_Right,       DeviseKey::RIGHT },
	{ XK_Down,        DeviseKey::DOWN },
	{ XK_Prior,       DeviseKey::PRIOR },
	{ XK_Next,        DeviseKey::NEXT },
	{ XK_End,         DeviseKey::END },
	{ XK_Begin,       DeviseKey::BEGIN },

	{ XK_Select,      DeviseKey::SELECT },
	{ XK_Print,       DeviseKey::PRINT },
	{ XK_Execute,     DeviseKey::EXECUTE },
	{ XK_Insert,      DeviseKey::INSERT },
	{ XK_Undo,        DeviseKey::UNDO },
	{ XK_Redo,        DeviseKey::REDO },
	{ XK_Menu,        DeviseKey::MENU },
	{ XK_Find,        DeviseKey::FIND },
	{ XK_Cancel,      DeviseKey::CANCEL },
	{ XK_Help,        DeviseKey::HELP },
	{ XK_Break,       DeviseKey::BREAK },
	{ XK_Mode_switch, DeviseKey::MODE_SWITCH },
	{ XK_script_switch, DeviseKey::SCRIPT_SWITCH },

	{ XK_KP_Space,    DeviseKey::KP_SPACE },
	{ XK_KP_Tab,      DeviseKey::KP_TAB },
	{ XK_KP_Enter,    DeviseKey::KP_ENTER },
	{ XK_KP_F1,       DeviseKey::KP_F1 },
	{ XK_KP_F2,       DeviseKey::KP_F2 },
	{ XK_KP_F3,       DeviseKey::KP_F3 },
	{ XK_KP_F4,       DeviseKey::KP_F4 },
	{ XK_KP_Equal,    DeviseKey::KP_EQUAL },
	{ XK_KP_Multiply, DeviseKey::KP_MULTIPLY },
	{ XK_KP_Add,      DeviseKey::KP_ADD },
	{ XK_KP_Separator, DeviseKey::KP_SEPARATOR },
	{ XK_KP_Subtract, DeviseKey::KP_SUBTRACT },
	{ XK_KP_Decimal,  DeviseKey::KP_DECIMAL },
	{ XK_KP_Divide,   DeviseKey::KP_DIVIDE },
	{ XK_KP_0,        DeviseKey::KP_0 },
	{ XK_KP_1,        DeviseKey::KP_1 },
	{ XK_KP_2,        DeviseKey::KP_2 },
	{ XK_KP_3,        DeviseKey::KP_3 },
	{ XK_KP_4,        DeviseKey::KP_4 },
	{ XK_KP_5,        DeviseKey::KP_5 },
	{ XK_KP_6,        DeviseKey::KP_6 },
	{ XK_KP_7,        DeviseKey::KP_7 },
	{ XK_KP_8,        DeviseKey::KP_8 },
	{ XK_KP_9,        DeviseKey::KP_9 },

	{ XK_F1,          DeviseKey::F1 },
	{ XK_F2,          DeviseKey::F2 },
	{ XK_F3,          DeviseKey::F3 },
	{ XK_F4,          DeviseKey::F4 },
	{ XK_F5,          DeviseKey::F5 },
	{ XK_F6,          DeviseKey::F6 },
	{ XK_F7,          DeviseKey::F7 },
	{ XK_F8,          DeviseKey::F8 },
	{ XK_F9,          DeviseKey::F9 },
	{ XK_F10,         DeviseKey::F10 },
	{ XK_F11,         DeviseKey::F11 },
	{ XK_F12,         DeviseKey::F12 },

#if defined(XK_Sys_Req)
	// these are not defined in X11R5, but are in X11R6
	{ XK_Sys_Req,     DeviseKey::SYS_REQ },
	{ XK_Page_Up,     DeviseKey::PAGE_UP },
	{ XK_Page_Down,   DeviseKey::PAGE_DOWN },
	{ XK_KP_Home,     DeviseKey::KP_HOME },
	{ XK_KP_Left,     DeviseKey::KP_LEFT },
	{ XK_KP_Up,       DeviseKey::KP_UP },
	{ XK_KP_Right,    DeviseKey::KP_RIGHT },
	{ XK_KP_Down,     DeviseKey::KP_DOWN },
	{ XK_KP_Prior,    DeviseKey::KP_PRIOR },
	{ XK_KP_Page_Up,  DeviseKey::KP_PAGE_UP },
	{ XK_KP_Next,     DeviseKey::KP_NEXT },
	{ XK_KP_Page_Down, DeviseKey::KP_PAGE_DOWN },
	{ XK_KP_End,      DeviseKey::KP_END },
	{ XK_KP_Begin,    DeviseKey::KP_BEGIN },
	{ XK_KP_Insert,   DeviseKey::KP_INSERT },
	{ XK_KP_Delete,   DeviseKey::KP_DELETE },
#endif


	{ 0, 0 }		// must be last entry!
    };

    for(int i = 0 ; xlate[i].xkey != 0 ; i++) {
	DOASSERT(devise_keymap.insert(xlate[i].xkey, xlate[i].dkey) == 0,
		 "couldn't insert devise_keymap entry");
    }
}

static XWindowRepInit xwindowrep_initializer;

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

XWindowRep::XWindowRep(Display* display, Window window, XDisplay* DVDisp,
					   XWindowRep* parent, Boolean backingStore)
	:	WindowRep(DVDisp), _parent(parent)
{
#if defined(DEBUG)
  printf("XWindowRep::XWindowRep(this = 0x%p, parent = %p,
    window = 0x%lx)\n", this, parent, window);
#endif

	_display = display;
	_win = window;
	_myWin = _win;
	_pixmap = 0;
	_myPixmap = 0;

	Init();
}

XWindowRep::XWindowRep(Display* display, Pixmap pixmap, XDisplay* DVDisp,
					   XWindowRep* parent, int x, int y)
	:	WindowRep(DVDisp), _x(x), _y(y), _parent(parent)
{
#if defined(DEBUG)
  printf("XWindowRep::XWindowRep(this = 0x%p, parent = %p,
    pixmap = 0x%lx)\n", this, parent, pixmap);
#endif

  _display = display;
  _win = 0;
  _myWin = 0;
  _pixmap = pixmap;
  _myPixmap = _pixmap;

  Init();
//  ClearPixmap();
}

XWindowRep::~XWindowRep(void)
{
#if defined(DEBUG)
  printf("XWindowRep::~XWindowRep(this = 0x%p)\n", this);
#endif

  XFreeGC(_display, _gc);
  XFreeGC(_display, _rectGc);
  
  FreeBitmap(_srcBitmap);
  FreeBitmap(_dstBitmap);
  
  /* _win or _pixmap is destroyed by XDisplay */
  DOASSERT(_win == 0 && _pixmap == 0, "X window or pixmap not freed");
  DOASSERT(_myWin == 0 && _myPixmap == 0, "X window or pixmap not freed");

  // This should have already been done by XDisplay::DestroyWindowRep(),
  // but do it again here just in case...  If it's already been done,
  // this won't actually do anything.
  (void) DaliFreeImages();
  delete [] _daliServer;

  (void) ETk_FreeWindows();
  delete [] _etkServer;
  
  if (_parent) {
    if (!_parent->_children.Delete(this))
      fprintf(stderr, "Cannot remove child from parent window\n");
  }

  if (_children.Size() > 0)
    reportErrNosys("Child windows should have been destroyed first");
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

void	XWindowRep::SetForeground(PColorID fgid)
{
	XColorID	xfgid = AP_GetXColorID(fgid);

	WindowRep::SetForeground(fgid);

#ifdef GRAPHICS
	if (_dispGraphics)
		XSetForeground(_display, _gc, xfgid);
#endif
}

void	XWindowRep::SetBackground(PColorID bgid)
{
	XColorID	xbgid = AP_GetXColorID(bgid);

	WindowRep::SetBackground(bgid);

#ifdef GRAPHICS
	if (_dispGraphics)
		XSetBackground(_display, _gc, xbgid);
#endif
}

//******************************************************************************
// Utility Functions
//******************************************************************************

void	XWindowRep::SetWindowBackground(PColorID bgid)
{
#ifdef GRAPHICS
	if (_dispGraphics)
		XSetWindowBackground(_display, _win, AP_GetXColorID(bgid));
#endif
}

void	XWindowRep::ClearPixmap(void)
{
#ifdef GRAPHICS
	PColorID	savePColorID = GetForeground();

	SetForeground(GetBackground());
	XFillRectangle(_display, _pixmap, _gc, 0, 0, _width, _height);
	SetForeground(savePColorID);
#endif
}

//******************************************************************************

void XWindowRep::Init()
{
  if (_parent)
    _parent->_children.Append(this);
  _backingStore = false;
  _lineStyle = LineSolid;

  _daliServer = NULL;
  _etkServer = NULL;

#if defined(LIBCS)
  _dispGraphics = true;
#else
  _dispGraphics = Init::DispGraphics();
#endif
  _compress = new SimpleCompress();

  UpdateWinDimensions();

  _unobscured = false;

  _gc = XCreateGC(_display, DRAWABLE, 0, NULL);
  SetCopyMode();
  SetLineWidth(0);

  _rectGc = XCreateGC(_display, DRAWABLE, 0, NULL);
  XSetState(_display, _rectGc, AllPlanes, AllPlanes, GXxor, AllPlanes);
  
  /* set default pattern to pattern 0: no stipple */
  WindowRep::SetPattern(Pattern0);
  XSetFillStyle(_display, _gc, FillSolid);
  
  /* set normal font */
  SetNormalFont(); 
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
 
  /* create pixmaps for character manipulation */
  _srcBitmap.inUse = false;
  _dstBitmap.inUse = false;
  int bitmapWidth = 80 * (fontStruct->max_bounds.rbearing -
			  fontStruct->min_bounds.lbearing);
  int bitmapHeight = fontStruct->max_bounds.ascent +
                     fontStruct->max_bounds.descent;
  AllocBitmap(_srcBitmap, bitmapWidth, bitmapHeight);
  AllocBitmap(_dstBitmap, 3 * bitmapWidth, 3 * bitmapHeight);

  _outWR = NULL; // drawing to our own X window

#ifdef TK_WINDOW_old
  _tkPathName[0] = 0;
  _tkWindow = 0;
#endif
}

/******************************************************************
  If child == true, make window pointed to by 'other' a new child of
  this window.
  If child == false, make window pointed to by 'other' a new parent of
  this window.
*/

void XWindowRep::Reparent(Boolean child, void *other, int x, int y)
{
  DOASSERT(_win, "Cannot reparent a pixmap");

  Window newParent = (child ? _win : (Window)other);
  Window newChild = (child ? (Window)other : _win);

  if (!newParent)
    newParent = DefaultRootWindow(_display);

#ifdef DEBUG
  printf("XWindowRep::Reparent 0x%lx to 0x%lx at %d,%d\n",
	 newChild, newParent, x, y);
#endif

  XReparentWindow(_display, newChild, newParent, x, y);
}

/******************************************************************/

void XWindowRep::PushClip(Coord x, Coord y, Coord w, Coord h)
{
#if defined(DEBUG)
  printf("XWindowRep::PushClip(%.2f,%.2f,%.2f,%.2f)\n", x, y, w, h);
#endif

  Coord xlow, ylow, xhi, yhi, width, height;
  Coord x1, y1, x2, y2;
  Transform(x, y, x1, y1);
  Transform(x + w, y + h, x2, y2);
  xlow = MIN(x1, x2);
  xhi = MAX(x1, x2);
  ylow = MIN(y1, y2);
  yhi = MAX(y1, y2);
  width = xhi - xlow + 1;
  height = yhi - ylow + 1;
  
#if defined(DEBUG)
  printf("XwindowRep::PushClip: transformed into (%.2f,%.2f,%.2f,%.2f)\n",
	 xlow, ylow, width, height);
#endif

  XRectangle rect;
  rect.x = ROUND(short, xlow);
  rect.y = ROUND(short, ylow);
  rect.width = (unsigned)(ROUND(int, xhi) - ROUND(int, xlow) + 1);
  rect.height = (unsigned)(ROUND(int, yhi) - ROUND(int, ylow) + 1);

#ifdef DEBUG
  printf("XwindowRep::PushClip: rounded to (%d,%d,%d,%d)\n",
	 rect.x, rect.y, rect.width, rect.height);
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XSetClipRectangles(_display, _gc, 0, 0, &rect, 1, Unsorted);
#endif

  WindowRep::_PushClip(xlow, ylow, width, height);

  return;
}

/*******************************************************************/

void XWindowRep::PopClip()
{
  WindowRep::_PopClip();
  Coord x, y, w, h;
  XRectangle rect; 

  if (WindowRep::ClipTop(x, y, w, h)) {
    rect.x = ROUND(short, x);
    rect.y = ROUND(short, y);
    rect.width = (unsigned short)ceil(w);
    rect.height = (unsigned short)ceil(h);
#ifdef GRAPHICS
    if (_dispGraphics)
      XSetClipRectangles(_display, _gc, 0, 0, &rect, 1, Unsorted);
#endif

  } else {
    /* no more clipping */
#ifdef GRAPHICS 
    if (_dispGraphics)
      XSetClipMask(_display, _gc, None);
#endif
  }

  return;
}

/* export window image */
/* Changing how this works -- always export to GIF; then if we need
 * PostScript, translate from GIF to PostScript.  RKW June 3, 1997. */

void XWindowRep::ExportImage(DisplayExportFormat format, const char *filename)
{
  DO_DEBUG(printf("XWindowRep::ExportImage(this = %p)\n", this));

  /* Generate the GIF file. */
  const char *gifFile;
  if (format == GIF) {
    gifFile = filename;
  } else if (format == POSTSCRIPT || format == EPS) {
    char tmpFile[L_tmpnam];
    tmpnam(tmpFile);
    gifFile = tmpFile;
  } else {
    printf("Requested export format not supported yet.\n");
    return;
  }

  FILE *fp = fopen(gifFile, "wb");
  if (!fp) {
      reportErrSys("open");
      fprintf(stderr, "Cannot open file %s for writing\n", gifFile);
      return;
  }
  ExportGIF(fp);
  if (fp != stderr && fp != stdout)
      fclose(fp);

  /* If GIF is what we want, we're done here. */
  if (format == GIF) return;

  /* Otherwise, translate the GIF file to PostScript. */
  char cmd[256];
  sprintf(cmd, "giftopnm %s | pnmtops -rle -noturn -nocenter > %s",
    gifFile, filename);

#ifdef DEBUG
  printf("ExportImage: executing %s\n", cmd);
#endif

#if defined(SUN) || defined(HPUX) || defined(ULTRIX)
  int errorcode = 127;
#else
  int errorcode = -1;
#endif

  for(int attempt = 0; attempt < 10; attempt++) {
    int status = system(cmd);
#if defined(LINUX)
    if ((status != -1) && (status != 127))
#else
    if (status != errorcode)
#endif
      break;
    if (errno == EAGAIN && attempt < 10 - 1) {
      perror("system");
      fprintf(stderr, "Retrying to execute command in 1.0 seconds...\n");
      sleep(1);
    } else {
      fprintf(stderr, "Could not execute command (%d attempts):\n",
	      attempt + 1);
      fprintf(stderr, "  %s\n", cmd);
      perror("The reason is");
      break;
    }
  }

  unlink(gifFile);
}

/*--------------------------------------------------------------------------
 * function: XWindowRep::DaliShowImage
 * Show a Dali image in this window.
 */
DevStatus
XWindowRep::DaliShowImage(Coord centerX, Coord centerY, Coord width,
	Coord height, char *filename, int imageLen, char *image,
	float timeoutFactor, Boolean maintainAspect)
{
#if defined(DEBUG)
  printf("XWindowRep::DaliShowImage(%f, %f, %f, %f, %s)\n", centerX, centerY,
    width, height, filename != NULL ? filename : "(image)");
#endif

  DevStatus result = StatusOk;

  if (_daliServer == NULL)
  {
    char *serverName;
    DevStatus tmpResult = DaliIfc::LaunchServer(serverName);
    result += tmpResult;
    if (tmpResult.IsComplete()) {
      GetDisplay()->SetTasvirServer(serverName);
      DeviseDisplay::GetPSDisplay()->SetTasvirServer(serverName);
#if !defined(LIBCS)
      Init::SetDaliServer(serverName);
      Init::SetDaliQuit(true);
#endif
  }
  }

  if (_daliServer == NULL)
  {
    reportError("No Tasvir server specified", devNoSyserr);
    result = StatusFailed;
  }
  else
  {
    if (filename == NULL) filename = "-";

    int handle;
    result += DaliIfc::ShowImage(_daliServer, DRAWABLE, (int) centerX,
      (int) centerY, (int) width, (int) height, filename, imageLen, image,
      handle, timeoutFactor, 1000, maintainAspect);
    if (result.IsComplete())
    {
#if defined(DEBUG)
      printf("Displayed Tasvir image; handle = %d\n", handle);
#endif

      _daliImages.Insert(handle);
    }
  }

  return result;
}

/*--------------------------------------------------------------------------
 * function: XWindowRep::DaliFreeImages
 * Free the Dali images associated with this window.
 */
DevStatus
XWindowRep::DaliFreeImages()
{
#if defined(DEBUG)
  printf("XWindowRep::DaliFreeImages()\n");
#endif

  DevStatus result = StatusOk;

  int index = _daliImages.InitIterator(false);
  while (_daliImages.More(index))
  {
    int handle = _daliImages.Next(index);
    _daliImages.DeleteCurrent(index);
    result += DaliIfc::FreeImage(_daliServer, handle);
  }
  _daliImages.DoneIterator(index);

  return result;
}

//------------------------------------------------------------------------
// BEGIN ETk interface
//

//
// Utility function for searching the ETk window list. Given an ETk
// handle, returns a pointer to the ETkInfo struct for that window,
// or NULL if no window exists with that handle.
//
static ETkInfo *
_etkSearch(ETkWinList *list, int handle)
{
    ETkInfo *current, *result;
    int i;
    result = NULL;
    i = list->InitIterator(false);
    while (list->More(i))
    {
	current = list->Next(i);
	if (current == NULL)
	{
	    continue;
	}
	if (current->handle == handle)
	{
	    result = current;
	}
    }
    list->DoneIterator(i);
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_CreateWindow()
//
// Show an embedded Tcl/Tk window in this WindowRep. filename is the
// name of a Tcl/Tk script to be executed. argc and argv carry the
// command-line arguments that will be passed to the Tcl/Tk script.
//
DevStatus
XWindowRep::ETk_CreateWindow(Coord x, Coord y,
			     Coord width, Coord height,
			     ETkIfc::Anchor anchor,
			     char *filename, int argc, char **argv,
			     int &handle)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_CreateWindow(%s)\n", filename);
#endif
    
    DevStatus result = StatusOk;

    if (!_win) { // Drawing to a pixmap instead of window...
#if defined(DEBUG) || defined(DEBUG_ETK)
      printf("Can't create ETk windows into a pixmap.\n");
#endif
      handle = 0;
      return result;
    }

    if (_etkServer == NULL) {
      char *serverName;
      DevStatus tmpResult = ETkIfc::LaunchServer(serverName);
      result += tmpResult;
      if (tmpResult.IsComplete()) {
        GetDisplay()->SetETkServer(serverName);
        ETkIfc::SetServer(serverName);
        ETkIfc::SetQuitFlag(true);
      }
    }
    
    if (_etkServer == NULL) {
	reportError("No ETk server specified", devNoSyserr);
	result = StatusFailed;
    } else {
	if (filename == NULL)
	    filename = "-";

	int wx = int(x);
	int wy = int(y);
	int wwidth = int(width);
	int wheight = int(height);
	result += ETkIfc::CreateWindow(_etkServer, _win,
				       wx, wy,
				       wwidth, wheight,
				       anchor,
				       filename,
				       argc, (const char **) argv,
				       handle);
	
	if (result.IsComplete()) {
#if defined(DEBUG) || defined(DEBUG_ETK)
	    printf("Displayed ETk window. handle = %d\n", handle);
#endif

	    ETkInfo *info = new ETkInfo;
	    DOASSERT(info, "Cannot create ETkInfo object");
	    info->handle = handle;
	    info->x = wx;
	    info->y = wy;
	    info->width = wwidth;
	    info->height = wheight;
	    info->in_use = true;
	    strncpy(info->script, filename, FILENAME_MAX);
	    info->script[FILENAME_MAX] = '\0';
	    _etkWindows.Insert(info);
	}
    }
    
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_FindWindow()
//
// Searches for an embedded Tk window at the specified coordinates.
// Returns a handle for the window, or -1 if none exists.
// NOTE: the x,y coords are DATA coords, not pixel values.
//
int
XWindowRep::ETk_FindWindow(Coord centerX, Coord centerY, char *script)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_FindWindow(%f,%f,%s)\n",
	   centerX, centerY, script);    
#endif
    int iter, handle;
    ETkInfo *info;
    
    iter = _etkWindows.InitIterator(false);
    handle = -1;
    
    while (_etkWindows.More(iter))
    {
	info = _etkWindows.Next(iter);
	if (info == NULL)
	{
	    reportError("Invalid ETkInfo pointer in ETkWindow list",
			devNoSyserr);
	    continue;
	}
	if (info->x == centerX && info->y == centerY
	    && !strcmp(info->script, script))
	{
	    handle = info->handle;
	    break;
	}
    }
    _etkWindows.DoneIterator(iter);

    return handle;

}

//------------------------------------------------------------------------
// XWindowRep::ETk_FreeWindows()
//
// Free all embedded Tk windows associated with this window.
//
DevStatus
XWindowRep::ETk_FreeWindows()
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_FreeWindows()\n");
#endif
    DevStatus result = StatusOk;
    int iter, handle;
    
    ETkInfo *info;
    iter = _etkWindows.InitIterator(false);
    while (_etkWindows.More(iter))
    {
	info = _etkWindows.Next(iter);
	if (info == NULL)
	{
	    reportError("NULL ETkInfo pointer found in ETkWindowList",
			devNoSyserr);
	    result = StatusFailed;
	    break;
	}
	_etkWindows.DeleteCurrent(iter);
	result += ETkIfc::SendCommand(_etkServer, "free",
				      0, NULL, info->handle);
	delete info;
    }
    _etkWindows.DoneIterator(iter);
    
    return result;

}

//------------------------------------------------------------------------
// XWindowRep::ETk_Cleanup()
//
// Free all embedded Tk windows that have in_use == false
//
DevStatus
XWindowRep::ETk_Cleanup()
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_Cleanup()\n");
#endif
    DevStatus result = StatusOk;
    int iter, handle;
    
    ETkInfo *info;
    iter = _etkWindows.InitIterator(false);
    while (_etkWindows.More(iter))
    {
	info = _etkWindows.Next(iter);
	if (info == NULL)
	{
	    reportError("NULL ETkInfo pointer found in ETkWindowList",
			devNoSyserr);
	    result = StatusFailed;
	    break;
	}
	if (info->in_use == false)
	{
	    _etkWindows.DeleteCurrent(iter);
	    result += ETkIfc::SendCommand(_etkServer, "free",
					  0, NULL, info->handle);
	    delete info;
	}
    }
    _etkWindows.DoneIterator(iter);
    
    return result;

}

//------------------------------------------------------------------------
// XWindowRep::ETk_MarkAll()
//
// Set the in_use flag for all embedded Tk windows
//
DevStatus
XWindowRep::ETk_MarkAll(bool in_use)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_MarkAll()\n");
#endif
    DevStatus result = StatusOk;
    int iter;
    
    ETkInfo *info;
    iter = _etkWindows.InitIterator(false);
    while (_etkWindows.More(iter))
    {
	info = _etkWindows.Next(iter);
	if (info == NULL)
	{
	    reportError("NULL ETkInfo pointer found in ETkWindowList",
			devNoSyserr);
	    continue;
	}
	info->in_use = in_use;
    }
    _etkWindows.DoneIterator(iter);
    
    return result;

}

//------------------------------------------------------------------------
// XWindowRep::ETk_Mark()
//
// Set the in_use flag for a single embedded Tk window
//
//
DevStatus
XWindowRep::ETk_Mark(int handle, bool in_use)
{
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_Mark()\n");
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to mark an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    info->in_use = in_use;
    return StatusOk;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_FreeWindow()
//
// Frees a single embedded Tk window
//
//
DevStatus
XWindowRep::ETk_FreeWindow(int handle)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_FreeWindow(%d)\n", handle);
#endif
    
    ETkInfo *info;
    DevStatus result = StatusOk;
    int iter, current;
    iter = _etkWindows.InitIterator(false);
    while (_etkWindows.More(iter))
    {
	info = _etkWindows.Next(iter);
	if (info == NULL)
	{
	    break;
	}
	if (info->handle == handle)
	{
	    _etkWindows.DeleteCurrent(iter);
	    delete info;
	    _etkWindows.DoneIterator(iter);
	    result += ETkIfc::SendCommand(_etkServer, "free",
					  0, NULL, handle);
	    return result;
	}
    }
    _etkWindows.DoneIterator(iter);
    reportError("Attempt to delete an invalid ETk window", devNoSyserr);
    result = StatusFailed;
    return result;
    
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_EvalCmd()
//
// Sends a command to the ETk window for evaluation inside the Tcl
// interpreter. If successful, result is stored in static area pointed
// to by returnValue.
//
DevStatus
XWindowRep::ETk_EvalCmd(int handle, int argc, char **argv,
			char *&returnValue)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_EvalCmd(%d)\n", handle);
#endif

    DevStatus result = StatusOk;
    ETkInfo *info;
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to operate on an invalid ETk window",
		    devNoSyserr);
	return StatusFailed;
    }

    result += ETkIfc::EvalCommand(_etkServer, handle,
				  argc, (const char **) argv,
				  returnValue);
    
    return result;

}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_MapWindow()
//
// Maps a previously created embedded Tk window
//
DevStatus
XWindowRep::ETk_MapWindow(int handle)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_MapWindow(%d)\n", handle);
#endif
    DevStatus result = StatusOk;
    ETkInfo *info;
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to map an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::SendCommand(_etkServer, "map",
				  0, NULL, handle);
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_UnmapWindow()
//
// Unmaps a previously created embedded Tk window
//
DevStatus
XWindowRep::ETk_UnmapWindow(int handle)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_UnmapWindow(%d)\n", handle);
#endif
    DevStatus result = StatusOk;
    ETkInfo *info;
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to unmap an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::SendCommand(_etkServer, "unmap",
				  0, NULL, handle);
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_MoveWindow()
//
// Moves an embedded Tk window
//
DevStatus
XWindowRep::ETk_MoveWindow(int handle, Coord centerX, Coord centerY)
{
    DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_MoveWindow(%d,%d,%d)\n", handle,
	   (int) centerX, (int) centerY);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to move an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::MoveWindow(_etkServer, handle,
				 (int) centerX, (int) centerY);
    if (result.IsComplete())
    {
	info->x = centerX;
	info->y = centerY;
    }
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_ResizeWindow()
//
// Resizes an embedded Tk window
//
DevStatus
XWindowRep::ETk_ResizeWindow(int handle, Coord width, Coord height)
{
    DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_ResizeWindow(%d,%d,%d)\n", handle,
	   (int) width, (int) height);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to resize an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::ResizeWindow(_etkServer, handle,
				   (int) width, (int) height);
    if (result.IsComplete())
    {
	info->width = width;
	info->height = height;
    }
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_NotifyResize()
//
// Allows ETk clients to notify DEVise when window size changes
//
DevStatus
XWindowRep::ETk_NotifyResize(int handle, Coord xcenter, Coord ycenter,
			     Coord width, Coord height)
{
    DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_NotifyResize(%d,%d,%d)\n", handle,
	   (int) width, (int) height);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to resize an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    info->x = xcenter;
    info->y = ycenter;
    info->width = width;
    info->height = height;
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_MoveResizeWindow()
//
// Moves and resizes an embedded Tk window
//
DevStatus
XWindowRep::ETk_MoveResizeWindow(int handle,
				 Coord xcenter, Coord ycenter,
				 Coord width, Coord height)
{
    DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_MoveResizeWindow(%d,%d,%d,%d,%d)\n",
	   handle, (int) xcenter, (int) ycenter,
	   (int) width, (int) height);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to resize an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::MoveResizeWindow(_etkServer, handle,
				       (int) xcenter, (int) ycenter,
				       (int) width, (int) height);
    if (result.IsComplete())
    {
	info->x = xcenter;
	info->y = ycenter;
	info->width = width;
	info->height = height;
    }
    return result;
}

//------------------------------------------------------------------------
//
// XWindowRep::ETk_GetInfo()
//
// Get info (coordinates, name of Tcl script) about an embedded Tk window
//
DevStatus
XWindowRep::ETk_GetInfo(int handle, ETkInfo &info)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("XWindowRep::ETk_GetInfo(%d)\n", handle);
#endif
    
    DevStatus result = StatusOk;
    ETkInfo *pInfo;
    if ((pInfo = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to retrieve info for an invalid ETk window",
		    devNoSyserr);
	return StatusFailed;
    }
    info = *pInfo;
    return result;

}

//
// END ETk interface
// -----------------------------------------------------------------------


/* get geometry of root window enclosing this window */

void XWindowRep::GetRootGeometry(int &x, int &y,
                                 unsigned int &w, unsigned int &h)
{
  if (_pixmap) {
    Origin(x, y);
    Dimensions(w, h);
    return;
  }

  Window root = FindTopWindow(_win);
  Window dummy;
  unsigned int border_width, depth;
  XGetGeometry(_display, root, &dummy, &x, &y, &w, &h, &border_width,
               &depth);
#ifdef DEBUG
  printf("Geometry of root 0x%lx of XWin 0x%p is %d,%d,%u,%u\n",
         root, this, x, y, w, h);
#endif
}

/* find top window */

Window XWindowRep::FindTopWindow(Window win)
{
  Window parent, root;
  Window *children;
  unsigned int nchildren;
  if (!XQueryTree(_display, win, &root, &parent,
                  &children, &nchildren)) {
    DOASSERT(0, "XQueryTree failed");
  }
  if (children)
    XFree(children);

  /* if the root window (display) is the parent of the current 
     window, we've found the top enclosing window enclosing */

  if (parent == root)
    return win;

  return FindTopWindow(parent);
}

/* export image as GIF */

void XWindowRep::ExportGIF(FILE *fp, int isView)
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::ExportGIF()\n", this);
#endif

  if (_win) {
    Window win;
/* Don't do FindTopWindow so we don't print the window manager border.
 * Also, for some reason, windows with Embedded Tk symbols don't work
 * if we do FindTopWindow().  RKW June 4, 1997. */
#if 1
    win = _win;
#else
    if (!isView) {
      win = FindTopWindow(_win);
    } else {
      win = _win;
    }
#endif
    XWindowAttributes xwa;
    if (!XGetWindowAttributes(_display, win, &xwa)) {
      fprintf(stderr, "Cannot get window attributes\n");
      return;
    }
    XRaiseWindow(_display, win);
    GetDisplay()->ConvertAndWriteGIF(win, xwa, fp);
    SetGifDirty(false);
    return;
  }

  // Cannot get attributes of pixmap using XGetWindowAttributes
  // so let's get the attributes from the root window and then
  // patch xwa with the pixmap's information.

  XWindowAttributes xwa;
  if (!XGetWindowAttributes(_display, DefaultRootWindow(_display), &xwa)) {
    fprintf(stderr, "Cannot get window attributes\n");
    return;
  }

  xwa.x = _x;
  xwa.y = _y;
  xwa.width = _width;
  xwa.height = _height;

  // Wow we must copy all child windows to this window because they
  // are stored in separate (non-overlapping) pixmaps; we want the
  // dump to look the same as on screen where the windows would
  // overlap.

  CoalescePixmaps(this);

  GetDisplay()->ConvertAndWriteGIF(_pixmap, xwa, fp);
  SetGifDirty(false);
}

void XWindowRep::CoalescePixmaps(XWindowRep *root)
{
  if (!root->GetPixmapId())
    return;

  // recursively copy the contents of sub-drawables onto the parent

  int index = root->_children.InitIterator();

  while(root->_children.More(index)) {

    XWindowRep *win = root->_children.Next(index);
    if (!win->GetPixmapId())
      continue;

    CoalescePixmaps(win);

#ifdef DEBUG
    printf("Copying from 0x%p to 0x%p, width %u, height %u, x %d, y %d\n",
	   win, root, win->_width, win->_height, win->_x, win->_y);
#endif
    XCopyArea(root->_display, win->GetPixmapId(), root->GetPixmapId(),
	      root->_gc, 0, 0, win->_width, win->_height, win->_x, win->_y);
  }

  root->_children.DoneIterator(index);
}

/* drawing primitives */

void XWindowRep::DrawPixel(Coord x, Coord y)
{
  Coord tx, ty;
  Transform(x,y,tx,ty);

#ifdef DEBUG
  printf("XWindowRep::DrawPixel: %.2f %.2f --> %.2f %.2f\n", x, y, tx, ty);
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XDrawPoint(_display, DRAWABLE, _gc, ROUND(int, tx), ROUND(int, ty));
#endif
}

void XWindowRep::DrawPixelArray(Coord *x, Coord *y, int num, int width)
{
#ifdef DEBUG
  printf("XWindowRep::DrawPixelArray: %d points, width = %d\n", num, width);

  printf("Transformation matrix: ");
  WindowRep::PrintTransform();
  printf("\n");

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", x[k], y[k]);
  }
  printf("\n");
#endif
#endif

  if (width == 1) {
    DOASSERT(num <= WINDOWREP_BATCH_SIZE, "points array will overflow");
    struct XPoint points[WINDOWREP_BATCH_SIZE];
    for(int i = 0; i < num; i++) {
      Coord tx, ty;
      Transform(x[i], y[i], tx, ty);
      points[i].x = ROUND(short, tx);
      points[i].y = ROUND(short, ty);
    }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
    printf("\nAfter transformation:\n\n");
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 10 == 0)
	printf("\n");
      printf("(%d,%d)",points[k].x, points[k].y);
    }
    printf("\n");
#endif
#endif

#ifdef GRAPHICS
    if (_dispGraphics)
      XDrawPoints(_display, DRAWABLE, _gc, points, num, CoordModeOrigin);
#endif

    return;
  }

  /* each "pixel" is wider than one screen pixel */

  int halfWidth = width/2;
  DOASSERT(num <= WINDOWREP_BATCH_SIZE, "rectAngles array will overflow");
  XRectangle rectAngles[WINDOWREP_BATCH_SIZE];
  for(int i = 0; i < num; i++) {
    Coord tx,ty;
    Transform(x[i],y[i],tx,ty);
    rectAngles[i].x = ROUND(short, tx - halfWidth);
    rectAngles[i].y = ROUND(short, ty - halfWidth);
    rectAngles[i].width = width;
    rectAngles[i].height = width;
  }
  
#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation 2:\n\n");
  for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%d,%d)", rectAngles[k].x, rectAngles[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XFillRectangles(_display, DRAWABLE, _gc, rectAngles, num);
#endif
}

/* Fill rectangles, variable width/height */

void XWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
			       Coord *height, int num,
			       SymbolAlignment alignment, Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::FillRectArray: %d points\n", num);

  printf("Transformation matrix: ");
  WindowRep::PrintTransform();
  printf("\n");

#if MAXPIXELDUMP > 0
  {
    printf("\nBefore transformation:\n\n");
    int k;
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 6 == 0)
        printf("\n");
      printf("(%.2f,%.2f,%.2f,%.2f)", xlow[k], ylow[k], width[k], height[k]);
    }
    printf("\n");
  }
#endif
#endif

  DOASSERT(num <= WINDOWREP_BATCH_SIZE, "rectAngles array will overflow");
  XRectangle rectAngles[WINDOWREP_BATCH_SIZE];
  for(int i = 0; i < num; i++) {
    Coord x1, y1, x2, y2;
#if defined(DEBUG)
    printf("XWindowRep::FillRectArray: got (x,y,w,h)=(%g, %g, %g, %g)\n",
	   xlow[i], ylow[i], width[i], height[i]);
    PrintTransform();
  printf("\n");
#endif

    Transform(xlow[i], ylow[i], x1, y1);
    Transform(xlow[i] + width[i], ylow[i] + height[i], x2, y2);

    // possible overflow when these numbers get rounded, so clip to MAXSHORT
    // to avoid any trouble
    x1 = MIN(MAX(x1, -MAXSHORT), MAXSHORT);
    y1 = MIN(MAX(y1, -MAXSHORT), MAXSHORT);
    x2 = MIN(MAX(x2, -MAXSHORT), MAXSHORT);
    y2 = MIN(MAX(y2, -MAXSHORT), MAXSHORT);

    unsigned pixelWidth = (unsigned) ABS(x1 - x2 + 1);
    pixelWidth = MAX(pixelWidth, 1);
    unsigned pixelHeight = (unsigned) ABS(y1 - y2 + 1);
    pixelHeight = MAX(pixelHeight, 1);
    
    rectAngles[i].x = ROUND(short, x1);
    rectAngles[i].y = ROUND(short, y1);
    rectAngles[i].width = ROUND(unsigned short, pixelWidth);
    rectAngles[i].height = ROUND(unsigned short, pixelHeight);
    
#if defined(DEBUG)
    printf("XWindowRep::FillRectArray: draw (x,y,w,h)=(%d, %d, %d, %d)\n",
	   rectAngles[i].x, rectAngles[i].y,
	   rectAngles[i].width, rectAngles[i].height); 
#endif
  }

#if defined(DEBUG)
#if MAXPIXELDUMP > 0
  {
    printf("\nAfter transformation:\n\n");
    int k;
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 6 == 0)
        printf("\n");
      printf("(%d,%d,%d,%d)", rectAngles[k].x, rectAngles[k].y,
	     rectAngles[k].width, rectAngles[k].height);
    }
    printf("\n");
  }
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics) {
      // special-case the no-fill rectangle
      bool fill = (GetPattern() != -Pattern1);
      DrawRectangles(rectAngles, num, fill, alignment, orientation);
  }
#endif
}

/* Fill rectangles, same width/height */

void XWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord width,
			       Coord height, int num,
			       SymbolAlignment alignment, Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::FillRectArray: %d points, width %.2f, height %.2f\n",
         num, width, height);

#if MAXPIXELDUMP > 0
  {
    printf("\nBefore transformation:\n\n");
    int k;
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 10 == 0)
        printf("\n");
      printf("(%.2f,%.2f)", xlow[k], ylow[k]);
    }
    printf("\n");
  }
#endif
#endif

  DOASSERT(num <= WINDOWREP_BATCH_SIZE, "rectAngles array will overflow");
  XRectangle rectAngles[WINDOWREP_BATCH_SIZE];
  for(int i = 0; i < num; i++) {
    Coord x1,y1,x2,y2;
    Transform(xlow[i], ylow[i], x1, y1);
    Transform(xlow[i] + width, ylow[i] + height, x2, y2);

    // possible overflow when these numbers get rounded, so clip to MAXSHORT
    // to avoid any trouble
    x1 = MIN(MAX(x1, -MAXSHORT), MAXSHORT);
    y1 = MIN(MAX(y1, -MAXSHORT), MAXSHORT);
    x2 = MIN(MAX(x2, -MAXSHORT), MAXSHORT);
    y2 = MIN(MAX(y2, -MAXSHORT), MAXSHORT);

    unsigned pixelWidth = (unsigned) ABS(x1 - x2 + 1);
    pixelWidth = MAX(pixelWidth, 1);
    unsigned pixelHeight = (unsigned) ABS(y1 - y2 + 1);
    pixelHeight = MAX(pixelHeight, 1);
    
    rectAngles[i].x = ROUND(short, x1);
    rectAngles[i].y = ROUND(short, y1);
    rectAngles[i].width = ROUND(unsigned short, pixelWidth);
    rectAngles[i].height = ROUND(unsigned short, pixelHeight);
  }

#if defined(DEBUG)
#if MAXPIXELDUMP > 0
  {
    printf("\nAfter transformation: width %d, height %d\n\n",
	   rectAngles[0].width, rectAngles[0].height);
    int k;
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 10 == 0)
        printf("\n");
      printf("(%d,%d)", rectAngles[k].x, rectAngles[k].y);
    }
    printf("\n");
  }
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    DrawRectangles(rectAngles, num, true, alignment, orientation);
#endif
}

void XWindowRep::FillRect(Coord xlow, Coord ylow, Coord width, 
			  Coord height, CursorStore *cstore)
{
#if defined(DEBUG)
  printf("XWindowRep::FillRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
         xlow, ylow, width, height);
#endif

  /* XXX: need to clip rect against window dimensions */

  Coord txlow, tylow, txmax, tymax;
  Coord x1, y1, x2, y2;
  Transform(xlow, ylow + height, x1, y1);
  Transform(xlow + width, ylow, x2, y2);
  txlow = MIN(x1, x2);
  txmax = MAX(x1, x2);
  tylow = MIN(y1, y2);
  tymax = MAX(y1, y2);
  
  /* fill rectangle, remember that the window coordinate
     system starts at the upper left corner */

  unsigned pixelWidth = (unsigned)(ROUND(int, txmax) - ROUND(int, txlow) + 1);
  if (pixelWidth == 0)
    pixelWidth = 1;
  unsigned pixelHeight = (unsigned)(ROUND(int, tymax) - ROUND(int, tylow) + 1);
  if (pixelHeight == 0)
    pixelHeight = 1;
  
#ifdef DEBUG
  printf("After transformation: x %d, y %d, width %d, height %d\n",
	 ROUND(int, txlow), ROUND(int, tylow), pixelWidth, pixelHeight);
#endif

#ifdef GRAPHICS
  if (_dispGraphics) {
      // special-case the no-fill rectangle
      bool fill = (GetPattern() != -Pattern1);
      if( fill ) {
	  XFillRectangle(_display, DRAWABLE, _gc, ROUND(int, txlow),
			 ROUND(int, tylow), pixelWidth, pixelHeight);
      }
      // do I need to reduce the bounding boxes height & width by 1??
      // draw bounding boxes
      if( GetLineWidth() >= 0 || !fill ) {
	  XGCValues gc_values;
	  XGetGCValues(_display, _gc, GCFillStyle, &gc_values);
	  XSetFillStyle(_display, _gc, FillSolid);
	  XDrawRectangle(_display, DRAWABLE, _gc, ROUND(int, txlow),
			 ROUND(int, tylow), pixelWidth, pixelHeight);
	  XSetFillStyle(_display, _gc, gc_values.fill_style);
      }
  }
#endif
}

void XWindowRep::FillRectAlign(Coord xlow, Coord ylow, Coord width, 
			       Coord height,
			       SymbolAlignment alignment, Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::FillRectAlign: x %.2f, y %.2f, width %.2f, height %.2f\n",
         xlow, ylow, width, height);
#endif

  /* XXX: need to clip rect against window dimensions */

  Coord x1, y1, x2, y2;
  Transform(xlow, ylow, x1, y1);
  Transform(xlow + width, ylow + height, x2, y2);

  // possible overflow when these numbers get rounded, so clip to MAXSHORT
  // to avoid any trouble
  x1 = MIN(MAX(x1, -MAXSHORT), MAXSHORT);
  y1 = MIN(MAX(y1, -MAXSHORT), MAXSHORT);
  x2 = MIN(MAX(x2, -MAXSHORT), MAXSHORT);
  y2 = MIN(MAX(y2, -MAXSHORT), MAXSHORT);

  unsigned pixelWidth = (unsigned) ABS(x1 - x2 + 1);
  pixelWidth = MAX(pixelWidth, 1);
  unsigned pixelHeight = (unsigned) ABS(y1 - y2 + 1);
  pixelHeight = MAX(pixelHeight, 1);

#ifdef DEBUG
  printf("After transformation: x %d, y %d, width %d, height %d\n",
         ROUND(int, x1), ROUND(int, y1), pixelWidth, pixelHeight);
#endif

#ifdef GRAPHICS
  if (_dispGraphics) {
      // special-case the no-fill rectangle
      bool fill = (GetPattern() != -Pattern1);
      DrawRectangle(ROUND(int, x1), ROUND(int, y1), pixelWidth,
		    pixelHeight, fill, alignment, orientation);
  }
#endif
}

/* Fill rectangle. All coordinates are in pixels. x and y are
   at the center of the rectangle */

void XWindowRep::FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			       Coord minWidth, Coord minHeight,
			       SymbolAlignment alignment, Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::FillPixelRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
         x, y, width, height);
  printf("  alignment: %d, orientation: %f\n", (int) alignment, orientation);
#endif

  unsigned pixelWidth = (unsigned)MAX(minWidth, width);
  unsigned pixelHeight = (unsigned)MAX(minHeight, height);

#if defined(DEBUG)
  printf("After transformation: x %d, y %d, width %d, height %d\n",
         ROUND(int, x), ROUND(int, y), pixelWidth, pixelHeight);
#endif

#ifdef GRAPHICS
  if (_dispGraphics) {
      bool fill = (GetPattern() != -Pattern1);
      DrawRectangle(ROUND(int, x), ROUND(int, y), pixelWidth,
		    pixelHeight, fill, alignment, orientation);
  }
#endif
}

/*************************************************************************
XXX: need to clip polygon against the window because a large polygon
can overlow the window's integer coordinate system.
*************************************************************************/

void XWindowRep::FillPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("XwindowRep::FillPoly: %d points\n", n);

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", pts[k].x, pts[k].y);
  }
  printf("\n");
#endif
#endif

  if (n <= 0)
    return;

  if (n > WINDOWREP_BATCH_SIZE) {
    printf("Point array too large: %d > %d\n", n, WINDOWREP_BATCH_SIZE);
    n = WINDOWREP_BATCH_SIZE;
  }

  DOASSERT(n <= WINDOWREP_BATCH_SIZE, "points array will overflow");
  struct XPoint points[WINDOWREP_BATCH_SIZE];
  for(int i = 0; i < n; i++) {
    Coord tx, ty;
    Transform(pts[i].x, pts[i].y, tx, ty);
    points[i].x = ROUND(short, tx);
    points[i].y = ROUND(short, ty);
  }
  
#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation:\n\n");
  for(k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%d,%d)", points[k].x, points[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics) {
      // special-case the no-fill rectangle
      bool fill = (GetPattern() != -Pattern1);
      if( fill ) {
	  XFillPolygon(_display, DRAWABLE, _gc, points, n,
		       Nonconvex, CoordModeOrigin);
      }
      // do I need to reduce the bounding boxes height & width by 1??
      // draw bounding boxes
      if( GetLineWidth() >= 0 || !fill ) {
	  XGCValues gc_values;
	  XGetGCValues(_display, _gc, GCFillStyle, &gc_values);
	  XSetFillStyle(_display, _gc, FillSolid);
	  XDrawLines(_display, DRAWABLE, _gc, points, n, CoordModeOrigin);
	  XSetFillStyle(_display, _gc, gc_values.fill_style);
      }
  }
#endif
}

/*************************************************************************
Draw polygon, given PIXEL coordinates of the corners of the polygon.
No transformation of the coordinates is done.
*************************************************************************/

void XWindowRep::FillPixelPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("XwindowRep::FillPixelPoly: %d points\n",n);

#if MAXPIXELDUMP > 0
  for(int j = 0; j < (n < MAXPIXELDUMP ? MAXPIXELDUMP : n); j++) {
    if ((j + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", pts[j].x, pts[j].y);
  }
  printf("\n");
#endif
#endif

  if (n <= 0)
    return;

  if (n > WINDOWREP_BATCH_SIZE) {
    printf("Point array too large: %d > %d\n", n, WINDOWREP_BATCH_SIZE);
    n = WINDOWREP_BATCH_SIZE;
  }

  DOASSERT(n <= WINDOWREP_BATCH_SIZE, "points array will overflow");
  struct XPoint points[WINDOWREP_BATCH_SIZE];
  for(int i = 0;  i < n; i++ ) {
    points[i].x = ROUND(short, pts[i].x);
    points[i].y = ROUND(short, pts[i].y);
  }

#ifdef GRAPHICS
  if (_dispGraphics) {
      // special-case the no-fill rectangle
      bool fill = (GetPattern() != -Pattern1);
      if( fill ) {
	  XFillPolygon(_display, DRAWABLE, _gc, points, n,
		       Nonconvex, CoordModeOrigin);
      }
      // do I need to reduce the bounding boxes height & width by 1??
      // draw bounding boxes
      if( GetLineWidth() >= 0 || !fill ) {
	  XGCValues gc_values;
	  XGetGCValues(_display, _gc, GCFillStyle, &gc_values);
	  XSetFillStyle(_display, _gc, FillSolid);
	  XDrawLines(_display, DRAWABLE, _gc, points, n, CoordModeOrigin);
	  XSetFillStyle(_display, _gc, gc_values.fill_style);
      }
  }
#endif
}

/********************************************************************/

void XWindowRep::Arc(Coord x, Coord y, Coord w, Coord h,
		     Coord startAngle, Coord endAngle)
{
#ifdef DEBUG
  printf("XWindowRep::Arc %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
	 x, y, w, h, startAngle, endAngle);
#endif
  
  Coord tx, ty, tempX, tempY;
  Transform(x, y, tx, ty);
  Transform(x + w, y + h, tempX, tempY);
  Coord width = fabs(tempX - tx);
  Coord height = fabs(tempY - ty);
  int realStart = ROUND(int, ToDegree(startAngle) * 64);
  int realEnd = ROUND(int, ToDegree(endAngle) * 64);
#ifdef GRAPHICS
  if (_dispGraphics) {
      // now done by SetLineWidth
      // XSetLineAttributes(_display, _gc, 0, LineSolid, CapButt, JoinRound);
      bool fill = (GetPattern() != -Pattern1);
      if( fill ) {
	  XFillArc(_display, DRAWABLE, _gc, ROUND(int, tx - width / 2),
		   ROUND(int, ty - height / 2), ROUND(int, width),
		   ROUND(int, height), realStart, realEnd);
      }
      if( GetLineWidth() >= 0 || !fill ) {
	  XGCValues gc_values;
	  XGetGCValues(_display, _gc, GCFillStyle, &gc_values);
	  XSetFillStyle(_display, _gc, FillSolid);
	  XDrawArc(_display, DRAWABLE, _gc, ROUND(int, tx - width / 2),
		   ROUND(int, ty - height / 2), ROUND(int, width),
		   ROUND(int, height), realStart, realEnd);
	  XSetFillStyle(_display, _gc, gc_values.fill_style);
      }
  }
#endif
}

/***********************************************************************/

void XWindowRep::Line(Coord x1, Coord y1, Coord x2, Coord y2, 
		      Coord width, CursorStore * cstore)
{
#if defined(DEBUG) || 0
  printf("XWindowRep::Line %.2f,%.2f,%.2f,%.2f\n", x1, y1, x2, y2);
#endif
  
  Coord tx1, ty1, tx2, ty2;
  Transform(x1 ,y1, tx1, ty1);
  Transform(x2, y2, tx2, ty2);
#if defined(GRAPHICS)
  if (_dispGraphics) {
    XSetLineAttributes(_display, _gc, ROUND(int, width), _lineStyle, CapButt,
		       JoinRound);
    XDrawLine(_display, DRAWABLE, _gc, ROUND(int, tx1), ROUND(int, ty1),
	      ROUND(int, tx2), ROUND(int, ty2));
    XSetLineAttributes(_display, _gc, 0, LineSolid, CapButt, JoinMiter);
  }
#endif
}

void XWindowRep::AbsoluteLine(int x1, int y1, int x2, int y2, int width)
{
#ifdef DEBUG
  printf("XWindowRep::AbsoluteLine (%d, %d), (%d, %d), %d\n", x1, y1,
      x2, y2, width);
#endif
  
#ifdef GRAPHICS
  if (_dispGraphics) {
    XSetLineAttributes(_display, _gc, width, _lineStyle, CapButt,
		       JoinRound);
    XDrawLine(_display, DRAWABLE, _gc, x1, y1, x2, y2);
    XSetLineAttributes(_display, _gc, 0, LineSolid, CapButt, JoinMiter);
  }
#endif
}

/************************************************************
function supplied to XCheckIfEvent to find the next
Expose event for this window
***************************************************************/

static int check_expose(Display *, XEvent *e, char *arg)
{
  if (e->xany.type == Expose && e->xany.window == *((Window *)arg))
    return 1;
  return 0;
}

/**************************************************************
Draw rubberband
****************************************************************/

void XWindowRep::DrawRubberband(int x1, int y1, int x2, int y2)
{
  DOASSERT(_win, "Cannot draw rubberband in pixmap");

  int minX = MIN(x1, x2);
  int minY = MIN(y1, y2);
  int maxX = MAX(x1, x2);
  int maxY = MAX(y1, y2);
  unsigned width = maxX - minX;
  unsigned height = maxY - minY;
  
  XDrawRectangle(_display, _win, _rectGc, minX, minY, width, height);
}

#ifndef RAWMOUSEEVENTS
static long buttonMasks[3] = {
  Button1MotionMask, Button2MotionMask, Button3MotionMask
};

/*********************************************************************
Handle button press event. Return the region covered by
the selection in window coordinates. 
***********************************************************************/

void XWindowRep::DoButtonPress(int x, int y, int &xlow, int &ylow, 
			       int &xhigh, int &yhigh, int button)
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::DoButtonPress()\n", this);
#endif

  DOASSERT(_win, "Cannot handle button press in pixmap");

  /* grab server */
  XGrabServer(_display);
  int x1,x2;
  x1 = x2 = x;
  int y1,y2;
  y1 = y2 = y;

  /* draw rubberband rectangle */
  DrawRubberband(x1, y1, x2, y2);
  
  Boolean done = false;
  long buttonMask = buttonMasks[button - 1];
  while(!done) {
    XEvent event;
    XWindowEvent(_display, _win, ButtonReleaseMask | buttonMask, &event);
    switch(event.xany.type) {
    case ButtonRelease:
      if (event.xbutton.button == (unsigned int)button) {
	/* final button position */
	DrawRubberband(x1,y1,x2,y2);
	x2 = event.xbutton.x;
	y2 = event.xbutton.y;
	done = true;
      }
      break;
    case MotionNotify:
      DrawRubberband(x1, y1, x2, y2);
      
      /* get rid of all remaining motion events */
      do {
	x2 = event.xmotion.x;
	y2 = event.xmotion.y;
      } while(XCheckWindowEvent(_display,_win, buttonMask, &event));
      
      DrawRubberband(x1, y1, x2, y2);
      break;
    }
  }
  
  xlow = MIN(x1, x2);
  ylow = MIN(y1, y2);
  xhigh = MAX(x1, x2);
  yhigh = MAX(y1, y2);
  
  if (xhigh - xlow <= 5 || yhigh - ylow <= 5) {
    xhigh = xlow;
    yhigh = ylow;
  }
  
  /* ungrab server */
  XUngrabServer(_display);
  XSync(_display,false);
}
#endif

/***********************************************************************
Handle the next X event
************************************************************************/

void XWindowRep::HandleEvent(XEvent &event)
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::HandleEvent()\n", this);
#endif

  DOASSERT(_win, "Cannot handle events for pixmap");

  XEvent ev;
  Atom protocol;
  int d_modifier;
  int ks;
  int d_key;
  int count;
  char buf[40];
  KeySym keysym;
  XComposeStatus compose;

  switch(event.xany.type) {

  case KeyPress:

    d_modifier = 0;
#if 1
    if( event.xkey.state & ControlMask ) {
	d_modifier |= DeviseKey::CONTROL;
	event.xkey.state &= ~ControlMask; // avoid conversion by XLookupString
    }
    if( event.xkey.state & ShiftMask ) {
	d_modifier |= DeviseKey::SHIFT;
    }
    if( event.xkey.state & AltMask ) {
	d_modifier |= DeviseKey::ALT;
    }
#endif

    count = XLookupString((XKeyEvent *)&event, buf, sizeof(buf),
                          &keysym, &compose);

    ks = int(keysym);
    d_key = 0;
    if( devise_keymap.lookup(ks, d_key) ) {
	// not found in xkey->devise_key translation map
	if( count == 1 ) {		// regular key
	    d_key = buf[0];
            if( d_modifier & (DeviseKey::CONTROL|DeviseKey::ALT) ) {
              // always report as upper case if control or alt used
              d_key = toupper(d_key);
            } else {
              // don''t report shift for normal keys, eg, 'C', not SHIFT-C
              d_modifier &= ~DeviseKey::SHIFT;
            }
	} else {
	    d_key = 0;		// ignore keypress
	}
    }

#if defined(DEBUG)
    printf("KeyPress: KeySym:0x%x, state: 0x%x, d_key: %c 0x%x, d_mod: 0x%x\n",
	   int(keysym), event.xkey.state,
	   isgraph(char(d_key)) ? d_key : '~', d_key, d_modifier);
#endif

    if (d_key) {
	d_key |= d_modifier;
	WindowRep::HandleKey(d_key, event.xkey.x, event.xkey.y);

        // Propagate the key event to any other WindowReps outputting
        // via this one.
        int index = _inputWins.InitIterator();
        while (_inputWins.More(index)) {
          XWindowRep *wr = _inputWins.Next(index);
	      wr->WindowRep::HandleKey(d_key, event.xkey.x, event.xkey.y);
        }
        _inputWins.DoneIterator(index);
    }

    break;

#ifdef RAWMOUSEEVENTS
  case ButtonPress:
  case ButtonRelease:
  case MotionNotify:
    WindowRep::HandleButton(event.xbutton.x, event.xbutton.y-1,
			    event.xbutton.button, event.xbutton.state,
			    event.xbutton.type);
    break;
#else
  case ButtonPress:
#if defined(DEBUG)
    printf("ButtonPress: button = %d, x = %d, y = %d\n", event.xbutton.button,
	event.xbutton.x, event.xbutton.y);
#endif
    int buttonXlow, buttonYlow, buttonXhigh, buttonYhigh;
    if (event.xbutton.button == 2) {
      /* handle popup */
      DoPopup(event.xbutton.x, event.xbutton.y, event.xbutton.button);
    } else if (event.xbutton.button <= 3) {
      DoButtonPress(event.xbutton.x, event.xbutton.y,
		    buttonXlow, buttonYlow, buttonXhigh, buttonYhigh,
		    event.xbutton.button);
      
      /*
	 if (buttonXhigh > buttonXlow && buttonYhigh > buttonYlow) {
      */
      WindowRep::HandleButtonPress(buttonXlow, buttonYlow,
				   buttonXhigh, buttonYhigh,
				   event.xbutton.button);

      // Propagate the button press to any other WindowReps outputting
      // via this one.
      int index = _inputWins.InitIterator();
      while (_inputWins.More(index)) {
        XWindowRep *wr = _inputWins.Next(index);
	    wr->WindowRep::HandleButtonPress(buttonXlow, buttonYlow,
				       buttonXhigh, buttonYhigh,
				       event.xbutton.button);
      }
      _inputWins.DoneIterator(index);
      /*
	 }
      */
    }
    break;

  case ButtonRelease:
  case MotionNotify:
    break;
#endif

  case Expose:
    Coord minX, minY, maxX, maxY, tminY, tmaxY;
    minX = (Coord)event.xexpose.x;
    minY = (Coord)event.xexpose.y;
    maxX = minX + (Coord)event.xexpose.width - 1;
    maxY = minY + (Coord)event.xexpose.height - 1;
#ifdef DEBUG
    printf("XWindowRep 0x%p Exposed %d,%d to %d,%d\n", this,
	   (int)minX, (int)minY, (int)maxX, (int)maxY);
#endif
    while (XCheckIfEvent(_display, &ev, check_expose, (char *)&_win)) {
      /* merge next expose event with current one to form
	 a bigger expose region */
#ifdef DEBUG
      printf("Merging expose with other exposure: %d,%d,%d,%d\n",
	     (int)ev.xexpose.x, (int)ev.xexpose.y,
	     (int)(ev.xexpose.x + ev.xexpose.width - 1),
	     (int)(ev.xexpose.y + ev.xexpose.height - 1));
#endif
      Geom::MergeRects(minX, minY, maxX, maxY,
		       (Coord)ev.xexpose.x, (Coord)ev.xexpose.y,
		       (Coord)(ev.xexpose.x + ev.xexpose.width - 1),
		       (Coord)(ev.xexpose.y + ev.xexpose.height - 1));
    }
    WindowRep::HandleExpose((int)minX, (int)minY,
			    (unsigned)(maxX - minX + 1), 
			    (unsigned)(maxY - minY + 1));
    break;

  case ConfigureNotify:
    /* resize event */
    int saveX, saveY; 
    unsigned int saveWidth, saveHeight;
    saveX = _x;
    saveY = _y;
    saveWidth = _width;
    saveHeight = _height;
    UpdateWinDimensions();
#ifdef DEBUG
    printf("XWindowRep 0x%p ConfigureNotify: %d,%d,%d,%d\n", this,
	   _x, _y, _width, _height);
#endif
    if (_x != saveX || _y != saveY || _width != saveWidth
	|| _height != saveHeight) {
      /* There is a real change in size */
      WindowRep::HandleResize(_x, _y, _width, _height);

      // Propagate the resize event to any other WindowReps outputting
      // via this one.
      // Note: the way piles are currently done, all of the views get
      // resize events anyhow, but Miron wants things done this way in
      // case that changes in the future.  RKW 12/28/98.
      int index = _inputWins.InitIterator();
      while (_inputWins.More(index)) {
        XWindowRep *wr = _inputWins.Next(index);
        wr->WindowRep::HandleResize(_x, _y, _width, _height);
      }
      _inputWins.DoneIterator(index);
    }
    break;

  case VisibilityNotify:
    if (event.xvisibility.state == VisibilityUnobscured) {
      _unobscured = true;
#ifdef DEBUG
      printf("0x%p unobscured %d\n", this, _unobscured);
#endif
    }
    else if (event.xvisibility.state == VisibilityPartiallyObscured) {
      _unobscured = false;
#ifdef DEBUG
      printf("0x%p partially obscured\n",this);
#endif
    }
    else {
      _unobscured = false;
#ifdef DEBUG
      printf("0x%p obscured\n",this);
#endif
    }
    break;

  case MapNotify:
#ifdef DEBUG
    printf("XWin 0x%lx mapped\n", event.xmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(true);
    break;

  case UnmapNotify:
#ifdef DEBUG
    printf("XWin 0x%lx unmapped\n", event.xunmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(false);
    break;

  case ReparentNotify:
#ifdef DEBUG
    printf("XWin 0x%lx reparented\n", event.xunmap.window);
#endif
    break;

  case DestroyNotify:
#ifdef DEBUG
    printf("XWin 0x%lx destroyed\n", event.xdestroywindow.window);
#endif
    break;

  case ClientMessage:
    protocol = (Atom)event.xclient.data.l[0];
#ifdef DEBUG
    printf("XWin 0x%lx receives client message %ld\n",
           event.xclient.window, protocol);
#endif
    if (protocol == XInternAtom(_display, "WM_DELETE_WINDOW", False)) {
#ifdef DEBUG
      printf("Client message is WM_DELETE_WINDOW\n");
#endif
      WindowRep::HandleWindowDestroy();
    } else {
      printf("Unrecognized client message %ld for window 0x%lx\n",
             protocol, event.xclient.window);
    }
    break;

  case GraphicsExpose:
#ifdef DEBUG
    printf("XWin 0x%lx receives graphics expose event (%d more)\n",
           event.xgraphicsexpose.drawable, event.xgraphicsexpose.count);
    printf("    at %d,%d size %dx%d\n", event.xgraphicsexpose.x,
           event.xgraphicsexpose.y,  event.xgraphicsexpose.width,
           event.xgraphicsexpose.height);
#endif
    break;

  case NoExpose:
#ifdef DEBUG
    printf("XWin 0x%lx receives no-expose event\n",
           event.xnoexpose.drawable);
#endif
    break;

  default:
    printf("Unexpected event %d for window 0x%lx\n",
           event.xany.type, event.xany.window);
    break;
  }
}

/* Draw absolute text */

void XWindowRep::AbsoluteText(char *text, Coord x, Coord y,
			      Coord width, Coord height,
			      SymbolAlignment alignment, 
			      Boolean skipLeadingSpace, Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::AbsoluteText: <%s> at %.2f,%.2f,%.2f,%.2f, orientation %.2f\n",
	 text, x, y, width, height, orientation);
#endif

  DrawText(false, text, x, y, width, height, alignment,
    skipLeadingSpace, orientation);
}
/* Draw absolute data text */

void XWindowRep::AbsoluteDataText(char *text, Coord x, Coord y,
			      Coord width, Coord height,
			      SymbolAlignment alignment, 
			      Boolean skipLeadingSpace, Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::AbsoluteDataText: <%s> at %.2f,%.2f,%.2f,%.2f, orientation %.2f\n",
	 text, x, y, width, height, orientation);
#endif

  DrawDataText(false, text, x, y, width, height, alignment,
    skipLeadingSpace, orientation);
}


/* Draw scaled text */

void XWindowRep::ScaledText(char *text, Coord x, Coord y, Coord width,
		      Coord height, SymbolAlignment alignment,
		      Boolean skipLeadingSpace, Coord orientation)
{
#if defined (DEBUG)
  printf("XWindowRep::ScaledText: <%s> at %.2f, %.2f, %.2f, %.2f, orientation %.2f\n",
	 text, x, y, width, height, orientation);
#endif

  DrawText(true, text, x, y, width, height, alignment,
    skipLeadingSpace, orientation);
}
 
/* Draw scaled data text */

void XWindowRep::ScaledDataText(char *text, Coord x, Coord y, Coord width,
		      Coord height, SymbolAlignment alignment,
		      Boolean skipLeadingSpace, Coord orientation)
{
#if defined (DEBUG)
  printf("XWindowRep::ScaledDataText: <%s> at %.2f, %.2f, %.2f, %.2f, orientation %.2f\n",
	 text, x, y, width, height, orientation);
#endif

  DrawDataText(true, text, x, y, width, height, alignment,
    skipLeadingSpace, orientation);
}

void XWindowRep::SetXorMode()
{
#ifdef DEBUG
  printf("XWindowRep::SetXorMode\n");
#endif

#ifdef GRAPHICS
  XSetState(_display, _gc, AllPlanes, AllPlanes, GXxor, AllPlanes);
#endif
}

void XWindowRep::SetOrMode()
{
#ifdef DEBUG
  printf("XWindowRep::SetOrMode\n");
#endif

#ifdef GRAPHICS
  XSetState(_display, _gc, AllPlanes, AllPlanes, GXand, AllPlanes);
#endif
}

void XWindowRep::SetCopyMode()
{
#ifdef DEBUG
	printf("XWindowRep::SetCopyMode\n");
#endif

	XColorID	fgid = AP_GetXColorID(GetForeground());
	XColorID	bgid = AP_GetXColorID(GetBackground());

#ifdef GRAPHICS
	XSetState(_display, _gc, fgid, bgid, GXcopy, AllPlanes);
#endif

//#if defined(X_DEBUG)
//  XDrawLine(_display, DRAWABLE, _gc, (int) startX - 10, (int) startY - 10,
//    (int) startX + 10, (int) startY + 10);
//  XDrawLine(_display, DRAWABLE, _gc, (int) startX + 10, (int) startY - 10,
//    (int) startX - 10, (int) startY + 10);
//#endif

//  if (scale != 1.0) XRotSetMagnification(scale);
//  XRotDrawAlignedString(_display, fontStruct, orientation,
//    DRAWABLE, _gc, startX, startY, text, xvtAlign);
//  if (scale != 1.0) XRotSetMagnification(1.0);
}

void XWindowRep::SetFont(char *family, char *weight, char *slant,
                         char *width, float pointSize)
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::SetFont(%s,%f)\n", this, family, pointSize);
#endif
  GetDisplay()->SetFont(family, weight, slant, width, pointSize);
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
  XSetFont(_display, _gc, fontStruct->fid);
}

void XWindowRep::SetNormalFont()
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::SetNormalFont()\n", this);
#endif
  GetDisplay()->SetNormalFont();
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
  XSetFont(_display, _gc, fontStruct->fid);
}


//TEMP -- I think that DrawText() and DrawDataText() are about 95% the
// same -- they should get consolidated into a single function.
// RKW 1999-01-05.
/* Draw scaled or absolute text */

void XWindowRep::DrawText(Boolean scaled, char *text, Coord x,
			  Coord y, Coord width, Coord height,
			  SymbolAlignment alignment,
			  Boolean skipLeadingSpace, Coord orientation)
{
  /* transform into window coords */
  Coord tx1, ty1, tx2, ty2;
  Transform(x, y, tx1, ty1);
  Transform(x + width, y + height, tx2, ty2);
  int winX, winY, winWidth, winHeight;
  winX = ROUND(int, MIN(tx1, tx2));
  winY = ROUND(int, MIN(ty1, ty2));
  winWidth = ROUND(int, MAX(tx1, tx2)) - winX + 1;
  winHeight = ROUND(int, MAX(ty1, ty2)) - winY + 1;

#ifdef GRAPHICS
#if defined(X_DEBUG)
  {
    Coord centerX = winX + winWidth / 2.0;
    Coord centerY = winY + winHeight / 2.0;
    XDrawLine(_display, DRAWABLE, _gc, (int) centerX - 10, (int) centerY,
      (int) centerX + 10, (int) centerY);
    XDrawLine(_display, DRAWABLE, _gc, (int) centerX, (int) centerY - 10,
      (int) centerX, (int) centerY + 10);

    SetPattern((Pattern) -2);
    XDrawRectangle(_display, DRAWABLE, _gc, winX, winY, winWidth, winHeight);
    SetPattern((Pattern) 0);
  }
#endif

  if (skipLeadingSpace) {
    /* skip leading spaces before drawing text */
    while (*text == ' ')
      text++;
  }
  
  int textLength = strlen(text);
  if (!textLength)
    return;
  
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
#if 1
  int direction, ascent, descent;
  XCharStruct overall;
  XTextExtents(fontStruct, text, textLength, &direction, &ascent, &descent,
    &overall);
  int textWidth = overall.width;
  int textHeight = overall.ascent + overall.descent;
  ascent = overall.ascent;
#else
  int textWidth = XTextWidth(fontStruct, text, textLength);
  int textHeight = fontStruct->max_bounds.ascent
                   + fontStruct->max_bounds.descent;
  int ascent = fontStruct->max_bounds.ascent;
#endif
  
  double scale = 1.0;
  if (scaled) {
    /* use original text to calculate scale factor 
     * to scale text to fit in rectangle of 
     * dimensions (winWidth, winHeight) */
    double xscale = (double)winWidth / (double)textWidth;
    double yscale = (double)winHeight / (double)textHeight;
    scale = MIN(xscale, yscale);
  } else {
    // Note: the orientation check here is kind of a kludge.  If orientation
    // is not 0, we should do some fancier checking, but I'm just totally
    // bypassing it for now so that dates will work as Y axis labels.
    // RKW 1998-12-22.
    if (orientation == 0.0) {
      if (textWidth > winWidth || textHeight > winHeight) {
        DrawText(true, text, x, y, width, height, alignment, skipLeadingSpace,
	  orientation);
        return;
      }
    }
  }
  
  int startX = 0, startY = 0;
  int xvtAlign = NONE;

  switch(alignment) {
  case AlignNorthWest:
    startX = winX; 
    startY = winY;
    xvtAlign = TLEFT;
    break;

  case AlignNorth:
    startX = winX + winWidth / 2;
    startY = winY;
    xvtAlign = TCENTRE;
    break;

  case AlignNorthEast:
    startX = winX + winWidth;
    startY = winY;
    xvtAlign = TRIGHT;
    break;

  case AlignWest: 
    startX = winX; 
    startY = winY + winHeight / 2;
    xvtAlign = MLEFT;
    break;

  case AlignCenter: 
    startX = winX + winWidth / 2;
    startY = winY + winHeight / 2;
    xvtAlign = MCENTRE;
    break;

  case AlignEast:
    startX = winX + winWidth;
    startY = winY + winHeight / 2;
    xvtAlign = MRIGHT;
    break;

  case AlignSouthWest:
    startX = winX; 
    startY = winY + winHeight;
    xvtAlign = BLEFT;
    break;

  case AlignSouth:
    startX = winX + winWidth / 2;
    startY = winY + winHeight;
    xvtAlign = BCENTRE;
    break;

  case AlignSouthEast:
    startX = winX + winWidth;
    startY = winY + winHeight;
    xvtAlign = BRIGHT;
    break;
  }
  
#if defined(DEBUG)
  printf("Drawing <%s> starting at %d,%d\n", text, startX, startY);
#endif

#if defined(X_DEBUG)
  XDrawLine(_display, DRAWABLE, _gc, (int) startX - 10, (int) startY - 10,
    (int) startX + 10, (int) startY + 10);
  XDrawLine(_display, DRAWABLE, _gc, (int) startX + 10, (int) startY - 10,
    (int) startX - 10, (int) startY + 10);
#endif

  if (scale != 1.0) XRotSetMagnification(scale);
  XRotDrawAlignedString(_display, fontStruct, orientation,
    DRAWABLE, _gc, startX, startY, text, xvtAlign);
  if (scale != 1.0) XRotSetMagnification(1.0);
#endif
}


/* Draw scaled or absolute data text */

void XWindowRep::DrawDataText(Boolean scaled, char *text, Coord x,
			     Coord y, Coord width, Coord height,
			     SymbolAlignment alignment,
			     Boolean skipLeadingSpace, Coord orientation)
{
  /* transform into window coords */

  Coord tx1, ty1, tx2, ty2;
  Transform(x, y, tx1, ty1);
  Transform(x + width, y + height, tx2, ty2);

  int winX, winY, winWidth, winHeight;
  int startwinX,startwinY;

  startwinX = ROUND(int, tx1);
  startwinY = ROUND(int, ty1);

  winX = ROUND(int,MIN(tx1,tx2));
  winY = ROUND(int,MIN(ty1,ty2));

#ifdef GRAPHICS
  winWidth = ROUND(int, MAX(tx1, tx2)) - winX + 1;
  winHeight = ROUND(int, MAX(ty1, ty2)) - winY + 1;
 
  if (skipLeadingSpace) {
    /* skip leading spaces before drawing text */
    while (*text == ' ')
      text++;
  }
  
  int textLength = strlen(text);
  if (!textLength)
    return;
  
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
#if 1
  int direction, ascent, descent;
  XCharStruct overall;
  XTextExtents(fontStruct, text, textLength, &direction, &ascent, &descent,
    &overall);
  int textWidth = overall.width;
  int textHeight = overall.ascent + overall.descent;
  ascent = overall.ascent;
#else
  int textWidth = XTextWidth(fontStruct, text, textLength);
  int textHeight = fontStruct->max_bounds.ascent
                   + fontStruct->max_bounds.descent;
  int ascent = fontStruct->max_bounds.ascent;
#endif
  
  double scale = 1.0;
  if (scaled) {
    /* use original text to calculate scale factor 
     * to scale text to fit in rectangle of 
     * dimensions (winWidth, winHeight) */
    double xscale = (double)winWidth / (double)textWidth;
    double yscale = (double)winHeight / (double)textHeight;
    scale = MIN(xscale, yscale);
  } else {
    if (textWidth > winWidth || textHeight > winHeight) {
      DrawDataText(true, text, x, y, width, height, alignment, skipLeadingSpace,
	orientation);
      return;
    }
  }
  
  int startX, startY;
  int xvtAlign = NONE;

  startX = startwinX;
  startY = startwinY;

  switch(alignment) {
  case AlignNorthWest:
    xvtAlign = TLEFT;
    break;

  case AlignNorth:
    xvtAlign = TCENTRE;
    break;

  case AlignNorthEast:
    xvtAlign = TRIGHT;
    break;

  case AlignWest: 
    xvtAlign = MLEFT;
    break;

  case AlignCenter: 
    xvtAlign = MCENTRE;
    break;

  case AlignEast:
    xvtAlign = MRIGHT;
    break;

  case AlignSouthWest:
    xvtAlign = BLEFT;
    break;

  case AlignSouth:
    xvtAlign = BCENTRE;
    break;

  case AlignSouthEast:
    xvtAlign = BRIGHT;
    break;
  }

#if defined(DEBUG)
  printf("Drawing <%s> starting at %d,%d\n", text, startX, startY);
#endif

#if defined(X_DEBUG)
  XDrawLine(_display, DRAWABLE, _gc, (int) startX - 10, (int) startY - 10,
    (int) startX + 10, (int) startY + 10);
  XDrawLine(_display, DRAWABLE, _gc, (int) startX + 10, (int) startY - 10,
    (int) startX - 10, (int) startY + 10);
#endif

  
  // currently magnifying and scaling here -- the text --
  // want to make it independent of the whole box business --
  
  if (scale != 1.0) XRotSetMagnification(scale);
  XRotDrawAlignedString(_display, fontStruct, orientation,
    DRAWABLE, _gc, startX, startY, text, xvtAlign);
  if (scale != 1.0) XRotSetMagnification(1.0);
#endif
}

/**********************************************************************
Scale and copy the rectangle (0,0,width,height) in _srcBitmap
into _dstBitmap (0,0,dstWidth, dstHeight). X windows coords are used.
***********************************************************************/

void XWindowRep::CopyBitmap(int width,int height,int dstWidth,int dstHeight)
{
#if defined(DEBUG)
  printf("XWindowRep::CopyBitmap: %d,%d,%d,%d\n", width, height,
         dstWidth, dstHeight);
#endif

  if (dstWidth > _dstBitmap.width || dstHeight > _dstBitmap.height)
    /* allocate a bigger pixmap */
    AllocBitmap(_dstBitmap, dstWidth, dstHeight);

  XImage *srcImage = XGetImage(_display, _srcBitmap.pixmap, 
			       0, 0, width, height, 1, XYPixmap);
  
  for (int y = 0; y < dstHeight; y++) {
    for (int x = 0; x < dstWidth; x++) {
      XPutPixel(_dstBitmap.image, x, y,
		XGetPixel(srcImage,
			  x * width / dstWidth,
			  y * height / dstHeight));
    }
  }
  XPutImage(_display, _dstBitmap.pixmap, _dstBitmap.gc, 
	    _dstBitmap.image, 0, 0, 0, 0,
	    ROUND(unsigned int, dstWidth),
	    ROUND(unsigned int, dstHeight));
  XDestroyImage(srcImage);
}

/***************************************************************
Set current pattern
****************************************************************/

void XWindowRep::SetPattern(Pattern p)
{
    
    if (WindowRep::GetPattern() != p) {
	WindowRep::SetPattern(p);
#ifdef GRAPHICS
	if (_dispGraphics) {
	    if( p == Pattern0 ) { // fill solid foreground color
		XSetFillStyle(_display, _gc, FillSolid);
	    } else {
		if( p < 0 ) {	// transparent pattern
		    XSetFillStyle(_display, _gc, FillStippled);
		    XSetStipple(_display, _gc, 
				GetDisplay()->GetPatternStipple(-p));
		} else {	// opaque pattern
		    XSetFillStyle(_display, _gc, FillOpaqueStippled);
		    XSetStipple(_display, _gc, 
				GetDisplay()->GetPatternStipple(p));
		}
	    }
	}
#endif
    }
}

/***************************************************************
Set current line width (for borders)
****************************************************************/

void XWindowRep::SetLineWidth(int w)
{
    WindowRep::SetLineWidth(w);
#ifdef GRAPHICS
    if (_dispGraphics) {
	if( w >= 0 ) {
	    XSetLineAttributes(_display, _gc, w, LineSolid, CapButt, JoinMiter);
	} else {
	    XSetLineAttributes(_display, _gc, 0, LineSolid, CapButt, JoinMiter);
	}
#endif
    }
}

#ifdef LIBCS
/***************************************************************
Set line dash pattern.
****************************************************************/

void XWindowRep::SetDashes(int dashCount, int dashes[], int startOffset)
{
#if defined(DEBUG)
  printf("XWindowRep::SetDashes(%d, ...)\n", dashCount);
#endif

  char errBuf[256];

  const int maxChar = (1 << 7) - 1;

  if (dashCount > 0) {
    _lineStyle = LineOnOffDash;
    char *dashList = new char[dashCount];
    int dashNum;
    for (dashNum = 0; dashNum < dashCount; dashNum++) {
      int dash = dashes[dashNum];
      if (dash <= 0) {
	sprintf(errBuf, "Illegal dash value (%d)", dash);
	reportErrNosys(errBuf);
        dashList[dashNum] = 1;
      } else if (dash > maxChar) {
	sprintf(errBuf, "Illegal dash value (%d)", dash);
	sprintf(errBuf, "Illegal dash value (%d)", dash);
	reportErrNosys(errBuf);
        dashList[dashNum] = maxChar;
      } else {
        dashList[dashNum] = (char) dash;
      }
    }
    XSetDashes(_display, _gc, startOffset, dashList, dashCount);
    delete [] dashList;
  } else {
    _lineStyle = LineSolid;
  }
}
#endif

/***********************************************************************/

void XWindowRep::AllocBitmap(XBitmapInfo &info, int width, int height)
{
  FreeBitmap(info);
  
  info.inUse = true;
  info.width = width;
  info.height = height;
  info.pixmap = XCreatePixmap(_display, DRAWABLE, width, height, 1);
  DOASSERT(info.pixmap, "Cannot create pixmap");
  info.gc = XCreateGC(_display, info.pixmap, 0, NULL);
  
  /* set foreground to 1 and background to 0 for later XCopyPlane() */
  XSetState(_display, info.gc, 1, 0, GXcopy, AllPlanes);
  
  XSetFont(_display,info.gc, GetDisplay()->GetFontStruct()->fid);
  info.image = XGetImage(_display, info.pixmap, 0, 0, width, height,
			 1, XYPixmap);
}

/* Get Window dimensions */

void XWindowRep::UpdateWinDimensions()
{
  Window root;
  unsigned int border_width, depth;

  if (_win) {
    XGetGeometry(_display, _win, &root, &_x, &_y, &_width, &_height,
		 &border_width, &depth);
  } else {
    // pixmaps don't have position information so XGetGeometry returns
    // X = 0 and Y = 0... just ignore them
    int dummyX, dummyY;
    XGetGeometry(_display, _pixmap, &root, &dummyX, &dummyY, &_width, &_height,
		 &border_width, &depth);
  }
}

/* Get window rep dimensions */

void XWindowRep::Dimensions(unsigned int &width, unsigned int &height)
{
  width = _width;
  height = _height;
}

void XWindowRep::PrintDimensions()
{
  cout << "Width = " << _width << endl;
  cout << "Height = " << _height << endl;
}

/* Get window rep origin */

void XWindowRep::Origin(int &x, int &y)
{
  x = _x;
  y = _y;
}

void XWindowRep::AbsoluteOrigin(int &x, int &y)
{
  if (!_win) {
    // We have a pixmap.  Location of pixmap can't have been changed
    // since it was created, so just return location params from
    // constructor.
    x = _x;
    y = _y;
    return;
  }

  /* Find the offset from root window */
  x = 0;
  y = 0;
  
  Window current, root;
  Window parent = _win;
  do {
    current = parent;
    
    /* add distace to parent */
    Window winRoot;
    int winX, winY;
    unsigned winW, winH, winBorder, winDepth;
    XGetGeometry(_display, current, &winRoot, &winX, &winY,
		 &winW, &winH, &winBorder, &winDepth);
    x += winX;
    y += winY;
    
    //
    // I don't understand at all what this code (looking at the children)
    // is for, but if we don't have it some sessions cannot be successfully
    // opened(!).  RKW May 14, 1998.
    Window *children;
    unsigned int nchildren;
    if (!XQueryTree(_display, current, &root, &parent,
		    &children, &nchildren)) {
      DOASSERT(0, "XQueryTree failed");
    }
    if (children != NULL)
      XFree(children);
  } while (parent != root);
}

/* Move and resize window, relative to the parent */

void XWindowRep::MoveResize(int x, int y, unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("Moving XWindowRep 0x%p to %d,%d, size %u,%u\n", this,
	 x, y, w, h);
#endif

  if (_win) {
    /* Tell X to move/resize window. We will be notified by an event
       when it's done */
    XMoveResizeWindow(_display, _win, x, y, w, h);
  } else {
    /* Resizing a pixmap involves deleting it and creating a new one */
    XFreePixmap(_display, _pixmap);
    unsigned int depth = DefaultDepth(_display, DefaultScreen(_display));
    _pixmap = XCreatePixmap(_display, DefaultRootWindow(_display),
			    w, h, depth);
    DOASSERT(_pixmap, "Cannot create pixmap");
    _x = x;
    _y = y;

    // clear all pixels in pixmap
    SetForeground(GetBackground());
    XFillRectangle(_display, _pixmap, _gc, 0, 0, w, h);
  }

  UpdateWinDimensions();
}

/* Iconify window. Not guaranteed to succeed. */

void XWindowRep::Iconify()
{
  DOASSERT(_win, "Cannot iconify pixmap");
  XIconifyWindow(_display, _win, 0);
}

#ifndef RAWMOUSEEVENTS
void XWindowRep::DoPopup(int x, int y, int button)
{
#if defined(DEBUG)
  printf("XWindowRep::DoPopup(%d, %d)\n", x, y);
#endif

  DOASSERT(_win, "Cannot display pop-up window in pixmap");

  char **msgs;
  int numMsgs;
  if (!HandlePopUp(x, y, button, msgs, numMsgs) || numMsgs <= 0)
    /* no message for pop-up */
    return;
  
  /* find width and height of window */
  SetNormalFont(); // In case the last font we used was different.
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
  int textWidth = 0;
  int textHeight = 0;
  int charAscent = fontStruct->max_bounds.ascent;
  int charHeight = fontStruct->max_bounds.ascent + 
    fontStruct->max_bounds.descent;

  int i;
  for(i = 0; i < numMsgs; i++) {
#if defined(DEBUG)
      printf("msgs[%d]: %s\n", i, msgs[i]);
#endif
    textWidth = MAX(textWidth,
			    XTextWidth(fontStruct, msgs[i], strlen(msgs[i])));
  }
  textHeight = charHeight * numMsgs;

  SetForeground(GetPColorID(blackColor));
  SetBackground(GetPColorID(whiteColor));
  XColorID	fgnd = AP_GetXColorID(GetForeground());
  XColorID	bgnd = AP_GetXColorID(GetBackground());

  /* Create window */
  XSetWindowAttributes attr;
  attr.background_pixmap	= None;
  attr.background_pixel		= bgnd;
  attr.border_pixmap		= CopyFromParent;
  attr.border_pixel		= fgnd;
  attr.bit_gravity		= ForgetGravity /* CenterGravity */;
  attr.win_gravity		= NorthWestGravity;
  attr.backing_store		= Always /* NotUseful WhenMapped */;
  attr.backing_planes		= AllPlanes;
  attr.backing_pixel		= 0;
  attr.save_under		= True;
  attr.event_mask		= 0;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect	= True;
  attr.colormap			= DefaultColormap(_display,
						  DefaultScreen(_display));
  attr.cursor			= None;
  
  /* Create the window. */
  Window win;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  int absX, absY;
  AbsoluteOrigin(absX, absY);
  x += absX;
  y += absY;

  /* Make bottom right of window appear at cursor position */
  textWidth +=  2; /* allow for border */
  textHeight += 2; /* allow for border */
  x = x - textWidth + 1;
  y = y - textHeight + 1;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  if ((win = XCreateWindow(_display, DefaultRootWindow(_display), x, y,
			   textWidth, textHeight, 1, 0, InputOutput,
			   CopyFromParent, AllPlanes, &attr)) == 0) {
    DOASSERT(0, "Cannot create popup window");
  }
  
  XSelectInput(_display, win, ExposureMask | ButtonPressMask);
  
  /* Map the window so that it appears on the display. */
  if (XMapWindow(_display, win) < 0) {
    DOASSERT(0, "Cannot map popup window");
  }
  
  /* Do a sync to force the window to appear immediately */
  XSync(_display, false);
  
  XGCValues gcVal;
  gcVal.function = GXcopy;
  gcVal.plane_mask = AllPlanes;
  gcVal.foreground = fgnd;
  gcVal.background = bgnd;
  gcVal.font = fontStruct->fid;
  GC popUpGc = XCreateGC(_display, win, GCFunction | GCPlaneMask
			 | GCForeground | GCBackground | GCFont,
			 &gcVal);
  /*
     XSetState(_display, popUpGc, fgnd, bgnd, GXcopy,AllPlanes);
  */
  
#if defined(LIBCS)
  Boolean savePopup = false;
#else
  Boolean savePopup = Init::SavePopup();
#endif
  if (!savePopup)
    XGrabServer(_display);

  /* Draw the text messages into window */
  for(i = 0; i < numMsgs; i++) {
      int startY = charHeight * i;
      XDrawString(_display, win, popUpGc, 1, startY + charAscent, 
		  msgs[i], strlen(msgs[i]));
  }

  while(1) {
    XEvent event;
    /* check parent for button release */
    if (savePopup) {
      if (XCheckWindowEvent(_display, win, ButtonPressMask, &event)) {
	/* done */
	XDestroyWindow(_display, win);
	break;
      }
    } else {
      if (XCheckWindowEvent(_display, _win, ButtonReleaseMask, &event)) {
	if (event.xbutton.button == (unsigned int)button) {
	  /* done */
	  XDestroyWindow(_display, win);
	  break;
	}
      }
    }
    
    /* check popUp window for exposure */
    if (XCheckWindowEvent(_display, win, ExposureMask, &event)) {
      /* Draw the text messages into window */
      for(i = 0; i < numMsgs; i++) {
	int startY = charHeight * i;
	XDrawString(_display, win, popUpGc, 1, startY + charAscent, 
		    msgs[i], strlen(msgs[i]));
      }
    }
  }

  XFreeGC(_display, popUpGc);
  if (!savePopup)
    XUngrabServer(_display);
  XSync(_display,false);
}
#endif

Boolean XWindowRep::Scrollable()
{
  /* window is crollable if it has a backing store or
     if it is not obscured */
  /*
     printf("XWindowRep::Scrollable: 0x%p backingstore: %d, unobscured %d\n",
     this, _backingStore, _unobscured);
     */
  if (_backingStore || _unobscured)
    return true;
  else return false;
}

void XWindowRep::Scroll(Coord x, Coord y, Coord w, Coord h,
			Coord dstX, Coord dstY)
{
#ifdef DEBUG
  printf("XwindowRep::Scroll(%.2f,%.2f,%.2f,%.2f,%.2f,%.2f)\n",
	 x, y, w, h, dstX, dstY);
#endif

  Coord xlow, ylow, xhi, yhi, width, height;
  Coord x1, y1, x2, y2;
  Transform(x, y, x1, y1);
  Transform(x + w, y + h, x2, y2);
  xlow = MIN(x1, x2);
  xhi = MAX(x1, x2);
  ylow = MIN(y1, y2);
  yhi = MAX(y1, y2);
  width = xhi - xlow + 1;
  height = yhi - ylow + 1;
  
  Coord tdx, tdy;
  Transform(dstX, dstY + h, tdx, tdy);
  
#ifdef DEBUG
  printf("XWindowRep::Scroll after xform x:%d,y:%d,w:%d,h:%d to x:%d, y%d\n",
	 ROUND(int, xlow), ROUND(int, ylow), ROUND(int, width),
	 ROUND(int, height), ROUND(int, tdx), ROUND(int, tdy));
#endif
  
  XCopyArea(_display, DRAWABLE, DRAWABLE, _gc, ROUND(int, xlow),
	    ROUND(int, ylow), ROUND(unsigned int, width),
	    ROUND(unsigned int, height), ROUND(int, tdx), ROUND(int, tdy));
}

void XWindowRep::ScrollAbsolute(int x, int y, unsigned width, unsigned height,
				int dstX, int dstY)
{
#ifdef DEBUG
  printf("XWindowRep::ScrollAbsolute(x:%d,y:%d,w:%d,h:%d,dx:%d,dy:%d)\n",
	 x, y, width, height, dstX, dstY);
#endif

  XCopyArea(_display, DRAWABLE, DRAWABLE, _gc, x, y, width, height,
	    dstX, dstY);
}

/* Raise window to top of stacking order */

void XWindowRep::Raise()
{
#ifdef DEBUG
  printf("XWindowRep::Raise window 0x%lx:\n", _win);
#endif

  if (_win)
    XRaiseWindow(_display, _win);

  if (!_parent)
    return;

  if (!_parent->_children.Delete(this)) {
    fprintf(stderr, "Cannot remove child from parent\n");
    return;
  }
  _parent->_children.Append(this);
}

/* Lower window to bottom of stacking order */

void XWindowRep::Lower()
{
#ifdef DEBUG
  printf("XWindowRep::Lower window 0x%lx:\n", _win);
#endif

  if (_win)
    XLowerWindow(_display, _win);

  if (!_parent)
    return;

  if (!_parent->_children.Delete(this)) {
    fprintf(stderr, "Cannot remove child from parent\n");
    return;
  }
  _parent->_children.Insert(this);
}

/* Flush windowRep's content to display */

void XWindowRep::Flush()
{
  /* Do a sync to force output */
  XSync(_display, false);
}


/* Find out whether X window has backing store.  (Note: _backingStore member
 * is meaningless.) */

Boolean XWindowRep::HasBackingStore()
{
  XWindowAttributes attr;
  XGetWindowAttributes(_display, _win, &attr);
  return attr.backing_store;
}


/* Get current window pixmap */

DevisePixmap *XWindowRep::GetPixmap()
{
#ifdef DEBUG
  printf("XWindowRep::GetPixmap()\n");
#endif

  const int MAX_PIXMAP_BUF_SIZE = 256 * 1024;
  char _pixmapBuf[MAX_PIXMAP_BUF_SIZE];

  /* see if window is entirely visible on screen or not (XGetImage
     fails if window is partially off-screen */

  int dummyX, dummyY;
  unsigned int width, height;
  Coord screenW, screenH;
  GetRootGeometry(dummyX, dummyY, width, height);
  GetDisplay()->Dimensions(screenW, screenH);
#ifdef DEBUG
  printf("XWin 0x%p is %d,%d,%u,%u, screen is %.2f,%.2f\n",
         this, dummyX, dummyY, width, height, screenW, screenH);
#endif

  if (dummyX < 0 || dummyX + width - 1 > screenW ||
      dummyY < 0 || dummyY + height - 1 > screenH) {
#ifdef DEBUG
    printf("Window partially off-screen\n");
#endif
    return NULL;
  }
  
  Window root;
  unsigned int border_width, depth;
  XGetGeometry(_display, DRAWABLE, &root, &dummyX, &dummyY, &width, &height,
	       &border_width, &depth);
  
  DevisePixmap *pixmap = new DevisePixmap;
  DOASSERT(pixmap, "Out of memory");
  pixmap->width = width;
  pixmap->height = height;
  Boolean initPixmap = true;
  int outIndex = 0;

  for(int i = 0; i < (int)height; i++) {
    XImage *image = XGetImage(_display, DRAWABLE, 0, i, width, 1,
                              AllPlanes, ZPixmap);
    if (!image) {
        delete pixmap;
      return NULL;
    }

#ifdef DEBUG
    printf("*****************%d bits per pixel \n", image->bits_per_pixel);
#endif

    if (initPixmap) {
      pixmap->imageBytes = image->bytes_per_line * height;
      pixmap->bytes_per_line = image->bytes_per_line;
      pixmap->padding = image->bitmap_pad;
      pixmap->data = (unsigned char *)image->data;
      initPixmap = false;
    }
#ifdef DEBUG
      fprintf(stderr,"%s ", (unsigned char *)pixmap->data);
      printf("\n");
#endif

    int outCount;
    char *cBuf = _compress->CompressLine(image->data, image->bytes_per_line,
					 outCount);
    XDestroyImage(image);
    if (outCount + outIndex + sizeof(int) >
		(unsigned int)MAX_PIXMAP_BUF_SIZE ) {
      /* no more buffer */
      delete pixmap;
      return NULL;
    }
    memcpy(&_pixmapBuf[outIndex], (char *)&outCount, sizeof(int));
    outIndex += sizeof(int);
    memcpy(&_pixmapBuf[outIndex], cBuf, outCount);
    outIndex += outCount;
  }
  
  pixmap->compressedBytes = outIndex;
  pixmap->data = (unsigned char *)malloc(outIndex);
  if (!pixmap->data) {
    delete pixmap;
    return NULL;
  }
  memcpy(pixmap->data, _pixmapBuf, outIndex);

#ifdef DEBUG
  printf("XWindowRep::GetPixmap: %d bytes\n",pixmap->compressedBytes);
#endif

  return pixmap;
}

/* Display pixmap in window */

void XWindowRep::DisplayPixmap(DevisePixmap *pixmap)
{
#ifdef DEBUG
  printf("XWindowRep::DisplayPixmap %d bytes\n", pixmap->compressedBytes);
#endif

  int screen = DefaultScreen(_display);
  int depth = DefaultDepth(_display,screen);
  Visual *visual = DefaultVisual(_display, screen);
  
  int index = 0;
  unsigned char *data = pixmap->data;
  for(int i = 0; i < pixmap->height; i++) {
    int count;
    int outCount;
    memcpy((char *)&count, (char *)&data[index], sizeof(int));
    index += sizeof(int);
    DOASSERT(index + count <= pixmap->compressedBytes,
	     "Invalid pixmap format");
    char *buf = _compress->DecompressLine((char *)&data[index],
					  count, outCount);
    index += count;
    DOASSERT(outCount == pixmap->bytes_per_line, "Invalid pixmap format");
    XImage *image = XCreateImage(_display, visual, depth, ZPixmap,
				 0, buf, pixmap->width, 1, pixmap->padding, 0);
    DOASSERT(image, "Cannot create image");
    XPutImage(_display, DRAWABLE, _gc, image, 0, 0, 0, i, image->width, 1);
    image->data = NULL;
    XDestroyImage(image);
  }
}

void XWindowRep::FreePixmap(DevisePixmap *pixmap)
{
  free(pixmap->data);
  delete pixmap;
}

#ifdef TK_WINDOW_old
void XWindowRep::Decorate(WindowRep *parent, char *name,
			  unsigned int min_width, unsigned int min_height)
{
  DOASSERT(!isInTkWindow(), "Invalid Tk window");
  EmbedInTkWindow((XWindowRep *)parent, name, min_width, min_height);
}

void XWindowRep::Undecorate()
{
  if (isInTkWindow())
    DetachFromTkWindow();
}

static void HandleTkEvents(ClientData win, XEvent *eventPtr)
{
#ifdef DEBUG
  printf("HandleTkEvents: received event %d\n", eventPtr->type);
#endif

  if (eventPtr->type != ConfigureNotify)
    return;

  ((XWindowRep *)win)->TkWindowSizeChanged();
}

void XWindowRep::TkWindowSizeChanged()
{
  extern Tcl_Interp *ControlPanelTclInterp;

  // first find out new size of Tk window

  DOASSERT(_tkWindow, "Invalid Tk window");
  unsigned int w = (unsigned int)Tk_Width(_tkWindow);
  unsigned int h = (unsigned int)Tk_Height(_tkWindow);

  // adjust with margins

  int x = _leftMargin;
  w -= _leftMargin + _rightMargin;
  int y = _topMargin;
  h -= _topMargin + _bottomMargin;

  MoveResize(x, y, w, h);
  WindowRep::HandleResize(x, y, w, h);

  char cmd[256];
  sprintf(cmd, "ResizeTkDataWindow %s %u %u", _tkPathName, w, h);
#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot resize Tk window");
}

static const unsigned int TkRootLeftMargin   = 0;
static const unsigned int TkRootRightMargin  = 0;
static const unsigned int TkRootTopMargin    = 0;
static const unsigned int TkRootBottomMargin = 20;

static const unsigned int TkViewLeftMargin   = 40;
static const unsigned int TkViewRightMargin  = 0;
static const unsigned int TkViewTopMargin    = 0;
static const unsigned int TkViewBottomMargin = 20;

void XWindowRep::EmbedInTkWindow(XWindowRep *parent,
				 char *name,
				 unsigned int min_width,
				 unsigned int min_height)
{
  DOASSERT(_win, "Cannot embed pixmap in Tk window");

  extern Tcl_Interp *ControlPanelTclInterp;
  extern Tk_Window ControlPanelMainWindow;

  // get location and size of window

  int x, y;
  unsigned int w, h;
  Origin(x, y);
  Dimensions(w, h);

  // figure out the correct margins for this type of window

  // default: root data display window

  _leftMargin   = TkRootLeftMargin;
  _rightMargin  = TkRootRightMargin;
  _topMargin    = TkRootTopMargin;
  _bottomMargin = TkRootBottomMargin;

  // for views: left and bottom margin

  if (parent) {
    _leftMargin   = TkViewLeftMargin;
    _rightMargin  = TkViewRightMargin;
    _topMargin    = TkViewTopMargin;
    _bottomMargin = TkViewBottomMargin;
  }

  static int tkWinCount = 1;
  char cmd[256];
  sprintf(_tkPathName, ".devisewin%d", tkWinCount++);
  sprintf(cmd, "CreateTkDataWindow %s {%s} %d %d %u %u %u %u %u %u %u %u",
	  _tkPathName, name, x, y, w, h, _leftMargin,
	  _rightMargin, _topMargin, _bottomMargin,
	  min_width, min_height);
#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot create Tk window");

  _tkWindow = Tk_NameToWindow(ControlPanelTclInterp,
			      _tkPathName,
			      ControlPanelMainWindow);
  DOASSERT(_tkWindow, "Cannot get name of Tk window");

#ifdef DEBUG
  printf("Created %s, id 0x%p, X id 0x%p, at %d,%d size %u,%u\n",
	 _tkPathName, _tkWindow, Tk_WindowId(_tkWindow),
	 x, y, w, h);
#endif

  unsigned long mask = StructureNotifyMask;
  Tk_CreateEventHandler(_tkWindow, mask, HandleTkEvents, this);

  // first make this window a child of the new Tk window

#ifdef DEBUG
  printf("XWindowRep::Reparenting 0x%p to 0x%p at %d,%d\n",
	 _win, Tk_WindowId(_tkWindow), _leftMargin, _topMargin);
#endif
  XReparentWindow(_display, _win, Tk_WindowId(_tkWindow),
		  _leftMargin, _topMargin);

  // make this window smaller so that margins have space inside Tk window

  w -= _leftMargin + _rightMargin;
  h -= _topMargin + _bottomMargin;
  MoveResize(_leftMargin, _topMargin, w, h);

  // then optionally make the Tk window a child of this window's parent
  // i.e. the Tk window gets inserted between this window and its parent
  
  if (parent) {
#ifdef DEBUG
    printf("XWindowRep::Reparenting 0x%p to 0x%p at %d,%d\n",
	   Tk_WindowId(_tkWindow), parent->_win, x, y);
#endif
    XReparentWindow(_display, Tk_WindowId(_tkWindow), parent->_win, x, y);
  }
}

void XWindowRep::DetachFromTkWindow()
{
  DOASSERT(_win, "Cannot detach pixmap from Tk window");

  extern Tcl_Interp *ControlPanelTclInterp;

#ifdef DEBUG
  printf("ViewWin::Detaching 0x%p from 0x%0x\n", this, _tkWindow);
#endif

  DOASSERT(_tkWindow, "Invalid Tk window");
  unsigned long mask = StructureNotifyMask;
  Tk_DeleteEventHandler(_tkWindow, mask, HandleTkEvents, this);

  // get location and size of window

  int x, y;
  unsigned int w, h;
  Origin(x, y);
  Dimensions(w, h);

  // adjust location and size since margins are disappearing

  x -= _leftMargin;
  w += _leftMargin + _rightMargin;
  y -= _topMargin;
  H += _topMargin + _bottomMargin;
  MoveResize(x, y, w, h);

  XReparentWindow(_display, _win, DefaultRootWindow(_display), x, y);

  // destroy Tk window

  char cmd[256];
  sprintf(cmd, "destroy %s", _tkPathName);
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot destroy Tk window");

  _tkPathName[0] = 0;
  _tkWindow = 0;
}

#endif

void XWindowRep::DrawRectangle(int symbolX, int symbolY, int width,
			       int height, Boolean filled,
			       SymbolAlignment alignment, Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::DrawRectangle(%d %d %d %d)\n", symbolX, symbolY,
    width, height);
#endif

#if 0
  /* Draw an "X" at the given location. */
  XDrawLine(_display, DRAWABLE, _gc, symbolX - 5, symbolY - 5, symbolX + 5,
    symbolY + 5);
  XDrawLine(_display, DRAWABLE, _gc, symbolX + 5, symbolY - 5, symbolX - 5,
    symbolY + 5);
#endif

  if (orientation != 0.0) {
    XPoint points[5];

    CalculateLocation(symbolX, symbolY, width, height, alignment,
      orientation, points);

    if (filled) {
      XFillPolygon(_display, DRAWABLE, _gc, points, 5, Convex,
	CoordModeOrigin);
    }

    if( GetLineWidth() >= 0 || !filled) {
      XGCValues gc_values;
      XGetGCValues(_display, _gc, GCFillStyle, &gc_values);
      XSetFillStyle(_display, _gc, FillSolid);
      XDrawLines(_display, DRAWABLE, _gc, points, 5, CoordModeOrigin);
      XSetFillStyle(_display, _gc, gc_values.fill_style);
    }
  } else {
    CalculateLocation(symbolX, symbolY, width, height, alignment,
      orientation);

    if (filled) {
      XFillRectangle(_display, DRAWABLE, _gc, symbolX,
        symbolY, width, height);
    }

    // do I need to reduce the bounding boxes height & width by 1??
    // draw bounding boxes
    if( GetLineWidth() >= 0 || !filled) {
      XGCValues gc_values;
      XGetGCValues(_display, _gc, GCFillStyle, &gc_values);
      XSetFillStyle(_display, _gc, FillSolid);
      XDrawRectangle(_display, DRAWABLE, _gc, symbolX,
        symbolY, width, height);
      XSetFillStyle(_display, _gc, gc_values.fill_style);
    }
  }
}

void XWindowRep::DrawRectangles(XRectangle rectangles[], int rectCount,
		    Boolean filled, SymbolAlignment alignment,
		    Coord orientation)
{
#if defined(DEBUG)
  printf("XWindowRep::DrawRectangles(%d)\n", rectCount);
#endif

  if (orientation != 0.0) {
    int rectNum;
    for (rectNum = 0; rectNum < rectCount; rectNum++) {
      DrawRectangle(rectangles[rectNum].x, rectangles[rectNum].y,
	rectangles[rectNum].width, rectangles[rectNum].height, filled,
	alignment, orientation);
    }
  } else {
    int symbolX, symbolY;
    int rectNum;
    for (rectNum = 0; rectNum < rectCount; rectNum++) {
      symbolX = rectangles[rectNum].x;
      symbolY = rectangles[rectNum].y;
      CalculateLocation(symbolX, symbolY, rectangles[rectNum].width,
	rectangles[rectNum].height, alignment, orientation);
      rectangles[rectNum].x = symbolX;
      rectangles[rectNum].y = symbolY;
    }

    if (filled) {
      XFillRectangles(_display, DRAWABLE, _gc, rectangles, rectCount);
    }

    // do I need to reduce the bounding boxes height & width by 1??
    // draw bounding boxes
    if( GetLineWidth() >= 0 || !filled) {
      XGCValues gc_values;
      XGetGCValues(_display, _gc, GCFillStyle, &gc_values);
      XSetFillStyle(_display, _gc, FillSolid);
      XDrawRectangles(_display, DRAWABLE, _gc, rectangles, rectCount);
      XSetFillStyle(_display, _gc, gc_values.fill_style);
    }
  }
}

/* symbolX and symbolY are input as the given location for the symbol.
 * They will get changed according to alignment to be the location of
 * the upper left corner of the symbol (if orientation is zero).  If
 * orientation is non-zero, symbolX and symbolY are pretty much meaningless
 * on output, and points will contain the points needed to draw the
 * symbol. */
void XWindowRep::CalculateLocation(int &symbolX, int &symbolY, int width,
			     int height, SymbolAlignment alignment,
			     Coord orientation, XPoint *points)
{
  Coord sine, cosine;
  if (orientation != 0.0) {
    DOASSERT(points != NULL, "points array needed for non-zero orientation");
    /* Minus sign is needed to preserve positive rotation being counter-
     * clockwise in X's upside-down coordinate system. */
    Coord radians = -orientation * M_PI / 180.0;
    sine = sin(radians);
    cosine = cos(radians);

    Point relativePoints[4];

    switch (alignment) {
    case AlignNorthWest:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = 0.0;
      relativePoints[1].y = height;
      relativePoints[2].x = width;
      relativePoints[2].y = height;
      relativePoints[3].x = width;
      relativePoints[3].y = 0.0;
      break;

    case AlignNorth:
      relativePoints[0].x = -width / 2.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = -width / 2.0;
      relativePoints[1].y = height;
      relativePoints[2].x = width / 2.0;
      relativePoints[2].y = height;
      relativePoints[3].x = width / 2.0;
      relativePoints[3].y = 0.0;
      break;

    case AlignNorthEast:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = -width;
      relativePoints[1].y = 0.0;
      relativePoints[2].x = -width;
      relativePoints[2].y = height;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = height;
      break;

    case AlignWest:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = height / 2.0;
      relativePoints[1].x = width;
      relativePoints[1].y = height / 2.0;
      relativePoints[2].x = width;
      relativePoints[2].y = -height / 2.0;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = -height / 2.0;
      break;

    case AlignCenter:
      relativePoints[0].x = width / 2.0;
      relativePoints[0].y = height / 2.0;
      relativePoints[1].x = -width / 2.0;
      relativePoints[1].y = height / 2.0;
      relativePoints[2].x = -width / 2.0;
      relativePoints[2].y = -height / 2.0;
      relativePoints[3].x = width / 2.0;
      relativePoints[3].y = -height / 2.0;
      break;

    case AlignEast:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = -height / 2.0;
      relativePoints[1].x = -width;
      relativePoints[1].y = -height / 2.0;
      relativePoints[2].x = -width;
      relativePoints[2].y = height / 2.0;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = height / 2.0;
      break;

    case AlignSouthWest:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = width;
      relativePoints[1].y = 0.0;
      relativePoints[2].x = width;
      relativePoints[2].y = -height;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = -height;
      break;

    case AlignSouth:
      relativePoints[0].x = width / 2.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = width / 2.0;
      relativePoints[1].y = -height;
      relativePoints[2].x = -width / 2.0;
      relativePoints[2].y = -height;
      relativePoints[3].x = -width / 2.0;
      relativePoints[3].y = 0.0;
      break;

    case AlignSouthEast:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = 0.0;
      relativePoints[1].y = -height;
      relativePoints[2].x = -width;
      relativePoints[2].y = -height;
      relativePoints[3].x = -width;
      relativePoints[3].y = 0.0;
      break;

    default:
      DOASSERT(false, "Illegal alignment value");
      break;
    }

    points[0].x = points[4].x = ROUND(int,
      symbolX + relativePoints[0].x * cosine - relativePoints[0].y * sine);
    points[0].y = points[4].y = ROUND(int,
      symbolY + relativePoints[0].y * cosine + relativePoints[0].x * sine);
    points[1].x = symbolX + ROUND(int, relativePoints[1].x * cosine -
      relativePoints[1].y * sine);
    points[1].y = symbolY + ROUND(int, relativePoints[1].y * cosine +
      relativePoints[1].x * sine);
    points[2].x = symbolX + ROUND(int, relativePoints[2].x * cosine -
      relativePoints[2].y * sine);
    points[2].y = symbolY + ROUND(int, relativePoints[2].y * cosine +
      relativePoints[2].x * sine);
    points[3].x = symbolX + ROUND(int, relativePoints[3].x * cosine -
      relativePoints[3].y * sine);
    points[3].y = symbolY + ROUND(int, relativePoints[3].y * cosine +
      relativePoints[3].x * sine);
  }

  switch (alignment) {
  case AlignNorthWest:
    // Symbol x and y values stay the same.
    break;

  case AlignNorth:
    symbolX -= width / 2;
    break;

  case AlignNorthEast:
    symbolX -= width;
    break;

  case AlignWest:
    symbolY -= height / 2;
    break;

  case AlignCenter:
    symbolX -= width / 2;
    symbolY -= height / 2;
    break;

  case AlignEast:
    symbolX -=  width;
    symbolY -=  height / 2;
    break;

  case AlignSouthWest:
    symbolY -= height;
    break;

  case AlignSouth:
    symbolX -= width / 2;
    symbolY -= height;
    break;

  case AlignSouthEast:
    symbolX -= width;
    symbolY -= height;
    break;

  default:
    DOASSERT(false, "Illegal alignment value");
    break;
  }
}

void
XWindowRep::SetGifDirty(Boolean dirty)
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::SetGifDirty(%d)\n", this, dirty);
#endif

  WindowRep::SetGifDirty(dirty);

  if (dirty) {
    if (_parent) {
      _parent->SetGifDirty(true);
    }
  }
}

void
XWindowRep::SetOutput(WindowRep *winRep)
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::SetOutput(0x%p)\n", this, winRep);
#endif

  if (winRep != this) {
    // winRep had better really be an XWindowRep!!
    XWindowRep *outWR = (XWindowRep *)winRep;

    _win = outWR->_win;
    _pixmap = outWR->_pixmap;

    outWR->AddInputWR(this);
    _outWR = outWR;
  }
}

void
XWindowRep::ResetOutput()
{
#if defined(DEBUG)
  printf("XWindowRep(0x%p)::ResetOutput()\n", this);
#endif

  if (_outWR) {
    _outWR->DeleteInputWR(this);
    _outWR = NULL;
  }

  _win = _myWin;
  _pixmap = _myPixmap;
}

void
XWindowRep::AddInputWR(XWindowRep *winRep)
{
  if (winRep != this) _inputWins.Append(winRep);
}

void
XWindowRep::DeleteInputWR(XWindowRep *winRep)
{
  if (!_inputWins.Delete(winRep)) {
    reportErrNosys("Warning: can't find winRep in _inputWins");
  }
}

//******************************************************************************
