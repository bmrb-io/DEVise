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

  $Log$*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ClientServer.h"

static char *_progName = 0;

class SampleWinServer : public WinServer {
 public:  
  SampleWinServer(char *name, int port) : WinServer(name, port) {
    _win = 0;
  }

  ~SampleWinServer() {
     assert(_display);
     if (_win)
      _display->DestroyWindowRep(_win);
  }

 protected:
  virtual void ProcessCmd(int argc, char **argv);

  virtual void BeginConnection() {
    assert(_display && !_win);
    _win = _display->CreateWindowRep(_name, 400, 100, 400, 400, BlackColor,
				     WhiteColor, 0, 10, 10, 0, 1);
    assert(_win);
    _win->RegisterCallback(this);
    Redraw();
  }

  virtual void Redraw() {
    int x, y;
    unsigned int w, h;
    assert(_win);
    _win->Origin(x, y);
    _win->Dimensions(w, h);

    /* use color from local colormap (see Color.h) */
    _win->SetFgColor(SeaGreenColor);
    _win->FillRect(x + 0.25 * w, y + 0.25 * h, 0.5 * w, 0.5 * h);
    _win->SetFgColor(BlackColor);
    _win->AbsoluteText("Server Display", x, y, w, h,
		       WindowRep::AlignCenter, false);

    /* bypass local colormap */
    for(float i = 0; i < 1.0; i += 0.1) {
      _win->SetFgRGB(1 - i, 0, 0);
      _win->FillRect(x + i * w, y + 0.80 * h, 0.1 * w, 0.05 * h);
      _win->SetFgRGB(0, i, 0);
      _win->FillRect(x + i * w, y + 0.85 * h, 0.1 * w, 0.05 * h);
      _win->SetFgRGB(0, 0, 1 - i);
      _win->FillRect(x + i * w, y + 0.90 * h, 0.1 * w, 0.05 * h);
      _win->SetFgRGB(i, i, i);
      _win->FillRect(x + i * w, y + 0.95 * h, 0.1 * w, 0.05 * h);
    }

    _win->Flush();
  }

  virtual void EndConnection() {
    assert(_win && _display);
    _display->DestroyWindowRep(_win);
    _win = 0;
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

  WindowRep *_win;                      // main window on display
};

void SampleWinServer::ProcessCmd(int argc, char **argv)
{
  // This function should act upon a client command (stored in argv)
  // and then return a positive acknowledgement by calling
  // NetworkSend() with API_ACK or a negative acknowledgement by
  // calling NetworkSend() with API_NAK.

  // This sample server recognizes only commands that begin with
  // the "file" or "edit" keywords, and the "exit" command.

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
