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
  Revision 1.4  2005/12/06 20:04:06  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.3.28.1  2005/09/06 22:04:55  wenger
  Added proper const-ness to HashTable.

  Revision 1.3  1997/11/24 23:15:06  weaver
  Changes for the new ColorManager.

  Revision 1.2.6.1  1997/05/21 20:40:36  weaver
  Changes for new ColorManager

  Revision 1.2  1997/03/28 16:10:23  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

using namespace std;

// Changed to non-pragma templates method. CEW 5/12/97
//#ifdef __GNUG__
//#pragma implementation "HashTable.h"
//#endif

#include "MapIntToInt.h"

//#ifdef __GNUG__
//template class HashTable<int, int>;
//#endif



int IntHash(const int& i, int buckets)
{
    return i % buckets;
}


