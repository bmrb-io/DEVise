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
  Revision 1.3  1998/02/26 20:35:11  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

  Revision 1.2  1998/02/12 17:14:48  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:23:55  liping
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "error.h"
#include "datum.h"
Datum NULL_DATUM;

Datum::Datum() {
	
	datasize = 0;
	dataval = NULL;
}

Datum::Datum(char *bytearray, int size) {
	
	datasize = size;
	dataval = new(char) [size];
	if (dataval == NULL) { ERROR(FATAL, strerror(errno)); }
	memmove(dataval, bytearray, size);
}

Datum::Datum(const Datum& d) {
		
	datasize = d.datasize;
	dataval = new(char) [datasize];
	if (dataval == NULL) { ERROR(FATAL, strerror(errno)); }
	memmove(dataval, d.dataval, datasize);
}

Datum::~Datum() {

	datasize = 0;
	delete [] dataval;
	dataval = NULL;
}

char *
Datum::data() const {
	
	return dataval;
}

int
Datum::size() const {

	return datasize;
}

void
Datum::set(char *bytearray, int size) {

	delete [] dataval;

	datasize = size;
	dataval = new(char) [size];
	if (dataval == NULL) { ERROR(FATAL, strerror(errno)); }
	memmove(dataval, bytearray, size);
}
int
Datum::fold() const {
	int x = 0;
	int i;

	for(i=0; i<datasize; i++) 
		x += dataval[i];
	return x;
}

Datum&
Datum::operator=(const Datum& d) {

	if (this == &d)
		return *this;

	delete [] dataval;

	datasize = d.datasize;
	dataval = new(char) [datasize];
	if (dataval == NULL) { ERROR(FATAL, strerror(errno)); }
	memmove(dataval, d.dataval, datasize);
	return *this;
}

int
Datum::operator==(const Datum& d) {

	if (datasize != d.datasize)
		return 0;

	if (!memcmp(dataval, d.dataval, datasize)) 
		return 1;

	return 0;
}

ostream&
operator<<(ostream& os, const Datum& d) {
	int i;

	os << "------------------------------------------------" << endl;
	os << "Size = " << d.datasize << endl;
	os << "Data = ";
	for(i=0; i<d.datasize; i++) 
		os << d.dataval[i];
	os << endl;
	os << "------------------------------------------------" << endl;
	return os;
}

