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
  Revision 1.19  1997/12/12 05:49:46  weaver
  *** empty log message ***

  Revision 1.18  1997/12/08 18:17:59  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_4 tag
  into the trunk (split of libcs into libdevcs and libdevwin).

  Revision 1.17.4.5  1997/12/30 16:47:57  wenger
  Removed single-client compile option to simplify the code.

  Revision 1.17.4.4  1997/12/29 22:12:41  wenger
  Got DEVise 1.4.7 to compile, link, and run on SGI.

  Revision 1.17.4.3  1997/12/17 17:30:12  wenger
  Got cslib to compile for Linux.

  Revision 1.17.4.2  1997/12/09 16:03:57  wenger
  Devise client now uses client/server library.

  Revision 1.17.4.1  1997/12/06 17:43:02  wenger
  Split libcs.a into libdevcs.a and libdevwin.a in preparation for
  incorporating client/server part into DEVise.

  Revision 1.17  1997/08/07 19:42:55  wenger
  Got this to compile for sun.

  Revision 1.16  1997/08/07 19:28:25  wenger
  Get this to compile on hpux.

  Revision 1.15  1997/05/21 21:05:13  andyt
  Support for multiple clients in client-server library. Single-client mode
  still supported by compiling with -DSINGLE_CLIENT. Client-server library
  can now display EmbeddedTk windows and Tasvir images. Added a sample
  Embedded-Tk script and GIF image to the repository (ETkSample.tcl,
  earth.gif). Modified tarcslib script to include all new files.

  Revision 1.14  1997/04/21 15:01:48  wenger
  Added DrawRubberband() virtual function to WindowRep class, so that it
  doesn't have to be called directly through the XWindowRep class;
  cleaned out unnecessary includes from the cslib example server code and
  distribution.

  Revision 1.13  1997/04/15 18:32:27  wenger
  Added more line width test code to cslib server program.

  Revision 1.12  1997/04/11 18:48:34  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.11  1997/03/25 17:58:10  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.10.4.1  1997/03/15 00:30:49  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.10  1997/01/29 17:02:25  wenger
  Fixed PSWindowRep::ScaledText().

  Revision 1.9  1997/01/28 19:46:18  wenger
  Fixed bug 139; better testing of ScaledText() in client/server example;
  fixes to Exit class for client/server library.

  Revision 1.8  1997/01/17 20:31:29  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.7  1996/12/27 17:34:49  wenger
  Fixed some problems with XWindowRep::ScaledText(); added some more stuff
  to the cslib example server.

  Revision 1.6  1996/12/20 18:43:20  wenger
  Unfilled arcs and variable line widths now work in PostScript output.

  Revision 1.5  1996/12/11 18:05:46  wenger
  Arc() method now works in PSWindowRep class; put SetSmallFont() method
  back into WindowRep classes for backwards compatibility for Opossum;
  server example program tests more of the WindowRep methods than before;
  removed 'not yet implemented' warnings from some PSWindowRep methods.

  Revision 1.4  1996/12/04 18:12:31  wenger
  Unimplemented methods in PSWindowRep now report an error but do not
  abort when called; fixed code in cslib server example that caused problems
  for Anand; corrected the values returned by some of the NumShapeAttrs()
  methods in MapInterpShape.

  Revision 1.3  1996/11/26 15:43:54  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.2  1996/11/23 00:24:02  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.1  1996/10/17 20:42:17  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(HPUX) || defined(SUN)
#include <sys/unistd.h>	// for getcwd()
#endif
#if defined(LINUX)
#include <unistd.h>	// for getcwd()
#endif

#include "WinServer.h"
#include "ClientAPI.h"
#include "DualWindowRep.h"

static char *_progName = 0;


class SampleWinServer : public WinServer {
  public:  
    SampleWinServer(char *name, int port, int maxClients = 10)
	: WinServer(name, port, maxClients)
	{}
    
    ~SampleWinServer()
    {
	assert(_screenDisp);
    }
    
  protected:
    virtual void ProcessCmd(ClientID clientID, int argc, char **argv);
    
    virtual void BeginConnection(ClientID clientID)
    {
	if (_numClients > 1)
	{
	    return;
	}
	WindowRep *win;
	assert(_screenDisp);
	
	if (!_winReps.GetWindowRep())
	{
	    Coord x = 400.0;
	    Coord y = 100.0;
	    Coord width = 400.0;
	    Coord height = 400.0;

		PColorID	fgid = GetColorID(blackColor);
		PColorID	bgid = GetColorID(whiteColor);
	    WindowRep *parent = NULL;
	    Coord minWidth = 10.0;
	    Coord minHeight = 10.0;
	    Boolean relative = false;
	    Boolean winBoundary = true;
	    
	    win = _screenDisp->CreateWindowRep(_name, x, y, width, height,
										   parent, minWidth,
										   minHeight, relative, winBoundary);
	    assert(win);
	    win->RegisterCallback(this);
	    _winReps.SetScreenWinRep(win);
	    
	    win = _fileDisp->CreateWindowRep(_name, x, y, width, height,
										 parent, minWidth,
										 minHeight, relative, winBoundary);
	    _winReps.SetFileWinRep(win); 
	    _winReps.SetScreenOutput();
	    
	    Redraw();
	}
    }
    
