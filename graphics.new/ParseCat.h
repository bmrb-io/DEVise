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
  Revision 1.14  1996/11/18 18:10:52  donjerko
  New files and changes to make DTE work with Devise

  Revision 1.13  1996/11/15 10:06:19  kmurli
  Changed importFile parameters and ParseCat parameters to take in the file type
  and data file name so that a whole query can be formed if necessary for calling
  DQL type. (In case of a query schema)

  Revision 1.12  1996/11/03 02:41:38  kmurli
  Modified to include the query schema level. Also modified to include DQL
  processing

  Revision 1.11  1996/11/01 19:28:21  kmurli
  Added DQL sources to include access to TDataDQL. This is equivalent to
  TDataAscii/TDataBinary. The DQL type in the Tcl/Tk corresponds to this
  class.

  Revision 1.10  1996/04/19 17:20:49  wenger
  Put the GenClassInfo code back in -- this is needed for tape data;
  started adding the tape-related code back in (it was previously
  deleted for some reason; I'm not yet done adding it back); added
  the 'DEVise parseSchema' command and the first parts of the code
  related to it.

  Revision 1.9  1996/04/17 16:34:41  wenger
  Conditionaled out the GenClassInfo class and all related code,
  since the program exits if that code is ever executed.

  Revision 1.8  1996/01/23 20:55:48  jussi
  Added isAscii parameter to Gen().

  Revision 1.7  1996/01/10 18:46:29  jussi
  Removed unnecessary function prototypes.

  Revision 1.6  1995/12/20 07:04:45  ravim
  High and low values of attrs can be specified.

  Revision 1.5  1995/12/13 02:07:50  ravim
  Groups identified by logical schema file name - new function to extract the
  file name from a path string.

  Revision 1.4  1995/12/12 03:42:33  ravim
  Fixed a bug.

  Revision 1.3  1995/12/11 18:03:29  ravim
  Physical and logical schemas handled.

  Revision 1.2  1995/09/05 22:15:11  jussi
  Added CVS header.
*/

#ifndef ParseCat_h
#define ParseCat_h

#include "DeviseTypes.h"
#include "ClassDir.h"
#include "AttrList.h"
#include <String.h>
#include "DataSourceBuf.h"
//#define NO_GEN_CLASS_INFO

/* Parse a catalog file and register new file type with the system.
   Return name of new file type if successful, else return NULL */
extern char *ParseCat(char *fileType,char *catFile,char *dataFile);
extern char *ParseCat(char *);
extern char *ParseDQL(char *name,char *schema,char * schemaFile,\
					  char *fileType,char *dataFile,char * query);

extern int ParseCatDQL(char *,String &,String &);
extern int ParseDQLCatPhysical(DataSource *,String &);
extern int ParseDQLCatLogical (DataSource *,String &);

/* Parse schema(s) from buffer(s) and register new "file type" with
   the system.  Return the name of the new "file type" if successful,
   otherwise return NULL. */
extern char *ParseSchema(char *schemaName, char *physSchema, char *logSchema);

#ifndef NO_GEN_CLASS_INFO
/* Register a new constructor for class. The
   constructor is called depending on the "source" statment stored
   in the catalog file. For example, "source tape" will call
   the function that generates a TData class that reads from tape */

class AttrList;
class GenClassInfo {
public:
  /* Generate a new TData interpreter */
  virtual ClassInfo *Gen(char *source, Boolean isAscii, char *className,
			 AttrList *attrList, int recSize, char *separators,
			 int numSeparators, Boolean isSeparator,
			 char *commentString) = 0;
};

extern void RegisterGenClassInfo(char *source, GenClassInfo *gen);
#endif

/* Return a list of catalog files */
extern void CatFiles(int &numFiles, char **&fileNames);

/* Delete all catalog files */
extern void ClearCats();

#endif
