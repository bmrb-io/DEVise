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

#include "ClientServer.h"
#include "DualWindowRep.h"
#include "PSDisplay.h"

static char *_progName = 0;


class SampleWinServer : public WinServer {
 public:  
  SampleWinServer(char *name, int port) : WinServer(name, port) {
  }

  ~SampleWinServer() {
     assert(_screenDisp);
  }

 protected:
  virtual void ProcessCmd(int argc, char **argv);

  virtual void BeginConnection() {
    assert(_screenDisp && !_winReps.GetWindowRep());
    WindowRep *win;

    Coord x = 400.0;
    Coord y = 100.0;
    Coord width = 400.0;
    Coord height = 400.0;
    GlobalColor foreground = BlackColor;
    GlobalColor background = WhiteColor;
    WindowRep *parent = NULL;
    Coord minWidth = 10.0;
    Coord minHeight = 10.0;
    Boolean relative = false;
    Boolean winBoundary = true;

    win = _screenDisp->CreateWindowRep(_name, x, y, width, height, foreground,
				     background, parent, minWidth, minHeight,
				     relative, winBoundary);
    assert(win);
    win->RegisterCallback(this);
    _winReps.SetScreenWinRep(win);

    win = _fileDisp->CreateWindowRep(_name, x, y, width, height, foreground,
				     background, parent, minWidth, minHeight,
				     relative, winBoundary);
    _winReps.SetFileWinRep(win);

    _winReps.SetScreenOutput();

    Redraw();
  }

  virtual void Redraw() {
    int x, y;
    unsigned int w, h;
    assert(_winReps.GetWindowRep());
    _winReps.GetWindowRep()->Origin(x, y);
    _winReps.GetWindowRep()->Dimensions(w, h);

    /* use color from local colormap (see Color.h) */
    _winReps.GetWindowRep()->SetFgColor(SeaGreenColor);
    float red, green, blue;
    _winReps.GetWindowRep()->GetFgRGB(red, green, blue);
    printf("SeaGreenColor RGB: %f %f %f\n", red, green, blue);
    _winReps.GetWindowRep()->FillRect(x + 0.25 * w, y + 0.25 * h, 0.5 * w,
      0.5 * h);

    _winReps.GetWindowRep()->SetFgColor(BlackColor);
    _winReps.GetWindowRep()->SetBgColor(SlateBlueColor);
    _winReps.GetWindowRep()->GetFgRGB(red, green, blue);
    printf("BlackColor RGB: %f %f %f\n", red, green, blue);
    _winReps.GetWindowRep()->SetNormalFont();
    _winReps.GetWindowRep()->AbsoluteText("Server Display", x, y, w, h,
		       WindowRep::AlignCenter, false);
    _winReps.GetWindowRep()->SetSmallFont();
    _winReps.GetWindowRep()->AbsoluteText("AlignWest", x, y, w, h,
		       WindowRep::AlignWest, false);

    _winReps.GetWindowRep()->SetFgColor(GoldenRodColor);
      _winReps.GetWindowRep()->FillRect(x, y + h/4, w/3, h/8);
    _winReps.GetWindowRep()->SetFgColor(BlackColor);
    _winReps.GetWindowRep()->ScaledText("Scaled Text", x, y + h/4, w/3, h/8,
		       WindowRep::AlignCenter, false);

    _winReps.GetWindowRep()->SetFgColor(GoldenRodColor);
      _winReps.GetWindowRep()->FillRect(x + w/2, y + h/4, w/2, h/8);
    _winReps.GetWindowRep()->SetFgColor(RedColor);
    _winReps.GetWindowRep()->ScaledText("abcdefg", x + w/2, y + h/4, w/2,
		       h/8, WindowRep::AlignNorthEast, false);

    /* bypass local colormap */
    for(float i = 0; i < 1.0; i += 0.1) {
      _winReps.GetWindowRep()->SetFgRGB(1 - i, 0, 0);
      _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.80 * h, 0.1 * w,
	0.05 * h);
      _winReps.GetWindowRep()->SetFgRGB(0, i, 0);
      _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.85 * h, 0.1 * w,
	0.05 * h);
      _winReps.GetWindowRep()->SetFgRGB(0, 0, 1 - i);
      _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.90 * h, 0.1 * w,
	0.05 * h);
      _winReps.GetWindowRep()->SetFgRGB(i, i, i);
      _winReps.GetWindowRep()->FillRect(x + i * w, y + 0.95 * h, 0.1 * w,
	0.05 * h);
    }

    /* test the Arc() and Pattern() functions */
    _winReps.GetWindowRep()->SetFgRGB(0.0, 0.0, 1.0);
    _winReps.GetWindowRep()->SetBgRGB(1.0, 0.0, 0.0);

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
    _winReps.GetWindowRep()->SetPattern(Pattern0);

    _winReps.GetWindowRep()->SetFgRGB(0.0, 0.0, 0.0);

    Coord size = 0.03;
    Coord lineWidth = 2.0;
    _winReps.GetWindowRep()->Line(xCenter1 - size * w, yCenter1,
      xCenter1 + size * w, yCenter1, lineWidth);
    _winReps.GetWindowRep()->Line(xCenter1, yCenter1 - size * h, xCenter1,
      yCenter1 + size * h, lineWidth);

    _winReps.GetWindowRep()->Line(xCenter2 - size * w, yCenter2,
      xCenter2 + size * w, yCenter2, lineWidth);
    _winReps.GetWindowRep()->Line(xCenter2, yCenter2 - size * h, xCenter2,
      yCenter2 + size * h, lineWidth);

