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
  Revision 1.30  1998/12/22 19:39:09  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.29  1998/11/19 21:13:27  wenger
  Implemented non-DTE version of DEVise (new code handles data source catalog
  functions; Tables, SQLViews, etc., are not implemented); changed version to
  1.6.0.

  Revision 1.28  1998/07/29 14:20:19  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.27  1998/06/17 17:20:33  wenger
  Devised now sends "real" session file list to JavaScreen.

  Revision 1.26  1998/05/14 18:21:10  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.25  1998/03/09 15:13:58  wenger
  Fixed compile warning.

  Revision 1.24  1998/02/23 23:22:30  wenger
  Merged cleanup_1_4_7_br_8 thru cleanup_1_4_7_br_9.

  Revision 1.23.2.1  1998/02/23 19:51:21  wenger
  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.

  Revision 1.23  1997/09/23 19:57:28  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.22  1997/05/05 16:53:46  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.21  1997/04/29 17:35:02  wenger
  Minor fixes to new text labels; added fixed text label shape;
  CheckDirSpace() no longer prints an error message if it can't get disk
  status.

  Revision 1.20  1996/12/03 20:24:22  jussi
  Added readn() and writen().

  Revision 1.19  1996/12/02 18:44:35  wenger
  Fixed problems dealing with DST in dates (including all date composite
  parsers); added more error checking to date composite parsers.

  Revision 1.18  1996/11/19 15:23:28  wenger
  Minor changes to fix compiles on HP, etc.

  Revision 1.17  1996/11/05 18:23:11  wenger
  Minor mods to get things to compile on SGI systems.

  Revision 1.16  1996/10/18 20:34:08  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.15  1996/10/09 14:33:45  wenger
  Had to make changes to get my new code to compile on HP and Sun.

  Revision 1.14  1996/10/07 22:53:50  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.13  1996/08/23 16:55:44  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

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

#if defined (SGI)
  #define STAT_BAVAIL f_bfree
#else
  #define STAT_BAVAIL f_bavail
#endif

#if defined(SOLARIS) || defined(OSF)
  #include <sys/statvfs.h>
  #define STAT_STRUCT statvfs
  #define STAT_FUNC statvfs
  #define STAT_FRSIZE f_frsize
#elif defined(AIX)
  #define _KERNEL
  #define _VOPS
  #include <sys/vfs.h>
  #include <sys/statfs.h>
  #define STAT_STRUCT statfs
  #define STAT_FUNC VFS_STATFS
  #define STAT_FRSIZE f_bsize
#else
  #include <sys/vfs.h>
  #define STAT_STRUCT statfs
  #define STAT_FUNC statfs
  #define STAT_FRSIZE f_bsize

  #if defined(SUN)
    extern "C" int statfs(const char *, struct statfs *);
  #else
    #if defined(SGI)
      #include <sys/statfs.h>
    #endif
  #endif
#endif

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

char *CopyString(const char *str)
{
  if (str == NULL) return NULL;
  // Changed from new to malloc here so the stack doesn't get too deep
  // for Purify to show.  RKW 1999-03-01.
  char *result = (char *)malloc(strlen(str) + 1);
  if (!result) {
    fprintf(stderr, "Insufficient memory for new string\n");
    Exit::DoExit(2);
  }
  strcpy(result, str);
  return result;
}

