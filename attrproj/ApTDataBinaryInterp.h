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
  Revision 1.1  1996/04/25 19:25:20  wenger
  Attribute projection code can now parse a schema, and create the
  corresponding TData object.

*/

#ifndef ApTDataBinaryInterp_h
#define ApTDataBinaryInterp_h

#include "DeviseTypes.h"
//#include "ClassDir.h"
#include "ApTDataBinary.h"

#if 0

class AttrList;
class AttrInfo;

class TDataBinaryInterpClassInfo: public ClassInfo {
public:
  TDataBinaryInterpClassInfo(char *className, AttrList *attrList, int recSize);
	
  TDataBinaryInterpClassInfo(char *className, char *name, char *alias,
			    TData *tdata);
  virtual ~TDataBinaryInterpClassInfo();

  /* Info for category */
  virtual char *CategoryName(){ return "tdata"; } 

  /* Info for class */
  virtual char *ClassName();    /* name of class */

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

  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, char **&argv);

private:
  char *_className;
  char *_name;
  char *_alias;
  TData *_tdata;
  int _recSize;
  AttrList *_attrList;
};
#endif

class TDataBinaryInterp: public TDataBinary {
public:
  TDataBinaryInterp(char *name, int recSize, AttrList *attrs);
  virtual ~TDataBinaryInterp();

  AttrList *GetAttrList(){ return _attrList; };

protected:
  /* Copy record to buffer. Return false if invalid record. */
  virtual Boolean Decode(RecId id, void *recordBuf, char *line);
  
  virtual Boolean WriteCache(int fd){ return true; }
  virtual Boolean ReadCache(int fd){ return true; }

private:
  AttrList *_attrList; /* list of attributes */
  Boolean hasComposite;
  char *_name;
  int _recSize;
  int _numAttrs;
};

#endif
