
#include "TkWindowRep.h"
#include "EmbeddedTk.h"
#include "ETkCompDate.h"
#include "BrowserCommands.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

#if defined(SunOS)
  extern "C" int socket(int, int, int);
  extern "C" int bind(int, struct sockaddr *, int);
  extern "C" int listen(int, int);
  extern "C" int accept(int, struct sockaddr *, int *);
  extern "C" int connect(int, struct sockaddr *, int);
  extern "C" int shutdown(int, int);
  extern "C" int setsockopt(int, int, int, char *, int);
#endif

extern "C"
{
    char *strdup(const char *s1);
}

#define DELIMITERS " \t"

typedef void Tk_FileProc(ClientData clientData, int mask);

static int     g_LastHandle = 0;
static int     g_ReplySent;
static FILE*   g_LogFile;

static char TheCommand[ETK_MAX_STRLEN];
static char TheReply[ETK_MAX_STRLEN];
Tk_FileProc ServiceConnectionRequest;

/********************************************************
   read a \n-terminated line from fd into s, replacing
   \n by \0. do not read more than maxc-1 characters
   return the number of characters read (excluding \n). 
*********************************************************/
static int
newlinefd(char *s, int fd, int maxc)
{
    int i, lim = maxc-1;
    char c;
    i = 0;
    while ((i < lim) && (read(fd, &c, 1) == 1))
    {
	if (c == '\n')
	{
	    break;
	}
	s[i++] = c;
    }
    
    s[i] = '\0';

#ifdef DEBUG
    fprintf(g_LogFile, "Read a line on fd %d: %s\n", fd, s);
#endif

    return i;
    
}

/******************************************************
  Read num bytes from fd into buff. Return
  # bytes actually read, which may be less only when
  EOF is prematurely reached.
*******************************************************/ 
static int
MyRead(int fd, unsigned char *buff, int num)
{
    unsigned char *temp;
    int tocome = num;
    int thistime;
    
    temp = buff;
    
    while (tocome > 0)
    {
	thistime = read(fd,temp,tocome);
	if (thistime <= 0) return(num-tocome);
	tocome -= thistime;
	temp = temp + thistime;
    }
    
    return(num);
    
}

/******************************************************
  Write num bytes into fd from buff. Return
  # bytes actually written, which may be less only when
  write(fd,..,..) returns <=0 at some point. 
*********************************************************/ 
static int
MyWrite(int fd, unsigned char *buff, int num)
{
    unsigned char *temp;
    int tocome = num;
    int thistime;
    
    temp = buff;
    
    while (tocome > 0)
    {
	thistime = write(fd, temp, tocome);
	if (thistime <= 0)
	    return(num-tocome);
	tocome -= thistime;
	temp = temp + thistime;
    }
    
    return(num);

}

static int
NewHandle(void)
{
    if (ETk_NumWindows == ETk_MaxWindows)
	return -1;
    while (ETk_Windows[g_LastHandle].in_use)
    {
	g_LastHandle++;
	if (g_LastHandle >= ETk_MaxWindows)
	    g_LastHandle = 0;
    }
    return g_LastHandle;
}

// send reply and close connection
static void
WriteReply(int ok, const char *reply, int fd)
{
    int len;
    char big_reply[ETK_MAX_STRLEN];
    
    sprintf(big_reply, "%s %s\n", ok ? "OK" : "ERROR", reply);
    
#ifdef DEBUG
    fprintf(g_LogFile, "Sending reply on fd %d: %s", fd, big_reply);
#endif

    if (g_ReplySent)
    {
#ifdef DEBUG
	fprintf(g_LogFile, 
		"Too late to send reply to fd = %d\n  Reply was: %s",
		fd, big_reply);
	fprintf(g_LogFile, "Closing fd %d\n", fd);
#endif
	close(fd);
	return;
    }
    
    g_ReplySent = 1;
    
    if ((len = strlen(big_reply)) > ETK_MAX_STRLEN)
    {
	len = ETK_MAX_STRLEN;
	// should not really come here 
    }
    
    if (MyWrite(fd, (unsigned char*) big_reply, len) != len)
    {
#ifdef DEBUG
	fprintf(g_LogFile, 
		"Failed to send part of reply to fd = %d\n  Reply was: %s",
		fd, big_reply);
#endif
    }

#ifdef DEBUG
    fprintf(g_LogFile, "Closing fd %d\n", fd);
#endif
    close(fd);
    
}

