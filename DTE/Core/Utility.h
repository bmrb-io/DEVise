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
  $Id$

  $Log$
  Revision 1.5  1997/07/30 21:39:22  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.4  1997/06/16 16:04:47  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.2  1997/03/28 16:07:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef UTILITY_H
#define UTILITY_H

#include<String.h>
#include<iostream.h>

class Catalog;

String selectFileName(const String& env, const String& def);
Catalog* getRootCatalog();
istream* getIndexTableStream();
ostream* getIndexTableOutStream(int mode = ios::out);

String& stripQuotes(char* str);	// can throw excetion

String& stripQuotes(istream& in);	// obsolete, can throw excetion

void stripQuotes(istream& in, char* buf, int bufsz);// can throw excetion

String& addQuotes(String str);

String* dupStrArr(const String* inp, int numFlds);

#endif
