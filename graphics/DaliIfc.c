/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.22  2013/09/20 16:54:02  wenger
  Merged devise_1_11_3_centos6_br_2 thru devise_1_11_3_centos6_br_3 to trunk.

  Revision 1.21  2013/06/13 22:03:10  wenger
  Merged devise_1_11_3_centos6_br_0 thru devise_1_11_3_centos6_br_2 to trunk.

  Revision 1.20.14.3  2013/09/20 15:29:27  wenger
  More Centos 6 compile fixes.

  Revision 1.20.14.2  2013/06/14 14:49:28  wenger
  More cleanups from CentOS6 compile...

  Revision 1.20.14.1  2013/06/13 21:02:54  wenger
  Changes to get DEVise to compile/link on CentOS6 (with comments for
  a bunch of unfixed warnings); minor mods to get this version to also
  compile on RHEL5...

  Revision 1.20.4.1  2014/01/17 21:46:22  wenger
  Fixed a bunch of possible buffer overflows.

  Revision 1.20  2008/09/11 20:28:04  wenger
  Committed more of the "easy" compile warning fixes.

  Revision 1.19  2001/08/28 21:40:52  wenger
  Environment variables are now expanded in Tasvir file names, EmbeddedTk
  script names and arguments, and physical schema paths within logical
  schemas (to make distribution setup easier).

  Revision 1.18  1999/12/02 16:26:44  wenger
  Fixed bug 518 (confirm before saving a session the first time); got rid
  of error message during normal Tasvir launch.

  Revision 1.17  1999/10/05 17:55:36  wenger
  Added debug log level.

  Revision 1.16  1999/09/21 18:58:15  wenger
  Devise looks for an already-running Tasvir before launching one; Devise
  can also launch a new Tasvir at any time if Tasvir has crashed; added
  warning if you use -gl flag with non-OpenGL linked DEVise.

  Revision 1.15  1999/07/16 21:35:48  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

  Revision 1.14  1998/09/10 23:22:25  wenger
  DEVise now launches Tasvir directly, rather than TasvirWithPaths.

  Revision 1.13  1998/09/04 17:26:11  wenger
  Got Tasvir images to work in pixmaps (when running the JavaScreen, for
  example) -- fixes bug 385.

  Revision 1.12  1998/02/26 00:18:51  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.11  1997/05/05 16:53:42  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.10  1997/04/11 18:48:49  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.9  1997/03/25 17:58:49  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.8  1997/02/14 16:47:31  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.7.4.3  1997/03/07 20:03:54  wenger
  Tasvir images now work in PostScript output; Tasvir images now freed
  on a per-window basis; Tasvir timeout factor can be set on the command
  line; shared memory usage enabled by default.

  Revision 1.7.4.2  1997/02/27 22:46:03  wenger
  Most of the way to having Tasvir images work in PostScript output;
  various WindowRep-related fixes; version now 1.3.4.

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
#define DEBUG_LOG
#define PRINT_DALI_STATUS 0

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

#include "machdep.h"
#include "DaliIfc.h"
#include "Dali.h"
#include "Util.h"
#include "DevError.h"
#include "DebugLog.h"


static DevStatus CloseConnection(int fd);
static DevStatus ConnectAndSend(const char *daliServer, const char *commandBuf,
  int imageLen, const char *image, char *replyBuf, double timeout);
static int MyWrite(int fd, unsigned char *buff, int num);
static int newlinefd(char *s, int fd, int maxc);
static DevStatus OpenConnection(const char *daliServer, int &fd,
  Boolean tryLaunch = true);
static DevStatus ReadImage(int fd, int numBytes, FILE *printfile);
static DevStatus SendCommand(int fd, const char *commandBuf, int imageLen,
  const char *image, char *replyBuf, double timeout);
static DevStatus WaitForReply(char *buf, int fd, int bufSize, double timeout);

DaliIfc::InfoFcn DaliIfc::_infoFcn = NULL;
static const char *_serverLaunchName = "localhost";


/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char _errBuf[DALI_MAX_STR_LENGTH+100];
static int _launchCount = 0;
static int _connectFailCount;

/*------------------------------------------------------------------------------
 * function: DaliIfc::ShowImage
 * Show an image using Dali.
 */
