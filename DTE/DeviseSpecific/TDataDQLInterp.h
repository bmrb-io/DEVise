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
  Revision 1.9  1997/04/21 22:45:41  guangshu
  Added function CreateWithParametersNew to do group by in DTE.

  Revision 1.8  1997/03/20 20:43:17  donjerko
  Increased the default size of Plex to 1000

  Revision 1.7  1997/03/07 17:40:37  donjerko
  Moved query execution from TDataDQLInterpClassInfo::CreateWithParams
  to TDataDQL::runQuery

  Revision 1.6  1997/03/02 00:03:12  donjerko
  Moved the query execution into TDataDQLInterp::CreateWithParameters.
  It used to be done in TDataDQLInterp constructor.

  Revision 1.5  1997/02/18 18:06:38  donjerko
  Added Drop Index command

  Revision 1.4  1997/02/03 04:09:03  donjerko
  Catalog management moved to DTE

  Revision 1.3  1996/12/16 11:13:45  kmurli
  Changed the naming of attributes from DTE

  Revision 1.2  1996/12/16 07:21:26  donjerko
  Fixed some compile problems

  Revision 1.1  1996/12/15 06:32:01  donjerko
  Added the DeviseSpecific directory to DTE

  Revision 1.5  1996/12/02 16:54:32  wenger
  Fixed compile warning; added standard headers to some files;
  conditionaled out debug code.

 */

/* interpreted TData using parsed information */

#ifndef TDataDQLInterp_h
#define TDataDQLInterp_h

#include "DeviseTypes.h"
#include "ClassDir.h"
#include "TDataDQL.h"
#include "AttrList.h"

#include "TuplePtr.XPlex.h"

class TDataDQLInterpClassInfo: public ClassInfo {
public:
  TDataDQLInterpClassInfo(char* tableName, const char* query);
  TDataDQLInterpClassInfo(char *className,
  AttrList attrs,char * name,char * type,char *query,TData *tdata,
  char* tableName);

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
  ClassInfo *CreateWithParamsNew(char **argv, char* queryS);

  /**************************************************
    Instance Info.
  ***************************************************/
  virtual char *InstanceName();
  virtual void *GetInstance();
  virtual AttrList * GetAttrList() {
  	assert(_tdata);
	return _tdata->GetAttrList();
//	return & _attrs;
  }	
  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, char **&argv);

private:
  char *_query;
  char * _name;
  char* _tableName;
  char * _className; 
  char * _type;
  TData *_tdata;
  int _recSize;
  int _numFlds;
  String* _types;
  AttrList _attrs;
  int* _sizes;
  int _counter;
};
class RecInterp;

class TDataDQLInterp: public TDataDQL {
public:
  TDataDQLInterp(AttrList attrs,char *name,char *type,
	int numFlds, String* types, int recSize, long totalRecs,
	int* sizes);
  TDataDQLInterp(char* tableName, List<char*>* attrList, char* query);
  virtual ~TDataDQLInterp();

//  AttrList *GetAttrList(){ return &_attrList; }
// Should call TDataDQL::GetAttrList

protected:
  /* Decode a record and put data into buffer. Return false if
     this line is not valid. */
  virtual Boolean Decode(void *recordBuf, int recPos, char *line);
  
  virtual void InvalidateIndex();
  virtual Boolean WriteIndex(int fd);
  virtual Boolean ReadIndex(int fd);

private:
//  AttrList  _attrList;             /* list of attributes */

};

#endif
