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
  Revision 1.17  1996/09/13 23:02:42  guangshu
  Added ExportImageAndMap function.

  Revision 1.16  1996/09/10 20:07:08  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.15  1996/09/09 14:31:41  jussi
  Added #ifdef LIBCS statements to make code compile in the
  ClientServer library target.

  Revision 1.14  1996/09/05 21:30:16  jussi
  Moved user-specified screen size to Display.

  Revision 1.13  1996/07/23 15:34:40  jussi
  Added mechanism for bypassing the Devise internal color map.

  Revision 1.12  1996/07/21 02:21:47  jussi
  Made Run() a public method which View::SubclassMapped() can call.

  Revision 1.11  1996/07/18 01:19:32  jussi
  Added ExportImage method.

  Revision 1.10  1996/06/24 19:36:43  jussi
  Removed unnecessary code that stored a pointer to the
  dispatcher in a member variable.

  Revision 1.9  1996/05/20 18:44:59  jussi
  Merged with ClientServer library code.

  Revision 1.8  1996/05/09 18:12:09  kmurli
  No change to this makefile.

  Revision 1.7  1996/04/09 18:05:49  jussi
  Added Flush() method.

  Revision 1.6  1996/04/08 16:56:44  jussi
  Minor fixes.

  Revision 1.5  1996/04/04 05:18:28  kmurli
  Major modification: The dispatcher now receives the register command
  from the displays directly (i.e. from XDisplay instead of from
  Display) corrected a bug in call to register function. Also now
  dispatcher uses socket number passed from the XDisplay class to
  select on it and call the relevant functions.

  Revision 1.4  1996/03/29 18:13:50  wenger
  Got testWindowRep to compile and run, added drawing in
  windows; fixed a few more compile warnings, etc.

  Revision 1.3  1996/01/30 00:02:31  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.2  1995/09/05 21:12:44  jussi
  Added/update CVS header.
*/

#ifndef Display_h
#define Display_h

#include "DeviseTypes.h"
#include "Geom.h"
#ifndef LIBCS
#include "Dispatcher.h"
#endif
#include "DList.h"
#include "ColorMgr.h"
#include "WindowRep.h"

const int InitColorMapSize = 512;
const int AdditionalColorMapSize = 512;

class DeviseDisplay;

DefinePtrDList(DeviseDisplayList, DeviseDisplay *);

#ifdef LIBCS
class DeviseDisplay {
#else
class DeviseDisplay: public DispatcherCallback {
#endif
public:
  DeviseDisplay();

  virtual ~DeviseDisplay();

  /* get the default display */
  static DeviseDisplay *DefaultDisplay();

  /* get the PostScript display */
  static DeviseDisplay *GetPSDisplay();

  /* get file descriptor for connection */
  virtual int fd() = 0;

  virtual int NumPlanes() = 0;
  virtual void Dimensions(Coord &width, Coord &height) =0;

  /* Create a new window rep, 
     relative == 1 if in relative dimensions.*/
  virtual WindowRep* CreateWindowRep(char *name, Coord x, Coord y,
				     Coord width, Coord height, 
				     Color fgnd = ForegroundColor,
				     Color bgnd = BackgroundColor, 
				     WindowRep *parentRep = NULL,
				     Coord min_width = 0.05,
				     Coord min_height = 0.05,
				     Boolean relative = true,
				     Boolean winBoundary = true) = 0;
	
  /* Destroy a window rep*/
  virtual void DestroyWindowRep(WindowRep *c) = 0;

  /* Do internal event processing, but do not block*/
  virtual void InternalProcessing() = 0;

  /* Flush buffered window operations to screen */
  virtual void Flush() = 0;

  /* Export display image to other graphics formats */
  virtual void ExportImage(DisplayExportFormat format, char *filename) = 0;
  virtual void ExportImageAndMap(DisplayExportFormat format, char *gifFilename, 
			char *mapFileame, char *url, char *defaultUrl) = 0;
  virtual void ExportGIF(FILE *fp, int isView) = 0;
  virtual void ExportView(DisplayExportFormat format, char *filename) = 0;

  /* Iterator to go through all displays */
  static int InitIterator() { return _displays.InitIterator(); }
  static Boolean More(int index){ return _displays.More(index); }
  static DeviseDisplay *Next(int index) { return _displays.Next(index); }
  static void DoneIterator(int index) { _displays.DoneIterator(index); }

  /* Process windowing events */
  void Run() { InternalProcessing(); }

  /* Get local color given global color. */
  Color GetLocalColor(Color globalColor);

#ifdef LIBCS
  /* Translate RGB colors to pixel values and back */
  virtual Color FindLocalColor(float r, float g, float b) = 0;
  virtual void FindLocalColor(Color c, float &r, float &g, float &b) = 0;
#endif

#ifndef LIBCS
  /* Get/set desired screen size */
  virtual int &DesiredScreenWidth() { return _desiredScreenWidth; }
  virtual int &DesiredScreenHeight() { return _desiredScreenHeight; }
#endif

protected:
#ifndef LIBCS
  /*
     This is to force the derived classes to register themselves
     with the dispatcher
 */
  virtual void Register() = 0;
#endif

  /* must be called from within the initializer of derived class
     to get the initial set of colors from the color manager */
  void InitializeColors() {
    ColorMgr::InitializeColors(this);
  }

  friend class ColorMgr;

  /* allocate internal colors for the given global color */
  virtual void AllocColor(char *name, Color globalColor) = 0;

  /* allocate internal color by RGB, from 0.0 to 1.0, for the 
     given global color. */
  virtual void AllocColor(float r, float g, float b, Color globalColor) = 0;

  /* functions for derived classes to facilitate color processing.
     Each display instance must match the global color to its own
     local color. MapColor() maps local color to global color, while
     GetLocalColor() retrieeves the local color given global color. */
  
  /* map local color to global color.*/
  void MapColor(Color localColor, Color globalColor);

  friend class WindowRep;

  virtual char *DispatchedName();
  
  static DeviseDisplayList _displays; /* list of all displays */
  
  unsigned long _numColors;
  unsigned long _colorMapSize;
  Color *_colorMap;
  static DeviseDisplay *_defaultDisplay;
  static DeviseDisplay *_psDisplay;

#ifndef LIBCS
  int _desiredScreenWidth;
  int _desiredScreenHeight;
#endif
};

#endif
