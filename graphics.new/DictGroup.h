/*
  $Id$

  $Log$*/

/* a dictionary with a group-by function.*/

#ifndef DictGroup_h
#define DictGroup_h
#include "DeviseTypes.h"
#include "DList.h"

const int DictGroup_MaxKeys = 512;
struct DictGroupEntry {
	void *key;
	VoidList *list;
};

class DictGroup {
public:
	DictGroup();

	/* insert value with given key. Values are sorted by key.
	Duplicates are eliminated. */
	void Append(void *key, void *val);

	/* Delete key/val. Ignore if does not exist. */
	void Delete(void *key, void *val);

	/* Iterator to find value given key */
	void InitIterator(void *key);
	Boolean More();
	void *Next();
	void DoneIterator();

	/* Return # of kesy */
	int NumKeys() { return _numKeys; }

	/* Get ith key */
	void *Key(int i);
public:
	DictGroupEntry _entries[DictGroup_MaxKeys];;
	int _numKeys; /* # of distinct keys currently stored */

	/* temp variables used for iterator */
	int _keyIndex;	/* index for the key used in iterator*/
	int _iteratorIndex; /* index for list iterator */
};

#endif
