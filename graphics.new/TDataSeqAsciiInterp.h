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
  $Id:

  $Log: 
*/

/* interpreted sequential TData (no index) using parsed information */

#ifndef TDataSeqAsciiInterp_h
#define TDataSeqAsciiInterp_h

#include "DeviseTypes.h"
#include "ClassDir.h"
#include "TDataSeqAscii.h"
#include "AttrList.h"

class RecInterp;

class TDataSeqAsciiInterp: public TDataSeqAscii {

  public:
    TDataSeqAsciiInterp(char *name, char *type, char *param,
                   int recSize, AttrList *attrs,
		   char *separators, int numSeparators, 
		   Boolean isSeparator, char *commentString);
   ~TDataSeqAsciiInterp();

    AttrList *GetAttrList(){ return &_attrList; }

  protected:
    /* Decode a record and put data into buffer. Return false if
       this line is not valid. */
    Boolean Decode(void *recordBuf, int recPos, char *line);
  
    void    InvalidateIndex();
    Boolean WriteIndex(int fd);
    Boolean ReadIndex(int fd);

  private:
    AttrList  _attrList;             /* list of attributes */
    Boolean   hasComposite;
    int       _recSize;
    char      *_separators;
    Boolean   _isSeparator;
    int       _numSeparators;
    char      *_commentString;       /* string for comment, or NULL */
    int       _commentStringLength;  /* length of comment string */
    int       _numAttrs;             /* # attributes (including composite) */
    int       _numPhysAttrs;         /* number of physical attributes */
    RecInterp *_recInterp;
};

#endif
