#ifndef _LMHashTable_H_
#define _LMHashTable_H_

enum HashValType { INT = 0, FLOAT, DOUBLE, STRING };
typedef 
struct HashElt {
  HashValType type;
  char *key;
  void *val;
  struct HashElt *next;
} HashElt;

class LMHashTable {
  private :
  HashElt **_table;
  int _size;
  int _offset;

  public :
  // Some initial entries of the LMHashTable are allocated for a fixed
  // set of properties whose indexes are known 
  // This number is the offset, the hash function depends on it
  LMHashTable(int size = 50, int offset = 7);
  ~LMHashTable();
  
  HashElt *LookUpElt(char *key);	// find elt by key
  void *LookUp(char *key)	// find val by key
    {
      HashElt *elt = LookUpElt(key);
      if (elt) {
	return elt->val;
      } else {
	return NULL;
      }
    }
  void *LookUp(int index);	// find value of elt at index 
  void Insert(char *key, HashValType type, void *val);
  void InsertAt(int index, char *key, HashValType type, void *val)
    {
      if (index <= _offset) {
	_table[index] = new HashElt;
	InsertElement(_table[index], key, type, val, NULL);
      }
    }
  void Delete(char *key);
  
  void Update(char *key, void *val);
  void Update(int index, void *val);


  friend 
    ostream &operator<<(ostream &os, const HashElt *h);
  friend
    ostream &operator<<(ostream &os, const LMHashTable &h);

  private :
  int Hash(char *key);
  void HandleCollision(int index, char *key, HashValType type, void *val);
  void InsertElement(HashElt *p, char *key, HashValType type, void *val, 
		     HashElt *next);
  void SetEltVal(HashElt *p,  void *val);
};
  
 
#endif