		virtual void Redraw()
		{
			int				x, y;
			unsigned int	w, h;
			PColorID		fgid = GetColorID(blackColor);

	assert(_winReps.GetWindowRep());
	_winReps.GetWindowRep()->Origin(x, y);
	_winReps.GetWindowRep()->Dimensions(w, h);
	
	/* outline the whole window */
	_winReps.GetWindowRep()->SetForeground(fgid);
	_winReps.GetWindowRep()->Line(x, y, x + w - 1, y, 1);
	_winReps.GetWindowRep()->Line(x + w - 1, y, x + w - 1, y + h - 1, 1);
	_winReps.GetWindowRep()->Line(x + w - 1, y + h - 1, x, y + h - 1, 1);
	_winReps.GetWindowRep()->Line(x, y + h - 1, x, y, 1);
	
	int dashes[] = {10, 10, 5, 5};
	int dashCount = sizeof(dashes) / sizeof(dashes[0]);
	_winReps.GetWindowRep()->SetDashes(dashCount, dashes, 0);
	_winReps.GetWindowRep()->Line(x, y, x + w - 1, y + h - 1, 1);
	_winReps.GetWindowRep()->SetDashes(0, NULL, 0);
	
	/* get name of current directory */
	char current_dir[FILENAME_MAX + 1];
	if (getcwd(current_dir, FILENAME_MAX + 1) == NULL)
	{
	    perror("getcwd() failed");
	    exit(1);
	}

#if !defined(SGI)
	/* create Embedded Tk window */
	if (_winReps.GetWindowRep()->ETk_WindowCount() == 0)
	{
	    int dummy;
	    char etk_script[FILENAME_MAX + 1];
	    char *etk_argv[] = { "ABC", "123", "Do Re Mi" };
	    sprintf(etk_script, "%s/ETkSample.tcl", current_dir);
	    _winReps.GetWindowRep()->ETk_CreateWindow(w/2.0, h/2.0 + h/8.0,
						      w/2.0 - 4.0, h/8.0,
						      ETkIfc::Center,
						      etk_script,
						      3, etk_argv,
						      dummy);
	}
	else
	{
	    ETkInfo *info;
	    int iter = _winReps.GetWindowRep()->ETk_InitIterator();
	    while (_winReps.GetWindowRep()->ETk_More(iter))
	    {
		info = _winReps.GetWindowRep()->ETk_Next(iter);
		_winReps.GetWindowRep()->ETk_MoveResizeWindow(info->handle,
							      w/2.0,
							      h/2.0 + h/8.0,
							      w/2.0 - 4.0,
							      h/8.0);
	    }
	    _winReps.GetWindowRep()->ETk_DoneIterator(iter);
	}
	
	/* draw a Tasvir image */
	_winReps.GetWindowRep()->DaliFreeImages();
	char image_name[FILENAME_MAX + 1];
	sprintf(image_name, "%s/earth.gif", current_dir);
	_winReps.GetWindowRep()->DaliShowImage(w/2.0 + w/4.0 + w/8.0, h/2.0,
					       w/4.0 - 4.0, h/4.0 - 4.0,
					       image_name,
					       0, NULL);
#endif
	
	/* draw rubberband line */
	_winReps.GetWindowRep()->DrawRubberband(10, 10, 100, 100);
	
	/* use color from local colormap (see Color.h) */
//	_winReps.GetWindowRep()->SetFgColor(SeaGreenColor);

//	float	red, green, blue;
//	_winReps.GetWindowRep()->GetFgRGB(red, green, blue);
//	printf("SeaGreenColor RGB: %f %f %f\n", red, green, blue);
	_winReps.GetWindowRep()->FillRect(x + 0.25 * w, y + 0.25 * h, 0.5 * w,
					  0.5 * h);
	
	_winReps.GetWindowRep()->SetXorMode();
	_winReps.GetWindowRep()->FillRect(x + 0.15 * w, y + 0.6 * h, 0.3 * w,
					  0.3 * h);
	_winReps.GetWindowRep()->SetCopyMode();
	
//	_winReps.GetWindowRep()->SetFgColor(BlackColor);
//	_winReps.GetWindowRep()->SetBgColor(SlateBlueColor);
//	_winReps.GetWindowRep()->GetFgRGB(red, green, blue);
//	printf("BlackColor RGB: %f %f %f\n", red, green, blue);
	_winReps.GetWindowRep()->SetNormalFont();
	_winReps.GetWindowRep()->AbsoluteText("Server Display", x, y, w, h,
					      WindowRep::AlignCenter, false);
	_winReps.GetWindowRep()->SetSmallFont();
	_winReps.GetWindowRep()->AbsoluteText("AlignWest", x, y, w, h,
					      WindowRep::AlignWest, false);
	
//	_winReps.GetWindowRep()->SetFgColor(GoldenRodColor);
	_winReps.GetWindowRep()->FillRect(x, y + h/4, w/3, h/45);
//	_winReps.GetWindowRep()->SetFgColor(BlackColor);
//	_winReps.GetWindowRep()->SetBgColor(PurpleColor);
	_winReps.GetWindowRep()->ScaledText("Scaled Text", x, y + h/4,
					    w/3, h/45,
					    WindowRep::AlignCenter, false);
	
//	_winReps.GetWindowRep()->SetFgColor(GoldenRodColor);
	_winReps.GetWindowRep()->FillRect(x + w/2, y + h/4, w/2, h/8);
//	_winReps.GetWindowRep()->SetFgColor(RedColor);
//	_winReps.GetWindowRep()->SetBgColor(DarkSeaGreen);
	_winReps.GetWindowRep()->ScaledText("abcdefg", x + w/2, y + h/4,
					    w/2, h/8,
					    WindowRep::AlignNorthEast, false);
	
	/* bypass local colormap */
	for(float i = 0; i < 1.0; i += 0.1) {
//	    _winReps.GetWindowRep()->SetFgRGB(1 - i, 0, 0);
	    _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.80 * h, 0.1 * w,
					      0.05 * h);
//	    _winReps.GetWindowRep()->SetFgRGB(0, i, 0);
	    _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.85 * h, 0.1 * w,
					      0.05 * h);
//	    _winReps.GetWindowRep()->SetFgRGB(0, 0, 1 - i);
	    _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.90 * h, 0.1 * w,
					      0.05 * h);
