/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.8  1997/11/23 21:23:44  donjerko
  *** empty log message ***

  Revision 1.7  1997/11/12 23:17:46  donjerko
  Improved error checking.

  Revision 1.6  1997/11/05 00:20:25  donjerko
  Added some error checking calls to the DTE.

  Revision 1.5  1997/09/23 19:58:19  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.4  1997/08/21 21:04:50  donjerko
  Implemented view materialization

  Revision 1.3  1997/03/28 16:07:35  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef CATALOG_COMM_H
#define CATALOG_COMM_H

#include <string>
#include "DeviseTypes.h"

char* executeQuery(const char* query); // throws exception

char* dteListCatalog(const char* catName, int& errorCode);

char* dteShowCatalogEntry(const char* catName, const char* entryName);

char* dteShowCatalogEntry(const char* tableName);

void dteDeleteCatalogEntry(const char* catName, const char* entryName);

void dteDeleteCatalogEntry(const char* tableName);

void dteMaterializeCatalogEntry(const char* tableName);

int dteInsertCatalogEntry(const char* catName, const char* values);

string dteCheckSQLViewEntry(const char* asClause, const char* queryToCheck);

char* dteImportFileType(char* name);

char* dteShowAttrNames(const char* schemaFile, const char* dataFile);

char* dteListAttributes(const char* tableName);

char* dteListQueryAttributes(const char* query);

char* dteReadSQLFilter(const char* fileName);

void dteCreateIndex(const char* tableName, const char* indexName, 
	const char* keyAttrs, const char* dataAttrs, const char* isStandAlone);

char* dteListAllIndexes(const char* tableName);

char* dteShowIndexDesc(const char* tableName, const char* indexName);

void dteDeleteIndex(const char* tableName, const char* indexName);

Boolean isDteType(const char *type);

#endif
