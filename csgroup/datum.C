/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.7.22.1  2013/06/13 21:02:41  wenger
  Changes to get DEVise to compile/link on CentOS6 (with comments for
  a bunch of unfixed warnings); minor mods to get this version to also
  compile on RHEL5...

  Revision 1.7  2005/12/06 20:01:13  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.6.14.1  2005/09/06 21:20:00  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.6  1999/11/19 17:17:18  wenger
  Added View::SetVisualFilterCommand() method to clean up command-related
  code for filter setting.

  Revision 1.5  1998/07/03 23:41:48  wenger
  Fixed some memory leaks; added provision to print data segment size
  at certain places in the code.

  Revision 1.4  1998/03/03 20:54:03  wenger
  Fixed bad free in csgroup code; cleaned up (somewhat) the use of the
  (highly-dangerous) ERROR macro.

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

Datum::Datum(const char *bytearray, int size) {
	
	datasize = size;
	dataval = new char[size];
        //TEMP -- const to non-const conversion warning here
	if (dataval == NULL) { ERROR(FATAL, strerror(errno)); }
	memmove(dataval, bytearray, size);
}

Datum::Datum(const Datum& d) {
		
	datasize = d.datasize;
	dataval = new char[datasize];
        //TEMP -- const to non-const conversion warning here
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
//TEMP -- lots of memory used here; at least some is leaked
	delete [] dataval;

	datasize = size;
	dataval = new char[size];
        //TEMP -- const to non-const conversion warning here
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
	dataval = new char[datasize];
        //TEMP -- const to non-const conversion warning here
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

