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

#ifndef ParseCat_h
#define ParseCat_h

#include "DeviseTypes.h"
#if 0
#include "ClassDir.h"
#include "AttrList.h"
#endif

#define NO_GEN_CLASS_INFO

/* Parse a catalog file and register new file type with the system.
   Return name of new file type if successful, else return NULL */
extern char *ParseCat(char *catFile);

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
