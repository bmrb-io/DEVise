/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.12  2000/03/14 17:05:33  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.11  1998/11/06 17:59:52  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.10  1998/11/04 20:34:00  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.9  1998/06/28 21:41:43  beyer
  changed to implicit templates

  Revision 1.8  1998/05/20 18:11:14  wenger
  Disabled automatic loading of the devise.strings file to avoid confusion
  -- load string table explicitly if necessary.

  Revision 1.7  1998/03/02 22:30:25  wenger
  Got DEVise to link on SGI (haha) -- had to change things so that all
  templates are implicit (they are still not implicit on other
  architectures, to save space), had to use GNU's ar instead of SGI's,
  various other kludges.

  Revision 1.6  1998/02/02 18:26:16  wenger
  Strings file can now be loaded manually; name of strings file is now
  stored in session file; added 'serverExit' command and kill_devised
  script to cleanly kill devised; fixed bug 249; more info is now
  printed for unrecognized commands.

  Revision 1.5  1997/11/24 23:15:17  weaver
  Changes for the new ColorManager.

  Revision 1.4  1997/07/17 18:44:00  wenger
  Added menu selections to report number of strings and save string space.

  Revision 1.3  1997/07/16 15:49:14  wenger
  Moved string allocation/deallocation within StringStorage class, fixed
  memory leak of strings.

  Revision 1.2.10.1  1997/05/21 20:40:46  weaver
  Changes for new ColorManager

  Revision 1.2  1997/01/30 19:47:13  jussi
  Added PopulateFromInitFile() method.

  Revision 1.1  1996/07/15 17:00:53  jussi
  Initial revision.
*/

// Changed to non-pragma templates method. CEW 5/12/97
//#ifdef __GNUG__
//#pragma implementation "HashTable.h"
//#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "StringStorage.h"
#include "Util.h"
#include "DevError.h"
#include "DList.h"

//#define DEBUG_STRINGS

//kb: changed to implict-templates, so all arch. must declare this class
//     for gcc 2.7;  egcs 2.9 does not need these two lines.
// #if defined(SGI)
template class HashTable<char *, int>;
template class HashTable<int, char *>;
//#endif

char *StringStorage::_stringFile = NULL;

DefinePtrDList(StringTabList, StringStorage *);

static const char *_defaultFile = "devise.strings";
static StringStorage *_defaultTable = NULL;
static StringTabList _tables;
static const char *_defaultTableName = "default";
static const char *_tableDelimiter = "%StringTable:";
static const char *_sortedStr = "sorted";

StringStorage::StringStorage(const char *name) :
  _stringNum(0),
  _strings(100, StringHash, StringComp),
  _keys(100, KeyHash, 0)
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage::StringStorage(%s)\n", name);
#endif

  _tableName = CopyString(name);
  _sorted = true;

  _tables.Append(this);
}

StringStorage::~StringStorage()
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage(%s)::~StringStorage()\n", _tableName);
#endif

  int check = _tables.Delete(this);
  DOASSERT(check, "String table not in table list");

  if (this == _defaultTable) _defaultTable = NULL;

  Clear();
  FreeString(_tableName);
}

StringStorage *
StringStorage::GetDefaultTable()
{
  if (_defaultTable == NULL) {
    _defaultTable = new StringStorage(_defaultTableName);
  }

  return _defaultTable;
}

StringStorage *
StringStorage::FindByName(const char *name)
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage::FindByName(%s)\n", name ? name : "NULL");
#endif

  // Make sure default table exists.
  (void) GetDefaultTable();

  StringStorage *result = NULL;

  if (name == NULL) name = _defaultTableName;

  int index = _tables.InitIterator();
  while (_tables.More(index) && result == NULL) {
    StringStorage *table = _tables.Next(index);
    if (!strcmp(name, table->_tableName)) {
      result = table;
    }
  }
  _tables.DoneIterator(index);

  return result;
}

int
StringStorage::Insert(char *string, int &key)
{
  if (_strings.lookup(string, key) >= 0) {
    // found string in table
    return 0;
  }
  _sorted = false;
  key = _stringNum++;
  char *tmpString = CopyString(string);
  int code = _strings.insert(tmpString, key);
#if defined(DEBUG_STRINGS)
  printf("Inserting <%s> into hash table\n", string);
  printf("  %d entries in hash table\n", _strings.num());
#endif
  if (code < 0) {
    FreeString(tmpString);
    return code;
  }
  code = _keys.insert(key, tmpString);
  if (code >= 0)
    return 1;
  return code;
}

int
StringStorage::Clear()
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage(%s)::Clear()\n", _tableName);
#endif

  // Delete the strings themselves.
  _strings.InitRetrieveIndex();
  void *current = NULL;
  char **string;
  int key;
  while (_strings.RetrieveIndex(current, string, key) == 0) {
#if defined(DEBUG_STRINGS)
    printf("  deleting <%s>\n", *string);
#endif
    FreeString(*string);
  }

  int code = _strings.clear();
  if (code >= 0) {
    code = _keys.clear();
    if (code >= 0) {
      _stringNum = 0;
       return 0;
    }
  }

  return code;
}

