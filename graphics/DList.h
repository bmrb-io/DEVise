/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2009
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
  Revision 1.16  2009/05/13 22:41:23  wenger
  Merged x86_64_centos5_br_0 thru x86_64_centos5_br_1/dist_1_9_1x2 to
  the trunk.

  Revision 1.15.42.2  2009/05/01 22:30:40  wenger
  Probably don't need the DList debug output any more.

  Revision 1.15.42.1  2009/05/01 22:26:34  wenger
  Debug code (and a few actual changes) trying to get DEVise to work
  on the x86_64/Centos 5 machines at BMRB (currently, opening
  histogram2.ds causes a core dump).

  Revision 1.15  2001/01/08 20:32:41  wenger
  Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
  back onto the trunk.

  Revision 1.13.2.1  2000/10/18 20:31:52  wenger
  Merged changes from fixed_bug_616 through link_gui_improvements onto
  the branch.

  Revision 1.14  2000/09/11 19:24:45  wenger
  Fixed bug 617 and some other problems with the cursor constraints code.

  Revision 1.13  2000/02/16 18:22:54  wenger
  Better error messages.

  Revision 1.12  1998/12/10 21:53:16  wenger
  Devised now sends GIFs to JavaScreen on a per-view rather than per-window
  basis; GIF "dirty" flags are now also referenced by view rather than by
  window.

  Revision 1.11  1998/09/22 17:23:39  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.10  1997/04/30 21:45:23  wenger
  Fixed non-constant strings in complex mappings bug; TDataAsciiInterp
  no longer gives warning message on blank data lines; added makefile
  targets to make a Purify'd version of multi; fixed uninitialized memory
  read in the DList code; fixed bug that caused 1.4 version of multi to
  always crash; better error messages in DTE command parser; version is
  now 1.4.4.

  Revision 1.9  1996/11/26 16:47:42  ssl
  Added support for Stacked Opaque and Transparent views

  Revision 1.8  1996/08/04 20:58:52  beyer
  Insertion with iterators is now ok.

  Revision 1.7  1996/07/25 14:33:48  guangshu
  Added member function InsertOrderly

  Revision 1.6  1996/04/16 20:06:54  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.5  1996/04/09 18:06:34  jussi
  Added error checking.

  Revision 1.4  1996/04/08 22:13:18  jussi
  Added assertions along the way.

  Revision 1.3  1995/12/28 18:45:12  jussi
  Added copyright notice and made small fixes to remove compiler
  warnings.

  Revision 1.2  1995/09/05 21:12:37  jussi
  Added/updated CVS header.
*/

#ifndef DList_h
#define DList_h

#include "Exit.h"

const int MaxIterators = 5;		/* max # of concurrent iterators */

#define DefineDList(listName, valType)\
class listName {\
\
	struct ListElement {\
		ListElement *next, *prev;\
		valType val;\
	};\
\
	struct IteratorData {\
		ListElement *current;	/* == NULL if not used */\
		int backwards; /* TRUE if iterate backwards */\
	};\
\
public:\
	listName();\
	~listName();\
\
	int Size();\
\
	void Insert(valType v);\
	void InsertOrderly(valType v, int order);\
	void Append(valType v);\
\
	/* Return true if v is in the list */\
	int Find(valType v);\
\
	/* Delete v from list. Return true if v is found */\
	int Delete(valType v);\
\
	/* Return the first element */\
	valType GetFirst();\
\
	/* Return the last element */\
	valType GetLast();\
\
	/* swap elements a and b */\
	void Swap(valType a, valType b);\
\
	/* iterator for the list. This implementation supports\
	multiple iterators, and also  supports \
	Deletecurrent(), which deletes the current element being looked at. \
	Note:  DeleteCurrent() is an error when more than 1 iterator is in \
        effect.\
	*/\
\
	/* Init and return index of iterator */\
	int InitIterator(int backwards=0);\
\
	/* Init iterator to return the last N records */\
	int InitIteratorLastN(int n=1);\
	int More(int index);\
	valType Next(int index); \
	void DeleteCurrent(int index) ;\
	void InsertAfterCurrent(int index, valType v); \
	void InsertBeforeCurrent(int index, valType v); \
	void DoneIterator(int index);\
	void DeleteAll();\
	void PrintIterators();\
\
	int ListOk(); \
\
protected:\
	/* insert node2 after node 1 */\
	void _Insert(ListElement *node1, ListElement *node2);\
\
	/* delete node */\
	void _DListDelete(ListElement *node);\
\
private:\
	void ClearIterators();\
	ListElement *_head;\
	int _size;		/* size of the list */\
\
	IteratorData _iterators[MaxIterators];\
	int _numIterators ;\
};

