/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2002
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
  Revision 1.8.14.1  2002/12/04 21:13:14  wenger
  *** empty log message ***

  Revision 1.8  2000/03/14 17:05:27  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.7  2000/01/13 23:07:02  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.6  1999/11/30 22:28:19  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.5  1999/10/26 16:29:40  wenger
  Fixed bug 519 (problems with opening various sequences of soil science
  sessions, caused by stupid composite parsers not getting reset when a
  session is closed).

  Revision 1.4  1996/09/05 23:14:16  kmurli
  Added a destructor to free the fileType char pointer after use.
  CVS ----------------------------------------------------------------------

  Revision 1.3  1996/03/26 20:22:01  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:35  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <malloc.h>

#include "RecInterp.h"
#include "CompositeParser.h"
#include "Exit.h"
#include "DevError.h"
#include "Util.h"

#define DEBUG 0

CompositeEntry CompositeParser::_entries[MAX_COMPOSITE_ENTRIES];
int CompositeParser::_numEntries = 0;
int CompositeParser::_hintIndex = -1;

void
CompositeParser::DestroyAll()
{
#if (DEBUG >= 1)
  printf("CompositeParser::DestroyAll()\n");
#endif

  for(int i = 0; i < _numEntries; i++) {
    delete _entries[i].userComposite;
    FreeString(_entries[i].fileType);
  }
  _numEntries = 0;
  _hintIndex = -1;
}
	
void CompositeParser::Register(char *fileType, UserComposite *userComposite){
#if (DEBUG >= 1)
  printf("CompositeParser::Register(%s, %p)\n", fileType, userComposite);
#endif

  if (_numEntries >= MAX_COMPOSITE_ENTRIES) {
    reportErrNosys("CompositeParser:: too many entries\n");
    Exit::DoExit(2);
  }

  _entries[_numEntries].fileType = fileType;
  _entries[_numEntries].userComposite = userComposite;
  _numEntries++;
}

void CompositeParser::Decode(const char *fileType, RecInterp *recInterp)
{
#if (DEBUG >= 1)
  printf("CompositeParser::Decode(%s, %p)\n", fileType, recInterp);
#endif

  if (_hintIndex >= 0 && !strcmp(fileType, _entries[_hintIndex].fileType)) {
    /* found it */
    _entries[_hintIndex].userComposite->Decode(recInterp);
    return;
  }

  /* search for a matching file type */

  for(int i = 0; i < _numEntries; i++) {
    if (!strcmp(fileType,_entries[i].fileType)) {
      /* found it */
      _hintIndex = i;
      _entries[_hintIndex].userComposite->Decode(recInterp);
      return;
    }
  }
  
  /* not found */
  char errBuf[1024];
  sprintf(errBuf, "Can't find user composite function for file type %s\n",
	  fileType);
  reportErrNosys(errBuf);
  Exit::DoExit(2);
}

void
CompositeParser::ResetAll()
{
#if (DEBUG >= 1)
  printf("CompositeParser::ResetAll()\n");
#endif

  for(int index = 0; index < _numEntries; index++) {
    _entries[index].userComposite->Reset();
  }
}
