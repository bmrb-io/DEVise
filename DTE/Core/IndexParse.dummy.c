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
  Revision 1.2  1997/08/22 23:13:02  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.1  1997/08/10 20:30:54  donjerko
  Fixed the NO_RTREE option.

  Revision 1.13  1997/07/30 21:39:17  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.12  1997/07/22 15:00:52  donjerko
  *** empty log message ***

  Revision 1.11  1997/06/21 22:47:58  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.10  1997/06/16 16:04:41  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.9  1997/04/18 20:46:16  donjerko
  Added function pointers to marshall types.

  Revision 1.7  1997/03/28 16:07:24  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

//#include<iostream.h>   erased for sysdep.h
//#include<memory.h>   erased for sysdep.h
#include <string>
#include<assert.h>
#include<math.h>
//#include<stdlib.h>   erased for sysdep.h
//#include<stdio.h>	// for perror   erased for sysdep.h
//#include<errno.h>   erased for sysdep.h

#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
#include "listop.h"
#include "ParseTree.h"
#include "Utility.h"
#include "Iterator.h"
#include "Aggregates.h"
#include "sysdep.h"

static const int DETAIL = 1;
LOG(extern ofstream logFile;)

Site* IndexParse::createSite(){
	assert(!"rtree index is not linked in");
	return NULL;
}