#define ImplementDList(listName,valType)\
\
listName::listName(){\
	_head = new ListElement;\
	_head->next = _head->prev = _head;\
	_size=0;\
	ClearIterators();\
\
}\
\
listName::~listName() { \
    DeleteAll(); \
	delete _head; \
	_head = NULL; \
}\
\
int listName::Size() { return _size; }\
\
void listName::Insert(valType v){\
	ListElement *node = new ListElement;\
	node->val = v;\
	_Insert(_head, node);\
}\
\
/* insert v in increasing order if order = 1, in decreasing order if order = 0 */\
void listName::InsertOrderly(valType v, int order){\
	ListElement *new_node = new ListElement;\
	ListElement *node;\
	new_node->val = v;\
	if(order == 1) {\
	    for (node = this->_head->next; node != this->_head && v >= node->val; node = node->next){}\
   	} else if(order == 0) {\
	    for (node = this->_head->next; node != this->_head && v <= node->val; node = node->next){}\
	} else {\
	    DOASSERT(0, "DList::InsertOrderly(): invalid order");\
	    return;\
	}\
	_Insert(node->prev, new_node);\
}\
\
void listName::Append(valType v){\
	ListElement *node = new ListElement;\
	node->val = v;\
	_Insert(_head->prev, node);\
}\
\
/* Return the first element */\
valType listName::GetFirst() {\
	DOASSERT(_head->next != _head, "DList::GetFirst(): empty list");\
	return _head->next->val;\
}\
\
/* Return the last element */\
valType listName::GetLast() {\
	DOASSERT(_head->prev != _head, "DList::GetLast(): empty list");\
	return _head->prev->val;\
}\
\
void listName::Swap(valType val1, valType val2) {\
	ListElement *node;\
	ListElement *node1=NULL;\
	ListElement *node2=NULL;\
	for (node = this->_head->next; node != this->_head; node = node->next){\
		if (node->val == val1) {\
			node1 = node;\
		} else if (node->val == val2) {\
			node2 = node;\
		}\
	}\
	DOASSERT(node1 && node2, "DList::Swap(): empty lists");\
	node1->val = val2;\
	node2->val = val1;\
}\
\
\
/* Init and return index of iterator */\
int listName::InitIterator(int backwards) {\
	/* find an empty slot in the array of active iterators */\
	for(int i = 0; i < MaxIterators; i++) {\
		if (_iterators[i].current == NULL){\
			/* found one */\
			_iterators[i].backwards = backwards;\
			if (backwards) {\
				_iterators[i].current = _head->prev;\
			} else {\
				_iterators[i].current = _head->next; \
                        }\
			_numIterators++;\
			return i;\
		}\
	}\
	DOASSERT(0, "DList::InitIterator(): no more space for iterators");\
	return -1; /* to keep compiler happy */\
}\
\
/* DEBUGGING FUNCTION : Print Iterators */ \
void listName::PrintIterators() { \
    printf("DList 0x%p printing active iterators (%d):\n", this, \
	    _numIterators );\
    for (int i = 0; i < MaxIterators; i++) { \
	if (_iterators[i].current != NULL) {\
	   printf("  iterator open %p\n", _iterators[i].current);\
	}\
    }\
}\
\
/* Init iterator to return the last N records */\
int listName::InitIteratorLastN(int n){\
	DOASSERT(n >= 1, "DList::InitIteratorLastN(): invalid parameter");\
	/* find an empty slot in the array of active iterators */\
	for(int i = 0; i < MaxIterators; i++) {\
		if (_iterators[i].current == NULL){\
			/* found one */\
			_iterators[i].backwards = 0;\
			if (Size() >= n){\
				/* more than enough entries. Scan backwards n records */\
				_iterators[i].current = _head->prev;\
				for (int j=1; j < n; j++)\
					_iterators[i].current = _iterators[i].current->prev;\
			}\
			else\
				_iterators[i].current = _head->next; \
			_numIterators++;\
			return i;\
		}\
	}\
	DOASSERT(0, "DList::InitIteratorLastN(): no more space for iterators");\
	return -1; /* to keep compiler happy */\
}\
\
int listName::More(int index) {\
	DOASSERT(index >= 0 && index < MaxIterators, "DList::More(): invalid iterator");\
	return _iterators[index].current != _head;\
}\
\
valType listName::Next(int index) { \
	DOASSERT(index >= 0 && index < MaxIterators, "DList::Next(): invalid iterator");\
	IteratorData *iData = &_iterators[index];\
	valType v = iData->current->val;\
	if (iData->backwards)\
		iData->current = iData->current->prev;\
	else iData->current = iData->current->next; \
	return v; \
}\
\
void listName::DeleteCurrent(int index) {\
	DOASSERT(index >= 0 && index < MaxIterators, "DList::DeleteCurrent(): invalid iterator");\
	DOASSERT(_numIterators <= 1, "DList::DeleteCurrent: cannot delete with multiple iterators");\
	if (_iterators[index].backwards)\
		_DListDelete(_iterators[index].current->next);\
	else\
		_DListDelete(_iterators[index].current->prev);\
}\
void listName::InsertAfterCurrent(int index, valType v){ \
	DOASSERT(index >= 0 && index < MaxIterators, "DList::InsertAfterCurrent(): invalid iterator");\
	IteratorData *iData = &_iterators[index];\
	ListElement *node = new ListElement;\
	node->val = v;\
	if (iData->backwards) \
		_Insert(iData->current, node);\
	else \
		_Insert(iData->current->prev, node);\
} \
void listName::InsertBeforeCurrent(int index, valType v){ \
	DOASSERT(index >= 0 && index < MaxIterators, "DList::InsertBeforeCurrent(): invalid iterator");\
	IteratorData *iData = &_iterators[index];\
	ListElement *node = new ListElement;\
	node->val = v;\
	if (iData->backwards) \
		_Insert(iData->current->prev, node);\
	else \
		_Insert(iData->current->prev->prev, node);\
} \
void listName::DoneIterator(int index){\
	DOASSERT(index >= 0 && index < MaxIterators\
		 && _iterators[index].current != NULL, "DList::DoneIterator(): invalid iterator");\
	_iterators[index].current = NULL;\
	--_numIterators;\
	DOASSERT(_numIterators >= 0, "DList::DoneIterator(): invalid iterator count");\
}\
\
/* insert node2 after node 1 */\
void listName::_Insert(ListElement *node1, ListElement*node2){\
	node2->next = node1->next;\
	node2->prev = node1;\
	node1->next->prev = node2;\
	node1->next = node2;\
	_size++;\
}\
\
/* delete node */\
void listName::_DListDelete(ListElement *node){\
	DOASSERT(node != _head, "DList::_DListDelete(): cannot delete head of list");\
	node->next->prev = node->prev;\
	node->prev->next = node->next;\
	node->next = node->prev = NULL;\
	delete node;\
	_size--;\
}\
\
void listName::ClearIterators(){\
	for(int i = 0; i < MaxIterators; i++)\
		_iterators[i].current = NULL;\
	_numIterators = 0;\
}\
\
int listName::Find(valType v) {\
	ListElement *node;\
	for (node = this->_head->next; node != this->_head; node = node->next){\
		if (node->val == v){\
			return 1; /* true */\
		}\
	}\
	return 0;	/* false */\
}\
\
int listName::Delete(valType v){\
	DOASSERT(!_numIterators, "DList::Delete(): cannot delete with iterator");\
	ListElement *node;\
	for (node = this->_head->next; node != this->_head; node = node->next){\
		if (node->val == v){\
			_DListDelete(node);\
			return 1; /* true */\
		}\
	}\
	return 0;	/* false */\
}\
\
void listName::DeleteAll(){\
	while (_head->next != _head)\
		_DListDelete(_head->next);\
	_size = 0;\
}\
\
int listName::ListOk() { \
	ListElement *node = _head; \
    for (int count = 0; count <= _size; count++) { \
		node = node->next; \
	} \
	if (node != _head) { \
		fprintf(stderr, "Error in next pointers\n"); \
		return 0; \
	} \
\
    for (int count = 0; count <= _size; count++) { \
		node = node->prev; \
	} \
	if (node != _head) { \
		fprintf(stderr, "Error in prev pointers\n"); \
		return 0; \
	} \
	return 1; \
} \

