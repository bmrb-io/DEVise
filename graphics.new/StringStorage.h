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

  $Log$*/

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
        return _keys.insert(key, string);
    }

    static int Lookup(char *string, int &key) {
        return _strings.lookup(string, key);
    }

    static int Lookup(int key, char *&string) {
        return _keys.lookup(key, string);
    }

    static int Clear() {
        int code = _strings.clear();
        if (code < 0) return code;
        return _keys.clear();
    }

  protected:
    static int StringHash(char *&string, int numBuckets) {
        int sum = 0;
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