char *DateString(time_t tm, const char *format)
{
#if 0
  if (tm < 0) {
    char errBuf[1024];
    sprintf(errBuf, "Illegal time value %ld\n", tm);
    reportErrNosys(errBuf);
  }
#endif

  static char dateBuf[32];

  // Note: ParseFloatDate() depends on this format.
  const char *defaultFormat = "%b %d %Y %T";

  if (format == NULL || strlen(format) == 0) format = defaultFormat;

  // Note: second arg. of cftime() should be declared const in system header
  // files.
  cftime(dateBuf, (char *)format, &tm);

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

/* Check whether we have enough space in a given directory. */
void CheckDirSpace(char *dirname, char *envVar, int warnSize, int exitSize)
{
  struct STAT_STRUCT stats;

  if (STAT_FUNC(dirname, &stats
#if defined(SGI)
    , sizeof(stats), 0
#endif
    ) != 0)
  {
#if defined(DEBUG)
    reportErrSys("Can't get status of file system");
#endif
  }
  else
  {
    int minBlocksFree = exitSize / stats.STAT_FRSIZE;
    int warnBlocksFree = warnSize / stats.STAT_FRSIZE;
    if (((int)stats.STAT_BAVAIL) < minBlocksFree)
    {
      char errBuf[1024];
      sprintf(errBuf, "%s directory (%s) has less than %d bytes free\n",
	envVar, dirname, exitSize);
      Exit::DoAbort(errBuf, __FILE__, __LINE__);
    }
    else if (((int)stats.STAT_BAVAIL) < warnBlocksFree)
    {
      fprintf(stderr,
	"Warning: %s directory (%s) has less than %d bytes free\n",
	envVar, dirname, warnSize);
    }
  }

  return;
}

//
// Read specified number of bytes. Recover from interrupted system calls.
//

int readn(int fd, char *buf, int nbytes)
{
    int nleft = nbytes;
    while (nleft > 0) {
        int nread = read(fd, buf, nleft);
        if (nread < 0) {
            if (errno == EINTR)
                continue;
            perror("read");
            return nread;
        }
        if (nread == 0)                 // EOF?
            break;
        nleft -= nread;
        buf   += nread;
    }
    
    return nbytes - nleft;
}
  
//
// Write specified number of bytes. Recover from interrupted system calls.
//

int writen(int fd, char *buf, int nbytes)
{
    int nleft = nbytes;
    while (nleft > 0) {
        int nwritten = write(fd, buf, nleft);
        if (nwritten < 0) {
            if (errno == EINTR)
                continue;
            return nwritten;
        }
        nleft -= nwritten;
        buf   += nwritten;
    }

    return nbytes - nleft;
}

fgets_result
nice_fgets(char *buf, int bufSize, FILE *fp)
{
  fgets_result result = fgets_ok;

  // In case no chars get transferred to buffer.
  buf[0] = '\0';

  char testChar = '\123';
  buf[bufSize - 1] = testChar;

  if (fgets(buf, bufSize, fp) == NULL) {
    if (feof(fp)) {
      result = fgets_eof;
    }
    if (ferror(fp)) {
      reportErrSys("Error in nice_fgets()");
      result = fgets_error;
    }
  }

  if (buf[bufSize - 1] != testChar) {
    // Buffer is too small (or the line had *exactly* bufSize-1 characters).
    reportErrNosys("Buffer overflow");
    result = fgets_bufoverflow;
  }

  // Make absolutely sure the string is terminated.
  buf[bufSize - 1] = '\0';

  StripTrailingNewline(buf);

  return result;
}

void
PrintArgs(FILE *fp, int argc, const char * const *argv, Boolean printNewline)
{
  int index;
  char *prefix = "";
  for (index = 0; index < argc; index++) {
    fprintf(fp, "%s<%s>", prefix, argv[index]);
    prefix = ", ";
  }
  if (printNewline) fprintf(fp, "\n");
}

void
CopyArgs(int argc, const char * const * argvOld, char **&argvNew)
{
  argvNew = new char *[argc];
  if (argvNew == NULL) {
    reportErrNosys("Insufficient memory");
    Exit::DoExit(2);
  }

  int index;
  for (index = 0; index < argc; index++) {
    argvNew[index] = CopyString(argvOld[index]);
  }
}

void FreeArgs(int argc, char **argv)
{
  int index;
  for (index = 0; index < argc; index++) {
    delete [] argv[index];
  }

  delete [] argv;
}