static void
ProcessShow(char *cmd, int fd)
{
    int argc;
    char **argv;
    char *nextarg;
    char *filename;
    Window parent;
    int x, y, width, height;
    int i;
    int handle;
    TkWindowRep* w;
    char *anchor;
        
    // Show command: show file window x y width height anchor argc
    // Then each arg goes on a separate line

    if (ETk_NumWindows >= ETk_MaxWindows)
    {
	WriteReply(0, "Too many Tk windows: show failed", fd);
	return;
    }
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get file name 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No script given: show failed", fd);
	return;
    }
    filename = strdup(nextarg);
    
    // get window 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No XWindow given: show failed", fd);
	return;
    }
    parent = (Window) strtol(nextarg, (char **) 0, 16);
    /*
    if (!parent)
    {
	WriteReply(0, "Invalid XWindow given: show failed", fd);
	return;
    }
    */
    
    // get coordinates 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "Missing coordinate(s): show failed", fd);
	return;
    }
    x = atoi(nextarg);
    
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "Missing coordinate(s): show failed", fd);
	return;
    }
    y = atoi(nextarg);
    
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "Missing coordinate(s): show failed", fd);
	return;
    }
    width = atoi(nextarg);
    
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "Missing coordinate(s): show failed", fd);
	return;
    }
    height = atoi(nextarg);
    
    // get anchor
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	anchor = "c";
    }
    else if (!strcmp(nextarg, "nw"))
    {
	anchor = "nw";
    }
    else
    {
	anchor = "c";
    }
    
    // get argc 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No argument count given: show failed", fd);
	return;
    }
    argc = atoi(nextarg);
    if (argc < 0)
    {
	WriteReply(0, "Invalid argument count given: show failed", fd);
	return;
    }
    
    if (argc > 0)
    {
	argv = new char*[argc];
	if (argv == NULL)
	{
	    WriteReply(0, "Out of memory", fd);
	    return;
	}
    }
    else
    {
	argv = NULL;
    }
    
    for (i = 0; i < argc; i++)
    {
	newlinefd(TheCommand, fd, ETK_MAX_STRLEN);
	argv[i] = strdup(TheCommand);
    }
    
    ETk_Status s;
    
    handle = NewHandle();    
    s = TkWindowRep::CreateTkWindowRep(parent, filename, argc, argv,
				       x, y, width, height, anchor,
				       handle, w);
    //
    // Free memory
    //
    free(filename);
    for (i = 0; i < argc; i++)
    {
	free(argv[i]);
    }
    if (argc > 0)
	free(argv);

    if (s != ETk_OK)
    {
	sprintf(TheReply, "%s",
		(s == ETk_TclError ? ETk_TclResult : ETk_StatusToString(s)));
	WriteReply(0, TheReply, fd);
	return;
    }
    
    ETk_Windows[handle].win = w;
    ETk_Windows[handle].in_use = 1;
    ETk_Windows[handle].script = new char [strlen(filename) + 1];
    strcpy(ETk_Windows[handle].script, filename);
    ETk_NumWindows++;
    
    sprintf(TheReply, "%d %d %d %d %d", handle, x, y, width, height);
    WriteReply(1, TheReply, fd);
    
#ifdef DEBUG
    fprintf(g_LogFile, "Done with ProcessShow() on fd %d\n", fd);
#endif
    
}

static void
ProcessEval(char *cmd, int fd)
{
    int argc;
    char **argv;
    char *nextarg;
    Window parent;
    int i;
    int handle;
    TkWindowRep* w;
    int cmdLen = 0;
    
    //
    // Eval command: eval handle argc
    // Then each arg goes on a separate line
    //
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get handle name 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No handle given: eval failed", fd);
	return;
    }
    handle = atoi(nextarg);
    if ((handle < 0) || (handle >= ETk_MaxWindows) ||
	!ETk_Windows[handle].in_use)
    {
	WriteReply(0, "Invalid handle given: eval failed", fd);
	return;
    }
    
    // get argc 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No argument count given: eval failed", fd);
	return;
    }
    argc = atoi(nextarg);
    if (argc <= 0)
    {
	WriteReply(0, "Invalid argument count given: eval failed", fd);
	return;
    }

    //
    // Read the argv array, one element on each line
    //
    int result = 1;
    argv = new char *[argc];
    if (argv == NULL)
    {
	WriteReply(0, "Out of memory", fd);
	return;
    }
    for (i = 0; i < argc; i++)
    {
	newlinefd(TheCommand, fd, ETK_MAX_STRLEN);
	argv[i] = strdup(TheCommand);
	cmdLen += (strlen(argv[i]) + 1);
    }
    
    //
    // Build a big command from the argv array
    //
    char *bigCmd = new char[cmdLen];
    bigCmd[0] = '\0';
    if (argv == NULL)
    {
	sprintf(TheReply, "Out of memory");
	result = 0;
    }
    else
    {
	for (i = 0; i < argc; i++)
	{
	    if (i > 0)
	    {
		strcat(bigCmd, " ");
	    }
	    strcat(bigCmd, argv[i]);
	}
    }

    //
    // Free memory
    //
    for (i = 0; i < argc; i++)
    {
	free(argv[i]);
    }
    if (argc > 0)
    {
	free(argv);
    }

    if (result)
    {
	//
	// Eval
	//
	ETk_Status s = ETk_EvalCmd(handle, bigCmd, TheReply);
	if (s != ETk_OK)
	{
	    result = 0;
	}
    }
    
    WriteReply(result, TheReply, fd);
    
    free(bigCmd);

