/*
  Copyright 1993-1996 by Jussi Myllymaki
  
  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name(s) of the copyright holder(s)
  not be used in advertising or publicity pertaining to distribution of
  the software without specific, written prior permission. The copyright
  holder(s) make no representations about the suitability of this
  software for any purpose.  It is provided "as is" without express
  or implied warranty.
  
  Author: Jussi Myllymaki
*/

/*
  $Id$

  $Log$
  Revision 1.6  1996/07/19 03:32:56  jussi
  Fixed bug in lookup().

  Revision 1.5  1996/07/15 16:59:42  jussi
  Added support for user-provided comparison functions.

  Revision 1.4  1996/07/15 14:58:06  jussi
  Updated copyright information to reflect original source.

  Revision 1.3  1995/09/22 15:43:26  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:39  jussi
  Added CVS header.
*/

#define MODIFIED

#ifndef HASH_H
#define HASH_H

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream.h>

// a generic hash bucket class

template <class Index, class Value>
class HashBucket {
 public:
  Index       index;                         // stored index
  Value      value;                          // associated value
  HashBucket<Index, Value> *next;            // next node in the hash table
};

// a generic hash table class

template <class Index, class Value>
class HashTable {
 public:
  HashTable(int tableSize,
	    int (*hashfcn)(Index &index,
			   int numBuckets),
            int (*compare)(Index &index1,
                           Index &index2) = 0);
  ~HashTable();

  int insert(Index &index, Value &value);
  int lookup(Index &index, Value &value);
  int getNext(Index &index, void *current, Value &value,
	      void *&next);
  int remove(Index &index);  
  int clear();

#ifdef MODIFIED
  int num() { return _num; }
#endif

 private:
#ifdef DEBUGHASH
  void dump();                               // dump contents of hash table
#endif

  int tableSize;                             // size of hash table
#ifdef MODIFIED
  int _num;                                  // number of entries in table
#endif

  HashBucket<Index, Value> **ht;             // actual hash table
  int (*hashfcn)(Index &index,
		 int numBuckets);            // hash function
  int (*compfcn)(Index &index1,
                 Index &index2);             // comparison function
};

// Construct hash table. Allocate memory for hash table and
// initialize its elements.

template <class Index, class Value>
HashTable<Index,Value>::HashTable(int tableSz,
				  int (*hashF)(Index &index,
					       int numBuckets),
                                  int (*compF)(Index &index1,
                                               Index &index2)) :
	tableSize(tableSz), hashfcn(hashF), compfcn(compF)
{
  if (!(ht = new HashBucket<Index, Value>* [tableSize])) {
    cerr << "Insufficient memory for hash table" << endl;
    exit(1);
  }
  for(int i = 0; i < tableSize; i++)
    ht[i] = 0;
#ifdef MODIFIED
  _num = 0;
#endif
}

// Insert entry into hash table mapping Index to Value.
// Returns 0 if OK, an error code otherwise.

template <class Index, class Value>
int HashTable<Index,Value>::insert(Index &index, Value &value)
{
  int idx = hashfcn(index, tableSize);

  HashBucket<Index, Value> *bucket;
  if (!(bucket = new HashBucket<Index, Value>)) {
    cerr << "Insufficient memory" << endl;
    return -1;
  }

  bucket->index = index;
  bucket->value = value;
  bucket->next = ht[idx];
  ht[idx] = bucket;

#ifdef MODIFIED
  ++_num;
#endif

#ifdef DEBUGHASH
  dump();
#endif

  return 0;
}

// Check if Index is currently in the hash table. If so, return
// corresponding value and OK status (0). Otherwise return -1.

template <class Index, class Value>
int HashTable<Index,Value>::lookup(Index &index, Value &value)
{
  int idx = hashfcn(index, tableSize);

  HashBucket<Index, Value> *bucket = ht[idx];
  while(bucket) {
    if ((!compfcn && bucket->index == index)
        || (compfcn && !compfcn(bucket->index, index))) {
      value = bucket->value;
      return 0;
    }
    bucket = bucket->next;
  }

#ifdef DEBUGHASH
  dump();
#endif

  return -1;
}

// A function which allows duplicate Indices to be retrieved
// iteratively. The first match is returned in next if current
// is null. Upon subsequent calls, caller should set
// current = next before calling again. If Index not found,
// returns -1.

template <class Index, class Value>
int HashTable<Index,Value>::getNext(Index &index, void *current,
				    Value &value, void *&next)
{
  HashBucket<Index, Value> *bucket;

  if (!current) {
    int idx = hashfcn(index, tableSize);
    bucket = ht[idx];
  } else {
    bucket = (HashBucket<Index, Value> *)current;
    bucket = bucket->next;
  }

  while(bucket) {
    if ((!compfcn && bucket->index == index)
        || (compfcn && !compfcn(bucket->index, index))) {
      value = bucket->value;
      next = bucket;
      return 0;
    }
    bucket = bucket->next;
  }

#ifdef DEBUGHASH
  dump();
#endif

  return -1;
}

// Delete Index entry from hash table. Return OK (0) if index was found.
// Else return -1.

template <class Index, class Value>
int HashTable<Index,Value>::remove(Index &index)
{
  int idx = hashfcn(index, tableSize);

  HashBucket<Index, Value> *bucket = ht[idx];
  HashBucket<Index, Value> *prevBuc = ht[idx];

  while(bucket) {
    if ((!compfcn && bucket->index == index)
        || (compfcn && !compfcn(bucket->index, index))) {
      if (bucket == ht[idx]) 
	ht[idx] = bucket->next;
      else
	prevBuc->next = bucket->next;
      delete bucket;
#ifdef MODIFIED
      --_num;
#endif
#ifdef DEBUGHASH
      dump();
#endif
      return 0;
    }
    prevBuc = bucket;
    bucket = bucket->next;
  }

  return -1;
}

// Clear hash table by deallocating hash buckets in table.

template <class Index, class Value>
int HashTable<Index,Value>::clear()
{
  for(int i = 0; i < tableSize; i++) {
    HashBucket<Index, Value> *tmpBuf = ht[i];
    while(ht[i]) {
      tmpBuf = ht[i];
      ht[i] = ht[i]->next;
      delete tmpBuf;
    }
  }

  return 0;
}

// Delete hash table by deallocating hash buckets in table and then
// deleting table itself.

template <class Index, class Value>
HashTable<Index,Value>::~HashTable()
{
  clear();
  delete ht;
}

#ifdef DEBUGHASH
// Dump hash table contents.

template <class Index, class Value>
void HashTable<Index,Value>::dump()
{
  for(int i = 0; i < tableSize; i++) {
    HashBucket<Index, Value> *tmpBuf = ht[i];
    if (tmpBuf) {
      cerr << "%%  Hash value " << i << ": ";
      while(tmpBuf) {
        cerr << tmpBuf->value << " ";
        tmpBuf = ht[i]->next;
      }
      cerr << endl;
    }
  }
}
#endif

#endif
