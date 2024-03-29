/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DaliIfc class.
 */

/*
  $Id$

  $Log$
  Revision 1.9  1999/07/16 21:35:49  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

  Revision 1.8  1998/09/04 17:26:12  wenger
  Got Tasvir images to work in pixmaps (when running the JavaScreen, for
  example) -- fixes bug 385.

  Revision 1.7  1997/05/05 16:53:42  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.6  1997/04/11 18:48:50  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.5  1997/03/25 17:58:49  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.4.4.2  1997/03/07 20:03:55  wenger
  Tasvir images now work in PostScript output; Tasvir images now freed
  on a per-window basis; Tasvir timeout factor can be set on the command
  line; shared memory usage enabled by default.

  Revision 1.4.4.1  1997/02/27 22:46:04  wenger
  Most of the way to having Tasvir images work in PostScript output;
  various WindowRep-related fixes; version now 1.3.4.

  Revision 1.4  1997/01/09 18:41:16  wenger
  Added workarounds for some Tasvir image bugs, added debug code.

  Revision 1.3  1996/09/04 21:24:48  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.2  1996/08/28 00:19:36  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.1  1996/08/23 16:55:29  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

 */

#ifndef _DaliIfc_h_
#define _DaliIfc_h_


#include "Xdef.h"
#include "DeviseTypes.h"


class DaliIfc
{
public:
  static DevStatus ShowImage(const char *daliServer, Drawable win, int centerX,
    int centerY, int width, int height, const char *filename, int imageLen,
    const char *image, int &handle, float timeoutFactor = 1.0,
    int maxImageSize = 1000, Boolean maintainAspect = true);
  static DevStatus PSShowImage(const char *daliServer, int width, int height,
    const char *filename, int imageLen, const char *image, FILE *printfile,
    float timeoutFactor = 1.0, Boolean maintainAspect = true);

  static DevStatus FreeImage(const char *daliServer, int handle);
  static DevStatus FreeWindowImages(const char *daliServer, Drawable win);
  static DevStatus Reset(const char *daliServer);
  static DevStatus Quit(const char *daliServer);

  static DevStatus LaunchServer();

  // Note: a function pointer is used here to avoid introducing more
  // compilation dependencies with the rest of DEVise.  RKW 1999-09-21.
  typedef void (*InfoFcn)(const char *serverName, Boolean killServer);
  static void SetInfoFcn(InfoFcn infoFcn) { _infoFcn = infoFcn; }

private:
  static InfoFcn _infoFcn;
};


#endif /* _DaliIfc_h_ */

/*============================================================================*/