#ifdef DEBUG
    fprintf(g_LogFile, "Done with ProcessEval() on fd %d\n", fd);
#endif
    
}

static void
ProcessFree(char *cmd, int fd)
{
    char *nextarg;
    int handle;
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get handle name 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No handle given: free failed", fd);
	return;
    }
    handle = atoi(nextarg);
    
    if ((handle < 0) || (handle >= ETk_MaxWindows) ||
	!ETk_Windows[handle].in_use)
    {
	WriteReply(0, "Invalid handle given: free failed", fd);
	return;
    }
    
    WriteReply(1, "", fd);
    
    ETk_FreeWindow(handle);
    
}

static void
ProcessMap(char *cmd, int fd)
{
    char *nextarg;
    int handle;
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get handle name 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No handle given: map failed", fd);
	return;
    }
    handle = atoi(nextarg);
    
    if ((handle < 0) || (handle >= ETk_MaxWindows) ||
	!ETk_Windows[handle].in_use)
    {
	WriteReply(0, "Invalid handle given: map failed", fd);
	return;
    }
    
    ETk_Status s = ETk_MapWindow(handle);
    
    if (s != ETk_OK)
    {
	sprintf(TheReply, "Error mapping window: %s", ETk_StatusToString(s));
	WriteReply(0, TheReply, fd);
	return;
    }
    
    WriteReply(1, "", fd);
    
}

static void
ProcessUnmap(char *cmd, int fd)
{
    char *nextarg;
    int handle;
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get handle name 
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No handle given: unmap failed", fd);
	return;
    }
    handle = atoi(nextarg);
    
    if ((handle < 0) || (handle >= ETk_MaxWindows) ||
	!ETk_Windows[handle].in_use)
    {
	WriteReply(0, "Invalid handle given: unmap failed", fd);
	return;
    }
    
    ETk_Status s = ETk_UnmapWindow(handle);
    
    if (s != ETk_OK)
    {
	sprintf(TheReply, "Error unmapping window: %s", ETk_StatusToString(s));
	WriteReply(0, TheReply, fd);
	return;
    }
    
    WriteReply(1, "", fd);
    
}

static void
ProcessMove(char *cmd, int fd)
{
    char *nextarg;
    int handle, xcenter, ycenter;
    
    // cmd format: move handle xcenter ycenter
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get handle name
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No handle given: move failed", fd);
	return;
    }
    handle = atoi(nextarg);
    
    // get x
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No X-coordinate given: move failed", fd);
	return;
    }
    xcenter = atoi(nextarg);
    
    // get y
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No Y-coordinate given: move failed", fd);
	return;
    }
    ycenter = atoi(nextarg);
    
    if ((handle < 0) || (handle >= ETk_MaxWindows) ||
	!ETk_Windows[handle].in_use)
    {
	WriteReply(0, "Invalid handle given: move failed", fd);
	return;
    }
    
    ETk_Status s = ETk_MoveWindow(handle, xcenter, ycenter);
    
    if (s != ETk_OK)
    {
	sprintf(TheReply, "Error moving window: %s", ETk_StatusToString(s));
	WriteReply(0, TheReply, fd);
	return;
    }
    
    WriteReply(1, "", fd);
    
}

