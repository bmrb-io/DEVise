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
  $Id$

  $Log$
 */

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __GNUG__
#pragma interface
#endif

#include <assert.h>

template <class T>
class List{
private:
     struct Cell{
		T element;
		Cell* next;
		Cell(T element) : element(element), next(NULL) {};
	};
	Cell* head;
	Cell* tail;
	Cell* current;
	Cell* previous;
	int numElem;
public:
     List() : head(NULL), tail(NULL), current(NULL), previous(NULL),
		numElem(0) {};
	~List(){
		current = head;
		while(current){
			Cell* next = current->next;
			delete current;
			current = next;
		}
	}
     void append(T element){	
		if(tail){
			tail->next = new Cell(element);
			tail = tail->next;
		}
		else{
			tail = new Cell(element);
			head = tail;
			current = head;
		}
		numElem++;
	}
     void prepend(T element){	
		if(head){
			Cell* oldHead = head;
			head = new Cell(element);
			head->next = oldHead;
		}
		else{
			head = new Cell(element);
			current = tail = head;
		}
		numElem++;
	}
	void remove(){
		assert(current);
		if(previous){
			previous->next = current->next;
		}
		if(current == head){
			head = current->next;
		}
		if(current == tail){
			tail = previous;
		}
		Cell* newCurrent = current->next;
		delete current;
		current = newCurrent;
		numElem--;
	}
	void rewind(){
		current = head;
		previous = NULL;
	}
	bool atEnd(){
		return current == NULL;
	}
	bool isEmpty(){
		return head == NULL;
	}
	T get(){
		assert(current);
		return current->element;
	}
	void step(){
		assert(current);
		previous = current;
		current = current->next;
	}
	void replace(T newElem){
		assert(current);
		current->element = newElem;
	}
	int cardinality(){
		return numElem;
	}
	List<T>* duplicate(){
		List<T>* tmp = new List<T>;
		*tmp = *this;     // Shalow copy
		return tmp;
	}
     void addList(List<T>* list){
          list->rewind();
          while(!list->atEnd()){
               append(list->get());
               list->step();
          }
     }
	bool exists(T item){
		rewind();
		while(!atEnd()){
			if(get() == item){
				return true;
			}
			step();
		}
		return false;
	}
};

#endif
