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
  Revision 1.3  1996/01/12 15:31:02  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.2  1995/09/05 22:16:13  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "UnixRecFile.h"
#include "Util.h"
#include "Exit.h"

/****************************************************************
Create a new file. Return NULL if file already exists
****************************************************************/

UnixRecFile *UnixRecFile::CreateFile(char *name, int recSize)
{
  /* open file */
  int fd;
  if ((fd = open(name, /* O_FSYNC| */O_RDWR,
		 S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH)) >= 0) {
    /* file already exists */
    close(fd);
    return NULL;
  }

  /* can't open file. Create it */
  if ((fd = open(name, /* O_FSYNC| */O_RDWR | O_CREAT,
		 S_IRUSR | S_IWUSR)) < 0) {
    perror("UnixRecFile: can't create file");
    fprintf(stderr, "file name is %s\n", name);
    Exit::DoExit(1);
  }

  return new UnixRecFile(name, fd, recSize, 0);
}

/****************************************************************
Open an existing file. Return NULL if file does not exist
*****************************************************************/

UnixRecFile *UnixRecFile::OpenFile(char *name, int recSize, Boolean trunc)
{
  /* open file */
  int fd;
  if ((fd = open(name, /* O_FSYNC| */ O_RDWR | (trunc? O_TRUNC : 0),
		 S_IRUSR|S_IWUSR))<0)
    /* can't open file */
    return NULL;

  /* find size of file */
  struct stat fileStat;
  if (fstat(fd,&fileStat) != 0) {
    perror("UnixRecFile: can't get file stat\n");
    Exit::DoExit(1);
  }
  
  if ((fileStat.st_size % recSize) != 0) {
    fprintf(stderr,"UnixRecFile: OpenFile %s not multiple of rec size\n",name);
    Exit::DoExit(1);
  }

  return new UnixRecFile(name, fd, recSize, fileStat.st_size / recSize);
}

char *UnixRecFile::GetName()
{
  return _name;
}

/**********************************************************
Constructor
***********************************************************/

UnixRecFile::UnixRecFile(char *name, int fd, int recSize, int numRecs)
{
  _name = CopyString(name);
  _fd = fd;
  _recSize = recSize;
  _seekRec = 0;
  _totalRecs = numRecs;
}

/**********************************************************************
Destructor
***********************************************************************/

UnixRecFile::~UnixRecFile()
{
  printf("~UnixRecFile fd %d\n", _fd);
  close(_fd);
}

/*******************************************************
Get data for this page and put it into buffer 
*********************************************************/

void UnixRecFile::ReadRec(int recNum, int numRecs, void *buf)
{
  if (recNum < 0 || recNum+numRecs > _totalRecs) {
    fprintf(stderr,"UnixRecFile::ReadRec: rec %d invalid,\n", recNum);
    fprintf(stderr,"UnixRecFile::ReadRec: there are %d recs,\n", _totalRecs);
    Exit::DoExit(1);
  }
  
  if (_seekRec != recNum) {
    off_t pos = (off_t) (recNum * _recSize);
    if (lseek(_fd,pos,SEEK_SET)!= pos) {
      perror("UnixRecFile::GetRec: can't seek");
      Exit::DoExit(1);
    }
  }
  
  int toRead = _recSize*numRecs;
  while (toRead > 0) {
    int len;
    len=read(_fd,(char *)buf, toRead);
    if (len< 0) {
      if ( errno != EINTR) {
	perror("UnixRecFile::GetRec: ");
	Exit::DoExit(1);
      }
    }
    else if (len == 0) {
      if (toRead != _recSize*numRecs) {
	/* read partial number of records, should not happen */
	fprintf(stderr,"UnixRecFile::ReadRec: partial read\n");
	Exit::DoExit(1);
      }
      else {
	/* trying to read at eof. */
	/*
	   _seekRec = recNum;
	   return;
	   */
	fprintf(stderr,"UnixRecFile::ReadRec: read beyond eof\n");
	Exit::DoExit(2);
      }
    }
    else {/* len > 0 */
      toRead -= len;
      buf = ((char *)buf)+len;
    }
  }

  /* After reading, _seekRec == recNum+numRecs */
  _seekRec = recNum +numRecs;
}

/**********************************************************************
Write a page.
***********************************************************************/

void UnixRecFile::WriteRec(int recNum, int numRecs, void *buf)
{
  if (recNum < 0) {
    fprintf(stderr,"UnixRecFile::WriteRec: rec %d invalid\n", recNum);
    Exit::DoExit(1);
  }
  
  if (_seekRec != recNum) {
    off_t pos = (off_t) (recNum * _recSize);
    if (lseek(_fd,pos,SEEK_SET)!= pos) {
      perror("UnixRecFile::GetRec: can't seek");
      Exit::DoExit(1);
    }
  }
  
  int toWrite = _recSize*numRecs;
  while (toWrite > 0) {
    int len;
    len=write(_fd,(char *)buf, toWrite);
    if (len< 0) {
      if (errno != EINTR) {
	perror("UnixRecFile::WriteRec: ");
	Exit::DoExit(1);
      }
    }
    else {/* len >= 0 */
      toWrite -= len;
      buf = ((char *)buf)+ len;
    }
  }
  /* After writing, _seekRec == recNum+numRecs */
  _seekRec = recNum +numRecs;
  
  /* adjust # of records */
  if ( _seekRec > _totalRecs)
    _totalRecs = _seekRec;
}

/* Get the time file last modified. Return an integer
   specifying when file was modified. We require that
   if r1, and r2 are two different files, and r1 was modified before
   r2, then r1->GetModTime() < r2->GetModFile()
*/

long UnixRecFile::GetModTime()
{
  struct stat sbuf;
  if (fstat(_fd,&sbuf) < 0) {
    fprintf(stderr,"UnixRecFile:: can't find stat\n");
    Exit::DoExit(1);
  }

  return (long)sbuf.st_mtime;
}

