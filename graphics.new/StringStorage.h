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
  Revision 1.9  1998/02/02 18:26:16  wenger
  Strings file can now be loaded manually; name of strings file is now
  stored in session file; added 'serverExit' command and kill_devised
  script to cleanly kill devised; fixed bug 249; more info is now
  printed for unrecognized commands.

  Revision 1.8  1997/07/17 18:44:01  wenger
  Added menu selections to report number of strings and save string space.

  Revision 1.7  1997/07/16 15:49:18  wenger
  Moved string allocation/deallocation within StringStorage class, fixed
  memory leak of strings.

  Revision 1.6  1997/02/26 16:31:45  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.5.4.1  1997/02/12 15:43:42  jussi
  Added re-initialization of _stringNum in Clear().

  Revision 1.5  1997/01/30 20:01:29  jussi
  Added call to PopulateFromInitFile() from Clear().

  Revision 1.4  1997/01/30 19:47:14  jussi
  Added PopulateFromInitFile() method.

  Revision 1.3  1996/08/29 18:24:42  wenger
  A number of Dali-related improvements: ShapeAttr1 now specifies image
  type when shape is 'image'; added new '-bytes' flag to Dali commands
  when sending images; TDataBinaryInterp now uses StringStorage so GData
  can access strings; fixed hash function for StringStorage so having the
  high bit set in a byte in the string doesn't crash the hash table;
  improved the error checking in some of the Dali code.

  Revision 1.2  1996/07/21 02:53:15  jussi
  The Insert() now returns 1 indicating that a new value was stored,
  or 0 if value was already in the table. A negative value indicates
  a failure.

  Revision 1.1  1996/07/15 17:00:53  jussi
  Initial revision.
*/

#ifndef StringStorage_h
#define StringStorage_h

#include "HashTable.h"
#include "DeviseTypes.h"

class StringStorage {
  public:
    StringStorage(const char *name);
    ~StringStorage();

    static StringStorage *GetDefaultTable();
    static StringStorage *FindByName(const char *name);

    // Return 0 if OK, -1 otherwise.
    static int LoadAll(const char *filename);

    // Return 0 if OK, -1 otherwise.
    static int ClearAll();

    // Return 0 if OK, -1 otherwise.
    static int SaveAll(const char *filename);

    static int GetTotalCount();

    int Insert(char *string, int &key);

    int Lookup(char *string, int &key) {
        return _strings.lookup(string, key);
    }

    int Lookup(int key, char *&string) {
        return _keys.lookup(key, string);
    }

    int GetCount() { return _strings.num(); }

    // Return 0 if OK, -1 otherwise.
    int Clear();

    void Sort();

    static const char *GetFile() { return _stringFile; }

  protected:
    static int StringHash(char *&string, int numBuckets);

    static int StringComp(char *&string1, char *&string2) {
        return strcmp(string1, string2);
    }

    static int KeyHash(int &key, int numBuckets) {
        return key % numBuckets;
    }

    int _stringNum;                   // sequence number for string
    HashTable<char *, int> _strings;  // hash table of strings
    HashTable<int, char *> _keys;     // hash table of keys

    char* _tableName;
    Boolean _sorted;

    static char* _stringFile;
};

#endif
