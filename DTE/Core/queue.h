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
  Revision 1.10  1997/06/16 16:04:53  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.8  1997/03/20 20:42:24  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.7  1997/02/03 04:11:35  donjerko
  Catalog management moved to DTE

  Revision 1.6  1996/12/26 03:42:02  kmurli
  MOdified to make joinprev work right

  Revision 1.5  1996/12/24 21:00:53  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.4  1996/12/21 22:21:50  donjerko
  Added hierarchical namespace.

  Revision 1.3  1996/12/09 10:01:54  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.2  1996/12/05 16:06:04  wenger
  Added standard Devise file headers.

 */

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __GNUG__
#pragma interface
#endif

#include <assert.h>
#include <stdio.h>
#include <iostream.h>

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
	int currPos;
public:
     List() : head(NULL), tail(NULL), current(NULL), previous(NULL),
		numElem(0),currPos(0) {};
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
	T remove(){
		if (!current)
			return NULL;
		T retVal = current->element;
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
		if (numElem < 0){ 
			currPos = numElem;
		}
		return retVal;

	}
	void removeAll(){
		previous = head;
		while(previous){
			current = previous->next;
			delete previous;
			previous = current;
		}
		head = NULL;
		tail = NULL;
		currPos = 0;
		current = NULL;
		numElem = 0;
	}
	void rewind(){
		current = head;
		previous = NULL;
		currPos = 0;
	}
	bool atEnd(){
		return current == NULL;
	}
	bool isEmpty() const {
		return head == NULL;
	}
	T get(){
		if (!current)
			return NULL;
		//assert(current);
		return current->element;
	}
	T getTail(){
		if (!tail)
			return NULL;
		//assert(current);
		return tail->element;
	}
	void step(){
		if (!current){
			previous = NULL;
			return;
		}
		//assert(current);
		previous = current;
		current = current->next;
		currPos ++;
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
		*tmp = *this;     // Shallow copy
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
	
	int getCurrPos(){
		
		return currPos;
	}
	bool setPos(T item){

		rewind();
		int pos = 0;
		while(!atEnd()){
			if(get() != item){
			pos ++;
			step();
			}
			else
				return true;
		}
		// Not found
		return false;
	}

	int getPos(T item){
		
		Cell * curr = current;
		Cell * prev = previous;
		int savePos = currPos;
		rewind();
		int pos = 0;
		while(!atEnd()){
			if(get() == item){
				current = curr;
				previous = prev;
				currPos = savePos;
				return pos;
			}
			pos ++;
			step();
		}
		// Not found
		current = curr;
		previous = prev;
		currPos = savePos;
		return -1;
	}

	void display(){
		
		Cell * curr = head;
		int i = 0;
		cout << " The pointer values are -- ";
		while(curr){
			printf(" pointer %d = %p ",i,curr->element);
			i++;
			curr = curr->next;
		}
		cout << endl;
	}
};

#endif
