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
  Revision 1.17  2000/02/16 18:51:45  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.16  2000/01/11 22:28:34  wenger
  TData indices are now saved when they are built, rather than only when a
  session is saved; other improvements to indexing; indexing info added
  to debug logs; moved duplicate TDataAscii and TDataBinary code up into
  TData class.

  Revision 1.15  1998/10/13 19:40:46  wenger
  Added SetAttrs() function to TData and its subclasses to allow Liping to
  push projection down to the DTE.

  Revision 1.14  1998/01/07 19:29:57  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.13.12.1  1997/12/29 21:23:23  wenger
  A given TDataAscii no longer reports more than 10 decode errors.

  Revision 1.13  1997/03/25 17:59:28  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.12  1997/03/23 23:46:15  donjerko
  *** empty log message ***

  Revision 1.11  1997/03/20 22:21:07  guangshu
  Added include files.

  Revision 1.10.4.1  1997/03/06 19:44:38  wenger
  Fixed various parsing bugs, improved error messages as a result of
  Miron's problems with the GIS data: warns if strings are too long;
  warns if improper separator/whitespace specification; better warnings
  if records don't parse; better error messages from
  MmDdYyHhMmAmPmComposite parser.

  Revision 1.10  1996/07/01 19:28:08  jussi
  Added support for typed data sources (WWW and UNIXFILE). Renamed
  'cache' references to 'index' (cache file is really an index).
  Added support for asynchronous interface to data sources.

  Revision 1.9  1996/06/27 18:12:40  wenger
  Re-integrated most of the attribute projection code (most importantly,
  all of the TData code) into the main code base (reduced the number of
  modules used only in attribute projection).

  Revision 1.8  1996/06/27 15:49:33  jussi
  TDataAscii and TDataBinary now recognize when a file has been deleted,
  shrunk, or has increased in size. The query processor is asked to
  re-issue relevant queries when such events occur.

  Revision 1.7  1996/05/07 16:46:01  jussi
  This class now makes a copy of the attribute list so that attribute
  hi/lo values can be maintained per data stream, not per schema.
  Hi/lo values are now computed after composite parser is executed.

  Revision 1.6  1996/05/05 03:07:46  jussi
  Removed array of pointers to attribute info for matching values.

  Revision 1.5  1996/03/26 21:33:01  jussi
  Added computation of max/min attribute values.

  Revision 1.4  1996/01/23 20:56:06  jussi
  Cleaned up the code a little bit.

  Revision 1.3  1995/11/22 17:05:00  jussi
  Added IsValid() method and cleaned up code. Added copyright notice.

  Revision 1.2  1995/09/05 22:15:52  jussi
  Added CVS header.
*/

/* interpreted TData using parsed information */

#ifndef TDataAsciiInterp_h
#define TDataAsciiInterp_h

#include <stdio.h>

#include "DeviseTypes.h"
#include "ClassDir.h"
#include "TDataAscii.h"

#include "AttrList.h"

#ifndef ATTRPROJ
class TDataAsciiInterpClassInfo: public ClassInfo {
public:
  TDataAsciiInterpClassInfo(const char *className, AttrList *attrList, 
			    int recSize, char *separators, int numSeparators,
			    Boolean isSeparator, char *commentString);
	
  TDataAsciiInterpClassInfo(const char *className, const char *name,
      const char *type, const char *param, TData *tdata);
  virtual ~TDataAsciiInterpClassInfo();

  /* Info for category */
  virtual const char *CategoryName() { return "tdata"; } 

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
  AttrList *_attrList;
  char *_separators;
  int _numSeparators;
  char *_commentString;
  Boolean _isSeparator;
};
#endif

class RecInterp;

class TDataAsciiInterp: public TDataAscii {
public:
  TDataAsciiInterp(char *name, char *type, char *param,
                   int recSize, AttrList *attrs,
		   char *separators, int numSeparators, 
		   Boolean isSeparator, char *commentString);
  virtual ~TDataAsciiInterp();

  AttrList *GetAttrList(){ return &_attrList; }
  virtual Boolean SetAttrs(const AttrList &attrs) {
    return _attrList.SetAttrs(attrs); }

protected:
  /* Decode a record and put data into buffer. Return false if
     this line is not valid. */
  virtual Boolean Decode(void *recordBuf, int recPos, char *line);
  
private:
  static void PrintRec(int numArgs, char **args) {
    printf(" ");
    int argNum;
    for (argNum = 0; argNum < numArgs; argNum++) {
      printf(" <%s>", args[argNum]);
    }
    printf("\n");
  }

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
  int       _decodeErrCount;
};

#endif
