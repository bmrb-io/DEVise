/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <libc.h>
#include "BufPageList.h"
#include "Exit.h"

/******************************************************************
Initializer
*******************************************************************/
BufPageList::BufPageList(){
	_head = new BufPage;
	_head->next = _head->prev = _head;
}

/******************************************************************
Destructor
******************************************************************/
BufPageList::~BufPageList(){
	delete _head;
}

/******************************************************************
insert page after current 
*******************************************************************/
void BufPageList::InsertAfter(BufPage *current, BufPage *page){
	current->next->prev = page;
	page->next = current->next;
	page->prev = current;
	current->next= page;
}


/******************************************************************
delete page from list
*******************************************************************/
void BufPageList::Delete(BufPage *page){
	page->prev->next = page->next;
	page->next->prev = page->prev;
	page->next = page->prev = NULL;
}

/******************************************************************
Delete 1st page from list
*******************************************************************/
BufPage *BufPageList::DeleteFirst() {
	if (Empty()){
		fprintf(stderr,"BufPageList::DeleteFirst(): empty buffer\n");
		Exit::DoExit(1);
	}
	BufPage *first= _head->next;
	Delete(first);
	return first;
}
/*********************************************************************
Delete last page from list
**********************************************************************/
BufPage *BufPageList::DeleteLast(){
	if (Empty()){
		fprintf(stderr,"BufPageList::DeleteLast(): empty buffer\n");
		Exit::DoExit(1);
	}
	BufPage *last= _head->prev;
	Delete(last);
	return last;
}

/* Get 1st element in the list. Return NULL if empty.*/
BufPage *BufPageList::GetFirst() { 
	return (_head->next == _head? NULL : _head->next);
}

/* Get last element ine list. return NULL if empty*/
BufPage *BufPageList::GetLast() { 
	return (_head->prev == _head? NULL: _head->prev);
}


/******************************************************************
Get next element in the list. Return NULL if no more 
******************************************************************/
BufPage *BufPageList::GetNext(BufPage *current) {
	if (current->next == _head)
		return NULL;
	else return current->next;
}
				
/********************************************************************
Get previous element in the list. Return NULL if no more 
***********************************************************************/
BufPage *BufPageList::GetPrev(BufPage *current){
	if (current->prev == _head)
		return NULL;
	else return current->prev;
}

/********************************************************************
Merge the given list with this one. Make the other list empty 
*********************************************************************/
void BufPageList::Merge(BufPageList *list){
	if (!list->Empty()){
		BufPage *first = list->GetFirst();
		BufPage *last = list->GetLast();

		first->prev = _head->prev;
		first->prev->next= first;

		last->next = _head;
		_head->prev = last;

		list->ReInit();
	}
}

