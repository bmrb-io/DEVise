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
  Revision 1.2  1997/03/28 16:07:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef UTILITY_H
#define UTILITY_H

#include<String.h>
#include<iostream.h>

String& stripQuotes(char* str);	// can throw excetion

String& stripQuotes(istream& in);	// can throw excetion

String& addQuotes(String str);

#endif
