/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.5  1997/05/21 22:09:50  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.4  1997/05/05 16:53:43  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.3  1997/03/28 16:09:12  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

#include "machdep.h"
#include "ETkIfc.h"
#include "ETk.h"
#include "Util.h"
#include "DevError.h"

//#define DEBUG

#define PRINT_ETK_STATUS 0

char *ETkIfc::_etkServer = NULL;
Boolean ETkIfc::_etkQuit = false;

static int MyWrite(int fd, unsigned char *buff, int num);
static int newlinefd(char *s, int fd, int maxc);
static DevStatus SendETkCommand(int fd, char *commandBuf,
				int argc, const char **argv,
				char *replyBuf, double timeout);
static DevStatus ConnectAndSendETkCommand(const char *etkServer,
					  char *commandBuf,
					  int argc, const char **argv,
					  char *replyBuf, double timeout);
static DevStatus WaitForReply(char *buf, int fd, int bufSize, double timeout);

//
// Static global variables.
//
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif
static char *	srcFile = __FILE__;

static int launchCount = 0;

//
// ETkIfc::CreateWindow()
//
// Show an embedded Tk window. filename is the name of a Tcl/Tk script
// to be executed in the new window. argc and argv carry the command-line
// arguments that will be passed to the Tcl/Tk script. If width or height
// is 0, that indicates that the new window should be sized by Tk. The
// actual width and height will be returned in reference parameters.
//
DevStatus
ETkIfc::CreateWindow(const char *etkServer, Window win,
		     int &x, int &y,
		     int &width, int &height,
		     Anchor anchor,
		     const char *filename,
		     int argc, const char **argv,
		     int &handle)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
    
    int i;
    char command[ETK_MAX_STR_LENGTH];
    char reply[ETK_MAX_STR_LENGTH];

    DOASSERT(etkServer != NULL, "No ETk server specified");
    
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("    Center (%d,%d), width %d, height %d\n",
	   x, y, width, height);
    printf("    Script %s\n", filename);
    printf("    argc: %d\n", argc);
    for (i = 0; i < argc; i++)
	printf("    arg: %s\n", argv[i] ? argv[i] : "(NULL)");
#endif
    
    DevStatus result = StatusOk;
    
    sprintf(command, "show %s 0x%lx %d %d %d %d %s %d",
	    filename, (long) win,
	    x, y, width, height,
	    AnchorToString(anchor), argc);
    
    double timeout = 5.0;
    
    //
    // ConnectAndSendETkCommand() will connect to server, send
    // command, send args, wait for reply, and close connection
    //
    DevStatus temp = ConnectAndSendETkCommand(etkServer, command,
					      argc, argv, reply, timeout);
    result += temp;

    if (result.IsComplete())
    {
	sscanf(reply, "%*s %d %d %d %d %d",
	       &handle, &x, &y, &width, &height);
    }
    
#if PRINT_ETK_STATUS
    ConnectAndSendETkCommand(etkServer, "status", 0, NULL, reply, 5.0);
    printf("ETk status: %s\n", replyBuf);
#endif
    
    END_ETK_TRACE(__FUNCTION__);
    return result;

}

//
// Function to convert ETk anchor positions to strings
//
char *
ETkIfc::AnchorToString(Anchor anchor)
{
    switch (anchor)
    {
      case NorthWest:
	return "nw";
	break;
      case Center:
	return "c";
	break;
    }
    return "";
}

//
// ETkIfc::EvalCommand()
//
// Sends a command to a Tcl interpreter inside the EmbeddedTk server.
// If successful, result stored in static memory that will be overwritten
// on next call. returnValue will point to the result
//
DevStatus
ETkIfc::EvalCommand(const char *etkServer, int handle,
		    int argc, const char **argv,
		    char *&returnValue)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
    
    int i;
    char command[ETK_MAX_STR_LENGTH];
    static char reply[ETK_MAX_STR_LENGTH];

    DOASSERT(etkServer != NULL, "No ETk server specified");
    
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("    handle: %d\n", handle);
    printf("    argc: %d\n", argc);
    for (i = 0; i < argc; i++)
    {
	printf("    arg: %s\n", argv[i] ? argv[i] : "(NULL)");
    }