/* Note: we should add code to test the following functions:
 * FillPoly FillPixelPoly DrawPixel DrawPixelArray FillPixelRect Line
 * AbsoluteLine SetXorMode SetCopyMode SetOrMode SetFont
 * SetNormalFont SetDaliServer DaliShowImage DaliFreeImages. */

    /* make sure everything gets drawn to the screen */
    _winReps.GetWindowRep()->Flush();
  }

  virtual void EndConnection() {
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
  }

  DualWindowRep _winReps;	// note: _not_ a pointer to a DualWindowRep

private:
  virtual void Print();
};

void SampleWinServer::ProcessCmd(int argc, char **argv)
{
  // This function should act upon a client command (stored in argv)
  // and then return a positive acknowledgement by calling
  // NetworkSend() with API_ACK or a negative acknowledgement by
  // calling NetworkSend() with API_NAK.

  // This sample server recognizes only commands that begin with
  // the "file" or "edit" keywords, and the "print" and "exit" commands.

  char *cmd = NetworkPaste(argc, argv);
  assert(cmd);
    
  int error = 0;
  char *errMsg = 0;

  if (argc >= 1) {
    if (!strcmp(argv[0], "file"))
      printf("Received file command from client: \"%s\"\n", cmd);
    else if (!strcmp(argv[0], "edit"))
      printf("Received edit command from client: \"%s\"\n", cmd);
    else if (!strcmp(argv[0], "exit"))
      printf("Received exit command from client\n");
    else if (!strcmp(argv[0], "print"))
      Print();
    else {
      printf("Received unrecognized command from client: \"%s\"\n", cmd);
      errMsg = "Unrecognized command";
      error = 1;
    }
  } else {
    printf("Received null command from client\n");
    errMsg = "Null command";
    error = 1;
  }

  delete cmd;

  int result;
  if (error) {
    char *args[] = { errMsg };
    result = NetworkSend(_clientFd, API_NAK, 0, 1, args);
  } else {
    char *args[] = { "done" };
    result = NetworkSend(_clientFd, API_ACK, 0, 1, args);
  }

  if (result < 0) {
    fprintf(stderr, "Client error.\n");
    CloseClient();
    return;
  }

  if (argc == 1 && !strcmp(argv[0], "exit"))
    CloseClient();
}

void SampleWinServer::Print()
{
  printf("Received print command from client\n");

  /* Open the print file. */
  char *filename = "/tmp/client_server.0.ps";
  DevStatus status = ((PSDisplay *) _fileDisp)->OpenPrintFile(filename);
  assert(status.IsComplete());
  ((PSDisplay *) _fileDisp)->PrintPSHeader("cslib sample server");

  /* Switch over to file output. */
  int xVal, yVal;
  unsigned int width, height;
  _winReps.GetFileWinRep()->Origin(xVal, yVal);
  _winReps.GetFileWinRep()->Dimensions(width, height);

  Rectangle viewGeom, parentGeom;
  viewGeom.x = parentGeom.x = xVal;
  viewGeom.y = parentGeom.y = yVal;
  viewGeom.width = parentGeom.width = width;
  viewGeom.height = parentGeom.height = height;
  _winReps.SetFileOutput(viewGeom, parentGeom);

  /* Redraw the window. */
  Redraw();

  /* Switch back to screen output. */
  _winReps.SetScreenOutput();

  /* Close the print file. */
  ((PSDisplay *) _fileDisp)->PrintPSTrailer();
  (void) ((PSDisplay *) _fileDisp)->ClosePrintFile();

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
