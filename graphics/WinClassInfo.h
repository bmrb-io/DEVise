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
  Revision 1.2  1995/09/05 21:13:31  jussi
  Added/updated CVS header.
*/

/* Window class info */

#ifndef WinClassInfo_h
#define WinClassInfo_h

#include "ClassDir.h"

class ViewWinVer;
class ViewWinHor;

class WinVerInfo: public ClassInfo {
public:
  WinVerInfo();					/* class constructor */
  WinVerInfo(char *name, ViewWinVer *win);	/* instance constructor */
  virtual ~WinVerInfo();			/* class constructor */

  /*********** Class methods **************************/
  virtual char *CategoryName() { return "window"; }
  virtual char *ClassName() { return "WinVertical"; }

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
  ViewWinVer *_win;
};

class WinHorInfo: public ClassInfo {
public:
  WinHorInfo();					/* class constructor */
  WinHorInfo(char *name, ViewWinHor *win);	/* instance constructor */
  virtual ~WinHorInfo();			/* class constructor */

  /*********** Class methods **************************/
  virtual char *CategoryName() { return "window"; }
  virtual char *ClassName() { return "WinHorizontal"; }

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
  ViewWinHor *_win;
};

#endif