#endif
    
    DevStatus result = StatusOk;
    double timeout = 5.0;
    sprintf(command, "eval %d %d", handle, argc);
    
    //
    // ConnectAndSendETkCommand() will connect to server, send
    // command, send args, wait for reply, and close connection
    //
    DevStatus temp = ConnectAndSendETkCommand(etkServer, command,
					      argc, argv, reply, timeout);
    result += temp;
    if (result.IsComplete())
    {
	if (!strncmp(reply, "OK", 2))
	{
	    returnValue = &reply[2];
	}
	else if (!strncmp(reply, "ERROR", 5))
	{
	    returnValue = &reply[5];
	}
	else
	{
	    returnValue = reply;
	}
	while (isspace(*returnValue))
	{
	    returnValue++;
	}
    }
    
#if PRINT_ETK_STATUS
    ConnectAndSendETkCommand(etkServer, "status", 0, NULL, reply, 5.0);
    printf("ETk status: %s\n", replyBuf);
#endif
    
    END_ETK_TRACE(__FUNCTION__);
    return result;

}

//
// ETkIfc::SendCommand()
//
// Sends a command to the EmbeddedTk server. The handle will be appended 
// to the command if handle != ETK_INVALID_HANDLE.
//
//
DevStatus
ETkIfc::SendCommand(const char *etkServer, const char *command,
		    int argc, const char **argv,
		    int handle)
{
    BEGIN_ETK_TRACE(__FUNCTION__);

#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("  command: %s\n", command);
    printf("  handle: %d\n", handle);
#endif
        
    DevStatus result = StatusOk;
    
    char commandBuf[ETK_MAX_STR_LENGTH];
    char replyBuf[ETK_MAX_STR_LENGTH];
    
    if (handle != ETK_INVALID_HANDLE)
	sprintf(commandBuf, "%s %d", command, handle);
    else
	sprintf(commandBuf, "%s", command);
    
    DevStatus temp = ConnectAndSendETkCommand(etkServer, commandBuf,
					      argc, argv, replyBuf, 1.0);
    result += temp;
    
#if PRINT_ETK_STATUS
    ConnectAndSendETkCommand(etkServer, "status", 0, NULL, replyBuf, 5.0);
    printf("ETk status: %s\n", replyBuf);
#endif
    
    END_ETK_TRACE(__FUNCTION__);
    return result;
}

//
// ETkIfc::MoveWindow()
//
// Moves an embedded Tk window
//
DevStatus
ETkIfc::MoveWindow(const char *etkServer, int handle,
		   int centerX, int centerY)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
#if defined(DEBUG)
    printf("ETkIfc::MoveWindow(%s,%d,%d,%d)\n",
	   etkServer, handle, centerX, centerY);
#endif
    
    DevStatus result = StatusOk;
    
    char commandBuf[ETK_MAX_STR_LENGTH];
    char replyBuf[ETK_MAX_STR_LENGTH];
    
    sprintf(commandBuf, "move %d %d %d", handle, centerX, centerY);
    
    DevStatus temp = ConnectAndSendETkCommand(etkServer, commandBuf,
					      0, NULL, replyBuf, 1.0);
    result += temp;
    
#if PRINT_ETK_STATUS
    ConnectAndSendETkCommand(etkServer, "status", 0, NULL, replyBuf, 5.0);
    printf("ETk status: %s\n", replyBuf);
#endif
    
    END_ETK_TRACE(__FUNCTION__);
    return result;
}

//
// ETkIfc::ResizeWindow()
//
// Resizes an embedded Tk window. Leaving the center of the window in
// the same position
//
DevStatus
ETkIfc::ResizeWindow(const char *etkServer, int handle,
		     int width, int height)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
#if defined(DEBUG)
    printf("ETkIfc::ResizeWindow(%s,%d,%d,%d)\n",
	   etkServer, handle, width, height);
#endif
    
    DevStatus result = StatusOk;
    
    char commandBuf[ETK_MAX_STR_LENGTH];
    char replyBuf[ETK_MAX_STR_LENGTH];
    
    sprintf(commandBuf, "resize %d %d %d", handle, width, height);
    
    DevStatus temp = ConnectAndSendETkCommand(etkServer, commandBuf,
					      0, NULL, replyBuf, 1.0);
    result += temp;
    
#if PRINT_ETK_STATUS
    ConnectAndSendETkCommand(etkServer, "status", 0, NULL, replyBuf, 5.0);
    printf("ETk status: %s\n", replyBuf);
#endif
    
    END_ETK_TRACE(__FUNCTION__);
    return result;
}

