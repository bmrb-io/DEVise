#ifndef CATALOG_COMM_H
#define CATALOG_COMM_H

char* executeQuery(const char* query); // throws exception

char* dteListCatalog(const char* catName);

char* dteShowCatalogEntry(const char* catName, const char* entryName);

char* dteShowCatalogEntry(const char* tableName);

void dteDeleteCatalogEntry(const char* catName, const char* entryName);

void dteDeleteCatalogEntry(const char* tableName);

void dteInsertCatalogEntry(const char* catName, const char* values);

char* dteImportFileType(char* name);

char* dteShowAttrNames(const char* schemaFile, const char* dataFile);

char* dteListAttributes(const char* tableName);

char* dteReadSQLFilter(const char* fileName);

#endif
