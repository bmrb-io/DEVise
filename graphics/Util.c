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
  Revision 1.12  1996/07/14 20:04:47  jussi
  Made code to compile in OSF/1.

  Revision 1.11  1996/07/05 14:39:47  jussi
  Fixed minor problem with null-termination in DateString().

  Revision 1.10  1996/05/20 18:44:42  jussi
  Replaced PENTIUM flag with SOLARIS.

  Revision 1.9  1996/03/27 17:54:56  wenger
  Changes to get DEVise to compile and run on Linux.

  Revision 1.8  1996/02/13 16:20:16  jussi
  Fixed for AIX.

  Revision 1.7  1996/01/10 19:11:17  jussi
  Added error checking to CopyString.

  Revision 1.6  1995/12/28 18:48:14  jussi
  Small fixes to remove compiler warnings.

  Revision 1.5  1995/12/14 17:12:38  jussi
  Small fixes.

  Revision 1.4  1995/10/18 14:55:32  jussi
  Changed mask of created directory to 0777 from 0755.

  Revision 1.3  1995/10/15 18:36:40  jussi
  Added HPUX-specific code.

  Revision 1.2  1995/09/05 21:13:13  jussi
  Added/updated CVS header.
*/

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#if defined(SOLARIS) || defined(HPUX) || defined(AIX)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "Util.h"
#include "Exit.h"
#include "DevError.h"

long ModTime(char *fname)
{
  struct stat sbuf;
  if (stat(fname, &sbuf) < 0) {
    fprintf(stderr, "Cannot get modtime of %s\n", fname);
    Exit::DoExit(2);
  }
  return (long)sbuf.st_mtime;
}

DevStatus
ReadFile(char *filename, int &size, char *&buffer)
{
  DevStatus result = StatusOk;

  struct stat sbuf;
  if (stat(filename, &sbuf) < 0)
  {
    reportError("Can't get size of file", errno);
    result = StatusFailed;
  }
  else
  {
    size = sbuf.st_size;
    buffer = new char[size];
    if (buffer == NULL)
    {
      reportError("Out of memory", errno);
      result = StatusFailed;
    }
    else
    {
      int fd = open(filename, O_RDONLY);
      if (fd < 0)
      {
        reportError("Can't open file", errno);
        result = StatusFailed;
      }
      else
      {
        if (read(fd, buffer, size) != size)
	{
          reportError("Error reading file", errno);
          result = StatusFailed;
	}

	if (close(fd) < 0)
	{
          reportError("Error closing file", errno);
          result = StatusWarn;
	}
      }

      if (!result.IsComplete()) delete [] buffer;
    }
  }

  return result;
}

char *CopyString(char *str)
{
  char *result = new char[strlen(str) + 1];
  if (!result) {
    fprintf(stderr, "Insufficient memory for new string\n");
    Exit::DoExit(2);
  }
  strcpy(result, str);
  return result;
}

static char dateBuf[21];

char *DateString(time_t tm)
{
  char *dateStr = ctime(&tm);
  int i;
  for(i = 0; i < 7; i++)
    dateBuf[i] = dateStr[i + 4];

  for(i = 7; i < 11; i++)
    dateBuf[i] = dateStr[i + 13];

  dateBuf[11] = ' ';
  
  for(i = 12; i < 20; i++)
    dateBuf[i] = dateStr[i - 1];

  dateBuf[20] = '\0';

  return dateBuf;
}

void ClearDir(char *dir)
{
  /* clear directory */

  DIR *dirp = opendir(dir);
  if (dirp != NULL){
#if defined(SOLARIS) || defined(HPUX) || defined(AIX) || defined(LINUX) \
      || defined(OSF)
    struct dirent *dp;
#else
    struct direct *dp;
#endif
    for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)){
#if defined(SOLARIS) || defined(HPUX) || defined(AIX) || defined(LINUX) \
      || defined(OSF)
      struct dirent *realdp = (struct dirent *)dp;
#else
      struct direct *realdp = dp;
#endif
      if (strcmp(realdp->d_name,".") != 0 &&
	  strcmp(realdp->d_name,"..") != 0 ){
	char buf[512];
	sprintf(buf,"%s/%s",dir,realdp->d_name);
	/*
	   printf("unlinking %s\n", buf);
	*/
	unlink(buf);
      }
    }
    closedir(dirp);
  }
}

/* Check if directory exists. Make directory if not already exists
   Clear directory if clear == true*/

void CheckAndMakeDirectory(char *dir, int clear )
{
  struct stat sbuf;
  int ret = stat(dir,&sbuf);
  if (ret >=  0 ) {
    if (!(sbuf.st_mode & S_IFDIR)){
      fprintf(stderr,"Init:: %s not a directory\n", dir);
      Exit::DoExit(1);
    }
    if (clear){
      ClearDir(dir);
    }
  } else {
    /* make new directory */
    int code = mkdir(dir,0777);
    if (code < 0 ){
      printf("Init::can't make directory %s\n",dir);
      perror("");
      Exit::DoExit(1);
    }
  }
}

