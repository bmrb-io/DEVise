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
  Header file for AttrList (schema attribute list) class and related
  stuff.
 */

/*
  $Id$

  $Log$
  Revision 1.4  1995/12/20 07:04:28  ravim
  High and low values of attrs can be specified.

  Revision 1.3  1995/09/05 21:12:23  jussi
  Added/update CVS header.

  Revision 1.2  1995/09/05 20:39:21  jussi
  Added CVS header.
*/

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
	Boolean hasHiVal;
	AttrVal hiVal;
	Boolean hasLoVal;
	AttrVal loVal;
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
		Boolean isComposite = false, Boolean isSorted = false,
			Boolean hasHiVal = false, 
			AttrVal *hiVal = (AttrVal *)NULL, 
			Boolean hasLoVal = false, 
			AttrVal *loVal = (AttrVal *)NULL);

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
	void printVal(AttrVal *aval, AttrType atype);
};

#endif
