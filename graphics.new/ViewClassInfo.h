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
#include "TDataViewX.h"
#include "ViewScatter.h"
#include "ViewLens.h"


class ViewClassInfo: public ClassInfo {
public:
  ViewClassInfo();
  ViewClassInfo(char *name, ViewGraph *view);
  virtual ~ViewClassInfo();

  virtual char *CategoryName() { return "view"; }
  virtual char *InstanceName() { return _name; }
  virtual void *GetInstance() { return _view; }

  virtual void ParamNames(int &argc, char **&argv);
  virtual void CreateParams(int &argc, char **&argv);
  virtual Boolean Changeable() { return true; }
  virtual void ChangeParams(int argc, char **argv);

protected:
  char *_name;
  char *_fgName;
  char *_bgName;
  ViewGraph *_view;
};

class ViewXInfo: public ViewClassInfo {
public:
  ViewXInfo() : ViewClassInfo() {}
  ViewXInfo(char *name, TDataViewX *view);
  virtual char *ClassName() { return "SortedX"; }
  virtual ClassInfo *CreateWithParams(int argc, char **argv);
};

class ViewScatterInfo : public ViewClassInfo {
public:
  ViewScatterInfo() : ViewClassInfo() {}
  ViewScatterInfo(char *name, ViewScatter *view);
  virtual char *ClassName() { return "Scatter"; }
  virtual ClassInfo *CreateWithParams(int argc, char **argv);
};

class ViewLensInfo : public ViewClassInfo {
public:
   ViewLensInfo() : ViewClassInfo() {}
   ViewLensInfo(char *name, ViewLens *view);
   virtual char *ClassName() { return "ViewLens";}
   virtual ClassInfo *CreateWithParams(int argc, char **argv);
};  
#endif
