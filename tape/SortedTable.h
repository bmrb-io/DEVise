/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:31:44  jussi
  Added CVS header.
*/

#ifndef SORTEDTABLE_H
#define SORTEDTABLE_H

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream.h>
#include <assert.h>

#define DUMP

// a generic sorted table with binary search interface

template <class Index, class Value>
class SortedTable {
 public:
  SortedTable();                        // constructor
  ~SortedTable();                       // destructor

  int insert(Index &index, Value &value);
  int insertOne(Index &index, Value &value);
  int lookup(Index &index, Value &value);
  int lookupLower(Index &index, Value &value);
  int lookupHigher(Index &index, Value &value);
  int getNext(Index &index, void *current, Value &value,
	      void *&next);
  int remove(Index &index);  
  int clear();

 private:
  int findEmpty(Index &index);          // find empty slot in table,
                                        // or expand if none available
#ifdef DEBUG
  void dump();                          // dump contents of sorted table
#endif

  struct TableEntry {                   // sorted table entry
    Index index;                        // index (sort) key
    Value value;                        // value
  };

  int tableSize;                        // size of sorted table
  TableEntry **st;                      // sorted table
};


// Construct sorted table.

template <class Index, class Value>
SortedTable<Index,Value>::SortedTable() :
	tableSize(0), st(0)
{
}


// Delete sorted table by deallocating table entries and then
// deleting table.

template <class Index, class Value>
SortedTable<Index,Value>::~SortedTable()
{
  clear();
  delete st;
}


// Insert entry into sorted table mapping Index to Value.
// Returns 0 if OK, an error code otherwise.

template <class Index, class Value>
int SortedTable<Index,Value>::insert(Index &index, Value &value)
{
  int idx = findEmpty(index);

  TableEntry *entry = new TableEntry;
  if (!entry) {
    cerr << "Insufficient memory" << endl;
    return -1;
  }

  entry->index = index;
  entry->value = value;
  st[idx] = entry;
  
#ifdef DEBUG
  dump();
#endif

  return 0;
}


// Insert entry into sorted table mapping Index to Value.
// Do not insert duplicates. Returns 0 if OK, an error code otherwise.

template <class Index, class Value>
int SortedTable<Index,Value>::insertOne(Index &index, Value &value)
{
  if (lookup(index, value) >= 0)
    return -1;

  return insert(index, value);
}


// Check if Index is currently in the sorted table. If so, return
// corresponding value and OK status (0). Otherwise return -1.

template <class Index, class Value>
int SortedTable<Index,Value>::lookup(Index &index, Value &value)
{
  // for now, just do a linear scan of table

  for(int idx = 0; idx < tableSize; idx++) {
    if (st[idx]) {
      if (st[idx]->index == index) {    // found key we're looking for?
	value = st[idx]->value;
	return 0;
      }
      if (st[idx]->index > index)       // found larger key value?
	break;
    }
  }

#ifdef DEBUG
  dump();
#endif

  return -1;
}


// Find maximum lower key value in the table. If found, return
// corresponding value and OK status (0), or return -1.

template <class Index, class Value>
int SortedTable<Index,Value>::lookupLower(Index &index, Value &value)
{
  // for now, just do a linear scan of table

  int highest = -1;

  for(int idx = 0; idx < tableSize; idx++) {
    if (st[idx]) {
      if (st[idx]->index >= index) {   // found key we're looking for?
	if (idx > 0) {
	  value = st[idx - 1]->value;
	  return 0;
	}
	break;
      }
      highest = idx;
    }
  }

  if (highest >= 0) {
    value = st[highest]->value;
    return 0;
  }

#ifdef DEBUG
  dump();
#endif

  return -1;
}


// Find minimum lower in the sorted table. If found, return
// corresponding value and OK status (0). Otherwise return -1.

template <class Index, class Value>
int SortedTable<Index,Value>::lookupHigher(Index &index, Value &value)
{
  // for now, just do a linear scan of table

  for(int idx = 0; idx < tableSize; idx++) {
    if (st[idx]
	&& st[idx]->index > index) {    // found key we're looking for?
      value = st[idx]->value;
      return 0;
    }
  }

#ifdef DEBUG
  dump();
#endif

  return -1;
}


// A function which allows duplicate Indices to be retrieved
// iteratively. The first match is returned in next if current
// is NULL. Upon subsequent calls, caller should set
// current = next before calling again. If Index not found,
// returns -1.

template <class Index, class Value>
int SortedTable<Index,Value>::getNext(Index &index, void *current,
				      Value &value, void *&next)
{
  int idx = 0;
  if (current)
    idx = ((TableEntry **)current) - st;
  assert(idx >= 0 && idx < tableSize);

  for(; idx < tableSize; idx++) {
    next = st + idx;
    if (st[idx]) {
      if (st[idx]->index == index) {    // found key we're looking for?
	value = st[idx]->value;
	return 0;
      }
      if (st[idx]->index > index)       // found larger key value?
	break;
    }
  }

#ifdef DEBUG
  dump();
#endif

  return -1;
}


// Delete Index entry from sorted table. Return OK (0) if index was found.
// Else return -1.

template <class Index, class Value>
int SortedTable<Index,Value>::remove(Index &index)
{
  for(int idx = 0; idx < tableSize; idx++) {
    if (st[idx]) {
      if (st[idx]->index == index) {    // found key we're looking for?
	delete st[idx];
	st[idx] = 0;
	return 0;
      }
      if (st[idx]->index > index)       // found larger key value?
	break;
    }
  }

  return -1;
}


// Clear sorted table by deallocating table entries.

template <class Index, class Value>
int SortedTable<Index,Value>::clear()
{
  for(int idx = 0; idx < tableSize; idx++) {
    if (st[idx]) {
      delete st[idx];
      st[idx] = 0;
    }
  }

  return 0;
}


// Find empty slot in table, or expand table if no slots available.

template <class Index, class Value>
int SortedTable<Index,Value>::findEmpty(Index &index)
{
  // just do a linear scan at this point; this is basically insertion
  // sort

  for(int idx = 0; idx < tableSize; idx++) {
    if (st[idx]
	&& st[idx]->index >= index)     // found spot we want to insert into?
      break;
  }

  int spot = idx;                       // spot where to insert

  TableEntry **newTable = new TableEntry * [tableSize + 1];

  assert(newTable);
  for(idx = 0; idx < spot; idx++)
    newTable[idx] = st[idx];
  newTable[spot] = 0;
  for(idx = spot; idx < tableSize; idx++)
    newTable[idx + 1] = st[idx];

  tableSize++;
  delete [] st;
  st = newTable;

  if (tableSize % 10 == 0)
    printf("Table %08x has now %d entries\n", (int)this, tableSize);

  return spot;
}


#ifdef DEBUG
// Dump sorted table contents.

template <class Index, class Value>
void SortedTable<Index,Value>::dump()
{
  for(int idx = 0; idx < tableSize; idx++) {
    cerr << "%%  Table entry " << idx;
    if (st[idx]) {
      cerr << " has key " << st[idx]->index
	   << ", value " << st[idx]->value << endl;
    } else {
      cerr << " is empty." << endl;
    }
  }
}
#endif

#endif
