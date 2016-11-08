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
  Revision 1.1.2.2  1998/02/02 08:23:57  liping
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
#include "entry.h"

Entry::Entry() {
	
	key = data = NULL;
}

Entry::Entry(const Entry& e) {

	if (key != NULL) {
		delete key;
		key = NULL;
	}
	if (data != NULL) {
		delete data;
		data = NULL;
	}
	key = new (Datum);
	data = new (Datum);
	*key = *(e.key);
	*data = *(e.data);
}

Entry::~Entry() {

	delete key;
	delete data;
}

void
Entry::set(Datum* t_key, Datum* t_data) {

	key = t_key;
	data = t_data;
}

Datum*
Entry::getKey() {
	
	return (key);
}

Datum*
Entry::getData() {

	return (data);
}

int
Entry::isempty() {

	return (key == NULL);
}

Entry&
Entry::operator=(const Entry& e) {

	if (this == &e) 
		return *this;
	if (key != NULL) {
		delete key;
		key = NULL;
	}
	if (data != NULL) {
		delete data;
		data = NULL;
	}
	key = new (Datum);
	data = new (Datum);
	*key = *(e.key);
	*data = *(e.data);
	return *this;
}

ostream&
operator<<(ostream& os, const Entry& e) {
	
	os << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	os << "Key = ";
	os << e.key;
	os << "Data = ";
	os << e.data;
	os << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	return os;
}
