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

#include "StringStorage.h"
#include "Util.h"
#include "DevError.h"

//#define DEBUG_STRINGS

#if defined(SGI)
template class HashTable<char *, int>;
template class HashTable<int, char *>;
#endif

int StringStorage::_stringNum = 0;
HashTable<char *, int> StringStorage::_strings(100, StringHash, StringComp);
HashTable<int, char *> StringStorage::_keys(100, KeyHash, 0);
char *StringStorage::_stringFile = NULL;

static const char *defaultFile = "devise.strings";

int
StringStorage::Insert(char *string, int &key)
{
  if (_strings.lookup(string, key) >= 0) {
    // found string in table
    return 0;
  }
  key = _stringNum++;
  char *tmpString = CopyString(string);
  int code = _strings.insert(tmpString, key);
#if defined(DEBUG_STRINGS)
  printf("Inserting <%s> into hash table\n", string);
  printf("  %d entries in hash table\n", _strings.num());
#endif
  if (code < 0) {
    delete [] tmpString;
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
  printf("StringStorage::Clear()\n");
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
    delete [] *string;
  }

  int code = _strings.clear();
  if (code >= 0) {
    code = _keys.clear();
    if (code >= 0) {
      _stringNum = 0;
       PopulateFromInitFile();
       return 0;
    }
  }

  delete [] _stringFile;
  _stringFile = NULL;

  return code;
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
StringStorage::PopulateFromInitFile()
{
#if 0
  return Load(defaultFile);
#else
  return 0;
#endif
}

int
StringStorage::Save(const char *filename)
{
  int result = 0;

  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    reportErrSys("can't open strings file");
    result = 1; 
  } else {
    // Note: this relies on keys being consecutive.
    int count = GetCount();
    int key;
    char *string;
    for (key = 0; key < count; key++) {
      Lookup(key, string);
      fprintf(fp, "%s\n", string);
    }

    if (fclose(fp) != 0) {
      reportErrSys("error closing strings file");
    }
  }

  return result;
}

int
StringStorage::Load(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    if (strcmp(filename, defaultFile)) {
      reportErrSys("can't open strings file");
      return -1;
    } else {
      return 0;
    }
  }

  printf("Initializing string table from %s\n", filename);

  char buf[256];
  while (fgets(buf, sizeof buf, fp)) {
    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = 0;
    int key;
    int code = Insert(buf, key);
#ifdef DEBUG
    printf("Inserted \"%s\" with key %d, code %d\n", buf, key, code);
#endif
  }

  if (fclose(fp) != 0) {
    reportErrSys("error closing strings file");
  }

  _stringFile = CopyString(filename);

  return 0;
}
