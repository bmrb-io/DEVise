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
  Revision 1.1.2.3  1998/02/02 08:23:49  liping
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
#include "bucket.h"

Bucket::Bucket() {
	int i;

	NoSlots = BUCKET_SIZE;
	SlotArray = new (Entry *) [NoSlots];
	for(i=0; i<NoSlots; i++)
		SlotArray[i] = NULL;
	next = NULL;
	FullSlots = 0;
	currSlot = 0;
}

Bucket::Bucket(int NumSlots) {
	int i;

	NoSlots = NumSlots;
	SlotArray = new (Entry *) [NoSlots];
	for(i=0; i<NoSlots; i++)
		SlotArray[i] = NULL;
	next = NULL;
	FullSlots = 0;
	currSlot = 0;
}

Bucket::Bucket(const Bucket& b) {
	int i;

	NoSlots = b.NoSlots;
	SlotArray = new (Entry *) [NoSlots];
	for(i=0; i<NoSlots; i++) {
		if (b.SlotArray[i] != NULL) {
			SlotArray[i] = new (Entry);
			*(SlotArray[i]) = *(b.SlotArray[i]);
		}
		else {
			SlotArray[i] = NULL;
		}
	}
	FullSlots = b.FullSlots;
	next = b.next;
}

Bucket::~Bucket() {

	for (; NoSlots > 0; NoSlots--) {
		delete SlotArray[NoSlots-1];
	}
	delete [] SlotArray;
}

Entry*
Bucket::getEntry(int index) {

	return SlotArray[index];
}

void
Bucket::setEntry(Entry* e, int index) {

	SlotArray[index] = e;
	FullSlots++;
}

void
Bucket::unsetEntry(int index) {

	delete SlotArray[index];
	SlotArray[index] = NULL;
	FullSlots--;
}

Bucket*
Bucket::getNextPtr() {

	return next;
}

int
Bucket::size() {

	return NoSlots;
}

void
Bucket::setNextPtr(Bucket *b) {

	next = b;
}

int
Bucket::isfull() {

	return (FullSlots == NoSlots);
}

int
Bucket::isempty() {
	
	return (FullSlots == 0);
}

int
Bucket::islast() {

	if (next == NULL)
		return 1;
	return 0;
}

Entry*
Bucket::getfirst() {
	
	if ((FullSlots == 0)&&(next == NULL))
		return NULL;
	if (FullSlots == 0) {
		currSlot = NoSlots;
		return (next->getfirst());
	}
	currSlot = 0;
	while(currSlot < NoSlots) {
		if (SlotArray[currSlot]) {
			return (SlotArray[currSlot++]);
		}
		currSlot++;
	}
	return NULL;
}

void
Bucket::resetCurrent() {
	currSlot = 0;
}

Entry*
Bucket::getnext() {

	if ((currSlot == NoSlots)&&(next == NULL))
		return NULL;
	if (currSlot == NoSlots)
		return (next->getnext());
	while(currSlot < NoSlots) {
		if (SlotArray[currSlot]) {
			return (SlotArray[currSlot++]);
		}
		currSlot++;
	}
	if (next == NULL)
		return NULL;
	return (next->getfirst());
}

Bucket&
Bucket::operator=(const Bucket& b) {

	if (this == &b)
		return *this;

	for(; NoSlots>0; NoSlots--)
		delete SlotArray[NoSlots-1];

	for(NoSlots=0; NoSlots < b.NoSlots; NoSlots++) {
		if (b.SlotArray[NoSlots] != NULL) {
			*(SlotArray[NoSlots]) = *(b.SlotArray[NoSlots]);
		}
		else {
			SlotArray[NoSlots] = NULL;
		}
	}
	
	FullSlots = b.FullSlots;
	next = b.next;
	
	return *this;
}

ostream&
operator<<(ostream& os, const Bucket& b) {
	int i;

	for(i=0; i<b.NoSlots; i++) 
		os << i << ": " << *(b.SlotArray[i]) << endl;

	return os;
}