static void
ProcessResize(char *cmd, int fd)
{
    char *nextarg;
    int handle, width, height;
    
    // cmd format: resize handle width height
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get handle name
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No handle given: resize failed", fd);
	return;
    }
    handle = atoi(nextarg);
    
    // get width
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No width specified: resize failed", fd);
	return;
    }
    width = atoi(nextarg);
    
    // get height
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No height specified: resize failed", fd);
	return;
    }
    height = atoi(nextarg);
    
    if ((handle < 0) || (handle >= ETk_MaxWindows) ||
	!ETk_Windows[handle].in_use)
    {
	WriteReply(0, "Invalid handle given: resize failed", fd);
	return;
    }
    
    ETk_Status s = ETk_ResizeWindow(handle, width, height);
    
    if (s != ETk_OK)
    {
	sprintf(TheReply, "Error resizing window: %s", ETk_StatusToString(s));
	WriteReply(0, TheReply, fd);
	return;
    }
    
    WriteReply(1, "", fd);
    
}

static void
ProcessMoveResize(char *cmd, int fd)
{
    char *nextarg;
    int handle, xCenter, yCenter, width, height;
    
    // cmd format: resize handle xCenter yCenter width height
    
    nextarg = strtok(cmd, DELIMITERS);
    
    // get handle name
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No handle given: MoveResize failed", fd);
	return;
    }
    handle = atoi(nextarg);
    
    // get x
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No X-coordinate specified: MoveResize failed", fd);
	return;
    }
    xCenter = atoi(nextarg);
    
    // get y
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No Y-coordinate specified: MoveResize failed", fd);
	return;
    }
    yCenter = atoi(nextarg);
    
    // get width
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No width specified: MoveResize failed", fd);
	return;
    }
    width = atoi(nextarg);
    
    // get height
    nextarg = strtok(0, DELIMITERS);
    if (!nextarg)
    {
	WriteReply(0, "No height specified: MoveResize failed", fd);
	return;
    }
    height = atoi(nextarg);
    
    if ((handle < 0) || (handle >= ETk_MaxWindows) ||
	!ETk_Windows[handle].in_use)
    {
	WriteReply(0, "Invalid handle given: MoveResize failed", fd);
	return;
    }
    
    ETk_Status s = ETk_MoveResizeWindow(handle, xCenter, yCenter,
					width, height);
    
    if (s != ETk_OK)
    {
	sprintf(TheReply, "Error resizing window: %s", ETk_StatusToString(s));
	WriteReply(0, TheReply, fd);
	return;
    }
    
    WriteReply(1, "", fd);
    
}

static void
ProcessStatus(char *cmd, int fd)
{
    WriteReply(1, "", fd);
}

static void
ProcessQuit(char *cmd, int fd)
{
    WriteReply(1, "", fd);
}

static int
ProcessClientRequest(int fd)
{
    newlinefd(TheCommand, fd, ETK_MAX_STRLEN);
    
#ifdef DEBUG
    fprintf(g_LogFile, "Processing: %s\n", TheCommand);
#endif
    
    if (!strncmp(TheCommand, "eval", 4))
    {
	ProcessEval(TheCommand, fd);
    }
    else if (!strncmp(TheCommand, "free", 4))
    {
	ProcessFree(TheCommand, fd);
    }
    else if (!strncmp(TheCommand, "map", 3))
    {
	ProcessMap(TheCommand, fd);
    }
    //
    // WARNING: make sure "moveresize" is tested before "move". More 
    // generally, any prefix must be tested *after* the longer word.
    //
    else if (!strncmp(TheCommand, "moveresize", 10))
    {
	ProcessMoveResize(TheCommand, fd);
    }
    else if (!strncmp(TheCommand, "move", 4))
    {
	ProcessMove(TheCommand, fd);
    }
    else if (!strncmp(TheCommand, "quit", 4))
    {
	ProcessQuit(TheCommand, fd);
	fprintf(stderr, "EmbeddedTk server exiting\n");
	exit(0);
    }
    else if (!strncmp(TheCommand, "resize", 6))
    {
	ProcessResize(TheCommand, fd);
    }
    else if (!strncmp(TheCommand, "show", 4))
    {
	ProcessShow(TheCommand, fd);
    }
    else if (!strncmp(TheCommand, "status", 4))
    {
	ProcessStatus(TheCommand, fd);
    }
    else if (!strncmp(TheCommand, "unmap", 5))
    {
	ProcessUnmap(TheCommand, fd);
    }
    else
    {
	WriteReply(0, "Unknown command", fd);
    }
    return(1);
}

