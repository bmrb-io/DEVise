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
  $Id$

  $Log$
  Revision 1.9  1998/12/15 14:55:26  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.8  1998/03/05 20:36:22  wenger
  Fixed bugs 304 and 309 (problems with view colors); fixed a few other
  problems related to *ClassInfo classes.

  Revision 1.7  1998/02/09 18:11:00  wenger
  Removed ViewScatter class (totally replaced by ViewData, which is a
  renamed version of TDataViewX); removed ViewRanges class (not used);
  re-made Solaris dependencies.

  Revision 1.6  1997/11/24 23:15:22  weaver
  Changes for the new ColorManager.

  Revision 1.5.10.1  1997/05/21 20:40:51  weaver
  Changes for new ColorManager

  Revision 1.5  1996/11/26 16:51:41  ssl
  Added support for piled viws

  Revision 1.4  1996/06/15 13:52:56  jussi
  Rewrote in order to reduce code size and redundancy. Added
  ChangeParams() method which allows Devise to change the
  view foreground and background colors at runtime.

  Revision 1.3  1996/01/29 23:58:16  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:16:16  jussi
  Added CVS header.
*/

#ifndef ViewClassInfo_h
#define ViewClassInfo_h

#include "ViewGraph.h"
#include "ViewData.h"


class ViewClassInfo: public ClassInfo {
public:
  ViewClassInfo();
  ViewClassInfo(const char *name, ViewGraph *view);
  virtual ~ViewClassInfo();

  virtual const char *CategoryName() { return "view"; }
  virtual const char *InstanceName() { return _name; }
  virtual void *GetInstance() { return _view; }

  virtual void ParamNames(int &argc, const char **&argv);
  virtual void CreateParams(int &argc, const char **&argv);
  virtual Boolean Changeable() { return true; }

protected:
  const char *_name;
  char *_fgName;
  char *_bgName;
  ViewGraph *_view;
};

class ViewXInfo: public ViewClassInfo {
public:
  ViewXInfo() : ViewClassInfo() {}
  ViewXInfo(const char *name, ViewData *view);
  virtual const char *ClassName() { return "SortedX"; }
  virtual ClassInfo *CreateWithParams(int argc, const char * const *argv);
};

class ViewScatterInfo : public ViewClassInfo {
public:
  ViewScatterInfo() : ViewClassInfo() {}
  ViewScatterInfo(char *name, ViewGraph *view);
  virtual const char *ClassName() { return "Scatter"; }
  virtual ClassInfo *CreateWithParams(int argc, const char * const *argv);
};

#endif