//
// ETkIfc::MoveResizeWindow()
//
// Moves and resizes an embedded Tk window
//
DevStatus
ETkIfc::MoveResizeWindow(const char *etkServer, int handle,
			 int xcenter, int ycenter,
			 int width, int height)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
#if defined(DEBUG)
    printf("ETkIfc::ResizeWindow(%s,%d,%d,%d,%d,%d)\n",
	   etkServer, handle, xcenter, ycenter, width, height);
#endif
    
    DevStatus result = StatusOk;
    
    char commandBuf[ETK_MAX_STR_LENGTH];
    char replyBuf[ETK_MAX_STR_LENGTH];
    
    sprintf(commandBuf, "moveresize %d %d %d %d %d", handle,
	    xcenter, ycenter, width, height);
    
    DevStatus temp = ConnectAndSendETkCommand(etkServer, commandBuf,
					      0, NULL, replyBuf, 1.0);
    result += temp;
    
#if PRINT_ETK_STATUS
    ConnectAndSendETkCommand(etkServer, "status", 0, NULL, replyBuf, 5.0);
    printf("ETk status: %s\n", replyBuf);
#endif
    
    END_ETK_TRACE(__FUNCTION__);
    return result;
}


//
// ETkIfc::LaunchServer()
//
// Attempts to launch an Embedded Tk server
//
DevStatus
ETkIfc::LaunchServer(char *&serverName)
{
  DevStatus result = StatusOk;
  const int maxLaunchTries = 2;

  if (launchCount >= maxLaunchTries) {
    printf("Lauching EmbeddedTk server failed %d times; not trying again",
      launchCount);
    result = StatusCancel;
  } else {
    launchCount++;

    printf("Launching EmbeddedTk server\n");

    pid_t pid = fork();

    if (pid < 0) {
      /* Error. */
      reportErrSys("fork() failed");
      result = StatusFailed;
    } else if (pid == 0) {
      /* Child. */
      char *args[2];
      args[0] = "EmbeddedTk";
      args[1] = NULL;
      execvp(args[0], args);
      /* execvp doesn't return if it works. */
      reportErrSys("execvp() failed");
      _exit(1);
    } else {
      /* Parent. */
      serverName = "localhost";
      char replyBuf[ETK_MAX_STR_LENGTH];
      int count = 0;
      Boolean done = false;

      (void) DevError::SetEnabled(false);
      const int maxConnectTries = 10;
      while (count < maxConnectTries && !done) {
        sleep(2);
        DevStatus tmpResult = ConnectAndSendETkCommand(serverName,
						       "status", 0, NULL,
						       replyBuf, 2.0);
        if (tmpResult.IsComplete()) {
	  done = true;
	}
        count++;
      }
      (void) DevError::SetEnabled(true);

      if (!done) {
        reportErrNosys("Launch of EmbeddedTk server failed");
        kill(pid, SIGKILL);
        result += StatusFailed;
      }
    }
  }

  return result;
}

//
// static function: SendETkCommand()
//
// Send a command to an ETk server.
//
// - Sends: commandBuf newline
//          argv[0] newline
//          argv[1] newline
//          ...
//          argv[argc-1] newline
// - Waits for a reply
//
// IMPORTANT: The command and arg strings should NOT already have
// newlines embedded in them. This function takes care of sending
// the trailing newline after each string.
//
static DevStatus
SendETkCommand(int fd, char *commandBuf,
	       int argc, const char **argv,
	       char *replyBuf, double timeout)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
    
    DevStatus result = StatusOk;
    int i;
    int length;
    
    if (argc < 0)
	argc = 0;
    
    for (i = 0; i < argc; i++)
    {
	if (argv[i] == NULL)
	    argv[i] = "";
    }
    
    
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("    command: %s\n", commandBuf);
    printf("    timeout: %f\n", timeout);
    printf("    argc: %d\n", argc);
    for (i = 0; i < argc; i++)
	printf("    arg: %s\n", argv[i] ? argv[i] : "(NULL)");