//	    _winReps.GetWindowRep()->SetFgRGB(i, i, i);
	    _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.95 * h, 0.1 * w,
					      0.05 * h);
	}
	
	/* test the Arc() and Pattern() functions */
//	_winReps.GetWindowRep()->SetFgRGB(0.0, 0.0, 1.0);
//	_winReps.GetWindowRep()->SetBgRGB(1.0, 0.0, 0.0);
	
	Coord xCenter1 = x + 0.25 * w;
	Coord yCenter1 =  y + 0.12 * h;
	Coord horizDiam = 0.2 * w;
	Coord vertDiam = 0.2 * h;
	_winReps.GetWindowRep()->SetPattern(Pattern10);
	_winReps.GetWindowRep()->Arc(xCenter1, yCenter1, horizDiam, vertDiam,
				     0.5, 2.8);
	
	Coord xCenter2 = x + 0.75 * w;
	Coord yCenter2 = y + 0.12 * h;
	horizDiam = 0.3 * w;
	vertDiam = 0.2 * h;
	_winReps.GetWindowRep()->SetPattern((Pattern) -Pattern1);
	_winReps.GetWindowRep()->SetLineWidth(2);
	_winReps.GetWindowRep()->Arc(xCenter2, yCenter2, horizDiam, vertDiam,
				     0.0, 6.3);
	_winReps.GetWindowRep()->SetLineWidth(0);
	_winReps.GetWindowRep()->SetPattern(Pattern0);
	
