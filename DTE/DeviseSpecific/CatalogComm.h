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
  Revision 1.3  1997/03/28 16:07:35  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef CATALOG_COMM_H
#define CATALOG_COMM_H

char* executeQuery(const char* query); // throws exception

char* dteListCatalog(const char* catName);

char* dteShowCatalogEntry(const char* catName, const char* entryName);

char* dteShowCatalogEntry(const char* tableName);

void dteDeleteCatalogEntry(const char* catName, const char* entryName);

void dteDeleteCatalogEntry(const char* tableName);

void dteMaterializeCatalogEntry(const char* tableName);

void dteInsertCatalogEntry(const char* catName, const char* values);

char* dteImportFileType(char* name);

char* dteShowAttrNames(const char* schemaFile, const char* dataFile);

char* dteListAttributes(const char* tableName);

char* dteReadSQLFilter(const char* fileName);

void dteCreateIndex(const char* tableName, const char* indexName, 
	const char* keyAttrs, const char* dataAttrs, const char* isStandAlone);

char* dteListAllIndexes();

char* dteShowIndexDesc(const char* tableName, const char* indexName);

void dteDeleteIndex(const char* tableName, const char* indexName);

#endif
