/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.12  1998/12/15 14:55:24  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.11  1997/05/03 19:53:56  wenger
  Fixed bug in AttrList class that caused Devise to crash on mappings
  that use recId; commented out debug code in UnixRecFile.c..

  Revision 1.10  1997/02/03 04:12:18  donjerko
  Catalog management moved to DTE

  Revision 1.9  1996/10/07 22:54:01  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.8  1996/10/02 15:23:53  wenger
  Improved error handling (modified a number of places in the code to use
  the DevError class).

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
  char errBuf[1024];

  /* Open file */
  int fd;
  //cout << "UnixRecFile::CreateFile(" << name << ", " << recSize << ")" << endl;
  if ((fd = open(name, O_RDWR, 0)) >= 0) {
    /* File already exists */
    close(fd);
    return NULL;
  }

  /* Cannot open file. Create it */
  if ((fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
    sprintf(errBuf, "opening file %s", name);
    reportErrSys(errBuf);
    Exit::DoExit(1);
  }

  return new UnixRecFile(name, fd, recSize, 0);
}

/****************************************************************
Open an existing file. Return NULL if file does not exist
*****************************************************************/

UnixRecFile *UnixRecFile::OpenFile(char *name, int recSize, Boolean trunc)
{
  char errBuf[1024];

  /* Open file */
  int fd;
  if ((fd = open(name, O_RDWR | (trunc ? O_TRUNC : 0), 0)) < 0)
    /* Cannot open file */
    return NULL;

  /* Find size of file */
  struct stat fileStat;
  if (fstat(fd, &fileStat) != 0) {
    sprintf(errBuf, "fstat on file %s", name);
    reportErrSys(errBuf);
    Exit::DoExit(1);
  }
  
  if ((fileStat.st_size % recSize) != 0) {
    sprintf(errBuf, "File %s not multiple of rec size\n", name);
    reportErrNosys(errBuf);
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
  FreeString(_name);
  close(_fd);
}

/*******************************************************
Get data for this page and put it into buffer 
*********************************************************/

void UnixRecFile::ReadRec(int recNum, int numRecs, void *buf)
{
  char errBuf[1024];

  if (recNum < 0 || recNum+numRecs > _totalRecs) {
    sprintf(errBuf, "ReadRec: rec %d invalid (%d)\n", recNum, _totalRecs);
    reportErrNosys(errBuf);
    Exit::DoExit(1);
  }
  
  if (_seekRec != recNum) {
    off_t pos = (off_t) (recNum * _recSize);
    if (lseek(_fd, pos, SEEK_SET) != pos) {
      sprintf(errBuf, "lseek on file %s", _name);
      reportErrSys(errBuf);
      Exit::DoExit(1);
    }
  }
  
  int toRead = _recSize * numRecs;
  while (toRead > 0) {
    int len;
    len = read(_fd, (char *)buf, toRead);
    if (len< 0) {
      if ( errno != EINTR) {
	sprintf(errBuf, "read on file %s", _name);
	reportErrSys(errBuf);
	Exit::DoExit(1);
      }
    } else if (len == 0) {
      if (toRead != _recSize * numRecs) {
	/* read partial number of records, should not happen */
	reportErrNosys("ReadRec: partial read");
	Exit::DoExit(1);
      } else {
	/* trying to read at eof. */
	reportErrNosys("ReadRec: read beyond EOF");
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
  char errBuf[1024];

  if (recNum < 0) {
    sprintf(errBuf, "WriteRec: rec %d invalid\n", recNum);
    reportErrNosys(errBuf);
    Exit::DoExit(1);
  }
  
  if (_seekRec != recNum) {
    off_t pos = (off_t)(recNum * _recSize);
    if (lseek(_fd, pos, SEEK_SET) != pos) {
      sprintf(errBuf, "lseek on file %s", _name);
      reportErrSys(errBuf);
      Exit::DoExit(1);
    }
  }
  
  int toWrite = _recSize * numRecs;
  while (toWrite > 0) {
    int len;
    len = write(_fd, (char *)buf, toWrite);
    if (len< 0) {
      if (errno != EINTR) {
	sprintf(errBuf, "writing file %s\n", _name);
	reportErrSys(errBuf);
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
  char errBuf[1024];

  struct stat sbuf;
  if (fstat(_fd, &sbuf) < 0) {
    sprintf(errBuf, "fstat on file %s", _name);
    reportErrSys(errBuf);
    Exit::DoExit(1);
  }

  return (long)sbuf.st_mtime;
}

