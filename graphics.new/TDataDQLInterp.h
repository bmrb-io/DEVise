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
 */

/* interpreted TData using parsed information */

#ifndef TDataDQLInterp_h
#define TDataDQLInterp_h

#include "DeviseTypes.h"
#include "ClassDir.h"
#include "TDataDQL.h"
#include "AttrList.h"

#include "TuplePtr.XPlex.h"

#ifndef ATTRPROJ
class TDataDQLInterpClassInfo: public ClassInfo {
public:
  TDataDQLInterpClassInfo(char*className,char * schemaFile,char *fileType,char *dataFile,char *query);
  TDataDQLInterpClassInfo(char *className,char * schemaFile,
  AttrList attrs,char * name,char * type,char *query,TData *tdata);

  virtual ~TDataDQLInterpClassInfo();

  /* Info for category */
  virtual char *CategoryName() { return "tdata"; } 

  /* Info for class */
  virtual char *ClassName();

  /* Get name of parameters and default/current values */
  virtual void ParamNames(int &argc, char **&argv);

  /* Create instance using the supplied parameters. Return
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, char **argv);

  /**************************************************
    Instance Info.
  ***************************************************/
  virtual char *InstanceName();
  virtual void *GetInstance();
  virtual AttrList * GetAttrList() { return & _attrs;}	
  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, char **&argv);

private:
  char *_query;
  char * _name;
  char * _className; 
  char * _schemaFile; 
  char * _type;
  TData *_tdata;
  int _recSize;
  int _numFlds;
  String* _types;
  AttrList _attrs;
  TuplePtrXPlex _result;
  int* _sizes;
};
#endif

class RecInterp;

class TDataDQLInterp: public TDataDQL {
public:
  TDataDQLInterp(AttrList attrs,char *name,char *type,
	int numFlds, String* types, int recSize, TuplePtrXPlex& result,
	int* sizes);
  virtual ~TDataDQLInterp();

  AttrList *GetAttrList(){ return &_attrList; }

protected:
  /* Decode a record and put data into buffer. Return false if
     this line is not valid. */
  virtual Boolean Decode(void *recordBuf, int recPos, char *line);
  
  virtual void InvalidateIndex();
  virtual Boolean WriteIndex(int fd);
  virtual Boolean ReadIndex(int fd);

private:
  AttrList  _attrList;             /* list of attributes */

};

#endif
