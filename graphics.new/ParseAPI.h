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
  Revision 1.1  1996/05/11 17:27:22  jussi
  Initial revision. Moved all API parsing to ParseAPI.C so that
  ServerAPI.c and TkControl.c would not have to duplicate it.
*/

#ifndef ParseAPI_h
#define ParseAPI_h

class ControlPanel;
class MapInterpClassInfo;

#define API_CMD 0
#define API_ACK 1
#define API_NAK 2
#define API_CTL 3

extern int ParseAPI(int argc, char **argv, ControlPanel *control);

#endif
