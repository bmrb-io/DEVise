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
  Implementation of DaliIfc class.
 */

/*
  $Id$

  $Log$
  Revision 1.7.4.1  1997/02/07 15:21:19  wenger
  Updated Devise version to 1.3.1; fixed bug 148 (GUI now forces unique
  window names); added axis toggling and color selections to Window menu;
  other minor fixes to GUI; show command to Tasvir now requests image to
  be shown all at once.

  Revision 1.7  1997/01/09 18:41:16  wenger
  Added workarounds for some Tasvir image bugs, added debug code.

  Revision 1.6  1996/12/18 18:52:00  wenger
  Devise requests Tasvir not to use subwindows for its images; sizing of
  Tasvir images now basically works like a RectX, except that the aspect
  ratio is fixed by the image itself; fixed a bug in action of +/- keys
  for RectX shapes; Devise won't request Tasvir images smaller than two
  pixels (causes error in Tasvir).

  Revision 1.5  1996/11/21 19:13:45  wenger
  Fixed more compile warnings; updated devise.dali to match current
  command-line flags.

  Revision 1.4  1996/09/04 21:24:47  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.3  1996/08/29 18:24:31  wenger
  A number of Dali-related improvements: ShapeAttr1 now specifies image
  type when shape is 'image'; added new '-bytes' flag to Dali commands
  when sending images; TDataBinaryInterp now uses StringStorage so GData
  can access strings; fixed hash function for StringStorage so having the
  high bit set in a byte in the string doesn't crash the hash table;
  improved the error checking in some of the Dali code.

  Revision 1.2  1996/08/28 00:19:35  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.1  1996/08/23 16:55:28  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

 */

#define _DaliIfc_c_

//#define DEBUG
#define PRINT_DALI_STATUS 0

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#include "machdep.h"
#include "DaliIfc.h"
#include "Dali.h"
#include "Util.h"
#include "DevError.h"


static int MyWrite(int fd, unsigned char *buff, int num);
static int newlinefd(char *s, int fd, int maxc);
static DevStatus SendCommand(char *daliServer, char *commandBuf, int imageLen,
  char *image, char *replyBuf, double timeout);
static DevStatus WaitForReply(char *buf, int fd, int bufSize, double timeout);


/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DaliIfc::ShowImage
 * Show an image using Dali.
 */