void
ServiceConnectionRequest(ClientData clientData, int mask)
{
    int fd = (int) clientData;
    mask = mask; // unused
    
    struct sockaddr_in tempAddr;
    int done = 0;
    int len = sizeof(struct sockaddr_in);

#ifdef DEBUG
    fprintf(g_LogFile, "\nWaiting for connection request\n");
#endif
    
    int socketFd = accept(fd, (struct sockaddr *)&tempAddr, &len);

    if (socketFd >= 0)
    {
#ifdef DEBUG
	fprintf(g_LogFile, "Connection request arrived\n");
#endif
	g_ReplySent = 0;
	ProcessClientRequest(socketFd);
    }

    else
    {
#ifdef DEBUG
	fprintf(g_LogFile, "Error in accept: %s\n", strerror(errno));
#endif
    }
    
}

int
main(int argc, char *argv[]) 
{
    int listenFd, socketFd;
    int enable, result;
    struct sockaddr_in servAddr;
    struct sockaddr_in tempAddr;
    fd_set *fdset;
    int len, n;
    char *display = NULL;
    ETk_Status status;
    int vclass;
    int port = ETK_PORT;
    
    char *executable = argv[0];
    char *last_slash;
    if ((last_slash = strrchr(executable, '/')) != NULL)
    {
	executable = last_slash + 1;
    }
    
#ifdef DEBUG
    g_LogFile = stderr;
#endif
    
    n = 1;
    while (n < argc)
    {
	if (!strcmp(argv[n], "-display"))
	{
	    n++;
	    if (n >= argc)
	    {
		fprintf(stderr, "No display specified\n");
		exit(1);
	    }
	    display = argv[n];
	}
	else if (!strcmp(argv[n], "-port"))
	{
	    n++;
	    if (n >= argc)
	    {
		fprintf(stderr, "No port number specified\n");
		exit(1);
	    }
	    port = atoi(argv[n]);
	}
	
	else
	{
	    fprintf(stderr,
		    "Usage: %s [-display name] [-port n]\n",
		    executable);
	    exit(1);
	}
	n++;
    }

    // one-time initialization of browser stuff
    BrowserInit();


    // Create a socket
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
	fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
	exit(1);
    }
    
    // Allow listening port number to be reused
    enable = 1;
    result = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR,
			(char *)&enable, sizeof(int));
    if (result < 0)
    {
	fprintf(stderr, "Could not set socket options: %s\n",
		strerror(errno));
	exit(1);
    }
    
    // Bind socket to port
    memset(&servAddr, 0, sizeof(struct sockaddr_in));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    result = bind(listenFd, (struct sockaddr *)&servAddr,
		  sizeof(struct sockaddr));
    if (result < 0)
    {
	fprintf(stderr, "Could not bind socket: %s\n", strerror(errno));
	exit(1);
    }

    // Listen
    result = listen(listenFd, 5);
    if (result < 0)
    {
	fprintf(stderr, "listen() failed: %s\n", strerror(errno));
	exit(1);
    }
    ETk_ListenFD = listenFd;

    status = ETk_Init(display, g_LogFile);
    if (status != ETk_OK)
    {
	fprintf(stderr, "Could not initialize ETk server: %s\n", 
		ETk_StatusToString(status));
	exit(1);
    }
    
#if TK_MAJOR_VERSION == 4
#  if TK_MINOR_VERSION == 0
    Tk_CreateFileHandler(listenFd, TK_READABLE,
			 ServiceConnectionRequest, (ClientData) listenFd);
#  else
    Tcl_CreateFileHandler(Tcl_GetFile((void *) listenFd, TCL_UNIX_FD),
			  TCL_READABLE,
			  ServiceConnectionRequest, (ClientData) listenFd);
#  endif
#else
    Tcl_CreateFileHandler(listenFd, TCL_READABLE,
      ServiceConnectionRequest, (ClientData) listenFd);
#endif
    
    fprintf(stderr, "-----------------------------------------------------\n");
    fprintf(stderr, " DEVise EmbeddedTk server\n");
    fprintf(stderr, " Compile date: %s\n", ETkCompDate::Get());
    fprintf(stderr, "-----------------------------------------------------\n");
    
    //
    // Main loop
    //
    while (1)
    {
	// Block until connection request arrives
	ServiceConnectionRequest((ClientData) listenFd, 0);
	// Let Tk handle everything until no more windows exist
	Tk_MainLoop();
    }
    
    //
    // Currently there is no way to reach this part of the code
    //
#if TK_MAJOR_VERSION == 4
#  if TK_MINOR_VERSION == 0
    Tk_DeleteFileHandler(listenFd);
#  else
    Tcl_DeleteFileHandler(Tcl_GetFile((void *) listenFd, TCL_UNIX_FD));
#  endif
#else
    Tcl_DeleteFileHandler(listenFd);
#endif
    close(listenFd);
    ETk_ShutDown();

    return 0;

}

