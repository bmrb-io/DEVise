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
  Revision 1.2  1995/09/05 21:13:49  jussi
  Added/update CVS header.
*/

#include <stdio.h>
#include <time.h>
#include "Util.h"

main(){
	double d1 = 626047200.0;
	double d2 = 626047200.0;
	printf("%s\n",DateString(d1));
	printf("%s\n",DateString(d2));
}