/* define a void list */
DefineDList(VoidList, void *)

/* template to convert pointer list to use VoidList. 
valType must be a pointer. */
#define DefinePtrDList(listName, valType)\
class listName {\
public:\
	int Size() { return _voidList.Size(); }\
\
	void Insert(valType v) { _voidList.Insert(static_cast<void *>(v));}\
	void Append(valType v) { _voidList.Append(static_cast<void *>(v));}\
\
	/* Return true if v is in the list */\
	int Find(valType v) { return _voidList.Find(static_cast<void *>(v));}\
\
	/* Delete v from list. Return true if v is found */\
	int Delete(valType v) { return _voidList.Delete(static_cast<void *>(v)); }\
\
	/* Return the first element */\
	valType GetFirst() { return static_cast<valType>(_voidList.GetFirst());} \
\
	/* Return the last element */\
	valType GetLast() { return static_cast<valType>(_voidList.GetLast());}\
\
	/* Swap two list elements */\
	void Swap(valType val1, valType val2) { \
		_voidList.Swap(static_cast<void *>(val1),static_cast<void *>(val2));\
	}\
\
	/* Init and return index of iterator */\
	int InitIterator(int backwards=0) {\
		return _voidList.InitIterator(backwards);}\
\
	/* Init iterator to return the last N records */\
	int InitIteratorLastN(int n=1){ return _voidList.InitIteratorLastN(n);}\
	int More(int index){return _voidList.More(index);}\
	valType Next(int index){ return static_cast<valType>(_voidList.Next(index));} \
	void DeleteCurrent(int index){_voidList.DeleteCurrent(index);} \
	void InsertAfterCurrent(int indx, valType v) { \
		_voidList.InsertAfterCurrent(indx,v);\
	}\
	void InsertBeforeCurrent(int indx, valType v) {\
		_voidList.InsertBeforeCurrent(indx,v);\
	}\
	void DoneIterator(int index){_voidList.DoneIterator(index);}\
	void DeleteAll() {_voidList.DeleteAll();}\
	void PrintIterators() {_voidList.PrintIterators();}\
\
	int ListOk() { return _voidList.ListOk(); } \
private:\
	VoidList _voidList; \
};

#endif
