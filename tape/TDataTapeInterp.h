/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.3  1995/09/22 15:43:31  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:49  jussi
  Added CVS header.
*/

#ifndef TDataTapeInterp_h
#define TDataTapeInterp_h

#include "DeviseTypes.h"
#include "ClassDir.h"
#include "TDataTape.h"

class AttrList;
class AttrInfo;

class TDataTapeInterpClassInfo: public ClassInfo {
public:
  TDataTapeInterpClassInfo(char *className, AttrList *attrList, 
			   int recSize, char *separators, int numSeparators,
			   Boolean isSeparator, char *commentString);
  
  TDataTapeInterpClassInfo(char *className, char *name, char *alias,
			   TData *tdata);
  virtual ~TDataTapeInterpClassInfo();

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
  char *_separators;
  int _numSeparators;
  char *_commentString;
  Boolean _isSeparator;
};

/* Max # of fields for which the interprter searches for a
   matching value */

const int TAPE_MAX_MATCHING_ATTRS = 10;

class RecInterp;

class TDataTapeInterp: public TDataTape {
public:
  TDataTapeInterp(char *name, int recSize, AttrList *attrs,
		  char *separators,int numSeparators, 
		  Boolean isSeparator, char *commentString);
  virtual ~TDataTapeInterp();
  
  AttrList *GetAttrList(){ return _attrList; };

protected:
  /* Decode a record; return false if this line is not valid. */
  virtual Boolean IsValid(char *line);
  
  /* Decode a record and put data into buffer. Return false if
     this line is not valid. */
  virtual Boolean Decode(RecId id, void *recordBuf, char *line);
  
  virtual Boolean WriteCache(int fd){ return true; }
  virtual Boolean ReadCache(int fd){ return true;}

private:
  AttrList *_attrList; /* list of attributes */
  Boolean hasComposite;
  char *_name;
  int _recSize;
  char *_separators;
  Boolean _isSeparator;
  int _numSeparators;
  char *_commentString; /* string for comment, or NULL */
  int _commentStringLength; /* length of comment string */
  int _numAttrs;
  AttrInfo *_matchingAttrs[TAPE_MAX_MATCHING_ATTRS];
  int _numMatchingAttrs;
  RecInterp *_recInterp;
};

#endif
