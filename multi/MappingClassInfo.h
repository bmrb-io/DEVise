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
  Revision 1.2  1996/05/07 21:13:11  jussi
  Renamed MappingInfo to MultiMappingInfo.
*/

#ifndef MappingClassInfo_h
#define MappingClassInfo_h

#include "ClassDir.h"

/* Info needed by mapping */
struct MapInfo {
  int *job_ordering;
  GlobalColor **colorArrays;
};

extern GlobalColor suspendColor;
extern GlobalColor abortColor;
extern MapInfo *mapInfo;

class MultiMappingInfo : public ClassInfo {
public:
  /* for creating class */
  MultiMappingInfo();

  /* for creating instance */
  MultiMappingInfo(char *tdataAlias, char *name, TDataMap *mapping);

  /* Info for category */
  virtual char *CategoryName() { return "mapping"; }

  /* Info for class */
  virtual char *ClassName() { return "MultiMap"; }

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
  char *_tdataAlias;
  TDataMap *_map;
};

#endif