DevStatus
DaliIfc::ShowImage(const char *daliServer, Drawable win, int centerX,
  int centerY, int width, int height, const char *filename, int imageLen,
  const char *image, int &handle, float timeoutFactor, int maxImageSize,
  Boolean maintainAspect)
{
  DOASSERT(daliServer != NULL, "No Tasvir server specified");

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

  char *fullfile = NULL;
  if (filename != NULL) {
    fullfile = RemoveEnvFromPath(filename);
  }

  int formatted = snprintf(commandBuf, sizeof(commandBuf),
    "show %s 0x%lx %d %d %d %d -dither -xbacking -nosubwin"
    " -showafter 0 -sync 0",
    fullfile, (long) win, topLeftX, topLeftY, botRightX, botRightY);
  if (checkAndTermBuf2(commandBuf, formatted) != StatusOk) {
    result = StatusFailed;
  }
  FreeString(fullfile);
  if (!maintainAspect) {
    if (nice_strncat(commandBuf, " -noaspect", sizeof(commandBuf))
        != StatusOk) {
      result = StatusFailed;
    }
  }

  if (image != NULL) {
    char bytesBuf[DALI_MAX_STR_LENGTH];
    formatted = snprintf(bytesBuf, sizeof(bytesBuf), " -bytes %d\n", imageLen);
    if (checkAndTermBuf2(bytesBuf, formatted) != StatusOk) {
      result = StatusFailed;
    }
    if (nice_strncat(commandBuf, bytesBuf, sizeof(commandBuf)) != StatusOk) {
      result = StatusFailed;
    }
  } else {
    if (nice_strncat(commandBuf, "\n", sizeof(commandBuf)) != StatusOk) {
      result = StatusFailed;
    }
  }

  if (result.IsComplete()) {
    double timeout = (image == NULL) ? 5.0 : 30.0;
    timeout *= timeoutFactor;
    result += ConnectAndSend(daliServer, commandBuf, imageLen, image, replyBuf,
      timeout);
  }

  if (result.IsComplete()) {
    int numColors;
    sscanf(replyBuf, "%*s %d %d", &handle, &numColors);

#if 0
// We can't free images immediately becaues they'll be erased.
    formatted = snprintf(commandBuf, sizeof(commandBuf), "free %d\n", handle);
    if (checkAndTermBuf2(commandBuf, formatted) != StatusOk) {
      result = StatusFailed;
    } else {
      result += ConnectAndSend(daliServer, commandBuf, 0, (char *) NULL,
        replyBuf);
    }
#endif
  }

#if PRINT_DALI_STATUS
  if (ConnectAndSend(daliServer, "status\n", 0, NULL, replyBuf,
    5.0).IsComplete()) {
    printf("Tasvir status: %s\n", replyBuf);
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::PSShowImage
 * Show a PostScript image using Dali (for printing to PostScript).
 */
DevStatus
DaliIfc::PSShowImage(const char *daliServer, int width, int height,
  const char *filename, int imageLen, const char *image, FILE *printfile,
  float timeoutFactor, Boolean maintainAspect)
{
  DOASSERT(daliServer != NULL, "No Tasvir server specified");

#if defined(DEBUG)
  printf("DaliIfc::PSShowImage(%s, %s)\n", daliServer, filename);
#endif

  DevStatus result = StatusOk;

  char commandBuf[DALI_MAX_STR_LENGTH];
  char replyBuf[DALI_MAX_STR_LENGTH];

  char *fullfile = NULL;
  if (filename != NULL) {
    fullfile = RemoveEnvFromPath(filename);
  }

  int formatted = snprintf(commandBuf, sizeof(commandBuf),
    "psshow %s - -width %d -height %d", fullfile, width,
    height);
  if (checkAndTermBuf2(commandBuf, formatted) != StatusOk) {
    result = StatusFailed;
  }
  FreeString(fullfile);
  if (!maintainAspect) {
    if (nice_strncat(commandBuf, " -noaspect", sizeof(commandBuf))
        != StatusOk) {
      result = StatusFailed;
    }
  }

  if (image != NULL) {
    char bytesBuf[DALI_MAX_STR_LENGTH];
    formatted = snprintf(bytesBuf, sizeof(bytesBuf), " -bytes %d\n", imageLen);
    if (checkAndTermBuf2(bytesBuf, formatted) != StatusOk) {
      result = StatusFailed;
    }
    if (nice_strncat(commandBuf, bytesBuf, sizeof(commandBuf)) != StatusOk) {
      result = StatusFailed;
    }
  } else {
    if (nice_strncat(commandBuf, "\n", sizeof(commandBuf)) != StatusOk) {
      result = StatusFailed;
    }
  }

  int fd;
  result += OpenConnection(daliServer, fd);

  if (result.IsComplete()) {
    double timeout = (image == NULL) ? 5.0 : 30.0;
    timeout *= timeoutFactor;
    result += SendCommand(fd, commandBuf, imageLen, image, replyBuf, timeout);
  }

  if (result.IsComplete()) {
    int numBytes;
    sscanf(replyBuf, "%*s %d", &numBytes);
    result += ReadImage(fd, numBytes, printfile);
  }

  result += CloseConnection(fd);

#if PRINT_DALI_STATUS
  if (ConnectAndSend(daliServer, "status\n", 0, NULL, replyBuf,
    5.0).IsComplete()) {
    printf("Tasvir status: %s\n", replyBuf);
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::FreeImage
 * Free a Dali image.
 */
DevStatus
DaliIfc::FreeImage(const char *daliServer, int handle)
{
  DO_DEBUG(printf("DaliIfc::FreeImage(%s, %d)\n", daliServer, handle));

  DevStatus result = StatusOk;

  char commandBuf[DALI_MAX_STR_LENGTH];
  char replyBuf[DALI_MAX_STR_LENGTH];

  int formatted = snprintf(commandBuf, sizeof(commandBuf), "free %d\n",
    handle);
  if (checkAndTermBuf2(commandBuf, formatted) != StatusOk) {
    result = StatusFailed;
  } else {
    result += ConnectAndSend(daliServer, commandBuf, 0, NULL, replyBuf, 5.0);
  }

#if PRINT_DALI_STATUS
  if (ConnectAndSend(daliServer, "status\n", 0, NULL, replyBuf,
    5.0).IsComplete()) {
    printf("Tasvir status: %s\n", replyBuf);
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::FreeWindowImages
 * Free all Tasvir images in a given window.
 */
DevStatus
DaliIfc::FreeWindowImages(const char *daliServer, Drawable win)
{
#if defined(DEBUG)
  printf("DaliIfc::FreeWindowImages(%s, 0x%lx)\n", daliServer, (long) win);
#endif

  DevStatus result = StatusOk;

  char commandBuf[DALI_MAX_STR_LENGTH];
  char replyBuf[DALI_MAX_STR_LENGTH];

  int formatted = snprintf(commandBuf, sizeof(commandBuf),
    "wapply 0x%lx free .\n", (long) win);
  if (checkAndTermBuf2(commandBuf, formatted) != StatusOk ) {
    result = StatusFailed;
  }

  int fd;
  result += OpenConnection(daliServer, fd);

  if (result.IsComplete())
  {
    result += SendCommand(fd, commandBuf, 0, NULL, replyBuf, 5.0);
  }

  if (result.IsComplete())
  {
    int numReplies;
    sscanf(replyBuf, "%*s %d", &numReplies);

    int replyNum;
    for (replyNum = 0; replyNum < numReplies; replyNum++) {
      result += WaitForReply(replyBuf, fd, DALI_MAX_STR_LENGTH, 5.0);
#if defined(DEBUG)
      printf("Tasvir reply: %s\n", replyBuf);
#endif
    }
  }

  result += CloseConnection(fd);

#if PRINT_DALI_STATUS
  if (ConnectAndSend(daliServer, "status\n", 0, NULL, replyBuf,
    5.0).IsComplete()) {
    printf("Tasvir status: %s\n", replyBuf);
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::Reset
 * Reset the Dali server.
 */
DevStatus
DaliIfc::Reset(const char *daliServer)
{
  DO_DEBUG(printf("DaliIfc::Reset(%s)\n", daliServer));
  DevStatus result = StatusOk;

  char replyBuf[DALI_MAX_STR_LENGTH];
  result += ConnectAndSend(daliServer, "reset\n", 0, NULL, replyBuf, 5.0);

#if PRINT_DALI_STATUS
  if (ConnectAndSend(daliServer, "status\n", 0, NULL, replyBuf,
    5.0).IsComplete()) {
    printf("Tasvir status: %s\n", replyBuf);
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::Quit
 * Kill the Dali server.
 */
DevStatus
DaliIfc::Quit(const char *daliServer)
{
#if defined(DEBUG)
  printf("DaliIfc::Quit(%s)\n", daliServer);
#endif

  DevStatus result = StatusOk;

  char replyBuf[DALI_MAX_STR_LENGTH];
  result += ConnectAndSend(daliServer, "quit\n", 0, NULL, replyBuf, 5.0);

  return result;
}

/*------------------------------------------------------------------------------
 * function: DaliIfc::LaunchServer
 * Attempt to launch a Tasvir server.
 */
DevStatus
DaliIfc::LaunchServer()
{
#if defined(DEBUG)
  printf("DaliIfc::LaunchServer()\n");
#endif

  DevStatus result = StatusOk;
  const int maxLaunchTries = 2;
  const char *serverName = _serverLaunchName;

  // See if there's already a Tasvir that we can talk to.
  int fd;
  DevStatus tmpResult = OpenConnection(_serverLaunchName, fd, false);
  if (tmpResult.IsComplete()) {
#if defined(DEBUG)
    printf("  Tasvir already running.\n");
#endif
    tmpResult += CloseConnection(fd);
    _infoFcn(serverName, false);
    return tmpResult;
  }

  if (_launchCount >= maxLaunchTries) {
    printf("Lauching Tasvir server failed %d times; not trying again",
      _launchCount);
    result = StatusCancel;
  } else {
    _connectFailCount = 0;
    _launchCount++;

    printf("Launching Tasvir server\n");
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	    "Launching Tasvir server");
#endif

    pid_t pid = fork();

    if (pid < 0) {
      /* Error. */
      reportErrSys("fork() failed");
      result = StatusFailed;
    } else if (pid == 0) {
      /* Child. */
      char *args[2];
#if 0
      args[0] = "TasvirWithPaths";
#else
      args[0] = CopyString("Tasvir");
#endif
      args[1] = NULL;
      execvp(args[0], args);
      /* execvp doesn't return if it works. */
      reportErrSys("execvp() failed");
      _exit(1);
    } else {
      /* Parent. */
      serverName = _serverLaunchName;
      char replyBuf[DALI_MAX_STR_LENGTH];
      int count = 0;
      Boolean done = false;

      (void) DevError::SetEnabled(false);
      const int maxConnectTries = 10;
      while (count < maxConnectTries && !done) {
        sleep(2);
        DevStatus tmpResult = ConnectAndSend(serverName, "status\n", 0, NULL,
          replyBuf, 2.0);
        if (tmpResult.IsComplete()) done = true;
        count++;
      }
      (void) DevError::SetEnabled(true);

      if (!done) {
        reportErrNosys("Launch of Tasvir server failed");
        kill(pid, SIGKILL);
        result += StatusFailed;
      }
    }
  }

  if (result.IsComplete() && _infoFcn) {
    _infoFcn(serverName, true);
  }

#if defined(DEBUG)
  printf("  serverName = <%s>\n", serverName);
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: ConnectAndSend
 * Connect, send a command to the Tasvir server, and disconnect.
 */
static DevStatus
ConnectAndSend(const char *daliServer, const char *commandBuf, int imageLen,
    const char *image, char *replyBuf, double timeout)
{
  DevStatus result = StatusOk;

/*
 * Establish connection with Dali server.
 */
  int fd;
  result += OpenConnection(daliServer, fd);

/*
 * Send the command.
 */
  if (result.IsComplete()) result += SendCommand(fd, commandBuf, imageLen,
    image, replyBuf, timeout);

/*
 * Close the connection.
 */
  result += CloseConnection(fd);

  return result;
}

/*------------------------------------------------------------------------------
 * function: SendCommand
 * Send a command to the Tasvir server.
 */
static DevStatus
SendCommand(int fd, const char *commandBuf, int imageLen, const char *image,
  char *replyBuf, double timeout)
{
  DevStatus result = StatusOk;

#if defined(DEBUG)
  printf("Tasvir command: %s", commandBuf);
#endif
#if defined(DEBUG_LOG)
  {
    char logBuf[MAXPATHLEN * 2];
    int formatted = snprintf(logBuf, sizeof(logBuf),
      "Tasvir command: <%s>\n", commandBuf);
    checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
  }
#endif

/*
 * Send the command.
 */
  int length = strlen(commandBuf);
  if (MyWrite(fd, (unsigned char *) commandBuf, length) != length)
  {
    reportError("Unable to send command to Tasvir", errno);
    result = StatusFailed;
  }

#if defined(DEBUG_LOG)
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
      "  Tasvir command sent");
#endif

/*
 * Send the image, if necessary.
 */
  if (result.IsComplete() && (imageLen > 0) && (image != NULL))
  {
    if (MyWrite(fd, (unsigned char *) image, imageLen) != imageLen)
    {
      reportError("Unable to send image to Tasvir", errno);
      result = StatusFailed;
    }
#if defined(DEBUG_LOG)
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, "  Tasvir image sent");
#endif
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
    DO_DEBUG(printf("Tasvir reply: %s\n", replyBuf));

    const char *okStr = "OK";
    const char *errStr = "ERROR";
    if (!strncmp(replyBuf, okStr, strlen(okStr)))
    {
      // Do nothing...
    }
    else if (!strncmp(replyBuf, errStr, strlen(errStr)))
    {
      int formatted = snprintf(_errBuf, sizeof(_errBuf),
          "Error from Tasvir server: %s",
          &replyBuf[strlen(errStr)]);
      checkAndTermBuf2(_errBuf, formatted);
      reportError(_errBuf, devNoSyserr);
      result = StatusFailed;
    }
    else
    {
      int formatted = snprintf(_errBuf, sizeof(_errBuf),
        "Can't understand reply from Tasvir (%s)", replyBuf);
      checkAndTermBuf2(_errBuf, formatted);
      reportError(_errBuf, devNoSyserr);
      result = StatusFailed;
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: OpenConnection
 * Open a connection to the Tasvir server.
 */
static DevStatus
OpenConnection(const char *daliServer, int &fd, Boolean tryLaunch)
{
#if defined(DEBUG)
  printf("OpenConnection(%s, %d)\n", daliServer, tryLaunch);
#endif

  DevStatus result = StatusOk;

#if defined(DEBUG_LOG)
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
      "Opening Tasvir connection");
#endif

  const int maxConnectFails = 25;

  if (_connectFailCount > maxConnectFails) {
    const char *msg = "Tasvir connect canceled because of previous failures";
    fprintf(stderr, "%s\n", msg);
#if defined(DEBUG_LOG)
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, msg);
#endif
    fd = -1;
    result = StatusCancel;
  } else {
    fd = DaliPatron((char *)daliServer, _errBuf);
    if (fd < 0)
    {
      if (tryLaunch) {
        result = DaliIfc::LaunchServer();
        if (result.IsComplete()) {
          result += OpenConnection(_serverLaunchName, fd, false);
        }
      } else {
        _connectFailCount++;
        reportError(_errBuf, errno);
        result = StatusFailed;
      }
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: CloseConnection
 * Close the connection to the Tasvir server.
 */
static DevStatus
CloseConnection(int fd)
{
  DevStatus result = StatusOk;

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
    double startTime = (double) time.tv_sec + ((double) time.tv_usec) * 1.0e-6;
    double timeLimit = startTime + timeout;

    Boolean done = false;
    while (!done)
    {
      if (newlinefd(buf, fd, bufSize) > 0) {
	done = true;
      } else {
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
	    reportError("Timed out waiting for Tasvir reply", devNoSyserr);
	    result = StatusFailed;
	    done = true;
	  }
	  else if (currentTime - startTime > 1.0)
	  {
	    sleep(1);
	  }
        }
      }
    }
  }

#if defined(DEBUG_LOG)
  if (result.IsComplete()) {
    char logBuf[1024];
    int formatted = snprintf(logBuf, sizeof(logBuf),
      "Tasvir reply: <%s>\n", buf);
    checkAndTermBuf2(logBuf, formatted);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: ReadImage
 * Read an image from the Tasvir server and write it to the print file.
 */
static DevStatus
ReadImage(int fd, int numBytes, FILE *printfile)
{
  DevStatus result = StatusOk;

#if defined(DEBUG)
  printf("ReadImage(%d)\n", numBytes);
#endif

  const int bufSize = 1024;
  char buffer[bufSize];

  int bytesLeft = numBytes;
  int bytesToRead, bytesRead, bytesWritten;

  while (bytesLeft > 0) {
    bytesToRead = MIN(bufSize, bytesLeft);
    bytesRead = read(fd, buffer, bytesToRead);
    if (bytesRead < 0) {
      reportErrSys("Error reading Tasvir image from socket");
      result += StatusWarn;
    } else {
      bytesLeft -= bytesRead;
    }

#if 1 // Conditional this out to just throw away image.
    if (bytesRead > 0) {
      bytesWritten = fwrite(buffer, 1, bytesRead, printfile);
      if (bytesWritten != bytesRead) {
        int formatted = snprintf(_errBuf, sizeof(_errBuf),
	    "Only %d of %d bytes written", bytesWritten, bytesRead);
	checkAndTermBuf2(_errBuf, formatted);
        reportErrSys(_errBuf);
        result += StatusFailed;
      }
    }
#endif
  }

  if (bytesLeft != 0) {
    int formatted = snprintf(_errBuf, sizeof(_errBuf),
        "%d of %d bytes missing", bytesLeft, numBytes);
    checkAndTermBuf2(_errBuf, formatted);
    reportErrNosys(_errBuf);
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
  return(-1); 
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
