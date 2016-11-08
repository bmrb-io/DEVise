/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.3.44.1  2005/09/06 21:20:11  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.3  1997/03/28 16:09:29  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 21:13:49  jussi
  Added/update CVS header.
*/

#include <stdio.h>
#include <time.h>
#include "Util.h"

int main(){
	double d1 = 626047200.0;
	double d2 = 626047200.0;
	printf("%s\n",DateString(d1));
	printf("%s\n",DateString(d2));

	return 0;
}
