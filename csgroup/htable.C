/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.3  1998/02/02 08:23:59  liping
  Added CVS header

*/

/* 
** Copyright 1997 Collaborator Design Team
** 
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted,
** provided that the above copyright notice appear in all copies and that
** both that copyright notice and this permission notice appear in
** supporting documentation, and that the names of the University of
** Wisconsin and the Collaborator Design Team not be used in advertising or
** publicity pertaining to distribution of the software without specific,
** written prior permission.  The University of Wisconsin and the Collaborator
** Design Team make no representations about the suitability of this
** software for any purpose.  It is provided "as is" without express
** or implied warranty.
** 
** THE UNIVERSITY OF WISCONSIN AND THE COLLABORATOR DESIGN TEAM DISCLAIM ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE UNIVERSITY OF
** WISCONSIN OR THE COLLABORATOR DESIGN TEAM BE LIABLE FOR ANY SPECIAL, 
** INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
** NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
** WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
** 
** Author:  Ashish Thusoo
** 	         University of Wisconsin, Computer Sciences Dept.
** 
*/ 
#include "htable.h"

Htable::Htable(int t_size, Hashfn t_f) {
	
	f = t_f;
	size = t_size;
	Table = new Bucket [size];
	currBucket = 0;
}

Htable::~Htable() {

	delete [] Table;
	size = 0;
}

void
Htable::insert(Datum* key, Datum* data) {
	int Address;
	int Size;
	int i;
	Bucket *bptr, *prev;
	Entry *E;

	E = new (Entry);
	E->set(key, data);
	Address = (*f)(*key, size);
	bptr = &(Table[Address]);
	prev = NULL;
	while(bptr != NULL) {
		if (bptr->isfull()) {
			prev = bptr;
			bptr = bptr->getNextPtr();
			continue;
		}
		Size = bptr->size();
		for(i=0; i<Size; i++) {
			if(bptr->getEntry(i))
				continue;
			bptr->setEntry(E, i); /* save copy here */
			return;
		}
	}

	Bucket *newbpt = new Bucket(Table[Address].size());
	newbpt->setEntry(E, 0); /* save copy here */
	/* DT, Feb 1, 98*/
	prev->setNextPtr(newbpt);
	/* DT, Feb 1, 98*/
	return;
}

Datum*
Htable::get(const Datum& key) {
	int Address;
	int Size;
	int i;
	Bucket *bptr;

	Address = (*f)(key, size);
	bptr = &(Table[Address]);
	while(bptr != NULL) {
		Size = bptr->size();
		for(i=0; i<Size; i++) {
			if (!(bptr->getEntry(i)))
				continue;
			if (*(bptr->getEntry(i)->getKey()) == key)
				return (bptr->getEntry(i)->getData());
		}
		bptr = bptr->getNextPtr();
	}
	return NULL;
}

void
Htable::del(const Datum& key) {
	int Address;
	int Size;
	int i;
	Bucket *bptr;
	Bucket *prevbptr = NULL;

	Address = (*f)(key, size);
	bptr = &(Table[Address]);
	while(bptr != NULL) {
		Size = bptr->size();
		for(i=0; i<Size; i++) {
			if (!(bptr->getEntry(i)))
				continue;
			if (!(*(bptr->getEntry(i)->getKey()) == key))
				continue;
			bptr->unsetEntry(i);
			if (bptr->isempty() && prevbptr != NULL) {
				prevbptr->setNextPtr(bptr->getNextPtr());
				delete bptr;
			}
			return;
		}
		prevbptr = bptr;
		bptr = bptr->getNextPtr();
	}
}

/* DT, Feb 3, 98*/
void
Htable::resetCurr(int index) {
	Bucket*	bpt;

	bpt = &Table[index];
	while (bpt != NULL)
	{
		bpt->resetCurrent();
		bpt = bpt->getNextPtr();
	}
	return;
}
/* DT, Feb 3, 98*/

Bucket*
Htable::getfirst() {
	currBucket = 0;
	resetCurr(currBucket);
	return(&(Table[currBucket++]));
}

Bucket*
Htable::getnext() {

	if (currBucket == size)
		return NULL;
	resetCurr(currBucket);
	return (&(Table[currBucket++]));
}

ostream&
operator<<(ostream& os, const Htable& ht) {
	int i;

	for(i=0; i<ht.size; i++)
		os << ht.Table[i];
	return os;
}