DevStatus
DaliIfc::ShowImage(char *daliServer, Window win, int centerX,
  int centerY, int width, int height, char *filename, int imageLen,
  char *image, int &handle, float timeoutFactor, int maxImageSize)
{
  DOASSERT(daliServer != NULL, "No Dali server specified");

#if defined(DEBUG)
  printf("DaliIfc::ShowImage(%s, 0x%lx, %d, %d, %d, %d, %s)\n",
    daliServer, (long) win, centerX, centerY, width, height, filename);
#endif

  DevStatus result = StatusOk;

  const int minTasvirSize = 2;
  width = MAX(width, minTasvirSize);
  height = MAX(height, minTasvirSize);

  if ((width > maxImageSize) || (height > maxImageSize)) {
    printf("Tasvir requested image size too large (%d x %d pixels)\n",
      width, height);
    printf("You should probably change your mapping\n");
    width = MIN(width, maxImageSize);
    height = MIN(height, maxImageSize);
    printf("Requested size changed to %d x %d pixels\n", width, height);
  }

  int topLeftX = centerX - (width / 2);
  int topLeftY = centerY - (height / 2);
  int botRightX = centerX + (width / 2) - 1;
  int botRightY = centerY + (height / 2) - 1;

  char commandBuf[DALI_MAX_STR_LENGTH];
  char replyBuf[DALI_MAX_STR_LENGTH];

  sprintf(commandBuf, "show %s 0x%lx %d %d %d %d -dither -xbacking -nosubwin"
    " -showafter 0 -sync 0",
    filename, (long) win, topLeftX, topLeftY, botRightX, botRightY);
  if (image != NULL)
  {
    char bytesBuf[DALI_MAX_STR_LENGTH];
    sprintf(bytesBuf, " -bytes %d\n", imageLen);
    strcat(commandBuf, bytesBuf);
  }
  else
  {
    strcat(commandBuf, "\n");
  }

  double timeout = (image == NULL) ? 5.0 : 30.0;
  timeout *= timeoutFactor;
  result += SendCommand(daliServer, commandBuf, imageLen, image, replyBuf,
    timeout);

  if (result.IsComplete())
  {
    int numColors;
    sscanf(replyBuf, "%*s %d %d", &handle, &numColors);

#if 0 // We can't free images immediately becaues they'll be erased.
    sprintf(commandBuf, "free %d\n", handle);
    result += SendCommand(daliServer, commandBuf, 0, (char *) NULL, replyBuf);
#endif
  }

#if PRINT_DALI_STATUS
  SendCommand(daliServer, "status\n", 0, NULL, replyBuf, 5.0);
  printf("Dali status: %s\n", replyBuf);
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::FreeImage
 * Free a Dali image.
 */
DevStatus
DaliIfc::FreeImage(char *daliServer, int handle)
{
  DO_DEBUG(printf("DaliIfc::FreeImage(%s, %d)\n", daliServer, handle));

  DevStatus result = StatusOk;

  char commandBuf[DALI_MAX_STR_LENGTH];
  char replyBuf[DALI_MAX_STR_LENGTH];

  sprintf(commandBuf, "free %d\n", handle);

  result += SendCommand(daliServer, commandBuf, 0, NULL, replyBuf, 5.0);

#if PRINT_DALI_STATUS
  SendCommand(daliServer, "status\n", 0, NULL, replyBuf, 5.0);
  printf("Dali status: %s\n", replyBuf);
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::Reset
 * Reset the Dali server.
 */
DevStatus
DaliIfc::Reset(char *daliServer)
{
  DO_DEBUG(printf("DaliIfc::Reset(%s)\n", daliServer));
  DevStatus result = StatusOk;

  char replyBuf[DALI_MAX_STR_LENGTH];
  result += SendCommand(daliServer, "reset\n", 0, NULL, replyBuf, 5.0);

#if PRINT_DALI_STATUS
  SendCommand(daliServer, "status\n", 0, NULL, replyBuf, 5.0);
  printf("Dali status: %s\n", replyBuf);
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::Quit
 * Kill the Dali server.
 */
DevStatus
DaliIfc::Quit(char *daliServer)
{
  DO_DEBUG(printf("DaliIfc::Quit(%s)\n", daliServer));
  DevStatus result = StatusOk;

  char replyBuf[DALI_MAX_STR_LENGTH];
  result += SendCommand(daliServer, "quit\n", 0, NULL, replyBuf, 5.0);

  return result;
}

/*------------------------------------------------------------------------------
 * function: SendCommand
 * Send a command to the Dali server.
 */
static DevStatus
SendCommand(char *daliServer, char *commandBuf, int imageLen, char *image,
  char *replyBuf, double timeout)
{
  DevStatus result = StatusOk;

  DO_DEBUG(printf("Dali command: %s", commandBuf));

/*
 * Establish connection with Dali server.
 */
  char errBuf[DALI_MAX_STR_LENGTH];
  int fd = DaliPatron(daliServer, errBuf);
  if (fd < 0)
  {
    reportError(errBuf, errno);
    result = StatusFailed;
  }

/*
 * Send the command.
 */
  if (result.IsComplete())
  {
    int length = strlen(commandBuf);
    if (MyWrite(fd, (unsigned char *) commandBuf, length) != length)
    {
      reportError("Unable to send command to Dali", errno);
      result = StatusFailed;
    }
  }

/*
 * Send the image, if necessary.
 */
  if (result.IsComplete() && (imageLen > 0) && (image != NULL))
  {
    if (MyWrite(fd, (unsigned char *) image, imageLen) != imageLen)
    {
      reportError("Unable to send image to Dali", errno);
      result = StatusFailed;
    }
  }

/*
 * Now wait for and decode the reply.
 */
  if (result.IsComplete())
  {
    result += WaitForReply(replyBuf, fd, DALI_MAX_STR_LENGTH, timeout);
  }

  if (result.IsComplete())
  {
    DO_DEBUG(printf("Dali reply: %s\n", replyBuf));

    const char *okStr = "OK";
    const char *errStr = "ERROR";
    char errBuf[DALI_MAX_STR_LENGTH+100];
    if (!strncmp(replyBuf, okStr, strlen(okStr)))
    {
      // Do nothing...
    }
    else if (!strncmp(replyBuf, errStr, strlen(errStr)))
    {
      sprintf(errBuf, "Error from Dali server: %s", &replyBuf[strlen(errStr)]);
      reportError(errBuf, devNoSyserr);
      result = StatusFailed;
    }
    else
    {
      sprintf(errBuf, "Can't understand reply from Dali (%s)", replyBuf);
      reportError(errBuf, devNoSyserr);
      result = StatusFailed;
    }
  }

  if (fd >= 0)
  {
    if (close(fd) < 0)
    {
      reportError("Error closing socket", errno);
      result += StatusWarn;
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: WaitForReply
 * Wait for a reply from the Dali server.
 */
static DevStatus
WaitForReply(char *buf, int fd, int bufSize, double timeout)
{
  DevStatus result = StatusOk;
  double currentTime;

  struct timeval time;
  if (gettimeofday(&time, NULL) == -1)
  {
    reportError("gettimeofday failed", errno);
    result = StatusFailed;
  }
  else
  {
    double timeLimit = (double) time.tv_sec +
      ((double) time.tv_usec) * 1.0e-6 + timeout;

    Boolean done = false;
    while (!done)
    {
      if (newlinefd(buf, fd, bufSize) > 0) done = true;

      if (gettimeofday(&time, NULL) == -1)
      {
        reportError("gettimeofday failed", errno);
        result = StatusFailed;
        done = true;
      }
      else
      {
        currentTime = (double) time.tv_sec + ((double) time.tv_usec) * 1.0e-6;
	if (currentTime > timeLimit)
	{
	  reportError("Timed out waiting for Dali reply", devNoSyserr);
	  result = StatusFailed;
	  done = true;
	}
      }
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: newlinefd
 * read a \n-terminated line from fd into s, replacing
 * \n by \0. do not read more than maxc-1 characters
 * return the number of characters read (excluding \n). 
 */
static int
newlinefd(char *s, int fd, int maxc)
{
  int i, lim = maxc-1; 
  char c;
  i = 0;
  while ((i < lim) && (read(fd,&c,1) == 1)) {
    if (c == '\n') {
      s[i] = '\0';
      return (i);
    }
    s[i++] = c;
  }
  s[i] = '\0';
  return(i); 
}

/*------------------------------------------------------------------------------
 * function: MyWrite
 * Write num bytes into fd from buff. Return
 * # bytes actually written, which may be less only when
 * write(fd,..,..) returns <=0 at some point. 
 */
static int
MyWrite(int fd, unsigned char *buff, int num)
{
  unsigned char *temp;
  int tocome = num;
  int thistime;
  
  temp = buff;

  while (tocome > 0) {
    thistime = write(fd,temp,tocome);
    if (thistime <= 0) return(num-tocome);
    tocome -= thistime;
    temp = temp + thistime;
  }

  return(num);
}

/*============================================================================*/
