/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
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

  DataCatalog();
  ~DataCatalog();

  // Returns a catalog entry (if found); format depends on the type of
  // entry.
  char *ShowEntry(char *entryName);

  // Returns a list of the format: {"<name>" "<type>"} {"<name>" "<type>"} ...
  char *ListCatalog(char *catName);

  // Returns 0 if okay, -1 if error.
  int AddEntry(char *catName, char *entry);

  // Returns 0 if okay, -1 if error.
  int DeleteEntry(char *entryName);

  // Finds the entry name (without quotes) given an entry string.
  // Returns true if it found a name.
  static Boolean GetEntryName(const char *entry, char nameBuf[], int bufSize);

protected:
  char *FindEntry(char *entryName, char *catFile);
  char *FindCatFile(char *catName, char *buf, int bufSize);

  char *_catFile;
};

#endif // _DataCatalog_h_

/*============================================================================*/
