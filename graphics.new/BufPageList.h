/*
  $Id$

  $Log$*/

/* 
   Doubly linked list of buffer pages.
   Uses the "next" and "prev" fields of a BufPage to link
   them into a list.
*/

#ifndef BufPageList_h
#define BufPageList_h
#include "DeviseTypes.h"
#include "BufMgr.h"

class BufPageList{
public:

	/* inesrt page into list */
	void Insert(BufPage *page){ InsertAfter(_head, page);}

	/* append page to end of list */
	void Append(BufPage *page){ InsertAfter(_head->prev, page);};

	/* insert page after current */
	void InsertAfter(BufPage *current, BufPage *page);

	/* delete page from list. Note that deletion is static 
	--> implementation does not know when a node is deleted. 
	Therefore, the size of the list can not be stored in the
	list itself. */
	static void Delete(BufPage *page);

	/* Delete 1st page from list */
	BufPage *DeleteFirst();

	/* Delete last page from list */
	BufPage *DeleteLast();

	/* Return TRUE if list is empty */
	Boolean Empty(){ return _head->next == _head;};

	/* Merge the given list with this one. Make the other list empty */
	void Merge(BufPageList *list);

	/* Get 1st element in the list. Return NULL if empty.*/
	BufPage *GetFirst();

	/* Get last element ine list. return NULL if empty*/
	BufPage *GetLast();

	/* Get next element in the list. Return NULL if no more */
	BufPage *GetNext(BufPage *current);

	/* Get previous element in the list. Return NULL if no more */
	BufPage *GetPrev(BufPage *current);

protected:
	friend class BufMgr;
	BufPageList();
	~BufPageList();

	/* Reinitialize list to empty list w/o deleting elements */
	void ReInit() { _head->next = _head->prev = _head; };

private:

	BufPage *_head;
};
#endif
