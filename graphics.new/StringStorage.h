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

class StringStorage {
  public:
    static int Insert(char *string, int &key) {
        if (_strings.lookup(string, key) >= 0) {
            // found string in table
            return 0;
        }
        key = _stringNum++;
        int code = _strings.insert(string, key);
        if (code < 0) return code;
        code = _keys.insert(key, string);
        if (code >= 0)
          return 1;
        return code;
    }

    static int Lookup(char *string, int &key) {
        return _strings.lookup(string, key);
    }

    static int Lookup(int key, char *&string) {
        return _keys.lookup(key, string);
    }

    static int Clear() {
        int code = _strings.clear();
        if (code >= 0) {
            code = _keys.clear();
            if (code >= 0) {
                PopulateFromInitFile();
                return 0;
            }
        }
        return code;
    }

    static int PopulateFromInitFile();

  protected:
    static int StringHash(char *&string, int numBuckets) {
        unsigned int sum = 0;
        for(int i = 0; i < (int)strlen(string); i++)
            sum += string[i];
        return sum % numBuckets;
    }

    static int StringComp(char *&string1, char *&string2) {
        return strcmp(string1, string2);
    }

    static int KeyHash(int &key, int numBuckets) {
        return key % numBuckets;
    }

    static int _stringNum;                   // sequence number for string
    static HashTable<char *, int> _strings;  // hash table of strings
    static HashTable<int, char *> _keys;     // hash table of keys
};

#endif
