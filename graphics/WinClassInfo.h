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
  Revision 1.3  1995/12/02 21:26:48  jussi
  Added horizontal view layout and added copyright notice.

  Revision 1.2  1995/09/05 21:13:31  jussi
  Added/updated CVS header.
*/

/* Window class info */

#ifndef WinClassInfo_h
#define WinClassInfo_h

#include "ClassDir.h"

class TileLayout;

class TileLayoutInfo: public ClassInfo {
public:
  TileLayoutInfo();			       /* class constructor */
  TileLayoutInfo(char *name, TileLayout *win); /* instance constructor */
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
  TileLayout *_win;
};

#endif
