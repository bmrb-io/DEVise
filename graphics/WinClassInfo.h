/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.5  1996/11/20 20:34:56  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.4  1995/12/14 15:29:54  jussi
  Replaced WinVertical and WinHorizontal with TileLayout which can
  do both, depending on run-time, user-settable parameters.

  Revision 1.3  1995/12/02 21:26:48  jussi
  Added horizontal view layout and added copyright notice.

  Revision 1.2  1995/09/05 21:13:31  jussi
  Added/updated CVS header.
*/

/* Window class info */

#ifndef WinClassInfo_h
#define WinClassInfo_h

#include "ClassDir.h"

class DevWindow {
public:
  static int GetCount() { return _windowCount; }

protected:

  friend class TileLayoutInfo;

  static int _windowCount;
};

#ifndef NEW_LAYOUT
class TileLayout;
#else
class Layout;
#endif

class TileLayoutInfo: public ClassInfo {
public:
  TileLayoutInfo();			       /* class constructor */
#ifndef NEW_LAYOUT
  TileLayoutInfo(char *name, TileLayout *win); /* instance constructor */
#else 
  TileLayoutInfo(char *name, Layout *win);
#endif

  virtual ~TileLayoutInfo();		       /* class constructor */

  /*********** Class methods **************************/
  virtual char *CategoryName() { return "window"; }
  virtual char *ClassName() { return "TileLayout"; }

  /* Get name of parameters */
  virtual void ParamNames(int &argc, char **&argv) ;

  /* Create instance using the supplied parameters. Return
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, char **argv);

  /************* Instance Methods ***************************/
  virtual char *InstanceName();
  virtual void *GetInstance();

  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, char **&argv);

private:
  char *arg[5];
  char *_name;
#ifndef NEW_LAYOUT
  TileLayout *_win;
#else
  Layout *_win;
#endif
};

#endif





