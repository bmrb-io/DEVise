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
  Revision 1.7  1996/08/04 21:59:56  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.6  1996/05/31 15:40:35  jussi
  Cleaned up a bit. Added copyright notice.

  Revision 1.5  1996/03/05 23:49:00  jussi
  Removed extra debugging statement.

  Revision 1.4  1996/03/05 23:22:00  jussi
  Cleaned up a bit.

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
#include "DevError.h"

/****************************************************************
Create a new file. Return NULL if file already exists
****************************************************************/

UnixRecFile *UnixRecFile::CreateFile(char *name, int recSize)
{
  /* Open file */
  int fd;
  if ((fd = open(name, O_RDWR, 0)) >= 0) {
    /* File already exists */
    close(fd);
    return NULL;
  }

  /* Cannot open file. Create it */
  if ((fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
    fprintf(stderr, "File %s: ", name);
    reportErrSys("open");
    Exit::DoExit(1);
  }

  return new UnixRecFile(name, fd, recSize, 0);
}

/****************************************************************
Open an existing file. Return NULL if file does not exist
*****************************************************************/

UnixRecFile *UnixRecFile::OpenFile(char *name, int recSize, Boolean trunc)
{
  /* Open file */
  int fd;
  if ((fd = open(name, O_RDWR | (trunc ? O_TRUNC : 0), 0)) < 0)
    /* Cannot open file */
    return NULL;

  /* Find size of file */
  struct stat fileStat;
  if (fstat(fd, &fileStat) != 0) {
    reportErrSys("fstat");
    Exit::DoExit(1);
  }
  
  if ((fileStat.st_size % recSize) != 0) {
    fprintf(stderr, "File %s not multiple of rec size\n",name);
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
: RecFile()
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
  delete _name;
  close(_fd);
}

/*******************************************************
Get data for this page and put it into buffer 
*********************************************************/

void UnixRecFile::ReadRec(int recNum, int numRecs, void *buf)
{
  if (recNum < 0 || recNum+numRecs > _totalRecs) {
    fprintf(stderr, "ReadRec: rec %d invalid (%d)\n", recNum, _totalRecs);
    Exit::DoExit(1);
  }
  
  if (_seekRec != recNum) {
    off_t pos = (off_t) (recNum * _recSize);
    if (lseek(_fd, pos, SEEK_SET) != pos) {
      reportErrSys("lseek");
      Exit::DoExit(1);
    }
  }
  
  int toRead = _recSize * numRecs;
  while (toRead > 0) {
    int len;
    len = read(_fd, (char *)buf, toRead);
    if (len< 0) {
      if ( errno != EINTR) {
	reportErrSys("read");
	Exit::DoExit(1);
      }
    } else if (len == 0) {
      if (toRead != _recSize * numRecs) {
	/* read partial number of records, should not happen */
	fprintf(stderr, "ReadRec: partial read\n");
	Exit::DoExit(1);
      } else {
	/* trying to read at eof. */
	fprintf(stderr, "ReadRec: read beyond EOF\n");
	Exit::DoExit(2);
      }
    } else {
      toRead -= len;
      buf = ((char *)buf) + len;
    }
  }

  /* After reading, _seekRec == recNum+numRecs */
  _seekRec = recNum + numRecs;
}

/**********************************************************************
Write a page.
***********************************************************************/

void UnixRecFile::WriteRec(int recNum, int numRecs, void *buf)
{
  if (recNum < 0) {
    fprintf(stderr, "WriteRec: rec %d invalid\n", recNum);
    Exit::DoExit(1);
  }
  
  if (_seekRec != recNum) {
    off_t pos = (off_t)(recNum * _recSize);
    if (lseek(_fd, pos, SEEK_SET) != pos) {
      reportErrSys("lseek");
      Exit::DoExit(1);
    }
  }
  
  int toWrite = _recSize * numRecs;
  while (toWrite > 0) {
    int len;
    len = write(_fd, (char *)buf, toWrite);
    if (len< 0) {
      if (errno != EINTR) {
	reportErrSys("write");
	Exit::DoExit(1);
      }
    } else {
      toWrite -= len;
      buf = ((char *)buf) + len;
    }
  }

  /* After writing, _seekRec == recNum+numRecs */
  _seekRec = recNum + numRecs;
  
  /* adjust # of records */
  if (_seekRec > _totalRecs)
    _totalRecs = _seekRec;
}

/*
   Get the time file last modified. Return an integer
   specifying when file was modified. We require that
   if r1, and r2 are two different files, and r1 was modified before
   r2, then r1->GetModTime() < r2->GetModFile()
*/

int UnixRecFile::GetModTime()
{
  struct stat sbuf;
  if (fstat(_fd, &sbuf) < 0) {
    reportErrSys("fstat");
    Exit::DoExit(1);
  }

  return (long)sbuf.st_mtime;
}

