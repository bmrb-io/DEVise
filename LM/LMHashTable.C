#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include "LMHashTable.h"


LMHashTable::LMHashTable(int size, int offset) 
{
  _size = size;
  _offset = offset;
  _table = new HashElt *[size];
  for (int i = 0; i < size; i++ ) {
    _table[i] = NULL;
  }
}


LMHashTable::~LMHashTable(void) 
{
  HashElt *p , *q;
  for (int i = 0; i < _size; i++ ) {
    if (_table[i]) {
      for (p = _table[i]; (p->next) ; q = p, p = p->next) 
	{
	  delete p->key;
	  delete p->val;
	  delete q;
	}
    }
  }
  delete _table;
}


void
LMHashTable::Insert(char *key, HashValType type, void *val) 
{
  int index = Hash(key);
  if (_table[index]) {		// collision!!
    if (strcmp(_table[index]->key,  key)== 0) { // element already present
      return;
    } else {
      HandleCollision(index, key, type, val);
    }
  } else {			// proper insert
    _table[index] = new HashElt;
    InsertElement(_table[index], key, type, val, NULL);
  }
}

void
LMHashTable::InsertElement(HashElt *p, char *key, HashValType type, void *val, 
			 HashElt *next)
{
  p->type = type;
  p->key = strdup(key);
  p->val = NULL;
  SetEltVal(p, val);
  p->next = next;
  
}

void 
LMHashTable::SetEltVal(HashElt *p,  void *val)
{
  switch(p->type) {
  case INT :
    {
      if (!p->val) {
	p->val = new int;
      }
      *((int *)p->val) = *((int *)val);
      break;
    }
  case FLOAT :
    {
      if (!p->val) {
	p->val = new float;
      }
      *((float *)p->val) = *((float *)val);
      break;
    }
  case DOUBLE :
    {
      if (!p->val) {
	p->val = new double;
      }
      *((double *)p->val) = *((double *)val);
      break;
    }	
  case STRING :
    {
      if (p->val) {
	delete p->val;
      }
      (char *)p->val = strdup((char *)val);
      break;
    }	
  }
}


void 
LMHashTable::HandleCollision(int index, char *key, HashValType type, void *val)
{
  HashElt *p = _table[index];
  _table[index] = new HashElt;
  InsertElement(_table[index], key, type, val, p);
}

void *
LMHashTable::LookUp(int index)
{
  if (index >= _offset) {
    cout << "Cannot lookup elements beyond offset" << endl;
    return NULL;
  } else {
    return _table[index]->val;
  }
}

HashElt *
LMHashTable::LookUpElt(char *key)
{
  int index = Hash(key);

  for ( HashElt *p = _table[index]; p ; p = p->next) 
    {
      if (strcmp(p->key, key) == 0) {
	return p;
      }
    }
  return NULL;
}

void 
LMHashTable::Delete(char *key) 
{
  int index = Hash(key);
  
  HashElt *prev = _table[index];
  for ( HashElt *p = _table[index]; p ; prev = p, p = p->next) 
    {
      if (strcmp(p->key, key) == 0) {
	if (prev == _table[index]) {
	  _table[index] = p->next;
	} else {
	  prev->next = p->next;
	}
	delete p->key;
	delete p->val;
	p->next = NULL;
	delete p;
	break;
      }
    }
}

int LMHashTable::Hash(char *key)
{
  char *p;
  unsigned h = 0, g;
  for (p = key; *p != '\0'; p++) {
    h = (h << 4) + (*p);
    if ( (g = (h & 0xf0000000)) ) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  } 
  int hashval = (h % (_size - _offset)) + _offset;
  cout << "Hash(" << key << ") =" << hashval << endl;
	 
  return hashval;
}

void LMHashTable::Update(char *key,  void *val)
{
  HashElt *elt = LookUpElt(key);
  SetEltVal(elt, val);
}

void LMHashTable::Update(int index,  void *val)
{
  if (index >= _offset) {
    cout << "Cannot lookup elements beyond offset" << endl;
  } else {
    SetEltVal(_table[index], val);
  }
}

ostream &operator <<(ostream &os, const HashElt *h)
{
  os << "---------------------------------------------------" << endl;
  for (const HashElt *p = h; p ; p = p->next) {
    os << setw(10) << p->key << setw(4) << p->type << "  ";
    switch(p->type) {
    case INT :
      {
	os <<  *((int *)p->val) << endl;		
	break;
      }
    case FLOAT :
      {
	os << *((float *)p->val) << endl;
	break;
      }
    case DOUBLE :
      {
	os << *((double *)p->val) << endl;
	break;
      }	
    case STRING :
      {
	os << (char *)p->val << endl;
	break;
      }	
    }
    os << "---------------------------------------------------" << endl;
  }
  return os;
}

ostream &operator <<(ostream &os, const LMHashTable &h)
{
  os << "**************LMHashTable : Size (" << h._size << ")******************" << endl;
  
  for (int i = 0; i < h._size; i++ ) {
    if ( h._table[i] ) {
      os << setw(4) << i << endl;
      os << h._table[i];
    }
  }
  os << "********************************************************************" << endl;
  return os;
} 










