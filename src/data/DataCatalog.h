/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DataCatalog class (implements data source catalog access
  for non-DTE version of DEVise).
 */

/*
  $Id$

  $Log$
  Revision 1.2  1999/09/07 19:01:14  wenger
  dteInsertCatalogEntry command changed to tolerate an attempt to insert
  duplicate entries without causing a problem (to allow us to get rid of
  Tcl in session files); changed Condor session scripts to take out Tcl
  control statements in data source definitions; added viewGetImplicitHome
  and related code in Session class so this gets saved in session files
  (still no GUI for setting this, though); removed SEQ-related code.

  Revision 1.1  1998/11/19 21:13:08  wenger
  Implemented non-DTE version of DEVise (new code handles data source catalog
  functions; Tables, SQLViews, etc., are not implemented); changed version to
  1.6.0.

 */

#ifndef _DataCatalog_h_
#define _DataCatalog_h_

#include <sys/types.h>

#include "DeviseTypes.h"

class DataCatalog {
public:
  static DataCatalog *Instance();

  DataCatalog(const char *filename = NULL);
  ~DataCatalog();

  // Returns a catalog entry (if found); format depends on the type of
  // entry.
  char *ShowEntry(const char *entryName);

  // Returns a list of the format: {"<name>" "<type>"} {"<name>" "<type>"} ...
  char *ListCatalog(const char *catName);

  // Returns 0 if okay, -1 if error.
  int AddEntry(const char *catName, const char *entry);

  // Returns 0 if okay, -1 if error.
  int DeleteEntry(const char *entryName);

  // Finds the entry name (without quotes) given an entry string.
  // Returns true if it found a name.
  static Boolean GetEntryName(const char *entry, char nameBuf[], int bufSize);

protected:
  char *FindEntry(const char *entryName, const char *catFile);
  char *FindCatFile(const char *catName, char buf[], int bufSize);

  char *_catFile;
};

#endif // _DataCatalog_h_

/*============================================================================*/
