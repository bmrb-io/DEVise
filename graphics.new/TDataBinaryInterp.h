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
  Revision 1.8  2000/01/11 22:28:34  wenger
  TData indices are now saved when they are built, rather than only when a
  session is saved; other improvements to indexing; indexing info added
  to debug logs; moved duplicate TDataAscii and TDataBinary code up into
  TData class.

  Revision 1.7  1998/10/13 19:40:47  wenger
  Added SetAttrs() function to TData and its subclasses to allow Liping to
  push projection down to the DTE.

  Revision 1.6  1996/07/01 19:28:11  jussi
  Added support for typed data sources (WWW and UNIXFILE). Renamed
  'cache' references to 'index' (cache file is really an index).
  Added support for asynchronous interface to data sources.

  Revision 1.5  1996/06/27 18:12:44  wenger
  Re-integrated most of the attribute projection code (most importantly,
  all of the TData code) into the main code base (reduced the number of
  modules used only in attribute projection).

  Revision 1.4  1996/06/27 15:49:35  jussi
  TDataAscii and TDataBinary now recognize when a file has been deleted,
  shrunk, or has increased in size. The query processor is asked to
  re-issue relevant queries when such events occur.

  Revision 1.3  1996/05/07 16:46:21  jussi
  This class now makes a copy of the attribute list so that attribute
  hi/lo values can be maintained per data stream, not per schema.
  Hi/lo values are now computed after composite parser is executed.

  Revision 1.2  1996/05/05 03:08:37  jussi
  Added support for composite attributes. Also added tape drive
  support.

  Revision 1.1  1996/01/23 20:54:51  jussi
  Initial revision.
*/

#ifndef TDataBinaryInterp_h
#define TDataBinaryInterp_h

#include "DeviseTypes.h"
#include "ClassDir.h"
#include "TDataBinary.h"
#include "AttrList.h"

#ifndef ATTRPROJ
class TDataBinaryInterpClassInfo: public ClassInfo {
public:
  TDataBinaryInterpClassInfo(const char *className, AttrList *attrList,
      int recSize);
	
  TDataBinaryInterpClassInfo(const char *className, const char *name,
      const char *type, const char *param, TData *tdata);
  virtual ~TDataBinaryInterpClassInfo();

  /* Info for category */
  virtual const char *CategoryName(){ return "tdata"; } 

  /* Info for class */
  virtual const char *ClassName();

  /* Get name of parameters and default/current values */
  virtual void ParamNames(int &argc, const char **&argv);

  /* Create instance using the supplied parameters. Return
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, const char * const *argv);

  /**************************************************
    Instance Info.
  ***************************************************/
  virtual const char *InstanceName();
  virtual void *GetInstance();

  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, const char **&argv);

private:
  const char *_className;
  const char *_name;
  const char *_type;
  const char *_param;
  TData *_tdata;
  int _recSize;
  int _physRecSize;
  AttrList *_attrList;
};
#endif

class RecInterp;

class TDataBinaryInterp: public TDataBinary {
public:
  TDataBinaryInterp(char *name, char *type, char *param,
                    int recSize, int physRecSize, AttrList *attrs);
  virtual ~TDataBinaryInterp();

  AttrList *GetAttrList() { return &_attrList; }

  virtual Boolean SetAttrs(const AttrList &attrs) {
    return _attrList.SetAttrs(attrs); }

protected:
  /* Copy record to buffer. Return false if invalid record. */
  virtual Boolean Decode(void *recordBuf, int recPos, char *line);
  
private:
  AttrList _attrList;  /* list of attributes */
  Boolean hasComposite;
  int _recSize;
  int _physRecSize;
  int _numAttrs;       /* number of attributes (including composite) */
  int _numPhysAttrs;   /* number of physical attributes */
  RecInterp *_recInterp;
};

#endif
