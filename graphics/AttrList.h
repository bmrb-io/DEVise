/* AttrList.h: keeps track of TData attributes */
#ifndef AttrList_h
#define AttrList_h
#include <sys/time.h>
#include "DeviseTypes.h"
#include "Config.h"
#include "DList.h"


enum AttrType { IntAttr, FloatAttr, DoubleAttr, StringAttr, DateAttr };
union AttrVal {
	double doubleVal;
	float floatVal;
	int intVal; 
	char *strVal;
	time_t dateVal; /* date value */
};

struct AttrInfo {
	char *name; /* name of attribute */
	int attrNum; /* attribute number, starting from 0 */
	int offset; /* offset from beginning of record */
	int length; /* max length of attribute */
	Boolean isComposite; /* true if this attribute is a composite
			requiring user defined parser. */
	Boolean isSorted; /* true if this attirubte is the sort attribute */
	AttrType type; /* attribute type */
	Boolean hasMatchVal; /* used for parsing only */
	AttrVal matchVal;
};

/*
DefinePtrDList(AttrInfoList, AttrInfo *);
*/

/* max # of attributes */
const int MAX_ATTRLIST_SIZE = DEVISE_MAX_ATTRS;

class AttrList {
public:
	AttrList(char *name);
	~AttrList();

	/* Insert attribute into list of attributes */
	void InsertAttr(int attrNum,
		char *name, int offset, int length, AttrType type,
		Boolean hasMatchVal= false, AttrVal *matchVal= (AttrVal *)NULL,
		Boolean isComposite = false, Boolean isSorted = false);

	char *GetName() { return _name; }

	/* Find an attribute, or NULL if not found */
	AttrInfo *Find(char *name);

	/* Get ith attribute info */
	AttrInfo *Get(int index);

	/* Get # of attributes */
	int NumAttrs() { return _size;}

	/* iterator for list of attributes */
	void InitIterator();
	Boolean More();
	AttrInfo *Next();
	void DoneIterator();

	void Print();
private:
	AttrInfo *_attrs[MAX_ATTRLIST_SIZE];
	int _size;
	int _index; /* used for iterator */
	char *_name;
};

#endif
