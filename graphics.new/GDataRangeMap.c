/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.8  1998/03/04 19:11:02  wenger
  Fixed some more dynamic memory errors.

  Revision 1.7  1996/12/03 20:39:16  jussi
  Improved error checking.

  Revision 1.6  1996/09/27 15:53:19  wenger
  Fixed a number of memory leaks.

  Revision 1.5  1995/12/14 21:17:26  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.4  1995/12/14 18:06:17  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.3  1995/11/27 16:21:57  jussi
  Added copyright notice and replaced bcopy()'s with more portable
  memcpy()'s.

  Revision 1.2  1995/09/05 22:14:52  jussi
  Added CVS header.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include "Exit.h"
#include "GDataRangeMap.h"
#include "FileMacro.h"
#include "TData.h"

//#define DEBUG

/*********************************************************************
Constructor
*********************************************************************/

GDataRangeMap::GDataRangeMap(int recSize, char *fname, Boolean trunc)
{
#ifdef DEBUG
  printf("GDataRangeMap::GDataRangeMap(0x%p)\n", this);
#endif

  if (recSize > MAX_RANGE_REC_SIZE) {
    fprintf(stderr,"GDataRangeMap::GDataRangeMap: recSize %d > %d\n",
	    recSize, MAX_RANGE_REC_SIZE);
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(1);
  }

  _recSize = recSize;
  _hint = NULL;
  _head.next = _head.prev = &_head;

  if (fname != NULL) {
    _fname = new char [strlen(fname) + 1];
    strcpy(_fname, fname);
#ifdef DEBUG
    printf("GDataRangeMap::GDataRangeMap() file name %s\n", _fname);
#endif
    if (!trunc)
      ReadRecords(_fname);
  } else
    _fname = NULL;
}

/*********************************************************************
Destructor
*********************************************************************/

GDataRangeMap::~GDataRangeMap()
{
#ifdef DEBUG
  printf("GDataRangeMap::~GDataRangeMap(0x%p)\n", this);
#endif

  if (_fname != NULL) {
#ifdef DEBUG
    printf("GDataRangeMap destructor write %s, 0x%p\n", _fname, this);
#endif
    WriteRecords(_fname);
    delete [] _fname;
  }

  // Free the list of GDataRangeMapRecs.  Note that this delete algorithm
  // only works  for deleting the whole list, since we're not updating the
  // prev pointers as we go.

  GDataRangeMapRec* nodeP = _head.next;
  GDataRangeMapRec* nextP = nodeP->next;

  while (nodeP != &_head) {
    delete nodeP;
    nodeP = nextP;
    nextP = nodeP->next;
  }
}

/********************************************************************
Read records from a file
*********************************************************************/

void GDataRangeMap::ReadRecords(char *fname)
{
  int fd;
  if ((fd = open(fname, O_RDONLY, 0)) < 0) {
    /* no such file: nothing to read. */
    return;
  }
  
  int status;
  GDataRangeMapRec *buf;
  GDataRangeMapRec *current = &_head;
  do {
    buf = new GDataRangeMapRec;
    DOASSERT(buf, "Out of memory");
    status = ReadChunkStatus(fd,(char *)buf, sizeof(GDataRangeMapRec));
    if (status) {
      /* read a valid record. Insert after current. */
      current->next->prev = buf;
      buf->next = current->next;
      buf->prev = current;
      current->next = buf;
      
      current = buf;
    }
  } while(status);
  
  delete buf;
  
  close(fd);
}

/*********************************************************************
Write record out to file
**********************************************************************/

void GDataRangeMap::WriteRecords(char *fname)
{
#ifdef DEBUG
  printf("GDataRangeMap WriteRecords %s, 0x%p\n", _fname, this);
#endif

  int fd;
  if ((fd = open(fname, O_CREAT | O_TRUNC | O_WRONLY, 0660)) < 0) {
    /* can't open file */
    printf("GDataRangeMap::WriteRecords(%s), 0x%p",fname, this);
    perror("open file: ");
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(2);
  }
  
  for(GDataRangeMapRec *current = _head.next; current != &_head;
      current = current->next) {
    WriteChunk(fd,(char *)current,sizeof(GDataRangeMapRec));
  }

  close(fd);
}

/* Find the range map record that contains gId,
   return NULL if not found.*/

