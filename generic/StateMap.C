/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-2002
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
  Revision 1.4  2002/06/17 19:40:52  wenger
  Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.

  Revision 1.3.22.2  2005/09/06 22:04:48  wenger
  Added proper const-ness to HashTable.

  Revision 1.3.22.1  2002/05/27 18:15:47  wenger
  Got DEVise to compile with gcc 2.96 (so I can compile it at NRG).

  Revision 1.3  1998/06/28 21:40:31  beyer
  changed to implicit templates

  Revision 1.2  1998/03/02 22:34:12  wenger
  Got DEVise to link on SGI (haha) -- had to change things so that all
  templates are implicit (they are still not implicit on other
  architectures, to save space), had to use GNU's ar instead of SGI's,
  various other kludges.

  Revision 1.1  1997/01/30 19:47:39  jussi
  Initial revision.
*/

//#ifdef __GNUG__
//#pragma implementation "HashTable.h"
//#endif

#include <string.h>

#include "StateMap.h"

int GenStringHash(char * const &string, int numBuckets)
{
  int sum = 0;
  for(int i = 0; i < (int)strlen(string); i++)
    sum += string[i];
  return sum % numBuckets;
}

int GenStringComp(char * const &string1, char * const &string2)
{
  return strcmp(string1, string2);
}

//kb: changed to implict-templates, so all arch. must declare this class
//     for gcc 2.7;  egcs 2.9 does not need this line.
// #if defined(SGI)
template class HashTable<char *, stateMapRec *>;
//#endif

HashTable<char *, stateMapRec *> genStateMap(50, GenStringHash,
                                             GenStringComp);