#endif
    
    //
    // Send the command.
    //
    length = strlen(commandBuf);
    if (MyWrite(fd, (unsigned char *) commandBuf, length) != length)
    {
	reportError("Unable to send command to ETk server", errno);
	result = StatusFailed;
    }
    if (MyWrite(fd, (unsigned char *) "\n", 1) != 1)
    {
	reportError("Unable to send command to ETk server", errno);
	result = StatusFailed;
    }
    
    //
    // Send the arguments
    //
    if (result.IsComplete())
    {
	for (i = 0; i < argc && result.IsComplete(); i++)
	{
	    length = strlen(argv[i]);
	    if (MyWrite(fd, (unsigned char *) argv[i], length) != length)
	    {
		reportError("Unable to send command to ETk server", errno);
		result = StatusFailed;
	    }
	    if (MyWrite(fd, (unsigned char *) "\n", 1) != 1)
	    {
		reportError("Unable to send command to ETk server", errno);
		result = StatusFailed;
	    }
	}
    }
    
    //
    // Now wait for and decode the reply.
    //
    if (result.IsComplete())
    {
	result += WaitForReply(replyBuf, fd, ETK_MAX_STR_LENGTH, timeout);
    }
    
    if (result.IsComplete())
    {
#if defined(DEBUG)
	printf("ETk server reply: %s\n", replyBuf);
#endif	
	const char *okStr = "OK";
	const char *errStr = "ERROR";
	char errBuf[ETK_MAX_STR_LENGTH+100];
	if (!strncmp(replyBuf, okStr, strlen(okStr)))
	{
	    // All is well. Do nothing.
	}
	else if (!strncmp(replyBuf, errStr, strlen(errStr)))
	{
	    sprintf(errBuf, "Error from ETk server: %s",
		    &replyBuf[strlen(errStr)]);
	    reportError(errBuf, devNoSyserr);
	    result = StatusFailed;
	}
	else
	{
	    sprintf(errBuf,
		    "Can't understand reply from ETk server(%s)", replyBuf);
	    reportError(errBuf, devNoSyserr);
	    result = StatusFailed;
	}
    }
    
    END_ETK_TRACE(__FUNCTION__);
    return result;

}

//
// static function: ConnectAndSendETkCommand()
//
// - Connects to an ETk server
// - Sends a command
// - Closes the connection
//
static DevStatus
ConnectAndSendETkCommand(const char *etkServer, char *commandBuf,
			 int argc, const char **argv,
			 char *replyBuf, double timeout)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
    
    DevStatus result = StatusOk;
    
    //
    // Establish connection with ETk server.
    //
    char errBuf[ETK_MAX_STR_LENGTH];
    int fd = ETkClient(etkServer, errBuf);
    if (fd < 0)
    {
	reportError(errBuf, errno);
	result = StatusFailed;
    }
    
    //
    // Send the command.
    //
    if (result.IsComplete())
    {
	result = SendETkCommand(fd, commandBuf, argc, argv,
				replyBuf, timeout);
    }

    //
    // Close the connection
    //
    if (fd >= 0)
    {
	if (close(fd) < 0)
	{
	    reportError("Error closing socket", errno);
	    result += StatusWarn;
	}
    }
    
    END_ETK_TRACE(__FUNCTION__);
    return result;

}

//
// static function: WaitForReply()
//
// Wait for a reply from the ETk server, and store the reply in buf.
// Returns an error if no reply is received in timeout seconds.
//
static DevStatus
WaitForReply(char *buf, int fd, int bufSize, double timeout)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("    timeout: %f\n", timeout);
#endif
    
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
        double startTime = (double) time.tv_sec +
            ((double) time.tv_usec) * 1.0e-6;
	double timeLimit = startTime + timeout;
	
	Boolean done = false;
	while (!done)
	{
	    if (newlinefd(buf, fd, bufSize) > 0)
	    {
		done = true;
	    }
	    
	    else if (gettimeofday(&time, NULL) == -1)
	    {
		reportError("gettimeofday failed", errno);
		result = StatusFailed;
		done = true;
	    }
	    else
	    {
		currentTime = (double) time.tv_sec +
		    ((double) time.tv_usec) * 1.0e-6;
                // printf("KSB: [%s] to=%f st=%f cur=%f lim=%f\n", buf, timeout, startTime, currentTime, timeLimit);
		if (currentTime > timeLimit)
		{
		    reportError("Timed out waiting for ETk server's reply",
				devNoSyserr);
		    result = StatusFailed;
		    done = true;
		}
	    }
	}
    }
    
    END_ETK_TRACE(__FUNCTION__);
    return result;
}

//
// static function: newlinefd()
//
// Read a \n-terminated line from fd into s, replacing
// \n by \0. do not read more than maxc-1 characters.
// Returns the number of characters read (excluding \n). 
//
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

//
// static function: MyWrite()
//
// Write num bytes into fd from buff. Return the number of bytes
// actually written, which may be less than num only when
// write(fd,..,..) returns <=0 at some point.
//
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