GDataRangeMapRec *GDataRangeMap::FindGRange(RecId gId)
{
  if (_head.next == &_head)
    /* empty list */
    return NULL;
  
  if (_hint != NULL && _hint->gLow <= gId && _hint->gHigh >= gId)
    return _hint;
  else {
    for (_hint =_head.next; _hint != &_head; _hint = _hint->next) {
      if ( _hint->gLow <= gId && _hint->gHigh >= gId)
	return _hint;
    }
    _hint = NULL;
    return NULL;
  }
}

/************************************************************************
Find the range map record with tLow <= tId <= tHigh. If
no such record exists, find the range map record whose
TData page is the lowst such that tlow >= tId.
(Minimum Upper bound record). Return NULL if not found.
*************************************************************************/

GDataRangeMapRec *GDataRangeMap::FindMinUpper(RecId tId)
{
  if (_head.next == &_head)
    /* empty list */
    return NULL;
  
  if (_hint == NULL)
    _hint = _head.next;
  
  if (tId < _hint->tLow) {
    /* search backwards */
    GDataRangeMapRec *prev;
    for (prev = _hint->prev; prev != &_head; _hint=prev, prev = prev->prev) {
      if (tId > prev->tHigh)
	/* found it */
	break;
    }
    return _hint;
  }
  else if (tId > _hint->tHigh) {
    /* search forwards */
    GDataRangeMapRec *next;
    for (next = _hint->next; next != &_head;_hint= next, next = next->next) {
      if (tId <= next->tHigh)
	/* found it */
	return next;
    }
    /* not found */
    return NULL;
  }
  else {
    /* _hint->tLow <= tId <= _hint->tHigh */
    return _hint;
  }
}

/************************************************************************
Find the range map record with tLow <= tId <= tHigh. If
no such record exists, find the range map record whose
TData page is the lowst such that tlow >= tId.
(Minimum Upper bound record). Return NULL if not found.
*************************************************************************/

GDataRangeMapRec *GDataRangeMap::FindMaxLower(RecId tId)
{
  if (_head.next == &_head)
    /* empty list */
    return NULL;
  
  if (_hint == NULL)
    _hint = _head.next;
  
  if (tId < _hint->tLow) {
    /* search backwards */
    for (_hint = _hint->prev; _hint != &_head; _hint = _hint->prev) {
      if (tId >= _hint->tLow)
	/* found it */
	return _hint;
    }
    /* no page found */
    _hint = _head.next;
    return NULL;
  }
  else if (tId > _hint->tHigh) {
    /* search forwards */
    GDataRangeMapRec *next;
    for (next = _hint->next; next != &_head;_hint= next, next = next->next) {
      if (tId < next->tLow)
	/* found it */
	return _hint;
    }
    /* _hint is the correct record */
    return _hint;
  }
  else {
    /* _hint->tLow <= tId <= _hint->tHigh */
    return _hint;
  }
}

/***********************************************************************
find range map record with highest tdata page number such that tHigh < tPage.
Return NULL if no such record exists.
exit with error if tPage overlaps with any range.
******************************************************************************/

GDataRangeMapRec *GDataRangeMap::FindNoOverlap(RecId tId)
{
  GDataRangeMapRec *rec = FindMaxLower(tId);
  if (!rec)
    return NULL;
  
  if (tId > rec->tHigh)
    return rec;

  fprintf(stderr, "%ld overlaps [%ld,%ld]\n", tId, rec->tLow, rec->tHigh);
  DOASSERT(0, "Inconsistent state");

  return 0;
}

/**************************************************************
Find next unprocessed T page with number >= tLow
***************************************************************/

Boolean GDataRangeMap::NextUnprocessed(RecId tId, RecId &tLow, RecId &tHigh)
{
  GDataRangeMapRec *rec = FindMaxLower(tId);

  if (!rec) {
    /* tId is smallest among all that have been processed */
    tLow = tId;
    if (_head.next == &_head)
      return true;
    tHigh = _head.next->tLow-1;
    return false;
  }

  if (tId > rec->tHigh) {
    /* next unprocessed is just beyond this one */
    tLow = tId;
    if (rec->next == &_head) {
      /* tId is bigger than all Ids that have been procesed */
      return true;
    }
    tHigh = rec->next->tLow-1;
    return false;
  }

  /* tLow <= tId <= tHigh, find next higher recId range.
     Since records with adjacent page numbers are not collapsed,
     we need to skip consecutive records in order to find next 
     higher page number.
  */

  tId = rec->tHigh+1;
  for ( rec = rec->next; rec != &_head; rec = rec->next) {
    if (tId < rec->tLow) {
      /* found */
        tLow = tId;
        tHigh = rec->tLow -1;
        return false;
      }
    tId = rec->tHigh+1;
  }

  /* end of list reached. Return page number of last element of list + 1 */
  tLow = tId;
  return true;
}

