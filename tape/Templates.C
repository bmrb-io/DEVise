/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.4  1996/07/13 02:32:57  jussi
  Changed template instantiation.

  Revision 1.3  1995/09/22 15:46:17  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:50  jussi
  Added CVS header.
*/

#ifdef __GNUG__
#pragma implementation "HashTable.h"
#pragma implementation "SortedTable.h"
#endif

#include "QueryProcFull.h"

#ifdef __GNUG__
template class SortedTable<Coord, RecId>;
#endif

