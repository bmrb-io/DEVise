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
  Revision 1.2  1995/09/05 22:16:16  jussi
  Added CVS header.
*/

#ifndef ViewClassInfo_h
#define ViewClassInfo_h

#include "ClassDir.h"

class TDataViewX;
class View;

class ViewXInfo: public ClassInfo {
public:
  ViewXInfo();
  ViewXInfo(char *name, char *bgName, TDataViewX *view);
  virtual ~ViewXInfo();

  virtual char *CategoryName() { return "view"; }
  virtual char *ClassName() { return "SortedX"; }

  virtual void ParamNames(int &argc, char **&argv);
  virtual ClassInfo *CreateWithParams(int argc, char **argv);
  
  virtual char *InstanceName();
  virtual void *GetInstance();

  virtual void CreateParams(int &argc, char **&argv);

private:
  char *arg[6];
  TDataViewX *_view;
  char *_name;
  char *_bgColorName;
};

class ViewScatter;

class ViewScatterInfo : public ClassInfo {
public:
  ViewScatterInfo();
  ViewScatterInfo(char *name, char *bgName, ViewScatter *view);
  virtual ~ViewScatterInfo();

  virtual char *CategoryName() { return "view"; }
  virtual char *ClassName() { return "Scatter"; }

  virtual void ParamNames(int &argc, char **&argv);
  virtual ClassInfo *CreateWithParams(int argc, char **argv);
  
  virtual char *InstanceName();
  virtual void *GetInstance();

  virtual void CreateParams(int &argc, char **&argv);

private:
  char *arg[6];
  ViewScatter *_view;
  char *_name;
  char *_bgName;
};

#endif
