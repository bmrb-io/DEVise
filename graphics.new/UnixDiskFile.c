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
  Revision 1.3  1996/01/12 15:27:52  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.2  1995/09/05 22:16:12  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "UnixDiskFile.h"
#include "Exit.h"
#include "PageSize.h"
#include "DevError.h"

/****************************************************************
Create a new file. Return NULL if file already exists
****************************************************************/

UnixDiskFile *UnixDiskFile::CreateFile(char *name)
{
  /* open file */
  int fd;
  if ((fd = open(name, /*O_SYNC|*/O_RDWR, S_IRUSR | S_IWUSR)) >= 0) {
    /* file already exists */
    close(fd);
    return NULL;
  }

  /* can't open file. Create it */
  if ((fd = open(name, /*O_SYNC|*/O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
    reportErrSys("UnixDiskFile: can't create file");
    fprintf(stderr,"file name is %s\n",name);
    Exit::DoExit(1);
  }

  return new UnixDiskFile(fd);
}

/****************************************************************
Open an existing file. Return NULL if file does not exist
*****************************************************************/

UnixDiskFile *UnixDiskFile::OpenFile(char *name)
{
  /* open file */
  int fd;
  if ((fd = open(name, O_SYNC | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
    /* can't open file */
    return NULL;

  /* find size of file */
  struct stat fileStat;
  if (fstat(fd,&fileStat) != 0) {
    reportErrSys("UnixDiskFile: can't get file stat\n");
    Exit::DoExit(1);
  }

  if ((fileStat.st_size % DISKFILE_PAGESIZE) != 0) {
    fprintf(stderr, "UnixDiskFile: OpenFile %s not multiple of page size\n",
	    name);
    Exit::DoExit(1);
  }
  return new UnixDiskFile(fd);
}


/**********************************************************
Constructor
***********************************************************/

UnixDiskFile::UnixDiskFile(int fd)
{
  
  _fd = fd;
  _seekPage = 0;
}

/**********************************************************************
Destructor
***********************************************************************/

UnixDiskFile::~UnixDiskFile()
{
  close(_fd);
}

/*******************************************************
Get data for this page and put it into buffer 
*********************************************************/

void UnixDiskFile::ReadPage(int pageNum, void *buf)
{
  if (pageNum < 0) {
    fprintf(stderr,"UnixDiskFile::ReadPage: page %d invalid\n", pageNum);
    Exit::DoExit(1);
  }
  
  if (_seekPage != pageNum) {
    off_t pos = (off_t) pageNum * DISKFILE_PAGESIZE;
    if (lseek(_fd,pos,SEEK_SET)!= pos) {
      reportErrSys("UnixDiskFile::GetPage: can't seek");
      Exit::DoExit(1);
    }
  }
  
  int toRead = DISKFILE_PAGESIZE;
  while (toRead > 0) {
    int len;
    len=read(_fd,(char *)buf, toRead);
    if (len< 0) {
      if ( errno != EINTR) {
	reportErrSys("UnixDiskFile::GetPage: ");
	Exit::DoExit(1);
      }
    }
    else if (len == 0) {
      if (toRead != DISKFILE_PAGESIZE) {
	/* read partial page, should not happen */
	fprintf(stderr,"UnixDiskFile::ReadPage: partial read\n");
	Exit::DoExit(1);
      }
      else {
	/* trying to read at eof. */
	/*
	   _seekPage = pageNum;
	   return;
	   */
	fprintf(stderr,"UnixDiskFile::ReadPage: read beyond eof\n");
	Exit::DoExit(2);
      }
    }
    else {/* len > 0 */
      toRead -= len;
      buf = ((char *)buf)+len;
    }
  }
  /* After reading, _seekPage == pageNum+1 */
  _seekPage = pageNum +1;
}

/**********************************************************************
Write a page.
***********************************************************************/

void UnixDiskFile::WritePage(int pageNum, void *buf)
{
  if (pageNum < 0) {
    fprintf(stderr,"UnixDiskFile::WritePage: page %d invalid\n", pageNum);
    Exit::DoExit(1);
  }
  
  if (_seekPage != pageNum) {
    off_t pos = (off_t) pageNum * DISKFILE_PAGESIZE;
    if (lseek(_fd,pos,SEEK_SET)!= pos) {
      reportErrSys("UnixDiskFile::GetPage: can't seek");
      Exit::DoExit(1);
    }
  }
  
  int toWrite = DISKFILE_PAGESIZE;
  while (toWrite > 0) {
    int len;
    len=write(_fd,(char *)buf, toWrite);
    if (len< 0) {
      if (errno != EINTR) {
	reportErrSys("UnixDiskFile::WritePage: ");
	Exit::DoExit(1);
      }
    }
    else {/* len >= 0 */
      toWrite -= len;
      buf = ((char *)buf)+ len;
    }
  }
  /* After writing, _seekPage == pageNum+1 */
  _seekPage = pageNum +1;
}