//	_winReps.GetWindowRep()->SetFgRGB(0.0, 0.0, 0.0);
	
	Coord size = 0.03;
	Coord lineWidth = 2.0;
	_winReps.GetWindowRep()->Line(xCenter1 - size * w, yCenter1,
				      xCenter1 + size * w, yCenter1,
				      lineWidth);
	_winReps.GetWindowRep()->Line(xCenter1, yCenter1 - size * h, xCenter1,
				      yCenter1 + size * h, lineWidth);
	
	_winReps.GetWindowRep()->Line(xCenter2 - size * w, yCenter2,
				      xCenter2 + size * w, yCenter2,
				      lineWidth);
	_winReps.GetWindowRep()->Line(xCenter2, yCenter2 - size * h, xCenter2,
				      yCenter2 + size * h, lineWidth);
	
	/* Note: we should add code to test the following functions:
	 * FillPoly FillPixelPoly DrawPixel DrawPixelArray FillPixelRect Line
	 * AbsoluteLine SetOrMode SetFont
	 * SetNormalFont SetDaliServer DaliShowImage DaliFreeImages. */
	
	/* make sure everything gets drawn to the screen */
	_winReps.GetWindowRep()->Flush();
    }
    
    virtual void EndConnection(ClientID clientID)
    {
	if (_numClients > 1)
	{
	    return;
	}
	assert(_winReps.GetWindowRep() && _screenDisp);
	_screenDisp->DestroyWindowRep(_winReps.GetScreenWinRep());
	_fileDisp->DestroyWindowRep(_winReps.GetFileWinRep());
	_winReps.SetScreenWinRep(NULL);
	_winReps.SetFileWinRep(NULL);
    }
    
    virtual void HandleButton(WindowRep *w, int x, int y,
			      int button, int state, int type) {
	printf("Button %d, state %d, type %d, x %d, y %d\n",
	       button, state, type, x, y);
    }
    
    virtual void HandleResize(WindowRep *w, int xlow,
			      int ylow, unsigned width,
			      unsigned height) {
	Redraw();
        SendControl(API_CTL, "puts \"Handling resize.\"");
    }
    
    DualWindowRep _winReps;	// note: _not_ a pointer to a DualWindowRep
    
  private:
    virtual void Print();

};

void SampleWinServer::ProcessCmd(ClientID clientID, int argc, char **argv)
{
    // This function should act upon a client command (stored in argv)
    // and then return a positive acknowledgement by calling
    // ReturnVal() with API_ACK or a negative acknowledgement by
    // calling ReturnVal() with API_NAK.
    
    // This sample server recognizes only commands that begin with
    // the "file" or "edit" keywords, and the "print" and "exit" commands.
    
    GetCmd(argc, argv);
    
    int error = 0;
    char *errMsg = 0;
    
    if (argc >= 1) {
	if (!strcmp(argv[0], "file"))
	    printf("Received file command from client: \"%s\"\n", _cmd);
	else if (!strcmp(argv[0], "edit"))
	    printf("Received edit command from client: \"%s\"\n", _cmd);
	else if (!strcmp(argv[0], "exit"))
	    printf("Received exit command from client\n");
	else if (!strcmp(argv[0], "print"))
	    Print();
	else {
	    printf("Received unrecognized command from client: \"%s\"\n", _cmd);
	    errMsg = "Unrecognized command";
	    error = 1;
	}
    }
    else
    {
	printf("Received null command from client\n");
	errMsg = "Null command";
	error = 1;
    }
    
    delete _cmd;
    
    int result;
    char *arg;
    bool doClose = false;
    
    if (error) {
	arg = errMsg;
    } else {
	arg = "done";
    }
    
    result = ReturnVal(clientID, error ? API_NAK : API_ACK, arg);

    if (result < 0) {
	doClose = true;
    } else if (argc == 1 && !strcmp(argv[0], "exit")) {
	doClose = true;
    }
    
    if (doClose) {
	CloseClient(clientID);
    }
}

void SampleWinServer::Print()
{
    printf("Received print command from client\n");
    
    /* Figure out the screen geometry of what we want to print. */
    int xVal, yVal;
    unsigned int width, height;
    _winReps.GetScreenWinRep()->Origin(xVal, yVal);
    _winReps.GetScreenWinRep()->Dimensions(width, height);
    
    Rectangle printRegion;
    printRegion.x = xVal;
    printRegion.y = yVal;
    printRegion.width = width;
    printRegion.height = height;
    
    /* Open the print file. */
    char *filename = "/tmp/client_server.0.ps";
    DevStatus status = _fileDisp->OpenPrintFile(filename);
    assert(status.IsComplete());
    _fileDisp->PrintPSHeader("cslib sample server", printRegion, true);
    
    /* Switch over to file output.  Since we're printing a single window,
     * the window geometry and the print region geometry are the same. */
    Rectangle windowGeom = printRegion;
    _winReps.SetFileOutput(windowGeom, printRegion);
    
    /* Redraw the window. */
    Redraw();
    
    /* Switch back to screen output. */
    _winReps.SetScreenOutput();
    
    /* Close the print file. */
    _fileDisp->PrintPSTrailer();
    _fileDisp->ClosePrintFile();
    
    printf("Output in file '%s'\n", filename);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Usage()
{
    fprintf(stderr, "Usage: %s [-p port]\n", _progName);
}

int main(int argc, char **argv)
{
    _progName = argv[0];
    
    int port = DefaultNetworkPort;
    
    for(int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-p")) {
	    if (i + 1 >= argc) {
		fprintf(stderr, "No port number specified with -p option.\n");
		Usage();
		exit(1);
	    }
	    port = atoi(argv[i + 1]);
	    i++;
	} else {
	    Usage();
	    exit(1);
	}
    }
    
    SampleWinServer server("Server", port);
    server.MainLoop();
    
    return 1;
}