/************************************************************************
Insert a new range using the following algorithm:
1) Error:  if [tLow,tHigh] overlaps with existing range.
2) Merge with existing range if:
	let e == existing range && tLow == e.tHigh+1 &&
	(gLow == e.gHigh || (e.numLastPage == 0 && gHigh == e.gHigh+1))
3) Otherwise, create a new range.
*********************************************************************/

void GDataRangeMap::InsertRange(RecId tLow, RecId tHigh, RecId gLow, 
				RecId gHigh, void *firstRec, void *lastRec)
{
  GDataRangeMapRec *rec = FindMaxLower(tLow);
  GDataRangeMapRec *insertPt = 0;

  if (rec == NULL) {
    /* insert after dummy head 0f list */
    if (_head.next != &_head && tHigh >= _head.next->tLow) {
      /* overlap with 1st range in the list */
      fprintf(stderr,"InsertRange: (%ld,%ld) overlaps with (%ld,%ld)\n",
	      tLow, tHigh, _head.next->tLow, _head.next->tHigh);
      reportErrNosys("Fatal error");//TEMP -- replace with better message
      Exit::DoExit(2);
    }
    insertPt = &_head;
  }
  else if ((tLow >= rec->tLow && tLow <= rec->tHigh) ||
	   (tHigh >= rec->tLow && tHigh <= rec->tHigh)) {
    fprintf(stderr,
	    "GDataRangeMap::InsertRange: (%ld,%ld) overlaps (%ld,%ld)\n",
	    tLow, tHigh, rec->tLow, rec->tHigh);
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(2);
  }
  else if( rec->next != &_head && tHigh >= rec->next->tLow) {
    /* high page overlaps with next range */
    fprintf(stderr,"InsertRange: (%ld,%ld) overlaps with (%ld,%ld)\n",
	    tLow, tHigh, rec->next->tLow, rec->next->tHigh);
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(2);
  }
  else if ( tLow == rec->tHigh+1 && gLow == rec->gHigh+1) {
    /* extend current range */
    rec->tHigh = tHigh;
    rec->gHigh = gHigh;
    memcpy(rec->lastRec, lastRec, _recSize);
    return;
  }
  else {
    /* create a new record after current */
    insertPt = rec;
  }
  
  /* If we get here, create a new record after insertPt */
  GDataRangeMapRec *newRec = new GDataRangeMapRec;
  DOASSERT(newRec, "Out of memory");
  newRec->tLow = tLow;
  newRec->tHigh = tHigh;
  newRec->gLow = gLow;
  newRec->gHigh = gHigh;
  insertPt->next->prev = newRec;
  newRec->next = insertPt->next;
  newRec->prev = insertPt;
  insertPt->next = newRec;
  memcpy(newRec->firstRec, firstRec, _recSize);
  memcpy(newRec->lastRec, lastRec, _recSize);
}

/**********************************************************************
print
***********************************************************************/

void GDataRangeMap::Print()
{
  GDataRangeMapRec *rec = _head.next;
  while (rec != &_head) {
    printf("  T: (%ld,%ld), G: (%ld,%ld)\n",
	   rec->tLow, rec->tHigh, rec->gLow, rec->gHigh);
    rec = rec->next;
  }
}

/* Find the highest RecIds in the range.
   Return false none exists. */

Boolean GDataRangeMap::highID(RecId &id)
{
  if (_head.prev == &_head)
    return false;
  id = _head.prev->tHigh;
  return true;
}

/* Find the lowest RecIds in the range.
   Return false none exists. */

Boolean GDataRangeMap::lowID (RecId &id)
{
  if (_head.next == &_head)
    return false;
  id = _head.next->tLow;
  return true;
}