int
StringStorage::ClearAll()
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage::ClearAll()\n");
#endif

  int result = 0;

  while (_tables.Size() > 0) {
    StringStorage *table = _tables.GetFirst();
    if (table->Clear() < 0) result = -1;
    delete table;
  }

  // Re-create default table.
  GetDefaultTable();

  FreeString(_stringFile);
  _stringFile = NULL;

  return result;
}

int
StringStorage::StringHash(char *&string, int numBuckets)
{
  unsigned int sum = 0;
  for(int i = 0; i < (int)strlen(string); i++)
    sum += string[i];
  return sum % numBuckets;
}

int
StringStorage::SaveAll(const char *filename)
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage::SaveAll(filename)\n");
#endif

  int result = 0;

  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    reportErrSys("can't open strings file");
    result = -1; 
  } else {
    int index = _tables.InitIterator();
    while (_tables.More(index)) {
      StringStorage *table = _tables.Next(index);
      const char *sortStr = table->_sorted ? _sortedStr : "";
      fprintf(fp, "%s %s %s\n", _tableDelimiter, table->_tableName, sortStr);

      // Note: this relies on keys being consecutive.
      int count = table->GetCount();
      int key;
      char *string;
      for (key = 0; key < count; key++) {
        if (table->Lookup(key, string) != 0) {
	  reportErrNosys("Key lookup failed");
	  result = -1;
	} else {
          fprintf(fp, "%s\n", string);
	}
      }

    }
    _tables.DoneIterator(index);

    if (fclose(fp) != 0) {
      reportErrSys("error closing strings file");
    }
  }

  if (result == 0) {
    FreeString(_stringFile);
    _stringFile = AddEnvToPath("DEVISE_SESSION", filename);
  }

  return result;
}

int
StringStorage::LoadAll(const char *filename)
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage::LoadAll(%s)\n", filename);
#endif

  int result = 0;

  if (ClearAll() < 0) {
    char errBuf[1024];
    sprintf(errBuf, "Error clearing string tables");
    reportErrNosys(errBuf);
  }

  char * path = RemoveEnvFromPath(filename);
  FILE *fp = fopen(path, "r");
  if (!fp) {
    if (strcmp(path, _defaultFile)) {
      FreeString(path);
      reportErrSys("can't open strings file");
      return -1;
    } else {
      FreeString(path);
      return 0;
    }
  }
  FreeString(path);

  printf("Initializing string table from %s\n", filename);

  StringStorage *table = GetDefaultTable();
  Boolean sort = false;
  const int delimLength = strlen(_tableDelimiter);

  char buf[256];
  while (fgets(buf, sizeof buf, fp)) {

    if (!strncmp(buf, _tableDelimiter, delimLength)) {
      //
      // Line defines a table name.
      //
      if (sort) {
        if (table->Sort() != 0) result = -1;
      }

      (void) strtok(buf, " \t\n"); // Bypass delimiter string
      char *name = strtok(NULL, " \t\n"); // Table name
      if (name == NULL) {
        reportErrNosys("No string table name given");
	table = GetDefaultTable();
      } else {
	table = FindByName(name);
	if (table == NULL) {
          table = new StringStorage(name);
	}

	char *sortStr = strtok(NULL, " \t\n"); // sorted?
	if (sortStr != NULL && !strcmp(sortStr, _sortedStr)) {
	  sort = true;
	} else {
	  sort = false;
	}
      }
    } else {
      //
      // Line is a string to insert into table.
      //
      if (buf[strlen(buf) - 1] == '\n')
          buf[strlen(buf) - 1] = 0;
      int key;
      int code = table->Insert(buf, key);
      if (code < 0) result = -1;
#ifdef DEBUG_STRINGS
      printf("Inserted \"%s\" with key %d, code %d\n", buf, key, code);
#endif
    }
  }
  if (sort) {
    if (table->Sort() != 0) result = -1;
  }

  if (fclose(fp) != 0) {
    reportErrSys("error closing strings file");
  }

  FreeString(_stringFile);
  _stringFile = CopyString(filename);

  return result;
}

int
StringStorage::Sort()
{
#if defined(DEBUG_STRINGS)
  printf("StringStorage(%s)::Sort()\n", _tableName);
#endif

  //
  // Copy strings to an array, clear out the hash tables, sort array with
  // qsort(), re-insert strings.
  //

  int strCount = GetCount();
  char **strings = new char*[strCount];
  if (!strings) {
    reportErrSys("Out of memory");
    return -1;
  }

  // Note: this relies on keys being consecutive.
  for (int key = 0; key < strCount; key++) {
    if (Lookup(key, strings[key]) != 0) {
      reportErrNosys("Key lookup failed");
      return -1;
    }
  }
  _strings.clear();
  _keys.clear();

  qsort(strings, strCount, sizeof(strings[0]), SortComp);

  // Note: this relies on keys being consecutive.
  for (int key = 0; key < strCount; key++) {
    int code = _strings.insert(strings[key], key);
    if (code != 0) {
      reportErrNosys("String insertion failed");
      return -1;
    }
    code = _keys.insert(key, strings[key]);
    if (code != 0) {
      reportErrNosys("Key insertion failed");
      return -1;
    }
  }

  _sorted = true;

  return 0;
}

int
StringStorage::GetTotalCount()
{
  int totalCount = 0;

  int index = _tables.InitIterator();
  while (_tables.More(index)) {
    totalCount += _tables.Next(index)->GetCount();
  }
  _tables.DoneIterator(index);

  return totalCount;
}
