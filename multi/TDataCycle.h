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
  Revision 1.4  1996/05/07 21:13:37  jussi
  Added record position parameter to Decode(). Added alias parameter
  to constructor.

  Revision 1.3  1996/04/12 23:43:54  jussi
  Removed IsValid() and removed RecId parameter from Decode().

  Revision 1.2  1995/11/25 19:52:56  jussi
  Added copyright notice and the IsValid() method.
*/

#ifndef TDataCycle_h
#define TDataCycle_h

#include "TDataAscii.h"
#include "ClassDir.h"

class TDataCycle;
class CycleClassInfo : public ClassInfo {
public:
  /* for creating class */
  CycleClassInfo();

  /* for creating instance */
  CycleClassInfo(char *name, char *type, char *param, TDataCycle *tdata); 
  virtual ~CycleClassInfo() {}

  /* Info for category */
  virtual char *CategoryName() { return "tdata"; }

  /* Info for class */
  virtual char *ClassName() { return "Cycle"; }

  /* Get name of parameters */
  virtual void ParamNames(int &argc, char **&argv); 

  /* Create instance using the supplied parameters. Return
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, char **argv);

  virtual char *InstanceName();
  virtual void *GetInstance();

  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, char **&argv);

private:
  char *_name;
  char *_type;
  char *_param;
  TDataCycle *_tdata;
};

class TDataCycle: public TDataAscii {
public:
  TDataCycle(char *name, char *type, char *param);
  virtual ~TDataCycle() {}

protected:
  /* Decode a record and put data into buffer */
  virtual Boolean Decode(void *recordBuf, int recPos, char *line);

private:
  int _first;
  Boolean _hasFirst;
  int _now;
};

#endif
