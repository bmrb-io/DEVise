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
  Revision 1.1.2.3  1998/02/02 08:24:05  liping
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
#include "error.h"
#include "queue.h"

Queue::Queue(QElemCmpFn fn) {

	head = tail = curr = NULL;
	size = 0;
	f = fn;
}

Queue::Queue(const Queue& q) {
	QueueElem *qElem;
	QueueElem *tElem;

	qElem = q.gethead();
	while (qElem != NULL) {
		tElem = new (QueueElem);
		(*tElem) = (*qElem);
		insert(tElem);
		qElem = qElem->getnext();
	}
	f = q.f;
}

Queue::~Queue() {
	QueueElem *qElem;
	QueueElem *nxtElem = NULL;

	qElem = head;
	while (qElem != NULL) {
		nxtElem = qElem->getnext();
		delete qElem;
		qElem = nxtElem;
	}
	head = tail = NULL;
	size = 0;
}

QueueElem*
Queue::getfirst() {

	curr = head;
	return curr;
}

QueueElem*
Queue::getnext() {

	curr = curr->getnext();
	return curr;
}

int
Queue::insert(QueueElem* qElem) {

	qElem->setnext(NULL);
	if ((head == NULL) && (tail == NULL)) {
		head = tail = qElem;
	}
	else {
		tail->setnext(qElem);
		tail = qElem;
	}
	size++;
	return 0;
}

int
Queue::delhead() {
	QueueElem *qElem;

	qElem = head;
	if (qElem == NULL) {
		return -1;
	}
	if (head == tail) {
		head = tail = NULL;
	}
	else {
		head = head->getnext();
	}

	delete qElem;
	size--;
	return 0;
}

int
Queue::del(QueueElem& qe) {
	QueueElem *qElem;
	QueueElem *pElem = NULL;

	qElem = head;
	while (qElem != NULL) {
		if (!((*f)(*qElem, qe))) {
			break;
		}
		pElem = qElem;
		qElem = qElem->getnext();
	}

	if (qElem == NULL) 
		return ER_NOTFOUND;

	if (pElem == NULL) {
		head = qElem->getnext();
		if (head == NULL) tail = NULL;
	}
	else {
		pElem->setnext(qElem->getnext());
	}

	/* DT, Feb 23 */
	if (qElem->getnext() == NULL)
	{
		tail = pElem;
	}
	/* DT, Feb 23 */

	size--;
	delete qElem;
	return 0;
}

QueueElem*
Queue::gethead() const {

	return (head);
}

QueueElem*
Queue::bubble(QueueElem& qe) {
	QueueElem *qElem;
	QueueElem *pElem = NULL;

	qElem = head;
	while (qElem != NULL) {
		if (!((*f)(*qElem, qe))) {
			break;
		}
		pElem = qElem;
		qElem = qElem->getnext();
	}

	if (qElem == NULL) 
	{
		size = 0;
		return NULL;
	}

	if (pElem == NULL) {
		head = qElem->getnext();
		if (head == NULL) tail = NULL;
	}
	else {
		pElem->setnext(qElem->getnext());
	}

	/* DT, Feb 23 */
	if (qElem->getnext() == NULL)
	{
		tail = pElem;
	}
	/* DT, Feb 23 */

	size--;
	return qElem;
}

QueueElem*
Queue::find(QueueElem& qe) {
	QueueElem *qElem;
	QueueElem *pElem = NULL;

	qElem = head;
	while (qElem != NULL) {
		if (!((*f)(*qElem, qe))) {
			break;
		}
		pElem = qElem;
		qElem = qElem->getnext();
	}

	if (qElem == NULL) 
		return NULL;

	return qElem;
}

int
Queue::getsize() const {
	
	return size;
}

ostream&
operator<<(ostream& os, const Queue& q) {

	QueueElem *qElem;
	
	qElem = q.gethead();
	while (qElem != NULL) {
		os << *qElem;
		qElem = qElem->getnext();
	}
	return os;
}
